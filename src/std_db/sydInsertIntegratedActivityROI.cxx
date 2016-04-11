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
#include "sydInsertIntegratedActivityROI_ggo.h"
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
  SYD_INIT_GGO(sydInsertIntegratedActivityROI, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Check pixelvalueunit
  //  auto punit = db->FindPixelValueUnit(args_info.pixelunit_arg);

  // Get timepoint to integrate
  syd::IdType id = atoi(args_info.inputs[0]);
  DD(id);
  syd::TimePoints::pointer tp;
  db->QueryOne(tp, id);
  DD(tp);

  // Check
  if (tp->images.size() == 0) {
    LOG(FATAL) << "TimePoints not associated with an image. Abort";
  }
  if (tp->images[0]->dicoms.size() == 0) {
    LOG(FATAL) << "TimePoints image not associated with a dicom (needed for injection). Abort";
  }

  // Create some models
  std::vector<syd::FitModelBase*> models;
  auto f2  = new syd::FitModel_f2;
  auto f3  = new syd::FitModel_f3;
  auto f4a = new syd::FitModel_f4a;
  auto f4  = new syd::FitModel_f4;
  models.push_back(f2);
  models.push_back(f3);
  models.push_back(f4a);
  models.push_back(f4);

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
  // builder.AddOutputImage(iter);
  // builder.AddOutputImage(eff_half_life);
  // builder.AddOutputImage(nb_points);
  // nb_points->restricted_tac_flag_ = args_info.restricted_tac_flag;
  //  builder.AddOutputImage(lambda);

  // Options
  syd::Injection::pointer injection = tp->images[0]->dicoms[0]->injection;
  DD(injection);
  builder.SetLambdaPhysicHours(injection->GetLambdaInHours());
  builder.SetR2MinThreshold(args_info.r2_min_arg);
  builder.SetRestrictedTACFlag(args_info.restricted_tac_flag);

  // additional values
  syd::TimePoints::pointer tp2 = tp;
  if (args_info.add_time_given and args_info.add_value_given) {
    db->New(tp2);
    tp2 = tp; // copy
    DD(tp2);
    tp2->times.push_back(args_info.add_time_given);
    tp2->values.push_back(args_info.add_value_given);
    DD(tp2);
    db->Insert(tp2);
  }

  // Set input TAC
  syd::TimeActivityCurve tac;
  tp2->GetTAC(tac);
  DD(tac);

  // restricted
  syd::TimeActivityCurve restricted_tac = tac;
  unsigned int first_index = 0;
  if (args_info.restricted_tac_flag) {
    // Select only the end of the curve (min 2 points); FIXME ?
    first_index = tac.FindMaxIndex();
    first_index = std::min(first_index, tac.size()-3);
    for(auto i=first_index; i<tac.size(); i++)
      restricted_tac.AddValue(tac.GetTime(i), tac.GetValue(i));
  }
  DD(restricted_tac);
  builder.SetInputTAC(restricted_tac);

  // Set the models
  for(auto i=0; i<args_info.model_given; i++) {
    std::string n = args_info.model_arg[i];
    bool b = false;
    for(auto m:models) {
      if (m->GetName() == n) {
        builder.AddModel(m, i+1); // start model id at 1 (such that 0 means no model)
        b = true;
      }
    }
    if (!b) {
      std::string km;
      for(auto m:models) km += m->GetName()+" ";
      LOG(FATAL) << "Error the model '" << n << "' is not found. Known models are: " << km;
    }
  }
  if (args_info.model_given == 0) {
    LOG(FATAL) << "At least a model must be given (--model).";
  }

  // Go !
  builder.CreateIntegratedActivityInROI();
  DD("done");

  // output ROI
  syd::FitResult::pointer res;
  db->New(res);
  res->timepoints = tp2;
  res->auc = builder.GetOutput()->value;
  res->r2 = r2->value;
  auto mi = best_model->value;
  res->model_name = models[mi]->GetName();
  res->params = models[mi]->GetParameters();
  DDS(res->params);
  res->first_index = first_index;
  db->UpdateTagsFromCommandLine(res->tags, args_info);
  DD(res);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
