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
#include "sydRoiTimepointsTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(RoiTimepoints);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::RoiTimepoints>::
BuildFields(const syd::Database * db) const
{
  // Retrive fields from Timepoints (inherit)
  auto map = syd::RecordTraits<syd::Timepoints>::GetTraits()->GetFieldsMap(db);
  for(auto & m:map) field_map_[m.first] = m.second->Copy();

  // For the moment only the first roi_stat
  auto f = [](pointer p) -> syd::RoiStatistic::pointer {
    if (p->roi_statistics.size() == 0) return nullptr;
    return p->roi_statistics[0]; };
  AddTableField<syd::RoiStatistic>("roi_statistic", f, "roi");

  // Format lists
  field_format_map_["default"] =
    "id patient.name[pat] injection.radionuclide.name[rad] injection.id[inj] times values unit.name[unit] std_deviations[std] comments roi_statistic.mask.roitype.name[roi] tags";
}
// --------------------------------------------------------------------

