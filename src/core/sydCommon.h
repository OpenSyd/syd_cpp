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

#ifndef SYDCOMMON_H
#define SYDCOMMON_H

// std
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <numeric>

// To get current working directory
// http://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

// syd
#include "sydDD.h"
#include "sydLog.h"
#include "sydException.h"
#include "sydDicomCommon.h" // needed for helpers functions (FileExists)
#include "md5.h"

// boost
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
namespace fs = boost::filesystem;

//--------------------------------------------------------------------
namespace syd {

  //--------------------------------------------------------------------
  /// Type for id in the db
  typedef long unsigned int IdType;
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  /// Default empty value for string field.
  static const std::string empty_value = "-";
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  std::string ConvertDateTime(std::string date, std::string time);
  void ConvertStringToDate(std::string s, tm & d);
  double DateDifferenceInHours(std::string end, std::string start);
  bool IsDateBefore(std::string d1, std::string d2);
  bool IsDigits(const std::string &str);

  /// A 'valid' date is something like "2015-04-01 10:00"
  bool IsDateValid(std::string d);

  std::string Now();
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  unsigned long ToULong(std::string);
  double ToDouble(std::string);
  template<class T>
  std::string ToString(const T & t);
  template<class T>
  std::string ArrayToString(const std::vector<T> & t, int precision=1);
  void SkipComment(std::istream & is);
  bool Replace(std::string& str, const std::string& from, const std::string& to);
  void GetWords(std::vector<std::string> & words, const std::string & phrase);
  void SetWords(std::string & phrase, const std::vector<std::string> & words);
  // trim from start
  std::string &ltrim(std::string &s);
  // trim from end
  std::string &rtrim(std::string &s);
  // trim from both ends
  std::string &trim(std::string &s);
  std::string AddDoubleQuoteAround(const std::string & s);
  std::string ToLowerCase(const std::string & s);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void loadbar(unsigned int x, unsigned int n, unsigned int w=50);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void ReadIdsFromInputPipe(std::vector<syd::IdType> & ids);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void BoxCoxTransform(double & v, const double lambda);
  double Rescale(const double v,
                 const double inputMin,
                 const double inputMax,
                 const double outputMin,
                 const double outputMax);
  double ComputeMedian(std::vector<double> x);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  int ExecuteCommandLine(const std::string & cmd, int logLevel);
  //--------------------------------------------------------------------


  // --------------------------------------------------------------------
  // Test if a string is an integer
  bool IsInteger(const std::string & s);
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  unsigned long long GetVersionNumber(unsigned long long v, int n);
  std::string GetVersionAsString(unsigned long long v);
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  template<typename T>
  std::vector<T> arange(T start, T stop, T step = 1);
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  template <typename T, typename Compare>
  std::vector<std::size_t> sort_permutation(const std::vector<T>& vec,
                                            Compare compare);
  template <typename T>
  std::vector<T> apply_permutation(const std::vector<T>& vec,
                                   const std::vector<std::size_t>& p);
  // --------------------------------------------------------------------


#include "sydCommon.txx"

} // end namespace

#endif /* end #define SYDCOMMON_H */
