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
#include "sydTIA.h"

// --------------------------------------------------------------------
syd::TIA::TIA():
  syd::Record(),
  syd::RecordWithHistory(),
  syd::RecordWithTags()
{
  min_activity = 0.0;
  r2_min = 0.9;
  max_iteration = 50;
  restricted_tac = false;
  akaike_criterion = "AIC";
  nb_pixels = 0;
  nb_success_pixels = 0;
  // FIXME additional points
  // FIXME post processes
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TIA::~TIA()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TIA::ToString() const
{
  std::stringstream ss;
  ss << id << " ";
  if (images.size() == 0) ss << " no images ";
  else ss << images[0]->GetPatientName() << " ";
  ss << images.size() << " imgs "
     << min_activity << " "
     << r2_min << " "
     << max_iteration << " "
     << (restricted_tac? "restricted":"non_restricted") << " "
     << models_name.size() << " mod ("
     << GetModelsName() << ") "
     << akaike_criterion << " "
     << GetAllComments() << " "
    // << GetOutputNames()
     << " pix: "
     << nb_success_pixels << "/" << nb_pixels;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TIA::GetOutputNames() const
{
  std::stringstream ss;
  for(auto i=0; i<outputs.size(); i++) {
    ss << outputs[i]->id << "(" << output_names[i] << ") ";
  }
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::TIA::GetModelsName() const
{
  std::stringstream ss;
  for(auto m:models_name) ss << m << " ";
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::TIA::DumpInTable(syd::PrintTable & ta) const
{
  auto format = ta.GetFormat();
  if (format == "default") DumpInTable_default(ta);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::TIA::DumpInTable_default(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", images[0]->GetPatientName());
  ta.Set("n", images.size());
  if (images.size() == 0) return;
  ta.Set("min", min_activity);
  ta.Set("R2min", r2_min, 3);
  ta.Set("rest", (restricted_tac? "Y":"N"));
  ta.Set("Ak", akaike_criterion);
  ta.Set("itm", max_iteration);
  ta.Set("models", GetModelsName());
  ta.Set("out", GetOutputNames(), 150);
  std::stringstream ss;
  ss << nb_success_pixels << "/" << nb_pixels
     << "(" << (double)nb_success_pixels/(double)nb_pixels*100.0 << "%)";
  ta.Set("res", ss.str());
  ta.Set("com", GetAllComments());
}
// --------------------------------------------------


// --------------------------------------------------
void syd::TIA::AddOutput(syd::Image::pointer output, std::string name)
{
  outputs.push_back(output);
  output_names.push_back(name);
}
// --------------------------------------------------


// --------------------------------------------------
syd::Image::pointer syd::TIA::GetOutput(std::string name)
{
  int i=0;
  for(auto & n:output_names) {
    if (n == name) return outputs[i];
    ++i;
  }
  return nullptr;
}
// --------------------------------------------------


