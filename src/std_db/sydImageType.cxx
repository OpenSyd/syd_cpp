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
#include "sydImageType.h"
#include "sydPixelUnitHelper.h"

syd::ImageType::vector syd::ImageType::all_types;

// --------------------------------------------------------------------
syd::ImageType::pointer syd::IdentifyImageType(syd::DicomSerie::pointer dicom)
{
  auto types = syd::ImageType::GetImageTypes();
  double best_value = 0.0;
  auto best_type = syd::ImageType::GetNotFoundImageType();
  // Consider all types and try to identify. Keep the best value
  // FIXME later --> warning if some test or not zero
  for(auto & type:types) {
    double v = type->Identify(dicom);
    if (v >= 1.0) {//}best_value) {
      best_value = v;
      // do a copy because the identify function may have set some option
      best_type = type->Clone();
    }
  }
  return best_type;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::ImageType()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::~ImageType()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::vector_const
syd::ImageType::GetImageTypes()
{
  if (all_types.size() != 0) return all_types;
  all_types.push_back(BuildImageType_NotFound()); // MUST be first
  all_types.push_back(BuildImageType_CT());
  all_types.push_back(BuildImageType_SPECT());
  all_types.push_back(BuildImageType_PlanarSPECT());
  all_types.push_back(BuildImageType_ProjectionSPECT());
  all_types.push_back(BuildImageType_AttMap());
  return all_types;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer
syd::ImageType::GetNotFoundImageType()
{
  return GetImageTypes()[0]; // first type os notfound
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ImageType::Identify(syd::DicomSerie::pointer dicom) const
{
  // call the specific function to identify the dicom
  return identifier(dicom);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ImageType::ToString() const
{
  std::ostringstream oss;
  oss << name  << " " << pixel_unit->name << " ";
  for(auto p:properties)
    oss << p.first << "(" << p.second << ") ";
  return oss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer syd::ImageType::Clone() const
{
  auto c = New();
  *c = *this;
  return c;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer syd::ImageType::New()
{
  syd::ImageType::pointer p = std::make_shared<syd::ImageType>();
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer syd::ImageType::BuildImageType_NotFound()
{
  auto p = syd::ImageType::New();
  p->name = "NotFound";
  auto f = [p](syd::DicomSerie::pointer dicom) -> double {
    auto db = dicom->GetDatabase<syd::StandardDatabase>();
    p->pixel_unit = syd::FindPixelUnit(db, "no_unit");
    return 0.0; };
  p->identifier = f;
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer syd::ImageType::BuildImageType_CT()
{
  auto p = syd::ImageType::New();
  p->name = "CT";
  auto f = [p](syd::DicomSerie::pointer dicom) -> double {
    auto db = dicom->GetDatabase<syd::StandardDatabase>();
    p->pixel_unit = syd::FindPixelUnit(db, "HU");
    if (dicom->dicom_modality != "CT") return 0.0;
    return 1.0;
  };
  p->identifier = f;
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// https://wiki.nci.nih.gov/display/CIP/Key+to+two-letter+Modality+Acronyms+in+DICOM
syd::ImageType::pointer syd::ImageType::BuildImageType_SPECT()
{
  auto p = syd::ImageType::New();
  p->name = "SPECT";
  auto f = [p](syd::DicomSerie::pointer dicom) -> double {
    auto db = dicom->GetDatabase<syd::StandardDatabase>();
    p->pixel_unit = syd::FindPixelUnit(db, "counts");
    // modality
    double v = 0.0;
    if (dicom->dicom_modality == "ST") v = 1.0; // Sure it is a SPECT
    if (dicom->dicom_modality == "OT") v = 0.5; // Maybe a SPECT
    if (dicom->dicom_modality == "NM") v = 0.5; // Maybe a SPECT
    if (v<0.5) return v; // Sure not a spect

    // CHeck properties
    p->properties.clear();
    auto d = dicom->dicom_description;
    size_t found;
    found = d.find("IRNC");
    if (found != std::string::npos) p->properties["AC"] = PropertiesValue::No;
    found = d.find("IRAC");
    if (found != std::string::npos) p->properties["AC"] = PropertiesValue::Yes;
    found = d.find("IRACSC");
    if (found != std::string::npos) p->properties["SC"] = PropertiesValue::Yes;
    found = d.find("IRACSCRR");
    if (found != std::string::npos) p->properties["RR"] = PropertiesValue::Yes;
    found = d.find("IRACRR");
    if (found != std::string::npos) p->properties["RR"] = PropertiesValue::Yes;
    found = d.find("TB_KNIT_NOAC");
    if (found != std::string::npos) p->properties["AC"] = PropertiesValue::No;
    found = d.find("TB_KNIT_AC");
    if (found != std::string::npos) p->properties["AC"] = PropertiesValue::Yes;
    found = d.find("TRANS-AC-S1");
    if (found != std::string::npos) p->properties["AC"] = PropertiesValue::Yes;
    found = d.find("SPECT_KNITTED");
    if (found != std::string::npos) v = 1.0; // We know it is a spect

    if (p->properties.size() > 0) v = 1.0; // Sure it is a SPECT
    return v;
  };
  p->identifier = f;
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer syd::ImageType::BuildImageType_PlanarSPECT()
{
  auto p = syd::ImageType::New();
  p->name = "PlanarSPECT";
  auto f = [p](syd::DicomSerie::pointer dicom) -> double {
    auto db = dicom->GetDatabase<syd::StandardDatabase>();
    p->pixel_unit = syd::FindPixelUnit(db, "counts");
    // modality
    double v = 0.0;
    if (dicom->dicom_modality == "ST") v = 1.0; // Sure it is a SPECT
    if (dicom->dicom_modality == "OT") v = 0.5; // Maybe a SPECT
    if (dicom->dicom_modality == "NM") v = 0.5; // Maybe a SPECT
    if (v<0.5) return v; // Sure not a planar spect

    // CHeck properties
    p->properties.clear();
    auto d = dicom->dicom_description;
    size_t found;
    found = d.find("ANT_EM\\POST_EM\\ANT_SC\\POST_SC");
    if (found != std::string::npos) p->properties["Planar4"] = PropertiesValue::Yes;

    found = d.find("ANTERIOR");
    if (found != std::string::npos) p->properties["Ant"] = PropertiesValue::Yes;

    found = d.find("POSTERIOR");
    if (found != std::string::npos) p->properties["Post"] = PropertiesValue::Yes;

    found = d.find("DIFFUSE");
    if (found != std::string::npos) p->properties["Scatter"] = PropertiesValue::Yes;

    if (p->properties.size() > 0) v = 1.0; // Sure it is a Planar_SPECT

    return v;
  };
  p->identifier = f;
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer syd::ImageType::BuildImageType_ProjectionSPECT()
{
  auto p = syd::ImageType::New();
  p->name = "ProjectionSPECT";
  auto f = [p](syd::DicomSerie::pointer dicom) -> double {
    auto db = dicom->GetDatabase<syd::StandardDatabase>();
    p->pixel_unit = syd::FindPixelUnit(db, "counts");
    // modality
    double v = 0.0;
    if (dicom->dicom_modality == "ST") v = 1.0; // Sure it is a SPECT
    if (dicom->dicom_modality == "OT") v = 0.5; // Maybe a SPECT
    if (dicom->dicom_modality == "NM") v = 0.5; // Maybe a SPECT
    if (v<0.5) return v; // Sure not a projection spect

    // CHeck properties
    p->properties.clear();
    auto d = dicom->dicom_description;
    size_t found;
    found = d.find("PROJECTION");
    if (found != std::string::npos) v = 1.0;
    return v;
  };
  p->identifier = f;
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageType::pointer syd::ImageType::BuildImageType_AttMap()
{
  auto p = syd::ImageType::New();
  p->name = "AttMap";
  auto f = [p](syd::DicomSerie::pointer dicom) -> double {
    auto db = dicom->GetDatabase<syd::StandardDatabase>();
    p->pixel_unit = syd::FindPixelUnit(db, "no_unit"); //FIXME
    // modality
    double v = 0.0;
    if (dicom->dicom_modality == "OT") v = 0.5; // Maybe an AttMap
    if (dicom->dicom_modality == "NM") v = 0.5; // Maybe an AttMap
    if (v<0.5) return v; // Sure not a AttMap

    // Check properties
    p->properties.clear();
    auto d = dicom->dicom_description;
    size_t found;
    found = d.find("ATT MAP");
    if (found != std::string::npos) v = 1.0;
    return v;
  };
  p->identifier = f;
  return p;
}
// --------------------------------------------------------------------


