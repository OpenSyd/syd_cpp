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
#include "sydPlotFitResult_ggo.h"
#include "sydPyPlotBuilder.h" // (must be first)
#include "sydCommonGengetopt.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"
#include "sydFitModels.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydPlotFitResult, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of fitresult
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::FitResult::vector fitresults;
  db->Query(fitresults, ids);
  if (fitresults.size() == 0) {
    LOG(FATAL) << "No FitResult ids given. I do nothing.";
  }

  // Create PyPlotBuilder to display curves
  syd::PyPlotBuilder builder;

  // Loop on Fit results
  for(auto f:fitresults) {

    // get timepoints
    auto tp = f->timepoints;

    // Label
    std::stringstream label;
    label << f->id << " "
          << tp->id << " "
          << tp->patient->name << " "
          << (tp->mask != NULL ? tp->mask->roitype->name:"no_mask") << " "
          << tp->injection->radionuclide->name << " "
          << f->model_name
          << " auc="  << f->auc
          << " r2= " << f->r2 << " (" << f->iterations << ")";

    // Fit
    syd::FitModelBase * model=NULL;
    if (f->model_name == "f2") model = new syd::FitModel_f2;
    if (f->model_name == "f3") model = new syd::FitModel_f3;
    if (f->model_name == "f4a") model = new syd::FitModel_f4a;
    if (f->model_name == "f4b") model = new syd::FitModel_f4b;
    if (f->model_name == "f4") model = new syd::FitModel_f4;
    if (model == NULL) {
      LOG(FATAL) << "Model " << f->model_name << " unknown.";
    }
    model->SetParameters(f->params);
    model->SetLambdaPhysicHours(tp->injection->GetLambdaInHours());
    double auc = model->Integrate();

    // Scale
    if (!args_info.norm_flag) auc = 1.0;
    else auc = tp->values[0];
    for(auto & v:tp->values) v /= auc;

    // Timepoints
    builder.AddCurve(tp->times, tp->values, "-o", "", "linewidth=2");

    // Models
    double last = tp->times[tp->times.size()-1];
    double l = last-tp->times[0];
    auto times = syd::arange<double>(0, last, l/100.0); // FIXME param
    std::vector<double> values;
    for(auto t:times) values.push_back(model->GetValue(t)/auc);
    builder.AddCurve(times, values, "-", label.str(), "color=base_line.get_color()");
    delete model;
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
