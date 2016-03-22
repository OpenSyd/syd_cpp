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


// --------------------------------------------------------------------
template<class PixelType>
typename itk::Image<PixelType,3>::Pointer
syd::StandardDatabase::ReadImage(const syd::DicomSerie::pointer dicom,
                                 bool flipAxeIfNegativeFlag) const
{
  typedef itk::Image<PixelType,3> ImageType;

  // Get the files
  LOG(4) << "Get the dicom files";
  syd::DicomFile::vector dicom_files;
  odb::query<syd::DicomFile> q = odb::query<syd::DicomFile>::dicom_serie->id == dicom->id;
  Query(dicom_files, q);
  if (dicom_files.size() == 0) {
    EXCEPTION("Error not DicomFile associated with this DicomSerie: " << dicom);
  }
  std::vector<std::string> dicom_filenames;
  for(auto f:dicom_files) {
    dicom_filenames.push_back(GetAbsolutePath(f));
  }
  LOG(4) << "Found " << dicom_files.size();

  typename ImageType::Pointer itk_image;
  try {
    if (dicom_filenames.size() == 1) {
      itk_image = syd::ReadDicomFromSingleFile<PixelType>(dicom_filenames[0]);
    }
    else {
      std::string folder = dicom_filenames[0];
      syd::Replace(folder, syd::GetFilenameFromPath(folder), "");
      LOG(4) << "ITK reader (sort the list of files according to dicom position)";
      // Cannot use ReadDicomSerieFromListOfFiles because the files are not sorted according to dicom
      // std::vector<std::string> files;
      // for(auto df:dicom_files) files.push_back(df->file->GetAbsolutePath(this));
      // itk_image = syd::ReadDicomSerieFromListOfFiles<PixelType>(folder, files);
      itk_image = syd::ReadDicomSerieFromFolder<PixelType>(folder, dicom->dicom_series_uid);
    }
  } catch (std::exception & e) {
    EXCEPTION("Error '" << e.what() << "' during ReadImage of dicom: " << dicom);
  }

  // Flip ?
  if (flipAxeIfNegativeFlag) {
    if (itk_image->GetDirection()[0][0] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 0);
    if (itk_image->GetDirection()[1][1] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 1);
    if (itk_image->GetDirection()[2][2] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 2);
  }

  return itk_image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ArgsInfo>
void syd::StandardDatabase::SetImageTagsFromCommandLine(syd::Image::pointer image, ArgsInfo args_info)
{
  // Remove all tags
  if (args_info.remove_all_tag_flag) image->tags.clear();

  // Remove some tags
  if (args_info.remove_tag_given) {
    for(auto i=0; i<args_info.remove_tag_given; i++) {
      std::string tagname = args_info.remove_tag_arg[i];
      syd::Tag::vector tags;
      try {
        FindTags(tags, tagname);
      } catch(std::exception & e) { } // ignore unknown tag
      for(auto t:tags) image->RemoveTag(t); // FIXME to change in RemoveTag(image->tags, t);
    }
  }

  // Add tags
  if (args_info.tag_given) {
    for(auto i=0; i<args_info.tag_given; i++) {
      std::string tagname = args_info.tag_arg[i];
      syd::Tag::vector tags;
      try {
        FindTags(tags, tagname);
      } catch(std::exception & e) {
        LOG(WARNING) << "Some tags are ignored. " << e.what();
      }
      for(auto t:tags) AddTag(image->tags, t);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ArgsInfo>
void syd::StandardDatabase::UpdateTagsFromCommandLine(syd::Tag::vector & tags, ArgsInfo & args_info)
{
  // Remove all tags
  if (args_info.remove_all_tag_flag) tags.clear();

  // Remove some tags
  if (args_info.remove_tag_given) {
    for(auto i=0; i<args_info.remove_tag_given; i++) {
      std::string tagname = args_info.remove_tag_arg[i];
      syd::Tag::vector tags_temp;
      try {
        FindTags(tags_temp, tagname);
      } catch(std::exception & e) { } // ignore unknown tag
      RemoveTag(tags, tags_temp);
    }
  }

  // Add tags
  if (args_info.tag_given) {
    for(auto i=0; i<args_info.tag_given; i++) {
      std::string tagname = args_info.tag_arg[i];
      syd::Tag::vector tags_temp;
      try {
        FindTags(tags_temp, tagname);
      } catch(std::exception & e) {
        LOG(WARNING) << "Some tags are ignored. " << e.what();
      }
      AddTag(tags, tags_temp);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ArgsInfo, class RecordType>
void syd::StandardDatabase::SetTagsFromCommandLine(typename RecordType::pointer record, ArgsInfo args_info)
{
  // Remove all tags
  if (args_info.remove_all_tag_flag) record->tags.clear();

  // Remove some tags
  if (args_info.remove_tag_given) {
    for(auto i=0; i<args_info.remove_tag_given; i++) {
      std::string tagname = args_info.remove_tag_arg[i];
      syd::Tag::vector tags;
      try {
        FindTags(tags, tagname);
      } catch(std::exception & e) { } // ignore unknown tag
      for(auto t:tags) record->RemoveTag(t);
    }
  }

  // Add tags
  if (args_info.tag_given) {
    for(auto i=0; i<args_info.tag_given; i++) {
      std::string tagname = args_info.tag_arg[i];
      syd::Tag::vector tags;
      try {
        FindTags(tags, tagname);
      } catch(std::exception & e) {
        LOG(WARNING) << "Some tags are ignored. " << e.what();
      }
      for(auto t:tags) record->AddTag(t);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::StandardDatabase::QueryByTags(generic_record_vector & records,
                                       const std::vector<std::string> & tag_names,
                                       const std::string & patient_name)
{
  if (tag_names.size() == 0) {
    Query(records, RecordType::GetStaticTableName());
    return;
  }

  typename RecordType::vector temp;
  QueryByTag<RecordType>(temp, tag_names[0], patient_name);
  for(auto record:temp) {
    int n=0;
    for(auto t:tag_names) { // brute force search !!
      auto iter = std::find_if(record->tags.begin(), record->tags.end(),
                               [&t](syd::Tag::pointer & tag)->bool { return tag->label == t;} );
      if (iter == record->tags.end()) continue;
      else ++n;
    }
    if (n == tag_names.size()) records.push_back(record);
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::StandardDatabase::QueryByTags(typename RecordType::vector & records,
                                        const std::vector<std::string> & tag_names,
                                        const std::string & patient_name)
{
  syd::Record::vector temp;
  records.clear();
  QueryByTags<RecordType>(temp, tag_names, patient_name);
  for(auto t:temp) records.push_back(std::static_pointer_cast<RecordType>(t));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// Specific query to match image with a tag name, in order to speed up
// a bit the FindImageByTag.
template<class RecordType>
void syd::StandardDatabase::QueryByTag(typename RecordType::vector & records,
                                       const std::string & tag_name,
                                       const std::string & patient_name)
{
  std::vector<syd::IdType> ids; // resulting id of the records

  auto desc = GetDatabaseDescription();
  syd::TableDescription * table_desc;
  desc->FindTableDescription(RecordType::GetStaticTableName(), &table_desc);

  // Create request code
  std::ostringstream sql; // request
  std::string t1="\""+table_desc->GetSQLTableName()+"\"";       // "\"syd::Image\"";
  std::string t2="\""+table_desc->GetSQLTableName()+"_tags\"";  // "\"syd::Image_tags\"";
  std::string t3="\"syd::Tag\"";
  sql << "select " << t1 << ".id ";
  sql << "from   " << t1 << "," << t2 << "," << t3 << " ";
  sql << "where  " << t1 << ".id == " << t2 << ".object_id ";
  sql << "and    " << t2 << ".value == " << t3 << ".id ";
  sql << "and " << t3 << ".label==" << "\"" << tag_name << "\" ";
  if (patient_name != "all") {
    syd::Patient::pointer p = FindPatient(patient_name);
    sql << " and " << t1 << ".patient == \"" << p->id << "\" ";
  }
  sql << ";";

  // Native query
  try {
  odb::sqlite::connection_ptr c (odb_db_->connection ());
  sqlite3 * sdb(c->handle());
  sqlite3_stmt * stmt;
  std::string s = sql.str();
  auto rc = sqlite3_prepare_v2(sdb, s.c_str(), -1, &stmt, NULL);
  if (rc==SQLITE_OK) {
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      std::string n = syd::sqlite3_column_text_string(stmt, 0);
      ids.push_back(atoi(n.c_str()));
    }
  }
  } catch(std::exception & e) {
    EXCEPTION("Error during sql query. Error is " << e.what());
  }

  // Retrieve records
  Query(records, ids);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::StandardDatabase::SortAndPrint(typename RecordType::vector & records)
{
  if (records.size() == 0) return;
  Sort(records);
  syd::PrintTable table;
  table.SetFormat("default");
  table.SetHeaderFlag(false);
  records[0]->InitTable(table);
  table.Dump(records, std::cout);
}
// --------------------------------------------------------------------
