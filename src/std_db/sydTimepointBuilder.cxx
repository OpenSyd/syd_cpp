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
syd::TimepointBuilder::TimepointBuilder(syd::StandardDatabase * db):syd::TimepointBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimepointBuilder::TimepointBuilder()
{
  db_ = NULL;
  intra_timepoint_min_hour_diff_ = 0.1; // 6min
  intra_timepoint_max_hour_diff_ = 1; // 60 min
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

  //GUESS  strategy ?
  /*
    consider one DicomSerie (+ injection + tag)
    1 - could be already in another tp (of same inj/tag) -> fail
    2 - far in time from all other tp -> probably create new
    3 - close to one other tp -> if not same modality, create new. If same fail.

    // basic  = CreateTimepoint(dicom) ; AddDicom(tp, dicom); --> no check

     // FIXME --> not needed ??
    intra_timepoint_max_hour_diff_
    intra_timepoint_min_hour_diff_ --> why not only zero ? (fov1 fov2 not same hours)
    inter_timepoint_min_hour_diff_

   */

  // Guess
  syd::Timepoint tp;
  std::vector<syd::Timepoint> timepoints;
  GuessState b=GuessTimepointForThisDicomSerie(dicom, timepoints);

  if (b == GuessDicomSerieAlreadyExist) {
    LOG(1) << "GuessDicomSerieAlreadyExist in " << timepoints[0];
    return;
  }

  if (b == GuessTimepointFound) {
    if (timepoints.size() > 1) {
      std::ostringstream os;
      for(auto t:timepoints) os << t << std::endl;
      LOG(WARNING) << "Error, the dicom could be associated with several timepoints. Don't know what to do, I ignore it" << std::endl
                   << "Dicom: " << dicom << std::endl
                   << "Timepoints: " << os.str();
      return;
    }
    LOG(1) << "GuessTimepointFound " << timepoints[0];
    tp = timepoints[0];
  }

  if (b == GuessCreateNewTimepoint) {
    tp = CreateTimepoint(dicom);
    db_->Insert(tp);
    LOG(1) << "Create new timepoint: ";// << tp;
  }

  // Add the dicom to the tp (created or existing)
  AddDicom(tp, dicom);
  db_->Update(tp);
  LOG(1) << "Timepoint: " << tp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimepointBuilder::GuessState
syd::TimepointBuilder::GuessTimepointForThisDicomSerie(syd::DicomSerie & dicom,
                                                       std::vector<syd::Timepoint> & candidates)
{
  // Retrieve all timepoints already existing for the current tag and
  // same injection (hence patient)
  syd::Injection injection = *dicom.injection;
  std::vector<syd::Timepoint> timepoints;
  db_->Query<syd::Timepoint>(odb::query<syd::Timepoint>::injection->id == injection.id and
                             odb::query<syd::Timepoint>::tag->id == tag_.id, timepoints);

  // If no timepoints -> create a new
  if (timepoints.size() == 0) return GuessCreateNewTimepoint;

  // Loop over those timepoints and try to guess if the current dicom could fit
  for(auto t:timepoints) {
    GuessState b = GuessIfDicomCanBeInThisTimepoint(dicom, t);

    // If the dicom already exist, we stop here.
    if (b == GuessDicomSerieAlreadyExist) {
      candidates.resize(1);
      candidates[0] = t;
      return GuessDicomSerieAlreadyExist;
    }

    // If the dicom can fit in the current tp, we store and continue.
    if (b == GuessTimepointFound) candidates.push_back(t);
  }

  // Nothing found: we probably need to create one
  if (candidates.size() == 0) return GuessCreateNewTimepoint;

  // Some candidates timepoints found
  return GuessTimepointFound;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimepointBuilder::GuessState
syd::TimepointBuilder::GuessIfDicomCanBeInThisTimepoint(syd::DicomSerie & dicom,
                                                        syd::Timepoint & timepoint)
{
  if (timepoint.dicoms.size() == 0) {
    LOG(FATAL) << "Error this timepoint has no associated dicom ?" << std::endl
               << "Timepoint: " << timepoint;
  }
  for(auto d:timepoint.dicoms) {
    if (dicom.id == d->id) return GuessDicomSerieAlreadyExist;
    // Compute the time difference
    double diff = fabs(syd::DateDifferenceInHours(dicom.acquisition_date, d->acquisition_date));
    DD(diff);

    // Time difference too large, probably not this timepoint
    if (diff > intra_timepoint_max_hour_diff_) {
      DD("time too large");
      return GuessNotThisTimepoint;
    }
  }

  return GuessTimepointFound;
  /*
    // Time difference not too large, not the same modality, could fit
    if (dicom.dicom_modality != d->dicom_modality) {
        return GuessTimepointFound;
    }


    if (diff < intra_timepoint_min_hour_diff_) {
      LOG(WARNING) << "The dates are too close for this dicom and the dicoms of this timepoint." << std::endl
                   << "Dicom: " << dicom << std::endl
                   << "Timepoint: " << timepoint << std::endl
                   << " with dicom: " << *d << std::endl;
      return GuessError;
    }
    if (diff > intra_timepoint_max_hour_diff_) return GuessNotFound;}

  return GuessFound;
  */
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
