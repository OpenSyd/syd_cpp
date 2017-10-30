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
#include "sydICRPOrganDoseHelper.h"
#include "sydRoiMaskImageHelper.h"
#include "../levenshtein-sse/levenshtein-sse.hpp"

// --------------------------------------------------------------------
syd::ICRPOrganDose::pointer
syd::NewICRPOrganDose(syd::SCoefficientCalculator::pointer c,
                      syd::FitTimepoints::pointer target_ft,
                      std::string target_name,
                      syd::FitTimepoints::vector source_fts,
                      std::vector<std::string> source_names,
                      bool scale_mass)
{
  // Get roitimepoints
  auto rtp = std::dynamic_pointer_cast<syd::RoiTimepoints>(target_ft->timepoints);

  // check
  if (source_names.size() != source_fts.size()) {
    DDS(source_fts);
    DDS(source_names);
    EXCEPTION("Error in NewICRPOrganDose, source_fts must have the same size than source_names.");
  }

  // Get the rad
  auto tp = target_ft->timepoints;
  auto rad = tp->injection->radionuclide;
  auto rad_name = rad->name;

  // Setup the calculator
  c->SetTargetOrgan(target_name);
  c->SetRadionuclide(rad_name);

  // Get the CT image for the mass
  auto db = target_ft->GetDatabase<syd::StandardDatabase>();
  auto frame_of_reference_uid = rtp->roi_statistics[0]->image->frame_of_reference_uid;
  odb::query<syd::Image> q =
    syd::QueryImageModality("CT") and
    odb::query<syd::Image>::frame_of_reference_uid == frame_of_reference_uid;
  syd::Image::vector images;
  db->Query<syd::Image>(images, q);
  if (images.size() == 0) {
    EXCEPTION("Cannot find corresponding CT image (to compute the mass)");
  }
  if (images.size() > 1) {
    std::ostringstream ss;
    for(auto im:images) ss << im << std::endl;
    std::string s = ss.str();
    s.pop_back();
    LOG(WARNING) << "Several CT were found. I consider the first one "
                 << std::endl << s;
  }
  auto ct = images[0];

  // Get the mass and indicate to the SCoefficientCalculator
  auto target_roistat = rtp->roi_statistics[0];
  auto roi_name = target_roistat->mask->roitype->name;

  int i=0;
  double dose = 0.0;
  std::vector<double> S;
  std::vector<std::string> curated_source_names;
  syd::FitTimepoints::vector curated_source_fts;
  for(auto source_name:source_names) {
    c->SetSourceOrgan(source_name);
    auto s = c->Run();
    if (s == 0) continue;
    curated_source_fts.push_back(source_fts[i]);
    curated_source_names.push_back(source_name);
    S.push_back(s);

    // Get the TIA in the source (with correct unit)
    auto auc = source_fts[i]->auc;
    auto unit = source_fts[i]->unit;
    double scaling_to_MB = 1.0;

    if (unit->name == "Bq.h") scaling_to_MB = 1.0/1000000.0;
    else {
      if (unit->name == "kBq.h") scaling_to_MB = 1.0/1000.0;
      else {
        if (unit->name == "MBq.h") scaling_to_MB = 1.0;
        else {
          LOG(WARNING) << "Unit of the FitTimepoints is unknown.";
        }
      }
    }
    auc = auc * scaling_to_MB;

    // Compute dose
    dose += auc*s;

    ++i;
  }

  // Mass scaling
  double mass_scaling = 1.0;
  double target_mass_kg = 0.0;
  if (scale_mass) {
    target_mass_kg = syd::ComputeMass(ct, roi_name);
    mass_scaling = c->GetTargetMassInKg()/target_mass_kg;
    dose *= mass_scaling;
  }

  // unity
  // S are in mGy/MBq.h
  // auc must be in MBq.h / kg
  dose = dose/1000.0;// to convert in Gy

  auto target_rtp = std::dynamic_pointer_cast<syd::RoiTimepoints>(target_ft->timepoints);
  if (!target_rtp or target_rtp->roi_statistics.size() == 0) {
    EXCEPTION("Cannot guess the (target) roitype for this FitTimepoint " << target_ft);
  }

  // Create a new ICRPOrganDose
  auto od = db->New<syd::ICRPOrganDose>();
  od->target_fit_timepoints = target_ft;
  od->sources_fit_timepoints = source_fts;
  od->radionuclide =  rad;
  od->S_coefficients = S;
  od->absorbed_dose_in_Gy = dose;
  od->phantom_name = c->GetPhantomName();
  od->target_organ_name = target_name;
  od->target_mass_in_kg = target_mass_kg;
  od->mass_scaling = mass_scaling;
  od->target_roitype = target_rtp->roi_statistics[0]->mask->roitype;
  for(auto ft:curated_source_fts) {
    auto source_rtp = std::dynamic_pointer_cast<syd::RoiTimepoints>(ft->timepoints);
    if (!source_rtp or source_rtp->roi_statistics.size() == 0) {
      EXCEPTION("Cannot guess the (target) roitype for this FitTimepoint " << ft);
    }
    od->source_roitypes.push_back(source_rtp->roi_statistics[0]->mask->roitype);
  }
  // source_roitypes;
  od->source_organ_names = curated_source_names;
  return od;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ICRPOrganDose::pointer
syd::NewICRPOrganDose(syd::SCoefficientCalculator::pointer c,
                      syd::FitTimepoints::pointer target_ft,
                      syd::FitTimepoints::vector source_fts,
                      bool scale_mass)
{
  // Guess target ROI name
  auto target_name = syd::GuessTargetRoiName(c, target_ft->timepoints);

  // Guess target ROI name
  std::vector<std::string> source_names;
  for(auto ft:source_fts) {
    auto t = syd::GuessSourceRoiName(c, ft->timepoints);
    source_names.push_back(t);
  }

  return NewICRPOrganDose(c, target_ft, target_name, source_fts, source_names, scale_mass);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GuessTargetRoiName(syd::SCoefficientCalculator::pointer c,
                                    syd::Timepoints::pointer tp)
{
  auto rtp = std::dynamic_pointer_cast<syd::RoiTimepoints>(tp);
  if (!rtp) {
    EXCEPTION("Cannot guess the (target) roi for this Timepoint " << tp);
  }
  if (rtp->roi_statistics.size() == 0) {
    EXCEPTION("Cannot guess the (target) roi for this Timepoint " << tp
              << " because it has no roi_statistics.");
  }
  auto roi_name = rtp->roi_statistics[0]->mask->roitype->name;
  auto target_names = c->GetListOfTargetOrgans();
  std::size_t mind = target_names.size()-2;
  std::string mint = "OrganNotFound";
  for(auto t:target_names) {
    auto d = levenshteinSSE::levenshtein(roi_name, t);
    if (d<mind) {
      mind = d;
      mint = t;
    }
  }
  return mint;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GuessSourceRoiName(syd::SCoefficientCalculator::pointer c,
                                    syd::Timepoints::pointer tp)
{
  auto rtp = std::dynamic_pointer_cast<syd::RoiTimepoints>(tp);
  if (!rtp) {
    EXCEPTION("Cannot guess the (source) roi for this Timepoint " << tp);
  }

  auto roi_name = rtp->roi_statistics[0]->mask->roitype->name;
  auto source_names = c->GetListOfSourceOrgans();
  std::size_t mind = source_names.size()-2;
  std::string mint = "OrganNotFound";
  for(auto t:source_names) {
    auto d = levenshteinSSE::levenshtein(roi_name, t);
    if (d<mind) {
      mind = d;
      mint = t;
    }
  }
  return mint;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::GetICRPNamesFromComments(const syd::FitTimepoints::vector & ftps,
                                   syd::FitTimepoints::vector & tftps,
                                   std::vector<std::string> & target_names,
                                   syd::FitTimepoints::vector & sftps,
                                   std::vector<std::string> & source_names)
{
  tftps.clear();
  sftps.clear();
  target_names.clear();
  source_names.clear();
  for(auto tp:ftps) {
    auto com = tp->timepoints->comments;
    auto target = GetAssociatedTargetName(com);
    if (target != "") {
      target_names.push_back(target);
      tftps.push_back(tp);
    }
    auto source = GetAssociatedSourceName(com);
    if (source != "") {
      source_names.push_back(source);
      sftps.push_back(tp);
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetAssociatedSourceName(std::vector<std::string> & com, std::string name)
{
  syd::RemoveAssociatedSourceName(com);
  std::ostringstream ss;
  ss << AssociatedSourceNameComment << name;
  com.push_back(ss.str());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetAssociatedTargetName(std::vector<std::string> & com, std::string name)
{
  syd::RemoveAssociatedTargetName(com);
  std::ostringstream ss;
  ss << AssociatedTargetNameComment << name;
  com.push_back(ss.str());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RemoveAssociatedSourceName(std::vector<std::string> & com)
{
  std::vector<std::string> temp;
  for(auto c:com) {
    auto it = c.find(AssociatedSourceNameComment);
    if (it == std::string::npos) temp.push_back(c);
  }
  com = temp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RemoveAssociatedTargetName(std::vector<std::string> & com)
{
  std::vector<std::string> temp;
  for(auto c:com) {
    auto it = c.find(AssociatedTargetNameComment);
    if (it == std::string::npos) temp.push_back(c);
  }
  com = temp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetAssociatedSourceName(std::vector<std::string> & com)
{
  std::string name = "";
  for(auto c:com) {
    auto it = c.find(AssociatedSourceNameComment);
    if (it == std::string::npos) continue;
    name = c.substr(it+AssociatedSourceNameComment.size(), c.size());
  }
  return name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetAssociatedTargetName(std::vector<std::string> & com)
{
  std::string name = "";
  for(auto c:com) {
    auto it = c.find(AssociatedTargetNameComment);
    if (it == std::string::npos) continue;
    name = c.substr(it+AssociatedTargetNameComment.size(), c.size());
  }
  return name;
}
// --------------------------------------------------------------------

