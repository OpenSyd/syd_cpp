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
syd::DicomFile::DicomFile():syd::Record("")
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
  ss << id << " "
     << dicom_serie->injection->patient->name << " "
     << dicom_serie->id << " "
     << dicom_instance_number << " "
     << file->filename;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool syd::DicomFile::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          file->IsEqual(p->file) and
          dicom_serie->IsEqual(p->dicom_serie) and
          dicom_sop_uid == p->dicom_sop_uid and
          dicom_instance_number == p->dicom_instance_number);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  file = p->file;
  dicom_serie = p->dicom_serie;
  dicom_sop_uid = p->dicom_sop_uid;
  dicom_instance_number = p->dicom_instance_number;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "Cannot insert DicomFile with 'Set'. Use sydInsertDicom.";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'DicomFile': " << std::endl
              << "\tdefault: id serie_id dicom_instance_nb file dop_uid" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("serie", 5);
  ta.AddColumn("nb", 8);
  ta.AddColumn("file", 5);
  ta.AddColumn("sop_uid", 20);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomFile::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << dicom_serie->id << dicom_instance_number << file->filename << dicom_sop_uid;
}
// --------------------------------------------------
