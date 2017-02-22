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
#include "sydFitImagesTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(FitImages);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::FitImages>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(history, syd::RecordHistory);
  ADD_FIELD_A(min_activity, double, "mina");
  ADD_FIELD_A(nb_pixels, int, "pix");
  ADD_FIELD_A(nb_success_pixels, int, "success");

  auto f_imn = [](pointer a) -> std::string { return syd::ToString(a->images.size(),0); };
  AddField<std::string>("images_n", f_imn, "n");

  auto f_im = [](pointer a) -> std::string {
    std::ostringstream oss;
    for(auto im:a->images) oss << im->id << " ";
    auto s = oss.str();
    return trim(s); };
  AddField<std::string>("images", f_im, "im");

  auto f_out = [](pointer a) -> std::string { return a->GetOutputNames(); };
  AddField<std::string>("outputs", f_out, "out");

  auto f_rate = [](pointer a) -> std::string {
    double d = 100.0*(double)a->nb_success_pixels/(double)a->nb_pixels;
    auto s = syd::ToString(d, 2)+"%";
    return s; };
  AddField<std::string>("success_rate", f_rate, "%");

  // patient & injection (first)
  auto f_fim = [](pointer a) -> syd::Image::pointer {
    if (a->images.size() == 0) return nullptr; // should not be here
    return a->images[0]; };
  AddTableField<syd::Image>("first_image", f_fim);

  // comments
  auto f_c = [](pointer p) -> std::string { return p->GetAllComments(); };
  AddField<std::string>("comments", f_c, "com");

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // FitOptions
  ADD_FIELD(r2_min, double);
  ADD_FIELD_A(max_iteration, int, "mit");
  ADD_FIELD_A(restricted_tac, bool, "rest");
  auto f_mo = [](pointer p) -> std::string { return p->GetModelsName(); };
  AddField<std::string>("models", f_mo);
  ADD_FIELD_A(akaike_criterion, std::string, "ak");

  // Format lists
  field_format_map_["default"] =
    "id first_image.patient.name[pat] first_image.injection.radionuclide.name[rad] images models min_activity r2_min akaike_criterion restricted_tac max_iteration images_n success_rate outputs";
}
// --------------------------------------------------------------------

