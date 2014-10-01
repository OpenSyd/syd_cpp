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


//------------------------------------------------------------------
// skip line which begin with a sharp '#'
void syd::SkipComment(std::istream & is)
{
  char c;
  char line[1024];
  if (is.eof()) return;
  is >> c ;
  while (is && (c == '#')) {
    is.getline (line, 1024);
    is >> c;
    if (is.eof()) return;
  }
  if (!(is.fail()) && c != '\n')
    is.unget();
}
//------------------------------------------------------------------
