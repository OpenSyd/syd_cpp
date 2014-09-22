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
#include "sydTimePointsDatabase.h"

// --------------------------------------------------------------------
std::string syd::TimePointsDatabase::GetFullPath(Patient patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimePointsDatabase::GetFullPathSPECT(TimePoint timepoint)
{
  Serie serie = cdb_->GetById<Serie>(timepoint.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::string p = GetFullPath(patient);
  return p+PATH_SEPARATOR+"spect"+toString(timepoint.number)+".mhd";
}
// --------------------------------------------------------------------


bool date_before(std::string d1, std::string d2)
{
  tm D1;
  tm D2;
  syd::ConvertStringToDate(d1, D1);
  syd::ConvertStringToDate(d2, D2);
  double diff = difftime(mktime(&D1), mktime(&D2));
  bool b = diff < 0;
  return b;
}

// --------------------------------------------------------------------
void syd::TimePointsDatabase::UpdateAllTimePointNumbers(IdType patient_id)
{
  DD("update");

  // Get all timepoint for this patient
  std::vector<TimePoint> timepoints;
  LoadVector<TimePoint>(timepoints, odb::query<TimePoint>::patient_id == patient_id);
  DDS(timepoints);

  // Get corresponding series acquisition_date
  std::vector<Serie> series;
  for(auto i=timepoints.begin(); i<timepoints.end(); i++) {
    series.push_back(cdb_->GetById<Serie>(i->serie_id));
  }

  // Ordered indices
  std::vector<size_t> indices;
  //  for(auto i: ordered<Serie>(series)) indices.push_back(i);
  for(auto i=0; i != series.size(); i++) indices.push_back(i);
  std::sort(begin(indices), end(indices),
            [&series](size_t a, size_t b) { return date_before(series[a].acquisition_date, series[b].acquisition_date); }  );

  // Set the new order for all the timepoints
  std::vector<std::string> old_paths(timepoints.size());
  for(auto i=0; i<timepoints.size(); i++) {
    std::string s = GetFullPathSPECT(timepoints[i]);
    old_paths[i] = s;
  }
  for(auto i=0; i<timepoints.size(); i++) { // two loops needed !
    timepoints[indices[i]].number = i+1;
  }
  DDS(old_paths);
  DDS(timepoints);

  // Rename file (use a temporary filename to avoir overwriting the files)
  for(auto i=0; i<timepoints.size(); i++) {
    if (old_paths[i] != GetFullPathSPECT(timepoints[i])) {
      VLOG(2) << "Rename (old) " << old_paths[i] << " to (new) " << GetFullPathSPECT(timepoints[i]);
      std::string path = GetFullPathSPECT(timepoints[i])+"TMP.mhd";
      syd::RenameMHDFileIfExist(old_paths[i], path, 3); // 2 is verbose level
    }
  }
  for(auto i=0; i<timepoints.size(); i++) {
    if (old_paths[i] != GetFullPathSPECT(timepoints[i])) {
      std::string pathTMP = GetFullPathSPECT(timepoints[i])+"TMP.mhd";
      std::string path = GetFullPathSPECT(timepoints[i]);
      syd::RenameMHDFileIfExist(pathTMP, path, 3); // 2 is verbose level
    }
  }

  // Update the DB
  odb::transaction t (db->begin());
  for(auto i=0; i<timepoints.size(); i++) {
    db->update(timepoints[i]);
  }
  t.commit();


  // CHECK mda5 ?

  // sort according to date
  // compute number
  // compute path + rename


}
// --------------------------------------------------------------------
