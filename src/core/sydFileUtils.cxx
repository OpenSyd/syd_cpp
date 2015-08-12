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

#include "sydFileUtils.h"

// --------------------------------------------------------------------
// bool syd::FileExists(std::string filename)
// {
//   return OFStandard::fileExists(filename.c_str());
// }
// --------------------------------------------------------------------


// // --------------------------------------------------------------------
// bool syd::DirExists(std::string folder)
// {
//   return OFStandard::dirExists(folder.c_str());
// }
// // --------------------------------------------------------------------


// // --------------------------------------------------------------------
// bool syd::FolderExists(std::string folder)
// {
//   return OFStandard::dirExists(folder.c_str());
// }
// // --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::GetWorkingDirectory(std::string & pwd) {
  char cCurrentPath[FILENAME_MAX];
  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) return false;
  pwd = std::string(cCurrentPath);
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// http://stackoverflow.com/questions/6163611/compare-two-files
bool syd::EqualFiles(std::ifstream & in1, std::ifstream & in2)
{
  std::ifstream::pos_type size1, size2;
  size1 = in1.seekg(0, std::ifstream::end).tellg();
  in1.seekg(0, std::ifstream::beg);
  size2 = in2.seekg(0, std::ifstream::end).tellg();
  in2.seekg(0, std::ifstream::beg);
  if(size1 != size2) return false;

  static const size_t BLOCKSIZE = 4096;
  size_t remaining = size1;
  while(remaining) {
    char buffer1[BLOCKSIZE], buffer2[BLOCKSIZE];
    size_t size = std::min(BLOCKSIZE, remaining);
    in1.read(buffer1, size);
    in2.read(buffer2, size);
    if(0 != memcmp(buffer1, buffer2, size)) return false;
    remaining -= size;
  }
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::EqualFiles(std::string in1, std::string in2)
{
  std::ifstream fin1(in1, std::ios::binary);
  std::ifstream fin2(in2, std::ios::binary);
  return EqualFiles(fin1, fin2);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CopyFile(std::string src, std::string dst)
{
  std::ifstream isrc(src, std::ios::binary);
  std::ofstream idst(dst, std::ios::binary);
  idst << isrc.rdbuf();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ConvertToAbsolutePath(std::string & folder)
{
  if (folder.empty()) {
    EXCEPTION("The 'foldername' is void. Abort.");
  }

  if (folder.at(0) != PATH_SEPARATOR)  { // the folder is not an absolute path
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))  {
      EXCEPTION("Error while trying to get current working dir.");
    }
    folder = std::string(cCurrentPath)+"/"+folder;
    //dirPrefix = OFString(cCurrentPath);
  }

  RemoveBackPathSeparator(folder);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RemoveBackPathSeparator(std::string & folder)
{
  // remove PATH_SEPARATOR at the end
  while (folder.back() == PATH_SEPARATOR) folder = folder.substr(0, folder.size()-1);
}
// --------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::GetFilenameFromPath(const std::string path) {
  if (path.back() == PATH_SEPARATOR) {
    sydLOG(FATAL) << "Cannot get the filename of '" << path << "' because it ends with a " << PATH_SEPARATOR;
  }
  size_t n = path.find_last_of(PATH_SEPARATOR);
  std::string p = path.substr(n+1);
  RemoveBackPathSeparator(p);
  return p;
}
//------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetPathFromFilename(std::string & path)
{
  if (path.back() == PATH_SEPARATOR) {
    sydLOG(FATAL) << "Cannot get the path of '" << path << "' because it ends with a " << PATH_SEPARATOR;
  }
  size_t n = path.find_last_of(PATH_SEPARATOR);
  if (n == std::string::npos) { // no separator
    //sydLOG(FATAL) << "Cannot get the path of '" << path << "' because not " << PATH_SEPARATOR << " was found.";
    std::ostringstream s;
    s << "." << PATH_SEPARATOR; // current path
    return s.str();
  }
  std::string p = path.substr(0,n);
  return p;
}
// --------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::GetExtension(const std::string filename) {
  size_t n = filename.find_last_of(".");
  return filename.substr(n+1);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::CreateTemporaryFile(const std::string & folder, const std::string & extension)
{
  char filename[512];
  sprintf(filename, "%s%c%s%s", folder.c_str(), PATH_SEPARATOR, "syd_temp_XXXXXX", extension.c_str());
  int fd = mkstemps(filename,extension.size());
  if (fd == -1) {
    sydLOG(FATAL) << "Could not create temporary file: " << filename;
  }
  return std::string(filename);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::CreateTemporaryDirectory(const std::string & folder)
{
  char foldername[512];
  sprintf(foldername, "%s%c%s", folder.c_str(), PATH_SEPARATOR, "syd_temp_XXXXXX");
  char * fd = mkdtemp(foldername);
  if (fd == NULL) {
    sydLOG(FATAL) << "Could not create temporary folder: " << foldername;
  }
  return std::string(fd);
}
//------------------------------------------------------------------
