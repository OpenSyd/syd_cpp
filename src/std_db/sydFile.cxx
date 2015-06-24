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
#include "sydFile.h"
#include "sydStandardDatabase.h"
#include "sydTable.h"
#include "sydImage.h"
#include "sydImage-odb.hxx"
#include "sydDicomFile.h"
#include "sydDicomFile-odb.hxx"

#include "sydTableFile.h"

// --------------------------------------------------
syd::File::File():TableElementBase()
{
  // default value
  filename = "";
  path = "";
  md5 = "";
  //look_for_dicomfile_on_delete_flag = true;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::File::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << path << " "
     << filename << " "
     << (md5 == "" ? "no_md5":"md5");
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::File::operator==(const File & p)
{
  return (id == p.id and
          filename == p.filename and
          path == p.path and
          md5 == p.md5);
}
// --------------------------------------------------



// --------------------------------------------------
void syd::File::OnDelete(syd::Database * d)
{
  syd::StandardDatabase * db = dynamic_cast<syd::StandardDatabase*>(d);

  // When a File is deleted, we also delete the file on disk
  std::string f = syd::GetAbsoluteFilePath(d, *this);//db->GetAbsolutePath(*this);
  db->AddFileToDelete(f);// list_of_files_to_delete.push_back(f);

  // We also look for images to be deleted. Manual search because dont know how to do
  std::vector<syd::Image> images_temp;
  std::vector<syd::Image> images;
  db->Query<syd::Image>(images_temp);
  for(auto i:images_temp) {
    for(auto f:i.files) {
      if (f->id == id) images.push_back(i);
    }
  }
  for(auto i:images) db->AddToDeleteList(i);


  // We also look for DicomFiles to be deleted.
  /* if (look_for_dicomfile_on_delete_flag) {
    std::vector<syd::DicomFile> dicomfiles;
    db->Query<syd::DicomFile>(odb::query<syd::DicomFile>::file == id, dicomfiles);
    for(auto i:dicomfiles) db->AddToDeleteList(i);
  }
  */
}
// --------------------------------------------------
