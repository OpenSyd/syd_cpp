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

#ifndef SYDFILEUTILS_H
#define SYDFILEUTILS_H

// std
#include <string>

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

using namespace sydlog;

// FIXME --> to remove
#include "sydDicomCommon.h" // needed for helpers functions (FileExists)


// boost
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

//--------------------------------------------------------------------
namespace syd {

  // fs::create_directories(path);
  // fs::exists(path);

  std::string GetExtension(const std::string filename);
  std::string GetFilenameFromPath(const std::string path);
  void ConvertToAbsolutePath(std::string & folder);
  void RemoveBackPathSeparator(std::string & folder);
  std::string GetPathFromFilename(std::string & path);
  bool GetWorkingDirectory(std::string & pwd);
  bool EqualFiles(std::ifstream & in1, std::ifstream & in2);
  bool EqualFiles(std::string in1, std::string in2);
  void CopyFile(std::string src, std::string dst);
  std::string CreateTemporaryFile(const std::string & folder, const std::string & extension);
  std::string CreateTemporaryDirectory(const std::string & folder);


} // end namespace syd
//--------------------------------------------------------------------

#endif /* end #define SYDFILEUTILS_H */
