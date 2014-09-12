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
void InitVerboseOptions(ArgsInfoType & args_info)
{
  // Load main default configuration
  el::Configurations defaultConf;
  defaultConf.setToDefault();
  defaultConf.set(el::Level::Global, el::ConfigurationType::ToFile, "0");
  defaultConf.set(el::Level::Global, el::ConfigurationType::Filename, "/dev/null");

  // Verbose (level = Verbose)
  std::string enabled = "0";
  defaultConf.set(el::Level::Verbose,
                  el::ConfigurationType::Format, "%datetime [%vlevel] %msg");
  defaultConf.set(el::Level::Verbose,
                  el::ConfigurationType::MillisecondsWidth, "1");
  if (args_info.verbose_arg >= 0) enabled = "1";
  else enabled ="0";
  defaultConf.set(el::Level::Verbose, el::ConfigurationType::Enabled, enabled);
  el::Helpers::storage()->vRegistry()->setLevel(args_info.verbose_arg);

  // Verbose SQL (level = INFO)
  defaultConf.set(el::Level::Info,
                  el::ConfigurationType::Format, "%datetime SQL %msg");
  defaultConf.set(el::Level::Info,
                  el::ConfigurationType::MillisecondsWidth, "1");
  if (args_info.verboseSQL_flag) enabled = "1";
  else enabled ="0";
  defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, enabled);

  // Auto spacing between <<
  //  el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);
  // default logger uses default configurations
  el::Loggers::reconfigureLogger("default", defaultConf);
}
//--------------------------------------------------------------------
