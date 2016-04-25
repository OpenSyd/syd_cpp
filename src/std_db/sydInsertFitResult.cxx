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
#include "sydInsertFitResult_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydIntegratedActivityImageBuilder.h"
#include "sydSubstituteRadionuclideImageBuilder.h"
#include "sydRoiStatisticBuilder.h"
#include "sydImageFillHoles.h"
#include "sydImageBuilder.h"
#include "sydImage_GaussianFilter.h"

// itk in syd source
#include <itkMedianWithMaskImageFilter.h>

// std
#include <algorithm>

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init ceres log
  SYD_CERES_STATIC_INIT;

  // Init
  SYD_INIT_GGO(sydInsertFitResult, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get timepoint to integrate
  syd::IdType id = atoi(args_info.inputs[0]);
  syd::Timepoints::pointer tp;
  db->QueryOne(tp, id);

  // Check
  if (tp->images.size() == 0) {
    LOG(FATAL) << "Timepoints not associated with an image. Abort";
  }
  if (tp->images[0]->dicoms.size() == 0) {
    LOG(FATAL) << "Timepoints image not associated with a dicom (needed for injection). Abort";
  }

  // Create main builder
  syd::IntegratedActivityImageBuilder builder;

  // Create some optional output types
  auto r2 = new syd::FitOutputImage_R2();
  auto best_model = new syd::FitOutputImage_Model();
  auto iter = new syd::FitOutputImage_Iteration();
  auto eff_half_life = new syd::FitOutputImage_EffHalfLife();
  auto nb_points = new syd::FitOutputImage_NbOfPointsForFit();
  auto lambda = new syd::FitOutputImage_Lambda();

  builder.AddOutputImage(r2);
  builder.AddOutputImage(best_model);
  builder.AddOutputImage(iter);

  // Options
  syd::Injection::pointer injection = tp->injection;
  builder.SetLambdaPhysicHours(injection->GetLambdaInHours());
  builder.SetR2MinThreshold(args_info.r2_min_arg);

  // additional values
  syd::Timepoints::pointer tp2 = tp;
  if (args_info.add_time_given and args_info.add_value_given) {
    db->New(tp2);
    tp2 = tp; // copy
    tp2->times.push_back(args_info.add_time_arg + tp->times.back());
    tp2->values.push_back(args_info.add_value_arg * tp->values.back());
    syd::Timepoints::pointer tp3;
    if (db->FindSameMD5<syd::Timepoints>(tp2, tp3)) {
      tp2 = tp3; // already exist, we retrieve it
      LOG(1) << "Retrieve existing Timepoints with additional point: "
             << tp3;
    }
    else {
      // does not exist, we insert
      db->Insert(tp2);
      LOG(1) << "Insert new Timepoints with one additional point: "
             << tp2;
    }
  }

  // Set input TAC
  syd::TimeActivityCurve::pointer tac = syd::TimeActivityCurve::New();
  tp2->GetTAC(*tac);
  DD(*tac);

  // Set the models
  std::vector<std::string> model_names;
  for(auto i=0; i<args_info.model_given; i++) model_names.push_back(args_info.model_arg[i]);
  builder.SetModels(model_names);
  auto models = builder.GetModels();
  if (models.size() == 0) {
    LOG(FATAL) << "Error, no models given. Use for example 'f3,f4a,f4,f4b'";
  }

  // Go !
  builder.SetRestrictedTACFlag(args_info.restricted_tac_flag);
  builder.CreateIntegratedActivity(tac);

  // output ROI
  syd::FitResult::pointer res;
  db->New(res);
  res->timepoints = tp2;

  auto mi = best_model->value-1; // because start at one
  if (mi != -1) {
    res->auc = builder.GetOutput()->value;
    res->r2 = r2->value;
    res->model_name = models[mi]->GetName();
    res->params = models[mi]->GetParameters();
    res->first_index = 0; :/ FIXME
    if (args_info.restricted_tac_flag) {
      DD("TODO RESTRICTED_TAC_FLAG");
    }
    res->iterations = iter->value;
    db->UpdateTagsFromCommandLine(res->tags, args_info);

    syd::FitResult::pointer temp;
    if (db->FindSameMD5<syd::FitResult>(res, temp)) {
      LOG(1) << "Same FitResult already exists, no modification"
             << std::endl << temp;
    }
    else {
      db->Insert(res);
      LOG(1) << "Insert " << res;
    }
  }
  else {
    LOG(WARNING) << "Fit do not converge.";
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
