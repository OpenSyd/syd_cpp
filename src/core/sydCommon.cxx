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
    LOG(FATAL) << "Error while creating the folder " << path;
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
std::string syd::GetDate(std::string date, std::string time)
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
    LOG(FATAL) << "Error could not convert the string '" << s << "' into unsigned long.";
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
    LOG(FATAL) << "Error could not convert the string '" << s << "' into double.";
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
bool syd::IsBefore(std::string d1, std::string d2)
{
  return (DateDifferenceInHours(d1,d2) < 0);
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
syd::PrintTable::PrintTable()
{
  Init();
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::AddColumn(std::string name, int w, int digit)
{
  headers.push_back(name);
  width.push_back(w);
  precision.push_back(digit);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Init()
{
  current_line = 0;
  current_column = 0;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTable & syd::PrintTable::operator<<(const double & value)
{
  if (values.size() == current_line) {
    std::vector<std::string> line(headers.size());
    values.push_back(line);
  }
  std::stringstream ss;
  ss << std::fixed << std::setprecision (precision[current_column]) << value;
  values[current_line][current_column] = ss.str();
  current_column++;
  if (current_column == headers.size()) {
    current_column = 0;
    current_line++;
  }
  return *this;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTable & syd::PrintTable::operator<<(const std::string & value)
{
  if (values.size() == current_line) {
    std::vector<std::string> line(headers.size());
    values.push_back(line);
  }
  values[current_line][current_column] = value;
  current_column++;
  if (current_column == headers.size()) {
    current_column = 0;
    current_line++;
  }
  return *this;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Print(std::ostream & out)
{
  for(auto i=0; i<headers.size(); i++) out << std::setw(width[i]) << headers[i];
  out << std::endl;
  for(auto i=0; i<values.size(); i++) {
    for(auto j=0; j<values[i].size(); j++) {
      out << std::setw(width[j]) << std::fixed << std::setprecision (precision[j]) << values[i][j];
    }
    out << std::endl;
  }
}
//------------------------------------------------------------------



//------------------------------------------------------------------
// https://www.ross.click/2011/02/creating-a-progress-bar-in-c-or-any-other-console-app/
// Process has done i out of n rounds,
// and we want a bar of width w and resolution r.
void syd::loadbar(unsigned int x, unsigned int n, unsigned int w)
{
  if ( (x != n) && (x % (n/100+1) != 0) ) return;

  float ratio  =  x/(float)n;
  int   c      =  ratio * w;

  std::cout << std::setw(3) << (int)(ratio*100) << "% [";
  for (int x=0; x<c; x++) std::cout << "=";
  for (int x=c; x<w; x++) std::cout << " ";
  std::cout << "]\r" << std::flush;
}
//------------------------------------------------------------------
