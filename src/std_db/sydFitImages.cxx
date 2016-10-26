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
#include "sydFitImages.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::FitImages::FitImages():
  syd::Record(),
  syd::RecordWithHistory(),
  syd::RecordWithTags(),
  syd::FitOptions()
{
  min_activity = 0.0;
  nb_pixels = 0;
  nb_success_pixels = 0;
  // FIXME additional points
  // FIXME post processes
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitImages::~FitImages()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitImages::ToString() const
{
  std::stringstream ss;
  ss << id << " ";
  if (images.size() == 0) ss << " no images ";
  else ss << images[0]->GetPatientName() << " ";
  ss << images.size() << " imgs ["
     << syd::FitOptions::ToString() << "] "
     << GetAllComments() << " "
     << " pix: "
     << nb_success_pixels << "/" << nb_pixels;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::FitImages::GetOutputNames() const
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
void syd::FitImages::DumpInTable(syd::PrintTable & ta) const
{
  auto format = ta.GetFormat();
  if (format == "default") DumpInTable_default(ta);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitImages::DumpInTable_default(syd::PrintTable & ta) const
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
  std::stringstream ss;
  ss << nb_success_pixels << "/" << nb_pixels
     << std::setprecision(3)
     << "(" << (double)nb_success_pixels/(double)nb_pixels*100.0 << "%)";
  ta.Set("res", ss.str());
  ta.Set("out", GetOutputNames(), 150);
  std::stringstream sss;
  for(auto im:images) sss << im->id << " ";
  ta.Set("img", sss.str());
  auto c = GetAllComments();
  if (c.size() > 0) ta.Set("com", c);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitImages::AddOutput(syd::Image::pointer output, std::string name)
{
  outputs.push_back(output);
  output_names.push_back(name);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::FitImages::GetOutput(std::string name)
{
  int i=0;
  for(auto & n:output_names) {
    if (n == name) return outputs[i];
    ++i;
  }
  return nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitImages::Callback(odb::callback_event event,
                              odb::database & db) const
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);

  if (event == odb::callback_event::post_erase) {
    for(auto o:outputs) db.erase(o);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FitImages::Callback(odb::callback_event event,
                              odb::database & db)
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);
  if (event == odb::callback_event::post_erase) {
    for(auto o:outputs) db.erase(o);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedActivityFitOptions syd::FitImages::GetOptions() const
{
  auto options = syd::FitOptions::GetOptions();
  if (images.size() > 0)
    options.SetLambdaDecayConstantInHours(images[0]->injection->GetLambdaDecayConstantInHours());
  return options;
}
// --------------------------------------------------------------------
