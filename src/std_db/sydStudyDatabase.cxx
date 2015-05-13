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
#include "sydStudyDatabase.h"

// --------------------------------------------------------------------
void syd::StudyDatabase::CreateTables()
{
  syd::ClinicDatabase::CreateTables();
  AddTable<syd::Timepoint>();
  AddTable<syd::Tag>();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TableElement * syd::StudyDatabase::InsertFromArg(const std::string & table_name,
                                                 std::vector<std::string> & arg)
{
  if (table_name == syd::Timepoint::GetTableName()) {
    DD("TODO Insert Timepoint");
    return syd::ClinicDatabase::InsertFromArg(table_name, arg);
  }
  return syd::ClinicDatabase::InsertFromArg(table_name, arg);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Timepoint syd::StudyDatabase::InsertTimepoint(syd::Injection & injection,
                                                   syd::Tag & tag,
                                                   std::vector<DicomSerie> & dicoms,
                                                   bool replaceTimepointFlag)
{
  LOG(FATAL) << "to remove";

  /* Some check are performed:
     - dicom list is not empty
     - all dicoms associated with same patient (from injection)
     - all dicoms with same frame_of_reference_uid
     - if no CT given, try to find a single one (error if several)
     - check if a previous timepoint exist with same injection+tag+date
   */


  // We consider all the dicom to the same timepoint
  syd::Patient patient = *injection.patient;
  if (dicoms.size() == 0) {
    LOG(FATAL) << "Cannot create Timepoint, no dicom to insert";
  }

  // Sort by date
  std::sort(dicoms.begin(), dicoms.end(),
            [&dicoms](syd::DicomSerie a, syd::DicomSerie b) { return
                syd::IsDateBefore(a.acquisition_date, b.acquisition_date); }  );

  // All dicoms *must* have the same frame_of_reference_uid
  std::string uid = dicoms[0].dicom_frame_of_reference_uid;
  for(auto d:dicoms) {
    if (d.dicom_frame_of_reference_uid != uid) {
      LOG(FATAL) << "The frame_of_reference_uid of all dicoms must be equal, but this it not the case for:"
                 << std::endl << dicoms[0]
                 << std::endl << d;
    }
  }

  // Group by modalities
  std::vector<syd::DicomSerie> cts;
  std::vector<syd::DicomSerie> nonct;
  for(auto d:dicoms) {
    if (d.dicom_modality == "CT") cts.push_back(d);
    else nonct.push_back(d);
  }
  if (cts.size() > 1) {
    LOG(FATAL) << "Only one CT by timepoint is allowed, while you set "
               << cts.size() << " images.";
  }


  // Check that two dicoms does not have the same acquisition_date
  std::map<std::string, syd::DicomSerie> map;
  for(auto d:nonct) {
    auto iter = map.find(d.acquisition_date);
    if (iter != map.end()) {
      LOG(FATAL) << "Error, try to insert two dicoms with the same acquisition date: "
                 << std::endl << iter->second
                 << std::endl << d;
    }
    map[d.acquisition_date] = d;
  }

  // Search for CT image
  syd::DicomSerie ct;
  if (cts.size() > 0) ct = cts[0];
  else {
    std::vector<syd::DicomSerie> cts;
    Query<syd::DicomSerie>(odb::query<DicomSerie>::dicom_modality == "CT" and
                           odb::query<DicomSerie>::dicom_frame_of_reference_uid == uid, cts);
    if (cts.size() == 0) {
      LOG(FATAL) << "Cannot find an associated CT for dicom: " << dicoms[0];
    }
    if (cts.size() > 1) {
      std::string s;
      for(auto c:cts) {
        s += c.ToString() + "\n";
      }
      LOG(FATAL) << "I found several CT image with the same dicom_frame_of_reference_uid "
                 << " for the dicom:" << dicoms[0]
                 << ". I don't know which one to choose. Please provide it on the command line. " << std::endl << s;
    }
    ct = cts[0];
    dicoms.push_back(ct); // add the ct in the image list
  }

  // Get the timing
  double t = syd::DateDifferenceInHours(dicoms[0].acquisition_date, injection.date);

  // Check if another timepoint already exist
  syd::Timepoint tp;
  bool update = false;
  std::vector<Timepoint> timepoints;
  Query<Timepoint>(odb::query<Timepoint>::tag->id == tag.id and
                   odb::query<Timepoint>::injection->id == injection.id and
                   odb::query<Timepoint>::time_from_injection_in_hours == t,
                   timepoints);
  if (timepoints.size() > 0) {
    if (replaceTimepointFlag) {
      LOG(0) << "Find similar timepoint, replacing.";
      tp = timepoints[0];
      tp.dicoms.clear();
      update = true;
    }
    else {
      LOG(FATAL) << "A timepoint with the same tag+injection+date already exist: "
                 << timepoints[0];
    }
  }

  // Create the tp
  tp.injection =  std::make_shared<Injection>(injection);
  tp.tag =  std::make_shared<syd::Tag>(tag);
  tp.time_from_injection_in_hours = t;
  for(auto d:dicoms) { // both ct and others images
    tp.dicoms.push_back(std::make_shared<syd::DicomSerie>(d));
  }

  if (update) Update<syd::Timepoint>(tp);
  else syd::Database::Insert<syd::Timepoint>(tp);

  return tp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StudyDatabase::DumpTimepoint(const std::vector<std::string> & args, std::ostream & os, bool verboseFlag)
{

  std::vector<syd::Timepoint> timepoints;
  Query(timepoints);

  for(auto t:timepoints) {
    DD(t);
    for(auto d:t.dicoms) {
      std::cout << "\t " << *d << std::endl;
    }
  }

}
// --------------------------------------------------------------------
