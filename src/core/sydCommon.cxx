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
void syd::RenameMHDFileIfExist(std::string old_path, std::string new_path, int verbose_level)
{
  // Check if not the same
  if (old_path == new_path) return; // do nothing

  // Check extension olf filename
  size_t n = old_path.find_last_of(".");
  std::string extension = old_path.substr(n+1);
  if (extension != "mhd") {
    LOG(FATAL) << "Error the filename must have mhd as extension : " << old_path;
  }
  std::string old_path_raw = old_path.substr(0,n)+".raw";

  // Check extension new filename
  n = new_path.find_last_of(".");
  extension = new_path.substr(n+1);
  if (extension != "mhd") {
    LOG(FATAL) << "Error the new filename must have mhd as extension : " << new_path;
  }
  std::string new_path_raw = new_path.substr(0,n)+".raw";

  if (!OFStandard::fileExists(old_path.c_str())) {
    LOG(WARNING) << "Warning path not exist : " << old_path;
  }
  else {
    if (!OFStandard::fileExists(old_path_raw.c_str())) {
      LOG(WARNING) << "Warning path not exist : " << old_path_raw;
      return;
    }
    VLOG(verbose_level) << "Rename " << old_path << " to " << new_path;

    int result = rename(old_path.c_str(), new_path.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << old_path << " to " << new_path;
    }

    // Change ElementDataFile
    std::ifstream in(new_path);
    std::ofstream out(new_path+"TMP");
    OFString r;
    OFStandard::getFilenameFromPath(r, old_path_raw.c_str());
    DD(r);
    std::string wordToReplace(r.c_str());
    DD(wordToReplace);
    OFStandard::getFilenameFromPath(r, new_path_raw.c_str());
    std::string wordToReplaceWith(r.c_str());
    DD(wordToReplaceWith);
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
    result = rename(std::string(new_path+"TMP").c_str(), new_path.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << std::string(new_path+"TMP") << " to " << new_path;
    }

    VLOG(verbose_level) << "Rename " << old_path_raw << " to " << new_path_raw;
    result = rename(old_path_raw.c_str(), new_path_raw.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << old_path_raw << " to " << new_path_raw;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::DateDifferenceInHours(std::string end, std::string start)
{
  tm startDate;
  tm endDate;
  DD(start);
  DD(end);
  syd::ConvertStringToDate(start, startDate);
  syd::ConvertStringToDate(end, endDate);
  double v = difftime(mktime(&endDate), mktime(&startDate))/3600.0;
  DD(v);
  return v;
}
// --------------------------------------------------------------------
