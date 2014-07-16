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
#include "sydCommon.h"

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
void syd::SQLAddValue(std::string & r, std::string col, std::string val, bool first)
{
  int n = r.find(")");
  if (n == std::string::npos) {
    std::cerr << "Error in request " << r << std::endl;
    exit(0);
  }
  if (first) r.insert(n, col);
  else r.insert(n, ", "+col);
  n = r.find_last_of(")");
  if (n == std::string::npos) {
    std::cerr << "Error in request " << r << std::endl;
    exit(0);
  }
  if (first) r.insert(n, val);
  else r.insert(n, ", '"+val+"'");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SQLAddFieldValue(std::string & r, std::string col, std::string val, bool first)
{
  if (!first) r+=", ";
  r += col + " = '" + val + "'";
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::PrepareQuery(sqlite3 * db, std::ostringstream & oss, sqlite3_stmt **ppStmt, bool verboseQuery)
{
  const char *pzTail;   //  OUT: Pointer to unused portion of zSql
  int r = sqlite3_prepare_v2(db, oss.str().c_str(), -1, ppStmt, &pzTail);
  if (r!= SQLITE_OK) FATAL(std::endl << "Error in the sql request '" << oss.str()
                           << "'. The error is : " << sqlite3_errmsg(db) << std::endl);
  if (verboseQuery) { std::cout << "SQL " << oss.str() << std::endl; }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::UpdateQuery(sqlite3 * db, std::ostringstream & oss, bool verbose)
{
  sqlite3_stmt *ppStmt;
  syd::PrepareQuery(db, oss, &ppStmt);
  int r = sqlite3_step(ppStmt);
  if (r != SQLITE_DONE)
    FATAL(std::endl << "Error in the sql request '" << oss.str()
          << "'. The error is : " << sqlite3_errmsg(db) << std::endl);
  if (verbose) { std::cout << "SQL " << oss.str() << std::endl; }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ConvertStringToDate(std::string s, tm & d)
{
  sscanf(s.c_str(),"%4d-%2d-%2d %2d:%2d",
         &d.tm_year,&d.tm_mon,&d.tm_mday,
         &d.tm_hour,&d.tm_min);
  d.tm_sec = 0;
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
