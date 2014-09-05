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

// clitk
#include <clitkCommon.h>

// syd
#include "sydCommon.h"

// itk
#include <itksys/SystemTools.hxx>

// --------------------------------------------------------------------
void syd::MakeDate(const std::string & date, const std::string & time, std::string & result)
{
  result = date.substr(0,4)+"-"+date.substr(4,2)+"-"+date.substr(6,2)+" "
    +time.substr(0,2)+":"+time.substr(2,2);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ReadTagString(gdcm::StringFilter & sf, uint group, uint element, std::string & value)
{
  gdcm::Tag tag(group, element);
  value = sf.ToStringPair(tag).second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ReadTagDouble(gdcm::StringFilter & sf, uint group, uint element, double& value)
{
  std::string str;
  ReadTagString(sf, group, element, str);
  value = atof(str.c_str());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ConvertStringToDate(std::string s, tm & d)
{
  // sscanf(s.c_str(),"%4d-%2d-%2d %2d:%2d",
  //        &d.tm_year,&d.tm_mon,&d.tm_mday,
  //        &d.tm_hour,&d.tm_min);
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
  return difftime(mktime(&endDate), mktime(&startDate))/3600.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AbortIfFileNotExist(std::string file) {
  bool exist = itksys::SystemTools::FileExists(file.c_str());
  if (!exist) FATAL(std::endl << "The file '" << file << "' does not exist." << std::endl);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AbortIfFileNotExist(std::string path, std::string file) {
  AbortIfFileNotExist(path+std::string("/")+file);
}
// --------------------------------------------------------------------


bool syd::replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}
