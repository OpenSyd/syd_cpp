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
#include "sydDD.h"

// gdcm
#include "gdcmDataSet.h"
#include "gdcmStringFilter.h"
#include "gdcmAttribute.h"

// sqlite
#include <sqlite3.h>

namespace syd {

  //--------------------------------------------------------------------
  // when everything goes wrong
#define WHEREAMI "[ " << __FILE__  << " ] line " << __LINE__
#define FATAL(a) { std::cerr << "ERROR in " << WHEREAMI << "."          \
                             << std::endl << a << std::endl; exit(0); }

  //--------------------------------------------------------------------
  // GGO with modified struct name
#define GGO(ggo_filename, args_info)                                    \
  args_info_##ggo_filename args_info;					\
  cmdline_parser_##ggo_filename##2(argc, argv, &args_info, 1, 1, 0);    \
  if (args_info.config_given)						\
    cmdline_parser_##ggo_filename##_configfile (args_info.config_arg, &args_info, 0, 0, 1); \
    else cmdline_parser_##ggo_filename(argc, argv, &args_info);

  void MakeDate(const std::string & date, const std::string & time, std::string & result);

  template<uint group, uint element>
  void ReadTagString(const gdcm::DataSet & ds, std::string & value);

  void ReadTagString(gdcm::StringFilter & sf, uint group, uint element, std::string & value);

  void ReadTagDouble(gdcm::StringFilter & sf, uint group, uint element, double& value);

  template<uint group, uint element, class T>
  void ReadTagFromSeq(const gdcm::DataSet & ds, uint seqgroup, uint seqelement, T & value, int n=1);

  void SQLAddValue(std::string & r, std::string col, std::string val, bool first=false);
  void SQLAddFieldValue(std::string & r, std::string col, std::string val, bool first=false);

  void PrepareQuery(sqlite3 * db, std::ostringstream & oss, sqlite3_stmt **ppStmt, bool verbose=false);
  void UpdateQuery(sqlite3 * db, std::ostringstream & oss, bool verbose);
  void ConvertStringToDate(std::string s, tm & d);
  double DateDifferenceInHours(std::string end, std::string start);

#include "sydCommon.txx"

}
