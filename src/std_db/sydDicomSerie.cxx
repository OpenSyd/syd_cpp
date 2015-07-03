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
#include "sydDicomSerie.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::DicomSerie::DicomSerie():syd::Record("")
{
  patient = NULL;
  injection = NULL;
  size[0] = size[1] = size[2] = 0;
  spacing[0] = spacing[1] = spacing[2] = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomSerie::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << patient->name << " "
     << (injection==NULL ? "no_inj":injection->radionuclide->name) << " "
     << dicom_modality << " "
     << acquisition_date << " "
     << reconstruction_date << " "
     << ArrayToString<int, 3>(size) << " "
     << ArrayToString<double, 3>(spacing) << " "
     << dicom_manufacturer << " "
     << dicom_description;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool syd::DicomSerie::IsEqual(const pointer p) const
{
  bool b = (syd::Record::IsEqual(p) and
            patient->IsEqual(p->patient) and
            acquisition_date == p->acquisition_date and
            reconstruction_date == p->reconstruction_date and
            dicom_study_uid == p->dicom_study_uid and
            dicom_series_uid == p->dicom_series_uid and
            dicom_frame_of_reference_uid == p->dicom_frame_of_reference_uid and
            dicom_modality == p->dicom_modality and
            dicom_manufacturer == p->dicom_manufacturer and
            dicom_description == p->dicom_description and
            size == p->size and
            spacing == p->spacing);
  if (!b) return b;
  if (injection == NULL and p->injection == NULL) return b;
  else return injection->IsEqual(p->injection);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "Cannot insert DicomSerie with 'Set'. Use sydInsertDicom.";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'DicomSerie': " << std::endl
              << "\tdefault: " << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("p", 8);
  ta.AddColumn("inj", 5);
  ta.AddColumn("mod", 5);
  ta.AddColumn("acqui_date", 25);
  ta.AddColumn("recon_date", 25);
  ta.AddColumn("description", 50);

}
// --------------------------------------------------


// --------------------------------------------------
void syd::DicomSerie::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << patient->name;
  if (injection == NULL) ta << "no_inj";
  else ta << injection->radionuclide->name;
  ta << dicom_modality
     << acquisition_date
     << reconstruction_date
     << std::string(dicom_description+dicom_manufacturer);
}
// --------------------------------------------------
