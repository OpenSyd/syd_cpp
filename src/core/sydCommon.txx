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

//--------------------------------------------------------------------
template<class ArgsInfoType>
void init_logging_verbose_options(ArgsInfoType & args_info)
{
  // this is needed to ensure that solver is SILENT
  google::InitGoogleLogging("");

  /*
  // Load main default configuration
  el::Configurations defaultConf;
  defaultConf.setToDefault();
  defaultConf.set(el::Level::Global, el::ConfigurationType::ToFile, "0");
  defaultConf.set(el::Level::Global, el::ConfigurationType::Filename, "/dev/null");

  // default date format
  std::string d = "<%datetime{%Y-%M-%d %h:%m:%s}>";

  // Verbose (level = Verbose)
  std::string enabled = "0";
  defaultConf.set(el::Level::Verbose,
                  el::ConfigurationType::Format, d+" [%vlevel] %msg");
  defaultConf.set(el::Level::Verbose,
                  el::ConfigurationType::MillisecondsWidth, "1");
  if (args_info.verbose_arg >= 0) enabled = "1";
  else enabled ="0";
  defaultConf.set(el::Level::Verbose, el::ConfigurationType::Enabled, enabled);
  el::Helpers::storage()->vRegistry()->setLevel(args_info.verbose_arg);

  // Verbose SQL (level = INFO)
  defaultConf.set(el::Level::Info,
                  el::ConfigurationType::Format, d+" SQL %msg");
  defaultConf.set(el::Level::Info,
                  el::ConfigurationType::MillisecondsWidth, "1");
  if (args_info.verboseSQL_flag) enabled = "1";
  else enabled ="0";
  defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, enabled);

  // Verbose warning (level = WARNING)
  defaultConf.set(el::Level::Warning,
                  el::ConfigurationType::Format, d+" WARNING %msg");
  defaultConf.set(el::Level::Warning,
                  el::ConfigurationType::MillisecondsWidth, "1");
  // if (args_info.verbose_arg < 0) enabled = "0";
  // else
  enabled ="1"; // always enable
  defaultConf.set(el::Level::Warning, el::ConfigurationType::Enabled, enabled);

  // Verbose FATAL (level = FATAL)
  defaultConf.set(el::Level::Fatal,
                  el::ConfigurationType::Format, "****** ERROR ****** \n"+d+" %msg");
  defaultConf.set(el::Level::Fatal,
                  el::ConfigurationType::MillisecondsWidth, "1");
  enabled = "1";
  defaultConf.set(el::Level::Fatal, el::ConfigurationType::Enabled, enabled);

  // Some flags
  // el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
  // el::Helpers::setCrashHandler(myCrashHandler);
  el::Loggers::addFlag( el::LoggingFlag::ColoredTerminalOutput );
  // Auto spacing between <<
  // el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);

  // default logger uses default configurations
  el::Loggers::reconfigureLogger("default", defaultConf);
  */
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Convert float, double ... to string
template<class T>
std::string ToString(const T & t)
{
  std::ostringstream myStream;
  myStream << t << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Convert array to string with 'x'
template<class T, int N>
std::string ArrayToString(const std::array<T, N> & t)
{
  std::ostringstream myStream;
  for(auto i=0; i<N-1; i++) myStream << t[i] << "x";
  myStream << t[N-1] << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------
