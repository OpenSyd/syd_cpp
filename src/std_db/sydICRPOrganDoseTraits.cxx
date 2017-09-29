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
#include "sydICRPOrganDoseTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(ICRPOrganDose);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::ICRPOrganDose>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(target_fit_timepoints, syd::FitTimepoints);
  ADD_TABLE_FIELD(radionuclide, syd::Radionuclide);
  ADD_TABLE_FIELD(target_roitype, syd::RoiType);

  ADD_RO_FIELD(absorbed_dose_in_Gy, double);
  ADD_RO_FIELD(phantom_name, std::string);
  ADD_RO_FIELD(target_organ_name, std::string);

  ADD_TABLE_FIELD(history, syd::RecordHistory);

  // Vector of source FitTimepoints
  auto f_source_ftp = [](pointer p) -> std::string {
    std::ostringstream ss;
    for(auto s:p->sources_fit_timepoints) ss << s->id << " ";
    return ss.str();
  };
  AddField<std::string>("sources_ftp", f_source_ftp);

  // Vector of S coefficients
  auto f_scoef = [](pointer p) -> std::string {
    std::ostringstream ss;
    for(auto s:p->S_coefficients) ss << s << " ";
    return ss.str();
  };
  AddField<std::string>("S", f_scoef);

  // Vector of source organ names
  auto f_source_names = [](pointer p) -> std::string {
    std::ostringstream ss;
    for(auto s:p->source_organ_names) ss << s << " ";
    return ss.str();
  };
  AddField<std::string>("source_organs", f_source_names);

  // Vector of source roitype
  auto f_source_rt = [](pointer p) -> std::string {
    std::ostringstream ss;
    for(auto s:p->source_roitypes) ss << s->name << " ";
    return ss.str();
  };
  AddField<std::string>("sources_rt", f_source_rt);

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // Format lists
  field_format_map_["default"] =
    "id target_fit_timepoints.id[t_fit_tp] tags sources_ftp[s_fit_tp] absorbed_dose_in_Gy[dose] S phantom_name[ph] target_organ_name[target] source_organs[sources] target_roitype.name[target] sources_rt[sources]";

  field_format_map_["hist"] =
    "default history.insertion_date[insert] history.update_date[update]";
}
// --------------------------------------------------------------------

