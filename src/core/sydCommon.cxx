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
#include "pstream.h"

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
void syd::GetWords(std::vector<std::string> & words, const std::string & phrase) {
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
void syd::SetWords(std::string & phrase, const std::vector<std::string> & words)
{
  for(auto & w:words) phrase += w+" ";
  if (words.size() > 1) phrase.pop_back(); // remove last " ";
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


// --------------------------------------------------------------------
std::string syd::Now()
{
  namespace pt = boost::posix_time;
  std::ostringstream msg;
  const pt::ptime now = pt::second_clock::local_time();
  pt::time_facet*const ff = new pt::time_facet("%Y-%m-%d %H:%M:%S");
  msg.imbue(std::locale(msg.getloc(),ff));
  msg << now;
  return msg.str();
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


//------------------------------------------------------------------
// Needed by ReadIdsFromInputPipe
class RedirectCinToConsole {
protected:
  std::ifstream m_console;
  std::streambuf *m_oldCin;
  bool m_success;

public:
  RedirectCinToConsole() :
    m_oldCin(0),
    m_success(false) {
    m_console.open("/dev/tty");
    // Winodws ?? std::filebuf f; f.open( "CONIN$", std::ios_base::in );
    // std::streambuf* save = std::cout.rdbuf(); std::cout.rdbuf( &theNewFileBuf );
    if (m_console.is_open()) {
      m_success = true;
      m_oldCin = std::cin.rdbuf(m_console.rdbuf());
    }
  }
  virtual ~RedirectCinToConsole() {
    if (m_oldCin) {
      std::cin.rdbuf(m_oldCin);
    }
    m_console.close();
  }
  operator bool () const { return m_success; }
};
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::ReadIdsFromInputPipe(std::vector<syd::IdType> & ids)
{
  // http://stackoverflow.com/questions/4542544/supporting-piping-a-useful-hello-world
  syd::IdType arg;
  if (!isatty(fileno(stdin))) {
    while (std::cin) {
      std::cin >> arg;
      if (std::cin) ids.push_back(arg);
    }
  }
  // need to set back cin to console
  // Dont work on osx (need Ctrl+D, after to end the cin input)
  // http://stackoverflow.com/questions/12164448/how-to-restore-stdcin-to-keyboard-after-using-pipe
  // Dont delete this pointer !
  RedirectCinToConsole * l_redirect = new RedirectCinToConsole;
  std::cin.clear();
}
//------------------------------------------------------------------


//------------------------------------------------------------------
int syd::ExecuteCommandLine(const std::string & cmd, int logLevel)
{
  const redi::pstreams::pmode mode = redi::pstreams::pstdout|redi::pstreams::pstderr;
  redi::ipstream child(cmd, mode);
  char buf[1024];
  std::streamsize n;
  bool finished[2] = { false, false };
  const std::string color = "\x1b[32m"; // green
  std::cout << color;
  bool error = false;
  while (!finished[0] || !finished[1]) {
    if (!finished[0]) {
      while ((n = child.err().readsome(buf, sizeof(buf))) > 0) {
        error = true;
        std::cerr << fatalColor;
        std::cerr.write(buf, n);
      }
      if (child.eof()) {
        finished[0] = true;
        if (!finished[1]) child.clear();
      }
    }

    if (!finished[1]) {
      while ((n = child.out().readsome(buf, sizeof(buf))) > 0) {
        if (Log::LogLevel() >= logLevel) std::cout.write(buf, n).flush(); // only print if level ok
      }
      if (child.eof()) {
        finished[1] = true;
        if (!finished[0]) child.clear();
      }
    }
  }
  std::cout << DD_RESET;
  if (error) return -1; // Error
  return 0; // ok
}
//------------------------------------------------------------------


//------------------------------------------------------------------
int syd::ExecuteCommandLineNoBlock(const std::string & cmd, int logLevel)
{
  DD(cmd);
  const redi::pstreams::pmode mode = redi::pstreams::pstdout|redi::pstreams::pstderr;
  redi::ipstream child(cmd, mode);
  DD("here");
  /*
  char buf[1024];
  std::streamsize n;
  bool finished[2] = { false, false };
  const std::string color = "\x1b[32m"; // green
  std::cout << color;
  bool error = false;
  while (!finished[0] || !finished[1]) {
    if (!finished[0]) {
      while ((n = child.err().readsome(buf, sizeof(buf))) > 0) {
        error = true;
        std::cerr << fatalColor;
        std::cerr.write(buf, n);
      }
      if (child.eof()) {
        finished[0] = true;
        if (!finished[1]) child.clear();
      }
    }

    if (!finished[1]) {
      while ((n = child.out().readsome(buf, sizeof(buf))) > 0) {
        if (Log::LogLevel() >= logLevel) std::cout.write(buf, n).flush(); // only print if level ok
      }
      if (child.eof()) {
        finished[1] = true;
        if (!finished[0]) child.clear();
      }
    }
  }
  std::cout << DD_RESET;
  if (error) return -1; // Error
  */
  return 0; // ok
}
//------------------------------------------------------------------


//------------------------------------------------------------------
// http://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int
bool syd::IsInteger(const std::string & s)
{
  if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
  char * p ;
  strtol(s.c_str(), &p, 10) ;
  return (*p == 0) ;
}
//------------------------------------------------------------------


// --------------------------------------------------------------------
unsigned long long syd::GetVersionNumber(unsigned long long v, int n)
{
  if (n == 0) return (v & 0xFF);
  else return GetVersionNumber(v / 0x100, n-1);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetVersionAsString(unsigned long long v)
{
  auto temp = v;
  int n=1;
  while (temp > 0xFF) { // how many levels ? (xx.yy.zz.ww)
    n++;
    temp = temp / 0x100;
  }
  std::ostringstream oss;
  oss << std::hex;
  for(auto i=n-1; i>=0; i--) oss << GetVersionNumber(v, i) << ".";
  std::string s = oss.str();
  s.pop_back(); // remove last "."
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::AddDoubleQuoteAround(const std::string & s)
{
  std::string a = "\""+s+"\""; // add the "" around the full name
  return a;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ToLowerCase(const std::string & s)
{
  std::string t = s;
  std::transform(t.begin(), t.end(), t.begin(), ::tolower);
  return t;
}
// --------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::BoxCoxTransform(double & x, const double lambda)
{
  if (lambda == 0) {
    if (x != 0) x = log(x);
  }
  else {
    double y = pow(x, lambda);
    x = (y-1.0)/lambda;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double syd::Rescale(const double v,
                    const double inputMin,
                    const double inputMax,
                    const double outputMin,
                    const double outputMax)
{
  return (v-inputMin)*(outputMax-outputMin)/(inputMax-inputMin) + outputMin;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
double syd::ComputeMedian(std::vector<double> values)
{
  auto N = values.size();
  double median;
  sort(values.begin(), values.end());
  if (N % 2 == 0) median = (values[N / 2 - 1] + values[N / 2]) / 2;
  else median = values[N/2];
  return median;
}
//--------------------------------------------------------------------
