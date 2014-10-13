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
#include "sydActivityDatabase.h"

// --------------------------------------------------------------------
syd::ActivityDatabase::ActivityDatabase(std::string name, std::string param):Database(name)
{
  // List all params
  std::istringstream f(param);
  SetFileAndFolder(f);
  std::string cdb_name;
  if (!getline(f, cdb_name, ';')) {
    LOG(FATAL) << "Error while parsing db cdb name for folder. db is "
               << name << " (" << get_typename() << ") params = " << param;
  }
  std::string sdb_name;
  if (!getline(f, sdb_name, ';')) {
    LOG(FATAL) << "Error while parsing db sdb name for folder. db is "
               << name << " (" << get_typename() << ") params = " << param;
  }

  OpenSqliteDatabase(filename_, folder_);

  cdb_ = syd::Database::OpenDatabaseType<ClinicDatabase>(cdb_name);
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(sdb_name);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ActivityDatabase::~ActivityDatabase()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::CreateDatabase()
{
  DD("CreateDatabase");

  // DD("here");
  // odb::transaction t (db_->begin());
  // odb::schema_catalog::create_schema (*db_);
  // t.commit();

  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::CheckIntegrity(std::vector<std::string> & args)
{
  DD("TODO ActivityDatabase::CheckIntegrity");
  DDS(args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Activity syd::ActivityDatabase::NewActivity(const Patient & patient)
{
  Activity activity;
  activity.patient_id = patient.id;
  RawImage ct;
  ct.patient_id = patient.id;
  ct.md5 = "";
  ct.pixel_type = "short";
  ct.filename = "average.mhd";
  DD("TODO");
  exit(0);
  //FIXME
  ct.path = patient.name+PATH_SEPARATOR;
  Insert(ct);
  activity.average_ct_image_id = ct.id;
  Insert(activity);
  return activity;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivity syd::ActivityDatabase::NewTimeActivity(const Timepoint & timepoint,
                                                         const RoiMaskImage & roi)
{
  TimeActivity ta;
  ta.timepoint_id = timepoint.id;
  ta.roi_id = roi.id;
  Insert(ta);
  return ta;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::Dump(std::ostream & os, std::vector<std::string> & args)
{
  DD("TODO");
  DD("ActivityDatabase:: Dump");
  DDS(args);
};
// --------------------------------------------------------------------
