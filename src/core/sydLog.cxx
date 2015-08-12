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

#include "sydLog.h"

std::ostream * syd::Log::output = &(std::cout);

// --------------------------------------------------------------------
syd::Log::Log()
{
  syd::Log::fatalFlag = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::ostringstream& syd::Log::Get(int level)
{
  if (level == syd::FATAL) {
    syd::Log::fatalFlag = true;
    os << syd::fatalColor;
  }
  if (level == syd::WARNING) os << syd::warningColor;
  os << "[" << NowTime();
  if (level != syd::FATAL and level != syd::WARNING) os << " " << level << "] ";
  else os << "] ";
  if (level == syd::WARNING) os << "(Warning) ";
  if (level == syd::FATAL) os << "Fatal. ";
  return os;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Log::~Log()
{
  os << std::endl;
  (*syd::Log::output) << os.str().c_str() << syd::resetColor;
  syd::Log::output->flush(); // required
  if (fatalFlag) exit(1);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int& syd::Log::LogLevel()
{
  static int logLevel = 0; // default level
  return logLevel;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool& syd::Log::SQLFlag()
{
  static bool sqlFlag = false; // default
  return sqlFlag;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
std::string syd::NowTime()
{
  const int MAX_LEN = 200;
  char buffer[MAX_LEN];
  if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
                     "HH':'mm':'ss", buffer, MAX_LEN) == 0)
    return "Error in NowTime()";

  char result[100] = {0};
  static DWORD first = GetTickCount();
  //  std::sprintf(result, "%s.%02ld", buffer, (long)(GetTickCount() - first) % 1000);
  std::sprintf(result, "%s", buffer);
  return result;
}
#else
#include <sys/time.h>
std::string syd::NowTime()
{
  char buffer[11];
  time_t t;
  time(&t);
  tm r = {0};
  strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
  struct timeval tv;
  gettimeofday(&tv, 0);
  char result[100] = {0};
  //  std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
  std::sprintf(result, "%s", buffer);
  return result;
}
#endif //WIN32
// --------------------------------------------------------------------
