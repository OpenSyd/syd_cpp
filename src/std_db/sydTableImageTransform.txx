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
template<class ImageTransform>
void InsertNewImageTransform(ImageTransform & transfo,
                             syd::Database * db,
                             syd::Image & fixed_image,
                             syd::Image & moving_image,
                             std::string & config_filename)
{
  // Set the images
  transfo.fixed_image = std::make_shared<syd::Image>(fixed_image);
  transfo.moving_image = std::make_shared<syd::Image>(moving_image);

  // Get folder (create if does not exist)
  std::string folder = syd::GetAbsoluteFolder<ImageTransform>(db, transfo);
  DD(folder);
  if (!syd::DirExists(folder)) syd::CreateDirectory(folder);

  // Create filename
  std::string filename = syd::GetFilenameFromPath(config_filename);

  // Create a file for the config file
  syd::File file;
  syd::InsertNewFile(file, db, filename, GetRelativeFolder(db, transfo)); // will check already exist
  DD(file);
  transfo.config_file = std::make_shared<syd::File>(file);

  // Copy the file to the right folder
  DD("copy");
  DD(config_filename);
  DD(GetAbsoluteFolder(db, file));
  syd::CopyFile(config_filename, GetAbsolutePath(db, file));

  // Insert
  db->Insert(transfo);
}
// --------------------------------------------------------------------
