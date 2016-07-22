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
#include "sydFileHelper.h"
#include "sydImageUtils.h"

// // --------------------------------------------------------------------
// void syd::FileHelper::RenameAndUpdateMHDFile(syd::File::pointer mhd_file,
//                                              syd::File::pointer raw_file,
//                                              std::string relative_path,
//                                              std::string filename)
// {
//   // To rename mhd need to change the content of the linked .raw file.
//   // Rename mhd file
//   std::string old_path = mhd_file->GetAbsolutePath();
//   mhd_file->RenameFile(relative_path, filename, false, true); // do not move on disk
//   std::string new_path = mhd_file->GetAbsolutePath();

//   // Move file on disk
//   syd::RenameMHDImage(old_path, new_path);

//   // Rename raw file
//   std::string f = filename;
//   syd::Replace(f, ".mhd", ".raw");
//   raw_file->RenameFile(relative_path, f, false, true); // do not move on disk
// }
// // --------------------------------------------------------------------
