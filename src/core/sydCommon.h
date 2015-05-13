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

// google logging
#include <glog/logging.h>

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
#include "sydDicomCommon.h" // needed for helpers functions (FileExists)
#include "md5.h"

//--------------------------------------------------------------------
using namespace sydlog;

namespace syd {

  //--------------------------------------------------------------------
  // GGO with modified struct name
#define GGO(ggo_filename, args_info)                                    \
  args_info_##ggo_filename args_info;					\
  cmdline_parser_##ggo_filename##2(argc, argv, &args_info, 1, 1, 0);    \
  if (args_info.config_given)						\
    cmdline_parser_##ggo_filename##_configfile (args_info.config_arg, &args_info, 0, 0, 1); \
    else cmdline_parser_##ggo_filename(argc, argv, &args_info);

  //--------------------------------------------------------------------
  // Static declaration To be include in main
#define SYD_STATIC_INIT                                         \
  syd::DatabaseManager * syd::DatabaseManager::singleton_;      \
  syd::PluginManager * syd::PluginManager::singleton_;

  //--------------------------------------------------------------------
  // To init tools with GGO, log and help
#define SYD_INIT(G, N)                       \
  GGO(G, args_info);                         \
  Log::SQLFlag() = args_info.verboseSQL_flag;           \
  Log::LogLevel() = args_info.verbose_arg;              \
  if (args_info.inputs_num < N) {                       \
    cmdline_parser_##G##_print_help();           \
    LOG(FATAL) << "Please provide at least "#N" params";   \
  }

  //--------------------------------------------------------------------
  // Type for id in the db
  typedef unsigned int IdType;

  //--------------------------------------------------------------------
  // FIXME : to change
  static const double HalfLife_Indium_in_days = 2.8047;
  static const double Lambda_Indium_in_days = log(2.0)/HalfLife_Indium_in_days;
  static const double Lambda_Indium_in_hours = log(2.0)/(HalfLife_Indium_in_days*24.0);
  static const double Lambda_Indium_in_sec = log(2.0)/(HalfLife_Indium_in_days*3600.0);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void CreateDirectory(std::string folder);
  bool FileExists(std::string filename);
  bool DirExists(std::string folder);
  std::string GetExtension(const std::string filename);
  std::string GetFilenameFromPath(const std::string path);
  void ConvertToAbsolutePath(std::string & folder);
  bool GetWorkingDirectory(std::string & pwd);
  bool EqualFiles(std::ifstream & in1, std::ifstream & in2);
  bool EqualFiles(std::string in1, std::string in2);
  void CopyFile(std::string src, std::string dst);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  std::string ConvertDateTime(std::string date, std::string time);
  void ConvertStringToDate(std::string s, tm & d);
  double DateDifferenceInHours(std::string end, std::string start);
  bool IsDateBefore(std::string d1, std::string d2);
  bool IsDigits(const std::string &str);

  /// A 'valid' date is something like "2015-04-01 10:00"
  bool IsDateValid(std::string d);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  unsigned long ToULong(std::string);
  double ToDouble(std::string);
  template<class T>
  std::string ToString(const T & t);
  template<class T, int N>
  std::string ArrayToString(const std::array<T, N> & t);
  void SkipComment(std::istream & is);
  bool Replace(std::string& str, const std::string& from, const std::string& to);
  void GetWords(const std::string & phrase, std::vector<std::string> & words);
  // trim from start
  std::string &ltrim(std::string &s);
  // trim from end
  std::string &rtrim(std::string &s);
  // trim from both ends
  std::string &trim(std::string &s);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void loadbar(unsigned int x, unsigned int n, unsigned int w=50);
  //--------------------------------------------------------------------

#include "sydCommon.txx"

} // end namespace

#endif /* end #define SYDCOMMON_H */
