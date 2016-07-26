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
#include "sydDicomFile.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::DicomFile::DicomFile():syd::Record()
{
  file = NULL;
  dicom_serie = NULL;
  dicom_sop_uid = "";
  dicom_instance_number = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomFile::ToString() const
{
  std::stringstream ss ;
  ss << id << " ";
  if (dicom_serie == NULL) ss << empty_value << " ";
  else {
    ss << dicom_serie->id << " "
       << (dicom_serie->patient != NULL ? dicom_serie->patient->name:empty_value) << " ";
  }
  ss << dicom_instance_number << " "
     << file->filename;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  if (event == odb::callback_event::pre_erase) {
    db.erase(file);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  if (event == odb::callback_event::pre_erase) {
    db.erase(file);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::DumpInTable(syd::PrintTable2 & ta) const
{
  ta.Set("id", id);
  ta.Set("serie", dicom_serie->id);
  ta.Set("nb", dicom_instance_number);
  ta.Set("file", file->filename, 100);
  ta.Set("sop_uid", dicom_sop_uid);
}
// --------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::DicomFile::Check() const
{
  return file->Check();
}
// --------------------------------------------------------------------
