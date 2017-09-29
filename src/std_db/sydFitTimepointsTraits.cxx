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
#include "sydFitTimepointsTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(FitTimepoints);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::FitTimepoints>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(timepoints, syd::Timepoints);
  ADD_TABLE_FIELD(history, syd::RecordHistory);
  ADD_TABLE_FIELD(unit, syd::PixelUnit);

  ADD_RO_FIELD(auc, double);
  ADD_RO_FIELD(r2, double);
  ADD_RO_FIELD(model_name, std::string);
  ADD_RO_FIELD_A(first_index, int, "index");
  ADD_RO_FIELD(iterations, int);

  // params
  auto f = [](pointer p) -> std::string {
    std::ostringstream oss;
    for(auto param:p->params) oss << param << " "; //FIXME how to deal with precision ?
    auto s = oss.str();
    return trim(s); };
  AddField<std::string>("params", f);

  // FitOptions
  ADD_FIELD(r2_min, double);
  ADD_FIELD_A(max_iteration, int, "mit");
  ADD_FIELD_A(restricted_tac, bool, "rest");
  auto f_mo = [](pointer p) -> std::string { return p->GetModelsName(); };
  AddField<std::string>("models", f_mo);
  ADD_FIELD_A(akaike_criterion, std::string, "ak");

  // if timepoints is a RoiTimepoints, display the roi
  auto f_roi = [](pointer p) -> std::string {
    auto roitp = std::dynamic_pointer_cast<syd::RoiTimepoints>(p->timepoints);
    if (!roitp) return empty_value;
    if (roitp->roi_statistics.size() == 0) return empty_value;
    return roitp->roi_statistics[0]->mask->roitype->name; };
  AddField<std::string>("roi", f_roi);

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // Format lists
  field_format_map_["default"] =
    "id timepoints.id[tid] timepoints.patient.name[pat] timepoints.injection.radionuclide.name[rad] roi tags model_name[model] auc unit.name[unit] r2 first_index iterations[it] params r2_min akaike_criterion restricted_tac max_iteration models";

  field_format_map_["hist"] =
    "default history.insertion_date[insert] history.update_date[update]";
}
// --------------------------------------------------------------------

