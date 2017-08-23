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

// itk (gdcm)
#include "gdcmGlobal.h"
#include "gdcmDictEntry.h"
#include "gdcmDicts.h"

// --------------------------------------------------------------------
/*
  Rename a mhd image, composed of 2 files XXX.mhd and XXX.raw. This
  function moves the two files and change the header to be linked with
  the renamed .raw file.
*/
void syd::RenameOrCopyMHDImage(std::string old_path,
                               std::string new_path,
                               bool erase,
                               bool overwrite_if_exists)
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
    LOG(FATAL) << "Rename MHD : Error path (mhd) not exist: " << old_path;
  }
  if (!fs::exists(old_path_raw)) {
    LOG(FATAL) << "Rename MHD : Error path (raw) not exist: " << old_path_raw;
  }

  // is files already exist ?
  if (!overwrite_if_exists) {
    if (fs::exists(new_path)) {
      EXCEPTION("Rename MHD : path (mhd) already exist: " << new_path);
    }
    if (fs::exists(new_path_raw)) {
      EXCEPTION("Rename MHD : path (raw)  already exist: " << new_path_raw);
    }
  }

  // Create dir if needed
  fs::path p(new_path);
  try {
    fs::create_directories(p.remove_filename());
  } catch(std::exception & e) {
    EXCEPTION("Cannot create directory for " << new_path << ".");
  }

  // header part : change ElementDataFile in the header
  std::ifstream in(old_path);
  std::vector<std::string> outlines;

  std::string r;
  fs::path op(old_path_raw);
  r = op.filename().string();
  std::string wordToReplace = r; //(r.c_str());
  fs::path np(new_path_raw);
  r = np.filename().string();
  std::string wordToReplaceWith(r.c_str());
  size_t len = wordToReplace.length();
  std::string line;
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

  // Delete old path (after writing new)
  if (erase) std::remove(old_path.c_str());

  // Rename or copy .raw part
  if (erase) {
    fs::rename(old_path_raw, new_path_raw);
  }
  else {
    if (overwrite_if_exists)
      fs::copy_file(old_path_raw, new_path_raw, fs::copy_option::overwrite_if_exists);
    else
      fs::copy_file(old_path_raw, new_path_raw, fs::copy_option::none);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DeleteMHDImage(std::string path)
{
  fs::remove(path);
  size_t n = path.find_last_of(".");
  std::string path_raw = path.substr(0,n)+".raw";
  fs::remove(path_raw);
}
// --------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::CopyMHDImage(std::string from, std::string to, bool overwrite_if_exists)
{
  RenameOrCopyMHDImage(from, to, false, overwrite_if_exists);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::RenameMHDImage(std::string from, std::string to, bool overwrite_if_exists)
{
  RenameOrCopyMHDImage(from, to, true, overwrite_if_exists);
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
  if (!fs::exists(filename)) {
    EXCEPTION("File '" << filename << "' does not exist.");
  }
  try {
    reader = itk::ImageIOFactory::CreateImageIO(filename.c_str(), itk::ImageIOFactory::ReadMode);
    reader->SetFileName(filename);
    gdcm::ImageHelper::SetForcePixelSpacing(true);
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
  LOG(FATAL) << "NOT IMPLEMENTED WriteImage";
  typedef itk::ImageBase<3> ImageBaseType;
  //  DD(image->GetComponentTypeAsString(image->GetComponentType()));
}
//--------------------------------------------------------------------
