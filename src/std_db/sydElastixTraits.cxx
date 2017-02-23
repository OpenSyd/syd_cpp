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
#include "sydElastixTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Elastix);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Elastix>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(fixed_image, syd::Image);
  ADD_TABLE_FIELD(moving_image, syd::Image);
  ADD_TABLE_FIELD(fixed_mask, syd::RoiMaskImage);
  ADD_TABLE_FIELD(moving_mask, syd::RoiMaskImage);
  ADD_TABLE_FIELD(history, syd::RecordHistory);
  ADD_TABLE_FIELD(config_file, syd::File);
  ADD_TABLE_FIELD(transform_file, syd::File);

  // comments
  auto f_c = [](pointer p) -> std::string { return p->GetAllComments(); };
  AddField<std::string>("comments", f_c, "com");

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // Format lists
  field_format_map_["default"] =
    "id fixed_image.patient.name[pat] "
    "fixed_image.id[fixed] moving_image.id[moving] "
    "fixed_mask.id[fmask] moving_mask.id[mmask] "
    "config_file.filename[config] transform_file.filename[transform] "
    "comments[com]";
  field_format_map_["hist"] =
    "default history.insertion_date[insert] history.update_date[update]";
}
// --------------------------------------------------------------------

