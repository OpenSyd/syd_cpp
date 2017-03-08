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
  for(auto m:model_names) options.AddModel(m);
  //  options.AddTimeValue(0,0);
  // options.AddTimeValue(0,0);
}
// --------------------------------------------------------------------
