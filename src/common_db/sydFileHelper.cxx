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

// --------------------------------------------------------------------
syd::File::pointer
syd::NewFile(syd::Database * db,
             std::string path,
             std::string filename)
{
  auto f = db->New<syd::File>();
  f->path = path;
  // Create the folder in the db
  auto absolute_folder = db->ConvertToAbsolutePath(path);
  fs::create_directories(absolute_folder);
  f->filename = filename;
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetFilenames(syd::File::vector files)
{
  if (files.size() == 0) return syd::empty_value;
  std::ostringstream ss;
  for(auto f:files) ss << f->filename << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetRelativeFolder(syd::File::vector files)
{
  if (files.size() == 0) return syd::empty_value;
  return files[0]->path+PATH_SEPARATOR;
}
// --------------------------------------------------------------------
