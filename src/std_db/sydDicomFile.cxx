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

DEFINE_TABLE_IMPL(DicomFile);

// --------------------------------------------------------------------
syd::DicomFile::DicomFile():syd::File()
{
  dicom_sop_uid = "";
  dicom_instance_number = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomFile::ToString() const
{
  std::stringstream ss ;
  ss << syd::File::ToString() << " ";
  ss << dicom_instance_number << " "
     << dicom_sop_uid << " ";
  return ss.str();
}
// --------------------------------------------------------------------


