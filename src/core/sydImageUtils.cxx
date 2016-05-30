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
#include "sydImageUtils.h"

// --------------------------------------------------------------------
/*
  Rename a mhd image, composed of 2 files XXX.mhd and XXX.raw. This
  function moves the two files and change the header to be linked with
  the renamed .raw file.
 */
void syd::RenameOrCopyMHDImage(std::string old_path,
                               std::string new_path,
                               int verbose_level,
                               bool erase)
{
  // Check if not the same
  if (old_path == new_path) {
    LOG(WARNING) << "Try to rename " << old_path << " to the same file. Do nothing.";
    return; // do nothing
  }

  // Check extension old filename
  size_t n = old_path.find_last_of(".");
  std::string extension = old_path.substr(n+1);
  if (extension != "mhd") {
    LOG(FATAL) << "Rename MHD : Error the filename must have mhd as extension : " << old_path;
  }
  std::string old_path_raw = old_path.substr(0,n)+".raw";

  // Check extension new filename
  n = new_path.find_last_of(".");
  extension = new_path.substr(n+1);
  if (extension != "mhd") {
    LOG(FATAL) << "Rename MHD : Error the new filename must have mhd as extension : " << new_path;
  }
  std::string new_path_raw = new_path.substr(0,n)+".raw";

  // Check files
  if (!fs::exists(old_path)) {
    LOG(FATAL) << "Rename MHD : Error path (mhd) not exist : " << old_path;
  }
  if (!fs::exists(old_path_raw)) {
    LOG(FATAL) << "Rename MHD : Error path (raw) not exist : " << old_path_raw;
  }
  // if (fs::exists(new_path)) {
  //   LOG(WARNING) << "Rename MHD : path (mhd) to rename already exist : " << new_path;
  // }
  // if (fs::exists(new_path_raw)) {
  //   LOG(WARNING) << "Rename MHD : path (raw) to rename already exist : " << new_path_raw;
  // }

  // verbose
  if (erase) {
    LOG(verbose_level) << "Rename header " << old_path << " to " << new_path;
  }
  else {
    LOG(verbose_level) << "Copy header " << old_path << " to " << new_path;
  }

  // header part : change ElementDataFile in the header
  std::ifstream in(old_path);
  //  std::ofstream out(new_path);
  std::vector<std::string> outlines;
  // std::ofstream out;
  OFString r;
  OFStandard::getFilenameFromPath(r, old_path_raw.c_str());
  std::string wordToReplace(r.c_str());
  OFStandard::getFilenameFromPath(r, new_path_raw.c_str());
  std::string wordToReplaceWith(r.c_str());
  size_t len = wordToReplace.length();
  std::string line;
  // Delete old path (before writing new)
  if (erase) std::remove(old_path.c_str());
  while (std::getline(in, line)) {
    while (line != "") {
      size_t pos = line.find(wordToReplace);
      if (pos != std::string::npos) {
        line.replace(pos, len, wordToReplaceWith);
        break;
      }
      else
        break;
    }
    outlines.push_back(line+"\n");
  }
  in.close();

  // Write
  std::ofstream out(new_path);
  for(auto i:outlines) out << i;
  out.close();

  // Rename or copy .raw part
  if (erase) {
    LOG(verbose_level) << "Rename raw " << old_path_raw << " to " << new_path_raw;
    int result = std::rename(old_path_raw.c_str(), new_path_raw.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << old_path_raw << " to " << new_path_raw;
    }
  }
  else {
    LOG(verbose_level) << "Copy raw " << old_path_raw << " to " << new_path_raw;
    // std::ifstream  src(old_path_raw.c_str(), std::ios::binary);
    // std::ofstream  dst(new_path_raw.c_str(), std::ios::binary);
    // dst << src.rdbuf();

    // I need to do that way to avoid overwriting if old/new are the same files.
    std::ifstream source(old_path_raw, std::ios::binary);

    // file size
    source.seekg(0, std::ios::end);
    std::ifstream::pos_type size = source.tellg();
    source.seekg(0);
    // allocate memory for buffer
    char* buffer = new char[size];

    // copy file
    source.read(buffer, size);
    source.close();
    std::ofstream dest(new_path_raw, std::ios::binary);
    dest.write(buffer, size);

    // clean up
    delete[] buffer;
    dest.close();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DeleteMHDImage(std::string path)
{
  OFStandard::deleteFile(path.c_str());
  size_t n = path.find_last_of(".");
  std::string path_raw = path.substr(0,n)+".raw";
  OFStandard::deleteFile(path_raw.c_str());
}
// --------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::CopyMHDImage(std::string from , std::string to, int verbose_level)
{
  RenameOrCopyMHDImage(from, to, verbose_level, false);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::RenameMHDImage(std::string from , std::string to, int verbose_level)
{
  RenameOrCopyMHDImage(from, to, verbose_level, true);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Convert PointType
std::string syd::PointToString(const itk::Point<double,3> & t)
{
  std::ostringstream myStream;
  myStream << t[0] << ";" << t[1] << ";" << t[2] << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
itk::ImageIOBase::Pointer syd::ReadImageHeader(const std::string & filename)
{
  itk::ImageIOBase::Pointer reader;
  try {
    reader = itk::ImageIOFactory::CreateImageIO(filename.c_str(), itk::ImageIOFactory::ReadMode);
    reader->SetFileName(filename);
    reader->ReadImageInformation();
  } catch(std::exception & e) {
    EXCEPTION("Error while reading header of image file " << filename << ". Error is: " << e.what());
  }
  return reader;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::WriteImage(typename itk::ImageBase<3>::Pointer image, std::string filename)
{
  typedef itk::ImageBase<3> ImageBaseType;
  //  DD(image->GetComponentTypeAsString(image->GetComponentType()));
}
//--------------------------------------------------------------------
