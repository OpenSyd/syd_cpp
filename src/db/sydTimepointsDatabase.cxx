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
#include "sydTimepointsDatabase.h"

//static syd::TimepointsDatabase::Database::type_name_ = "TimepointsDatabase";

// --------------------------------------------------------------------
syd::TimepointsDatabase::TimepointsDatabase(std::string name):
  Database("TimepointsDatabase", name)
{
  cdb_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimepointsDatabase::GetFullPath(Patient patient)
{
  return get_folder()+patient.path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimepointsDatabase::GetFullPathSPECT(Timepoint timepoint)
{
  Serie serie = cdb_->GetById<Serie>(timepoint.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::string p = GetFullPath(patient);
  return p+PATH_SEPARATOR+"spect"+toString(timepoint.number)+".mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimepointsDatabase::GetFullPathCT(Timepoint timepoint)
{
  Serie serie = cdb_->GetById<Serie>(timepoint.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::string p = GetFullPath(patient);
  return p+PATH_SEPARATOR+"ct"+toString(timepoint.number)+".mhd";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimepointsDatabase::UpdateAllTimepointNumbers(IdType patient_id)
{
  // Get all timepoint for this patient
  std::vector<Timepoint> timepoints;
  LoadVector<Timepoint>(timepoints, odb::query<Timepoint>::patient_id == patient_id);

  // Get corresponding series acquisition_date
  std::vector<Serie> series;
  for(auto i=timepoints.begin(); i<timepoints.end(); i++) {
    series.push_back(cdb_->GetById<Serie>(i->serie_id));
  }

  // Ordered indices
  std::vector<size_t> indices;
  for(auto i=0; i != series.size(); i++) indices.push_back(i);
  std::sort(begin(indices), end(indices),
            [&series](size_t a, size_t b) { return syd::IsBefore(series[a].acquisition_date, series[b].acquisition_date); }  );

  // Set the new order for all the timepoints
  std::vector<std::string> old_paths_spect(timepoints.size());
  std::vector<std::string> old_paths_ct(timepoints.size());
  for(auto i=0; i<timepoints.size(); i++) {
    std::string s = GetFullPathSPECT(timepoints[i]);
    old_paths_spect[i] = s;
    s = GetFullPathCT(timepoints[i]);
    old_paths_ct[i] = s;
  }
  for(auto i=0; i<timepoints.size(); i++) { // two loops needed !
    timepoints[indices[i]].number = i+1;
  }

  // Rename file (use a temporary filename to avoir overwriting the files)
  for(auto i=0; i<timepoints.size(); i++) {
    if (old_paths_spect[i] != GetFullPathSPECT(timepoints[i])) {
      VLOG(3) << "Rename (old) " << old_paths_spect[i] << " to (new) " << GetFullPathSPECT(timepoints[i]);
      std::string path = GetFullPathSPECT(timepoints[i])+"TMP.mhd";
      syd::RenameMHDImage(old_paths_spect[i], path, 4); // 4 is verbose level
    }
    if (old_paths_ct[i] != GetFullPathCT(timepoints[i])) {
      VLOG(3) << "Rename (old) " << old_paths_ct[i] << " to (new) " << GetFullPathCT(timepoints[i]);
      std::string path = GetFullPathCT(timepoints[i])+"TMP.mhd";
      syd::RenameMHDImage(old_paths_ct[i], path, 4);
    }
  }
  for(auto i=0; i<timepoints.size(); i++) {
    if (old_paths_spect[i] != GetFullPathSPECT(timepoints[i])) {
      std::string pathTMP = GetFullPathSPECT(timepoints[i])+"TMP.mhd";
      std::string path = GetFullPathSPECT(timepoints[i]);
      syd::RenameMHDImage(pathTMP, path, 4);
    }
    if (old_paths_ct[i] != GetFullPathCT(timepoints[i])) {
      std::string pathTMP = GetFullPathCT(timepoints[i])+"TMP.mhd";
      std::string path = GetFullPathCT(timepoints[i]);
      syd::RenameMHDImage(pathTMP, path, 4);
    }
  }

  // Update the DB
  odb::transaction t (db->begin());
  for(auto i=0; i<timepoints.size(); i++) {
    db->update(timepoints[i]);
  }
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TimepointsDatabase::Print(Timepoint t)
{
  Serie serie = cdb_->GetById<Serie>(t.serie_id);
  Patient patient = cdb_->GetById<Patient>(serie.patient_id);
  std::stringstream ss;
  ss << patient.name << " " << t.id << " " << t.number << " " << serie.acquisition_date;
  return ss.str();
}
// --------------------------------------------------------------------
