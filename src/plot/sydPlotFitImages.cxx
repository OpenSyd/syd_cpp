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
#include "sydPlotFitImages_ggo.h"
#include "sydPyPlotBuilder.h" // (must be first)
#include "sydCommonGengetopt.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"
#include "sydFitModels.h"
#include "sydFitImagesHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydPlotFitImages, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get FitImages
  syd::IdType id = atoi(args_info.inputs[0]);
  auto fitimages = db->QueryOne<syd::FitImages>(id);
  DD(fitimages);

  // Get images and pixel
  auto images = fitimages->images;
  std::vector<double> p;
  p.push_back(args_info.pixel_arg[0]);
  p.push_back(args_info.pixel_arg[1]);
  p.push_back(args_info.pixel_arg[2]);
  DDS(p);

  // Create timepoints and associated fit
  auto ftp = syd::NewFitTimepointsAtPixel(fitimages, p);
  DD(ftp);
  auto tp = ftp->timepoints;
  DD(tp);

  // Create PyPlotBuilder to display curves
  syd::PyPlotBuilder builder;
  std::stringstream label;
  label << tp->patient->name << " "
        << tp->injection->radionuclide->name;

  // Curve 1: timepoints
  builder.AddCurve(tp->times, tp->values, "-o", label.str());

  // Curve 2: Display curve fit from models
  if (ftp->iterations != 0) { // else, means that fit fails
    auto model = ftp->NewModel();
    DD(model);
    double last = tp->times[tp->times.size()-1];
    double first = last-tp->times[0];
    auto times = syd::arange<double>(0, last, first/100.0); // FIXME 100 = param
    std::vector<double> values;
    for(auto t:times) values.push_back(model->GetValue(t));
    DDS(values);
    builder.AddCurve(times, values, "-", label.str());//, "color=base_line.get_color()");
  }

  // axes labels
  builder.Add("plt.xlabel('Times from injection in hours')");
  builder.Add("plt.ylabel('Activity')");

  // Ending part
  if (args_info.pdf_given) builder.AddPdfOutput(args_info.pdf_arg);
  builder.AddEndPart();

  // Get and save output
  if (args_info.output_given) {
    auto o = builder.GetOutput();
    std::ofstream os(args_info.output_arg);
    os << o;
    os.close();
  }

  // Plot
  builder.Run();

  // ------------------------------------------------------------------
  // This is the end, my friend.
}
// --------------------------------------------------------------------
