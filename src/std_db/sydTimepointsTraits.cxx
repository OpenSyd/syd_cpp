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
#include "sydTimepointsTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Timepoints);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Timepoints>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(patient, syd::Patient);
  ADD_TABLE_FIELD(injection, syd::Injection);
  ADD_TABLE_FIELD(history, syd::RecordHistory);

  // times
  auto f_times = [](pointer p) -> std::string {
    std::ostringstream oss;
    for(auto t:p->times) oss << t << " ";
    auto s = oss.str();
    return syd::trim(s); };
  AddField<std::string>("times", f_times);

  // values
  auto f_values = [](pointer p) -> std::string {
    std::ostringstream oss;
    for(auto t:p->values) oss << t << " "; //FIXME how to deal with precision ?
    auto s = oss.str();
    return syd::trim(s); };
  AddField<std::string>("values", f_values);

  // std_deviations
  auto f_std_deviations = [](pointer p) -> std::string {
    std::ostringstream oss;
    for(auto t:p->std_deviations) oss << t << " ";
    auto s = oss.str();
    return syd::trim(s); };
  AddField<std::string>("std_deviations", f_std_deviations);

  // comments
  auto f_c = [](pointer p) -> std::string { return p->GetAllComments(); };
  AddField<std::string>("comments", f_c, "com");

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // md5
  ADD_FIELD(md5, std::string);

  // Format lists
  field_format_map_["default"] =
    "id patient.name[pat] injection.radionuclide.name[rad] injection.id[inj] times values std_deviations[std] comments[com]";
}
// --------------------------------------------------------------------

