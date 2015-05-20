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



// --------------------------------------------------
bool syd::DicomFile::operator==(const DicomFile & p)
{
  return (id == p.id and
          *file == *p.file and
          *dicom_serie == *p.dicom_serie and
          dicom_sop_uid == p.dicom_sop_uid and
          dicom_instance_number == p.dicom_instance_number);
}
// --------------------------------------------------
