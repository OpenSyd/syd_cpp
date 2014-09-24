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

// syd
#include "sydDD.h"
#include "sydDicomCommon.h"

// easylogging
// The first macro is needed to prevent default log file to be created
#define _ELPP_NO_DEFAULT_LOG_FILE 1
#include "easylogging++.h"

// To get current working directory
// http://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

//--------------------------------------------------------------------
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


  //--------------------------------------------------------------------
  template<class ArgsInfoType>
  void init_logging_verbose_options(ArgsInfoType & args_info);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void CreateDirectory(std::string folder);
  void RenameMHDImage(std::string old_path, std::string new_path, int verbose_level=2);
  void DeleteMHDImage(std::string path);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  std::string GetDate(std::string date, std::string time);
  void ConvertStringToDate(std::string s, tm & d);
  double DateDifferenceInHours(std::string end, std::string start);
  bool IsBefore(std::string d1, std::string d2);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  unsigned long toULong(std::string);
  double toDouble(std::string);
  template<class T>
  std::string toString(const T & t);
  void SkipComment(std::istream & is);
  //--------------------------------------------------------------------


#include "sydCommon.txx"

} // end namespace

#endif /* end #define CLITKCOMMON_H */