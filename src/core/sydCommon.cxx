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

#include "sydCommon.h"

// --------------------------------------------------------------------
void syd::CreateDirectory(std::string path)
{
  // See http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
  typedef struct stat Stat;
  Stat            st;
  int             status = 0;
  if (stat(path.c_str(), &st) != 0) {
    /* Directory does not exist. EEXIST for race condition */
    if (mkdir(path.c_str(), 0777) != 0 && errno != EEXIST)
      status = -1;
  }
  else {
    LOG(WARNING) << "The folder " << path << " already exist.";
  }
  if (status != 0) {
    LOG(FATAL) << "Error while creating the folder " << path;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetDate(std::string date, std::string time)
{
  std::string result;
  result= date.substr(0,4)+"-"+date.substr(4,2)+"-"+date.substr(6,2)+" "
    + time.substr(0,2)+":"+time.substr(2,2);
  return result;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
unsigned long syd::toULong(std::string s)
{
  std::istringstream iss(s);
  unsigned long i;
  iss >> std::ws >> i >> std::ws;
  if (!iss.eof()) {
    LOG(FATAL) << "Error could not convert the string '" << s << "' into unsigned long.";
  }
  return i;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::toDouble(std::string s)
{
  std::istringstream iss(s);
  double i;
  iss >> std::ws >> i >> std::ws;
  if (!iss.eof()) {
    LOG(FATAL) << "Error could not convert the string '" << s << "' into double.";
  }
  return i;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ConvertStringToDate(std::string s, tm & d)
{
  strptime(s.c_str(), "%Y-%m-%d %H:%M", &d);
  d.tm_sec = 0;
  d.tm_isdst = 0; // important, if not set, is random, and error can occur
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*
  Rename a mhd image, composed of 2 files XXX.mhd and XXX.raw. This
  function moves the two files and change the header to be linked with
  the renamed .raw file.
 */
void syd::RenameMHDImage(std::string old_path, std::string new_path, int verbose_level)
{
  // Check if not the same
  if (old_path == new_path) return; // do nothing

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
  if (!OFStandard::fileExists(old_path.c_str())) {
    LOG(FATAL) << "Rename MHD : Error path (mhd) not exist : " << old_path;
  }
  if (!OFStandard::fileExists(old_path_raw.c_str())) {
    LOG(FATAL) << "Rename MHD : Error path (raw) not exist : " << old_path_raw;
  }
  if (OFStandard::fileExists(new_path.c_str())) {
    LOG(FATAL) << "Rename MHD : Error path (mhd) to rename already exist : " << new_path;
  }
  if (OFStandard::fileExists(new_path_raw.c_str())) {
    LOG(FATAL) << "Rename MHD : Error path (raw) to rename already exist : " << new_path_raw;
  }

  // verbose
  VLOG(verbose_level) << "Rename header " << old_path << " to " << new_path;

  // header part
  // int result = rename(old_path.c_str(), new_path.c_str());
  // if (result != 0) {
  //   LOG(FATAL) << "Error while renaming " << old_path << " to " << new_path;
  // }

  // Change ElementDataFile in the header
  std::ifstream in(old_path);
  std::ofstream out(new_path);
  OFString r;
  OFStandard::getFilenameFromPath(r, old_path_raw.c_str());
  std::string wordToReplace(r.c_str());
  OFStandard::getFilenameFromPath(r, new_path_raw.c_str());
  std::string wordToReplaceWith(r.c_str());
  size_t len = wordToReplace.length();
  std::string line;
  while (std::getline(in, line)) {
    while (true) {
      size_t pos = line.find(wordToReplace);
      if (pos != std::string::npos)
        line.replace(pos, len, wordToReplaceWith);
      else
        break;
    }
    out << line << '\n';
  }
  in.close();
  out.close();

  // Delete old path
  OFStandard::deleteFile(old_path.c_str());

  // Rename .raw part
  VLOG(verbose_level) << "Rename raw " << old_path_raw << " to " << new_path_raw;
  int result = rename(old_path_raw.c_str(), new_path_raw.c_str());
  if (result != 0) {
    LOG(FATAL) << "Error while renaming " << old_path_raw << " to " << new_path_raw;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::DateDifferenceInHours(std::string end, std::string start)
{
  tm startDate;
  tm endDate;
  syd::ConvertStringToDate(start, startDate);
  syd::ConvertStringToDate(end, endDate);
  double v = difftime(mktime(&endDate), mktime(&startDate))/3600.0;
  return v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::IsBefore(std::string d1, std::string d2)
{
  return (DateDifferenceInHours(d1,d2) < 0);
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
