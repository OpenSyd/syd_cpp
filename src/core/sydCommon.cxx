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

#include "sydCommon.h"

// --------------------------------------------------------------------
void syd::CreateDirectory(std::string path)
{
  // See http://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
  typedef struct stat Stat;
  Stat            st;
  int             status = 0;
  if (stat(path.c_str(), &st) != 0) {
    /* Directory does not exist. EEXIST for race condition */
    if (mkdir(path.c_str(), 0777) != 0 && errno != EEXIST)
      status = -1;
  }
  else {
    LOG(WARNING) << "The folder " << path << " already exist.";
  }
  if (status != 0) {
    EXCEPTION("Error while creating the folder " << path);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::FileExists(std::string filename)
{
  return OFStandard::fileExists(filename.c_str());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::DirExists(std::string folder)
{
  return OFStandard::dirExists(folder.c_str());
}
// --------------------------------------------------------------------


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
void syd::CopyFile(std::string src, std::string dst)
{
  std::ifstream isrc(src, std::ios::binary);
  std::ofstream idst(dst, std::ios::binary);
  idst << isrc.rdbuf();
}
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
    //dirPrefix = OFString(cCurrentPath);
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
  if (n == std::string::npos) {
    LOG(FATAL) << "Cannot get the path of '" << path << "' because not " << PATH_SEPARATOR << " was found.";
  }
  std::string p = path.substr(0,n);
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ConvertDateTime(std::string date, std::string time)
{
  std::string result;
  result= date.substr(0,4)+"-"+date.substr(4,2)+"-"+date.substr(6,2)+" "
    + time.substr(0,2)+":"+time.substr(2,2);
  return result;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
unsigned long syd::ToULong(std::string s)
{
  std::istringstream iss(s);
  unsigned long i;
  iss >> std::ws >> i >> std::ws;
  if (!iss.eof()) {
    EXCEPTION("Error could not convert the string '" << s << "' into unsigned long.");
  }
  return i;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ToDouble(std::string s)
{
  std::istringstream iss(s);
  double i;
  iss >> std::ws >> i >> std::ws;
  if (!iss.eof()) {
    EXCEPTION("Error could not convert the string '" << s << "' into double.");
  }
  return i;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ConvertStringToDate(std::string s, tm & d)
{
  strptime(s.c_str(), "%Y-%m-%d %H:%M", &d);
  d.tm_sec = 0;
  d.tm_isdst = 0; // important, if not set, is random, and error can occur
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start
std::string & syd::ltrim(std::string &s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
std::string & syd::rtrim(std::string &s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

// trim from both ends
std::string & syd::trim(std::string &s)
{
  return ltrim(rtrim(s));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::GetWords(const std::string & phrase, std::vector<std::string> & words) {
  std::istringstream iss(phrase); // consider all words in 'phrase'
  do {
    std::string s;
    iss >> s;
    s = trim(s);
    if (s != "") words.push_back(s);
  } while (iss);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::DateDifferenceInHours(std::string end, std::string start)
{
  tm startDate;
  tm endDate;
  syd::ConvertStringToDate(start, startDate);
  syd::ConvertStringToDate(end, endDate);
  double v = difftime(mktime(&endDate), mktime(&startDate))/3600.0;
  return v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::IsDateBefore(std::string d1, std::string d2)
{
  return (DateDifferenceInHours(d1,d2) < 0);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// http://stackoverflow.com/questions/8888748/how-to-check-if-given-c-string-or-char-contains-only-digits
bool syd::IsDigits(const std::string &str)
{
  return std::all_of(str.begin(), str.end(), ::isdigit);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::IsDateValid(std::string d)
{
  bool valid = true;
  if (d.size() != 16) return false;
  // Check "-" between y m d, then space, then ":"
  if (d[4] != '-') return false;
  if (d[7] != '-') return false;
  if (d[10] != ' ') return false;
  if (d[13] != ':') return false;
  // Check number
  if (!IsDigits(d.substr(0,4))) return false;
  if (!IsDigits(d.substr(5,2))) return false;
  if (!IsDigits(d.substr(8,2))) return false;
  if (!IsDigits(d.substr(11,2))) return false;
  if (!IsDigits(d.substr(14,2))) return false;
  // Ok, it is correct
  return true;
}
// --------------------------------------------------------------------


//------------------------------------------------------------------
// skip line which begin with a sharp '#'
void syd::SkipComment(std::istream & is)
{
  char c;
  char line[1024];
  if (is.eof()) return;
  is >> c ;
  while (is && (c == '#')) {
    is.getline (line, 1024);
    is >> c;
    if (is.eof()) return;
  }
  if (!(is.fail()) && c != '\n')
    is.unget();
}
//------------------------------------------------------------------


//------------------------------------------------------------------
//http://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
bool syd::Replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::GetExtension(const std::string filename) {
  size_t n = filename.find_last_of(".");
  return filename.substr(n+1);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
/// https://www.ross.click/2011/02/creating-a-progress-bar-in-c-or-any-other-console-app/
/// Current process is x out of n; w is the bar width in the screen
void syd::loadbar(unsigned int x, unsigned int n, unsigned int w)
{
  // First check the last value to break line.
  if (x == n-1) {
    std::cout << std::endl;
  }

  // Do nothing if not a %
  if ( (x != n) && (x % (n/100+1) != 0) ) return;

  float ratio  =  x/(float)n;
  int   c      =  ratio * w;

  std::cout << std::setw(3) << (int)(ratio*100) << "% [";
  for (int x=0; x<c; x++) std::cout << "=";
  for (int x=c; x<w; x++) std::cout << " ";
  std::cout << "]\r" << std::flush;
}
//------------------------------------------------------------------
