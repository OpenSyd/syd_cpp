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
#include "sydTableDicomSerie.h"
#include "sydStandardDatabase.h"
#include "sydTableFile.h"

// --------------------------------------------------------------------
template<>
void syd::Table<syd::DicomSerie>::Dump(std::ostream & os,
                                  const std::string & format,
                                  const std::vector<syd::IdType> & ids)
{
  std::vector<syd::DicomSerie> dicoms;
  Query(ids, dicoms); // (no sort)
    std::sort(begin(dicoms), end(dicoms),
            [dicoms](DicomSerie a, DicomSerie b) {
              return syd::IsDateBefore(a.acquisition_date, b.acquisition_date); });
  Dump(os, format, dicoms);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void syd::Table<syd::DicomSerie>::Dump(std::ostream & os,
                                  const std::string & format,
                                  const std::vector<syd::DicomSerie> & dicoms)
{

  if (format == "help") {
    std::cout << "Default format  : id p time acqui modality recon_date injec size desc device" << std::endl
              << "format 'file'   : full filename only (only first file)" << std::endl
              << "format 'file_n' : full filenames with line break (only first file" << std::endl;
    return;
  }

  // Get the db
  syd::StandardDatabase * db = dynamic_cast<syd::StandardDatabase*>(database_);

  // Format file
  if (format == "file" or format == "file_n") {
    for(auto & s:dicoms) {
      std::vector<syd::DicomFile> files;
      db->Query<syd::DicomFile>(odb::query<syd::DicomFile>::dicom_serie == s.id, files);
      //      os << db->GetAbsolutePath(*files[0].file);
      os << syd::GetAbsoluteFilePath(db, files[0]);
      if (format == "file") os << " ";
      else os << std::endl;
    }
    if (format == "file") os << std::endl;
    return;
  }

  // Format default
  syd::PrintTable table;
  table.AddColumn("#id", 4);
  table.AddColumn("p", 5);
  table.AddColumn("time", 6, 1);
  table.AddColumn("acqui", 18);
  table.AddColumn("mod", 4);
  table.AddColumn("recon", 18);
  table.AddColumn("inj",12);
  table.AddColumn("size",12);
  table.AddColumn("desc",90);
  table.AddColumn("device",30);
  table.Init();

  //std::string previous;// = d.acquisition_date;
  for(auto d:dicoms) {
    //    double max_time_diff = 0.6;//args_info.max_diff_arg;
    //double diff = syd::DateDifferenceInHours(d.acquisition_date, previous);
    //if (diff > max_time_diff) table.SkipLine();
    table << d.id
          << d.patient->name
          << syd::DateDifferenceInHours(d.acquisition_date, d.injection->date)
          << d.acquisition_date
          << d.dicom_modality
          << d.reconstruction_date
          << d.injection->radionuclide->name
          << syd::ArrayToString<int, 3>(d.size)
          << d.dicom_description
          << d.dicom_manufacturer;
    //previous = d.acquisition_date;
  }
  table.Print(std::cout);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
std::string syd::ComputeRelativeFolder(const syd::Database * db, const syd::DicomSerie & serie)
{
  Patient patient = db->QueryOne<Patient>(serie.patient->id);
  std::string f = syd::ComputeRelativeFolder(db, patient);

  // Add date
  std::string d = serie.acquisition_date;
  //  syd::Replace(d, " ", "_");
  // remove the hour and keep y m d
  d = d.substr(0, 10);
  f = f+PATH_SEPARATOR+d;

  // Add modality
  f = f+PATH_SEPARATOR+serie.dicom_modality;
}
// --------------------------------------------------------------------
