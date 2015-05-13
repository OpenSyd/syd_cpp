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

#ifndef SYDLOG_H
#define SYDLOG_H

/// --------------------------------------------------------
/// Adapted from : http://www.drdobbs.com/cpp/logging-in-c/201804215
/// --------------------------------------------------------

#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>

// Needed if include <glog/logging.h> before
#undef LOG

namespace sydlog {
  /// Must be negative to always being displayed
  static int FATAL = -666;
  static int WARNING = -555;
  static int SQL = -444;

  static const char * resetColor = "\x1b[0m";
  static const char * fatalColor = "\x1b[31m";
  static const char * warningColor = "\x1b[33m";
}

inline std::string NowTime();

// --------------------------------------------------------------------
class Log
{
public:
  Log();
  virtual ~Log();
  std::ostringstream& Get(int level);
public:
  static int& LogLevel();
  static bool& SQLFlag();
  static std::string ToString(int level);
  static int FromString(const std::string& level);
  bool fatalFlag;
  static std::ostream * output;
protected:
  std::ostringstream os;
private:
  Log(const Log&);
  Log& operator =(const Log&);
};
// --------------------------------------------------------------------


// ----------------------------------------------------------------------------
typedef Log FILELog;

#define FILE_LOG(level)                   \
  if (level > FILELog::LogLevel()) ;      \
  else Log().Get(level)

/// ----------------------------------------------------------------------------
/// Main LOG macros
#define LOG(level)                        \
  if (level > Log::LogLevel()) ;          \
  else Log().Get(level)

#define LOGSQL             \
  if (!Log::SQLFlag()) ;   \
  else Log().Get(0)

#endif /* end #define SYDLOG_H */
