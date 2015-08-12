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

#ifndef SYDCOMMONGENGETOPT_H
#define SYDCOMMONGENGETOPT_H

//--------------------------------------------------------------------
#define GGO2(ggo_filename, args_info)                                                                      \
  args_info_##ggo_filename args_info;                                                                      \
  cmdline_parser_##ggo_filename##_params args_params;                                                      \
  cmdline_parser_##ggo_filename##_params_init(&args_params);                                               \
  args_params.print_errors = 1;                                                                            \
  args_params.check_required = 0;                                                                          \
  args_params.override = 1;                                                                                \
  args_params.initialize = 1;                                                                              \
  if(0 != cmdline_parser_##ggo_filename##_ext(argc, argv, &args_info, &args_params) )                      \
    {                                                                                                      \
    std::cerr << "Error in cmdline_parser_" #ggo_filename "_ext" << std::endl;                             \
    exit(1);                                                                                               \
    }                                                                                                      \
  std::string configFile;                                                                                  \
  if(args_info.config_given)                                                                               \
    configFile = args_info.config_arg;                                                                     \
  cmdline_parser_##ggo_filename##_free(&args_info);                                                        \
  if (configFile != "")                                                                                    \
    {                                                                                                      \
    if(0 != cmdline_parser_##ggo_filename##_config_file (configFile.c_str(), &args_info, &args_params) )   \
      {                                                                                                    \
      std::cerr << "Error in cmdline_parser_" #ggo_filename "_config_file" << std::endl;                   \
      exit(1);                                                                                             \
      }                                                                                                    \
    args_params.initialize = 0;                                                                            \
    }                                                                                                      \
  args_params.check_required = 1;                                                                          \
  if(0 != cmdline_parser_##ggo_filename##_ext(argc, argv, &args_info, &args_params) )                      \
    {                                                                                                      \
    std::cerr << "Error in cmdline_parser_" #ggo_filename "_ext" << std::endl;                             \
    exit(1);                                                                                               \
    }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
#define SYD_INIT_GGO(ggo_filename, N)                                 \
  GGO2(ggo_filename, args_info);                                      \
  syd::Log::SQLFlag() = args_info.verboseSQL_flag;                    \
  syd::Log::LogLevel() = args_info.verbose_arg;                       \
  if (args_info.inputs_num < N) {                                     \
    cmdline_parser_##ggo_filename##_print_help();                     \
      sydLOG(syd::FATAL) << "Please provide at least "#N" params";    \
  }
//--------------------------------------------------------------------


#endif
