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
#include "sydImageTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Image);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Image>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_TABLE_FIELD(patient, syd::Patient);
  ADD_TABLE_FIELD(injection, syd::Injection);
  ADD_TABLE_FIELD(pixel_unit, syd::PixelUnit);
  ADD_TABLE_FIELD(history, syd::RecordHistory);

  ADD_FIELD(type, std::string);
  ADD_FIELD_A(pixel_type, std::string, "pix");
  ADD_FIELD_A(frame_of_reference_uid, std::string, "fr");
  ADD_FIELD_A(acquisition_date, std::string, "date");
  ADD_FIELD_A(modality, std::string, "mod");
  //ADD_FIELD(dicoms, syd::DicomSerie::vector);

  // Read only fields
  ADD_RO_FIELD(dimension, unsigned short int);
  auto f_fn = [](pointer p) -> std::string { return p->GetAbsolutePath(); };
  AddField<std::string>("filename", f_fn, "file");
  auto f_size = [](pointer p) -> std::string { return p->SizeAsString(); };
  AddField<std::string>("size", f_size);
  auto f_spacing = [](pointer p) -> std::string { return p->SpacingAsString(); };
  AddField<std::string>("spacing", f_spacing, "sp");

  // dicoms
  auto f_dicoms = [](pointer p) -> std::string {
    if (p->dicoms.size() == 0) return empty_value;
    std::ostringstream oss;
    for(auto d:p->dicoms) oss << d->id << " ";
    auto s = oss.str();
    return syd::trim(s); };
  AddField<std::string>("dicoms", f_dicoms);

  // comments
  auto f_c = [](pointer p) -> std::string { return p->GetAllComments(); };
  AddField<std::string>("comments", f_c, "com");

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // Abbreviation
  field_map_["pat"] = db->NewField("Image", "patient.name", "pat");
  field_map_["rad"] = db->NewField("Image", "injection.radionuclide.name", "rad");

  // Format lists
  field_format_map_["short"] =
  "id pat acquisition_date tags rad modality";
  field_format_map_["default"] =
  "id pat acquisition_date tags rad injection.id modality size spacing dicoms comments";

}
// --------------------------------------------------------------------

