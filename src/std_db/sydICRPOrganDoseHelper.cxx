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
#include "../levenshtein-sse/levenshtein-sse.hpp"

// --------------------------------------------------------------------
//syd::ICRPOrganDose::pointer
void
syd::NewICRPOrganDose(syd::SCoefficientCalculator::pointer c,
                      syd::FitTimepoints::pointer target_ft,
                      syd::FitTimepoints::vector source_fts)
{
  // Guess target ROI name
  auto target_name = syd::GuessTargetRoiName(c, target_ft->timepoints);

  // Guess target ROI name
  std::vector<std::string> source_names;
  for(auto ft:source_fts) {
    auto t = syd::GuessSourceRoiName(c, ft->timepoints);
    source_names.push_back(t);
  }

  // Get the rad
  auto tp = target_ft->timepoints;
  auto rad = tp->injection->radionuclide;
  auto rad_name = rad->name;

  // Setup the calculator
  c->SetTargetOrgan(target_name);
  c->SetRadionuclide(rad_name);

  int i=0;
  double dose = 0.0;
  for(auto source_name:source_names) {
    c->SetSourceOrgan(source_name);
    auto s = c->Run();

    // Get the TIA in the Source
    auto auc = source_fts[i]->auc;

    // Compute dose
    dose += auc*s;

    ++i;
  }
  DD(dose);
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
