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
#include "sydDicomInfo_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydDicomInfo, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the dicom serie
  syd::IdType id = atoi(args_info.inputs[0]);
  syd::DicomSerie::pointer dicomserie;
  db->QueryOne(dicomserie, id);

  // Retrieve the filename
  syd::DicomFile::vector dfiles;
  typedef odb::query<syd::DicomFile> QDF;
  QDF q = QDF::dicom_serie == id;
  db->Query(dfiles, q);
  std::string file = dfiles[0]->file->GetAbsolutePath(db); // only first file

  // Output
  std::cout << dicomserie << std::endl
            << file << std::endl;

  // Dump dicom fields
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  ImageIOType::Pointer dicomIO = ImageIOType::New();
  dicomIO->SetLoadPrivateTags(true);
  dicomIO->LoadPrivateTagsOn ();
  dicomIO->LoadSequencesOn ();
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(file);
  reader->SetImageIO(dicomIO);
  reader->Update();

  // itk Examples_2IO_2DicomImageReadPrintTags
  typedef itk::MetaDataDictionary DictionaryType;
  const DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();
  typedef itk::MetaDataObject< std::string > MetaDataStringType;
  DictionaryType::ConstIterator itr = dictionary.Begin();
  DictionaryType::ConstIterator end = dictionary.End();
  while (itr != end) {
    itk::MetaDataObjectBase::Pointer entry = itr->second;
    MetaDataStringType::Pointer entryvalue =
      dynamic_cast<MetaDataStringType*>(entry.GetPointer());
    if (entryvalue) {
      std::string tagkey   = itr->first;
      std::string labelId;
      bool found =  itk::GDCMImageIO::GetLabelFromTag( tagkey, labelId );
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      if (found) {
        std::cout << "(" << tagkey << ") " << labelId;
        std::cout << " = " << tagvalue.c_str() << std::endl;
      }
      else {
        std::cout << "(" << tagkey <<  ") " << "Unknown";
        std::cout << " = " << tagvalue.c_str() << std::endl;
      }
    }
    ++itr;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
