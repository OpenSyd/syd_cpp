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
#include "sydDicomUtils.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydDicomInfo, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db =
    m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the dicom serie
  syd::IdType id = atoi(args_info.inputs[0]);
  syd::DicomSerie::pointer dicomserie;
  db->QueryOne(dicomserie, id);

  // Retrieve the filename
  syd::DicomFile::vector dfiles;
  typedef odb::query<syd::DicomFile> QDF;
  QDF q = QDF::dicom_serie == id;
  db->Query(dfiles, q);
  std::string file = dfiles[0]->file->GetAbsolutePath();
  // Only the first file is read

  // Output
  std::cout << dicomserie << std::endl
            << file << std::endl;

  // Read dicom header
  auto dicomIO = syd::ReadDicomHeader(file);

  // itk Examples_2IO_2DicomImageReadPrintTags
  typedef itk::MetaDataDictionary DictionaryType;
  const DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();
  typedef itk::MetaDataObject< std::string > MetaDataStringType;
  typedef itk::MetaDataObject< double > MetaDataDoubleType;
  DictionaryType::ConstIterator itr = dictionary.Begin();
  DictionaryType::ConstIterator end = dictionary.End();
  while (itr != end) {
    itk::MetaDataObjectBase::Pointer entry = itr->second;
    MetaDataStringType::Pointer entryvalue =
      dynamic_cast<MetaDataStringType*>(entry.GetPointer());
    if (entryvalue) {
      std::string tag_key = itr->first;
      std::string tag_name = "Unknown";
      std::string tag_value = entryvalue->GetMetaDataObjectValue();
      bool found = itk::GDCMImageIO::GetLabelFromTag(tag_key, tag_name);
      if (!found) {
        tag_name = syd::SearchDicomTagNameFromTagKey(tag_key);
      }
      std::cout << "(" << tag_key << ") " << tag_name;
      std::cout << " = " << tag_value << std::endl;
    }
    else {
      std::cout << "bug entry value ??" << std::endl;
    }
    ++itr;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
