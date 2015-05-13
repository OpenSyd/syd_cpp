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

// --------------------------------------------------------------------
syd::DicomFile::DicomFile():TableElement()
{
  dicom_sop_uid = "";
  dicom_instance_number = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomFile::~DicomFile() {}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomFile::DicomFile(const syd::DicomFile & other) { copy(other); }
// --------------------------------------------------


// --------------------------------------------------
syd::DicomFile & syd::DicomFile::operator= (const syd::DicomFile & other) {
  if (this != &other) { copy(other); }
  return *this;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::copy(const syd::DicomFile & t) {
  id = t.id;
  file = t.file;
  dicom_serie = t.dicom_serie;
  dicom_sop_uid = t.dicom_sop_uid;
  dicom_instance_number = t.dicom_instance_number;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::SetValues(std::vector<std::string> & arg)
{
  LOG(FATAL) << "Could not insert a DicomFile with this function. TODO.";
}
// --------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomFile::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << dicom_serie->injection->patient->name << " "
     << dicom_serie->id << " "
     << dicom_instance_number << " "
     << file->filename;
  return ss.str();
}
// --------------------------------------------------------------------
