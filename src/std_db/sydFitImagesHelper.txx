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

// --------------------------------------------------------------------
template<class ArgsInfo>
void syd::SetOptionsFromCommandLine(syd::TimeIntegratedActivityFitOptions & options,
                                 ArgsInfo & args_info)
{
  options.SetRestrictedFlag(args_info.restricted_tac_flag);
  options.SetR2MinThreshold(args_info.r2_min_arg);
  options.SetMaxNumIterations(args_info.iterations_arg);
  options.SetAkaikeCriterion(args_info.akaike_arg);
  options.SetFitVerboseFlag(args_info.fit_verbose_flag);

  // List the model names
  std::vector<std::string> model_names;
  for(auto i=0; i<args_info.model_given; i++)
    model_names.push_back(args_info.model_arg[i]);
  if (model_names.size() == 0)
    model_names.push_back("f4"); // default model
  for(auto m:model_names) options.AddModel(m);

  // Maybe Later
  // options.AddTimeValue(0,0);
  // options.AddTimeValue(0,0);
}
// --------------------------------------------------------------------
