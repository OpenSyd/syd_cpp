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
#include "sydPlotTimepoints_ggo.h"
#include "sydPyPlotBuilder.h" // (must be first)
#include "sydCommonGengetopt.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydPlotTimepoints, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of timepoints
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Timepoints::vector timepoints;
  db->Query(timepoints, ids);
  if (timepoints.size() == 0) {
    LOG(FATAL) << "No timepoints ids given. I do nothing.";
  }

  // Create PyPlotBuilder to display curves
  syd::PyPlotBuilder builder;
  for(auto tp:timepoints) {
    std::stringstream label;
    label << tp->id << " "
          << tp->patient->name << " "
          << (tp->mask != NULL ? tp->mask->roitype->name:"no_mask") << " "
          << tp->injection->radionuclide->name;
    builder.AddCurve(tp->times, tp->values, "-o", label.str());
  }

  // axes labels
  builder.Add("plt.xlabel('Times from injection in hours')");
  builder.Add("plt.ylabel('Activity')");

  // Plot
  if (args_info.pdf_given) builder.AddPdfOutput(args_info.pdf_arg);
  builder.AddEndPart();
  builder.Run();

  // Get and save output
  if (args_info.output_given) {
    auto o = builder.GetOutput();
    std::ofstream os(args_info.output_arg);
    os << o;
    os.close();
  }

  // ------------------------------------------------------------------
  // This is the end, my friend.
}
// --------------------------------------------------------------------
