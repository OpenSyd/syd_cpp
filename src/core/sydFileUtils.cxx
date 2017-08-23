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
#include "sydFileUtils.h"
#include "sydException.h"
#include "sydLog.h"
using namespace sydlog;

// std
#include <set>

// --------------------------------------------------------------------
bool syd::GetWorkingDirectory(std::string & pwd) {
  char cCurrentPath[FILENAME_MAX];
  if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) return false;
  pwd = std::string(cCurrentPath);
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// http://stackoverflow.com/questions/6163611/compare-two-files
bool syd::EqualFiles(std::ifstream & in1, std::ifstream & in2)
{
  std::ifstream::pos_type size1, size2;
  size1 = in1.seekg(0, std::ifstream::end).tellg();
  in1.seekg(0, std::ifstream::beg);
  size2 = in2.seekg(0, std::ifstream::end).tellg();
  in2.seekg(0, std::ifstream::beg);
  if(size1 != size2) return false;

  static const size_t BLOCKSIZE = 4096;
  size_t remaining = size1;
  while(remaining) {
    char buffer1[BLOCKSIZE], buffer2[BLOCKSIZE];
    size_t size = std::min(BLOCKSIZE, remaining);
    in1.read(buffer1, size);
    in2.read(buffer2, size);
    if(0 != memcmp(buffer1, buffer2, size)) return false;
    remaining -= size;
  }
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::EqualFiles(std::string in1, std::string in2)
{
  std::ifstream fin1(in1, std::ios::binary);
  std::ifstream fin2(in2, std::ios::binary);
  return EqualFiles(fin1, fin2);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// void syd::CopyFile(std::string src, std::string dst)
// {
//   std::ifstream isrc(src, std::ios::binary);
//   std::ofstream idst(dst, std::ios::binary);
//   idst << isrc.rdbuf();
// }
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ConvertToAbsolutePath(std::string & folder)
{
  if (folder.empty()) {
    EXCEPTION("The 'foldername' is void. Abort.");
  }

  if (folder.at(0) != PATH_SEPARATOR)  { // the folder is not an absolute path
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))  {
      EXCEPTION("Error while trying to get current working dir.");
    }
    folder = std::string(cCurrentPath)+"/"+folder;
  }

  RemoveBackPathSeparator(folder);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RemoveBackPathSeparator(std::string & folder)
{
  // remove PATH_SEPARATOR at the end
  while (folder.back() == PATH_SEPARATOR) folder = folder.substr(0, folder.size()-1);
}
// --------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::GetFilenameFromPath(const std::string path) {
  if (path.back() == PATH_SEPARATOR) {
    LOG(FATAL) << "Cannot get the filename of '" << path << "' because it ends with a " << PATH_SEPARATOR;
  }
  size_t n = path.find_last_of(PATH_SEPARATOR);
  std::string p = path.substr(n+1);
  RemoveBackPathSeparator(p);
  return p;
}
//------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetPathFromFilename(std::string & path)
{
  if (path.back() == PATH_SEPARATOR) {
    LOG(FATAL) << "Cannot get the path of '" << path << "' because it ends with a " << PATH_SEPARATOR;
  }
  size_t n = path.find_last_of(PATH_SEPARATOR);
  if (n == std::string::npos) { // no separator
    //LOG(FATAL) << "Cannot get the path of '" << path << "' because not " << PATH_SEPARATOR << " was found.";
    std::ostringstream s;
    s << "." << PATH_SEPARATOR; // current path
    return s.str();
  }
  std::string p = path.substr(0,n);
  return p;
}
// --------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::GetExtension(const std::string filename) {
  size_t n = filename.find_last_of(".");
  return filename.substr(n+1);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::CreateTemporaryFile(const std::string & folder, const std::string & extension)
{
  char filename[512];
  sprintf(filename, "%s%c%s%s", folder.c_str(), PATH_SEPARATOR, "syd_temp_XXXXXX", extension.c_str());
  int fd = mkstemps(filename,extension.size());
  if (fd == -1) {
    LOG(FATAL) << "Could not create temporary file: " << filename;
  }
  return std::string(filename);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::CreateTemporaryDirectory(const std::string & folder)
{
  char foldername[512];
  sprintf(foldername, "%s%c%s", folder.c_str(), PATH_SEPARATOR, "syd_temp_XXXXXX");
  char * fd = mkdtemp(foldername);
  if (fd == NULL) {
    LOG(FATAL) << "Could not create temporary folder: " << foldername;
  }
  return std::string(fd);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
int syd::GetPageContent(char const *argv[], std::ostream & os) {
  //   http://stackoverflow.com/questions/27491288/read-http-file-content-to-string-with-boost-c
  LOG(4) << "Try to connect to " << argv[0] << argv[1];
  try {
    boost::asio::io_service io_service;
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[0], "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end) {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error)
      throw boost::system::system_error(error);
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << argv[1] << " HTTP/1.0\r\n";
    request_stream << "Host: " << argv[0] << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";
    // Send the request.
    boost::asio::write(socket, request);
    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");
    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
      std::cerr << "Invalid response\n";
      return 1;
    }
    if (status_code != 200) {
      std::cerr << "Response returned with status code " << status_code << "\n";
      return 1;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;

    while (std::getline(response_stream, header) && header != "\r")
      LOG(4) << header;

    LOG(4) << "Writing content data";
    // Write whatever content we already have to output.
    if (response.size() > 0) {
      os << &response;
    }

    // Read until EOF, writing data to output as we go.
    while (true) {
      size_t n = boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error);

      if (!error)
        {
          if (n)
            os << &response;
        }

      if (error == boost::asio::error::eof)
        break;

      if (error)
        throw boost::system::system_error(error);
    }
  }
  catch (std::exception &e) {
    std::cerr << "Exception, could not connect to " << argv[0] << " exception is : " << e.what() << "\n";
  }

  return 0;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::SearchAndAddFilesInFolder(std::vector<std::string> & files,
                                    std::string folder,
                                    bool recurse)
{
  if (recurse) {
    for (fs::recursive_directory_iterator end, dir(folder); dir != end; ++dir) {
      std::string s = dir->path().string();
      if (!fs::is_directory(s))
        files.push_back(dir->path().string());
    }
  }
  else {
    for ( fs::directory_iterator end, dir(folder); dir != end; ++dir ) {
      if (!fs::is_directory(dir->path()))
        files.push_back(dir->path().string());
    }
  }
}
//------------------------------------------------------------------


//------------------------------------------------------------------
// http://stackoverflow.com/questions/8593608/how-can-i-copy-a-directory-using-boost-filesystem
bool syd::copyDir(boost::filesystem::path const & source,
                  boost::filesystem::path const & destination)
{
  namespace fs = boost::filesystem;
  try
    {
      // Check whether the function call is valid
      if (!fs::exists(source) || !fs::is_directory(source)) {
        EXCEPTION("Source directory " << source.string()
                  << " does not exist or is not a directory." << '\n');
        return false;
      }
      // if (fs::exists(destination)) {
      //     EXCEPTION("Destination directory " << destination.string()
      //               << " already exists." << '\n'
      //       ;
      //     return false;
      //   }
      // Create the destination directory
      if (!fs::exists(destination)) {
        if (!fs::create_directory(destination)) {
          EXCEPTION("Unable to create destination directory "
                    << destination.string() << '\n');
          return false;
        }
      }
    }
  catch(fs::filesystem_error const & e) {
    EXCEPTION(e.what() << '\n');
    return false;
  }
  // Iterate through the source directory
  for(fs::directory_iterator file(source);
      file != fs::directory_iterator(); ++file) {
    try {
      fs::path current(file->path());
      if (fs::is_directory(current)) {
        // Found directory: Recursion
        if (!copyDir(current,destination / current.filename())) {
          return false;
        }
      }
      else{
        // Found file: Copy
        fs::copy_file(current, destination / current.filename(),
                      fs::copy_option::overwrite_if_exists);
      }
    }
    catch(fs::filesystem_error const & e) {
      std:: cerr << e.what() << '\n';
    }
  }
  return true;
}
// --------------------------------------------------------------------
