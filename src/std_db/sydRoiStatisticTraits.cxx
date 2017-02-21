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
#include "sydRoiStatisticTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(RoiStatistic);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::RoiStatistic>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(image, syd::Image);
  ADD_TABLE_FIELD(mask, syd::RoiMaskImage);
  ADD_TABLE_FIELD(history, syd::RecordHistory);

  ADD_FIELD(mean, double);
  ADD_FIELD(std_dev, double);
  ADD_FIELD(n, double);
  ADD_FIELD(min, double);
  ADD_FIELD(max, double);
  ADD_FIELD(sum, double);

  // comments
  auto f_c = [](pointer p) -> std::string { return p->GetAllComments(); };
  AddField<std::string>("comments", f_c, "com");

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // Format lists
  field_format_map_["default"] =
    "id image.patient.name mask.roitype.name image.id tags mean std_dev n min max sum";
  field_format_map_["hist"] =
    "id image.patient.name mask.roitype.name image.id tags mean std_dev n min max sum history.insertion_date history.update_date";
}
// --------------------------------------------------------------------

