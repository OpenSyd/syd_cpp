/*=========================================================================
  Program:   syd

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

// syd
#include "sydTimepointBuilder.h"
#include "sydTable.h"

// --------------------------------------------------------------------
syd::TimepointBuilder::TimepointBuilder(syd::StudyDatabase * db):syd::TimepointBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimepointBuilder::TimepointBuilder()
{
  db_ = NULL;
  intra_timepoint_min_hour_diff_ = 0.1; // 6min
  intra_timepoint_max_hour_diff_ = 0.6; // 48 min
  inter_timepoint_min_hour_diff_ = 0.5; // 30 min
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimepointBuilder::SetTag(syd::Tag & tag)
{
  tag_ = tag;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::TimepointBuilder::InsertDicomSerie(syd::DicomSerie & dicom)
{
  // Consider the other timepoint for this injection/tag
  if (dicom.injection == NULL) {
    LOG(WARNING) << "Error no associated injection with the dicom: " << dicom << ", ignoring.";
    return;
  }

  // Guess
  bool isNew=false;
  Timepoint tp;
  GuessState b=GuessTimepointForThisDicomSerie(dicom, tp);
  if (b == GuessError) return;
  if (b == GuessNotFound) {
    isNew = true;
    tp = CreateTimepoint(dicom);
    db_->Insert(tp);
  }
  AddDicom(tp, dicom);
  db_->Update(tp);
  if (isNew) {
    LOG(1) << "Create new timepoint: " << tp;
  }
  else {
    LOG(1) << "Add dicom to existing timepoint: " << tp;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimepointBuilder::GuessState
syd::TimepointBuilder::GuessTimepointForThisDicomSerie(syd::DicomSerie & dicom, syd::Timepoint & tp)
{
  syd::Injection injection = *dicom.injection;
  std::vector<syd::Timepoint> timepoints;
  db_->Query<syd::Timepoint>(odb::query<syd::Timepoint>::injection->id == injection.id and
                             odb::query<syd::Timepoint>::tag->id == tag_.id, timepoints);
  int n=0;
  std::vector<syd::Timepoint> a;
  for(auto t:timepoints) {
    GuessState b = GuessIfDicomCanBeInThisTimepoint(dicom, t);
    if (b == GuessFound) { ++n; a.push_back(t); }
    if (b == GuessError) return b;
  }
  if (n==0) return GuessNotFound;
  if (n==1) { tp = a[0]; return GuessFound; }
  LOG(WARNING) << "Error, this dicom could belong to several timepoints, something is wrong." << std::endl
             << "Dicom: " << dicom << std::endl
             << "Timepoint 0: " << a[0] << std::endl
             << "Timepoint 1: " << a[1] << std::endl;
  return GuessError;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimepointBuilder::GuessState
syd::TimepointBuilder::GuessIfDicomCanBeInThisTimepoint(syd::DicomSerie & dicom, syd::Timepoint & timepoint)
{
  if (timepoint.dicoms.size() == 0) {
    LOG(FATAL) << "Error this timepoint has no associated dicom ?" << std::endl
               << "Timepoint: " << timepoint;
  }
  for(auto d:timepoint.dicoms) {
    double diff = fabs(syd::DateDifferenceInHours(dicom.acquisition_date, d->acquisition_date));
    if (diff < intra_timepoint_min_hour_diff_) {
      if (dicom.id == d->id) {
        LOG(WARNING) << "The dicom is already inside the following timpoint:" << timepoint
                     << std::endl << "Dicom: " << dicom;
        return GuessError;
      }
      LOG(WARNING) << "The dates are too close for this dicom and the dicoms of this timepoint." << std::endl
                   << "Dicom: " << dicom << std::endl
                   << "Timepoint: " << timepoint << std::endl
                   << " with dicom: " << *d << std::endl;
      return GuessError;
    }
    if (diff > intra_timepoint_max_hour_diff_) return GuessNotFound;
  }
  return GuessFound;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
syd::Timepoint syd::TimepointBuilder::CreateTimepoint(syd::DicomSerie & dicom)
{
  // Get information
  syd::Injection & injection = *dicom.injection;

  // Create
  syd::Timepoint tp;
  tp.injection = std::make_shared<syd::Injection>(injection);
  tp.tag = std::make_shared<syd::Tag>(tag_);
  return tp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimepointBuilder::AddDicom(syd::Timepoint & timepoint, syd::DicomSerie & dicom)
{
  timepoint.dicoms.push_back(std::make_shared<syd::DicomSerie>(dicom));

  // policy if several dicom: consider the first one
  std::string date = timepoint.dicoms[0]->acquisition_date;
  for(auto d:timepoint.dicoms) {
    double diff = syd::DateDifferenceInHours(date, d->acquisition_date);
    if (diff>0) date = d->acquisition_date;
  }
  timepoint.time_from_injection_in_hours =
    syd::DateDifferenceInHours(date, timepoint.injection->date);
}
// --------------------------------------------------------------------
