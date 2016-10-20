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
#include "syd_test6_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"
#include "sydTimeIntegratedActivityImageFilter.h"
#include "sydTimeIntegratedActivityImageBuilder.h"
#include "sydRoiStatisticHelper.h"

void TestDouble(double a, double b);

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_test6, 0);
  LOG(WARNING) << "Need the test4 result";

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Database names
  std::string dbname = "test6.db";
  std::string folder = "data6";
  std::string ref_dbname = "test6_ref.db";
  std::string ref_folder = "data6_ref";

  // Get the database (copy the db3)
  auto db4 = m->Open<syd::StandardDatabase>("test4.db");
  db4->Copy(dbname, folder);
  std::cout << "Open " << dbname << " as StandardDatabase" << std::endl;
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(dbname);

  // type
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef unsigned char MaskPixelType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;

  // Get patient
  syd::Patient::pointer patient;
  db->QueryOne<syd::Patient>(patient, 1);
  std::cout << "Patient: " << patient << std::endl;
  syd::Injection::pointer injection;
  db->QueryOne<syd::Injection>(injection, 1);
  injection->date = "2013-02-12 10:16";
  injection->activity_in_MBq = 1.0;
  std::cout << "Injection: " << injection << std::endl;

  // Insert image
  syd::Image::vector images;
  images.push_back(syd::InsertImageFromFile("input/OT_248.mhd", patient));
  images.push_back(syd::InsertImageFromFile("input/OT_249.mhd", patient));
  images.push_back(syd::InsertImageFromFile("input/OT_250.mhd", patient));
  images.push_back(syd::InsertImageFromFile("input/OT_251.mhd", patient));
  images.push_back(syd::InsertImageFromFile("input/OT_252.mhd", patient));
  for(auto image:images) {
    image->injection = injection;
  }
  images[0]->acquisition_date = "2013-02-12 12:34";
  images[1]->acquisition_date = "2013-02-12 16:15";
  images[2]->acquisition_date = "2013-02-13 11:15";
  images[3]->acquisition_date = "2013-02-14 12:06";
  images[4]->acquisition_date = "2013-02-18 11:00";
  db->Update(images);
  auto mask = syd::InsertImageFromFile("input/OT_248_mask.mhd", patient);
  auto itk_mask = syd::ReadImage<MaskImageType>(mask->GetAbsolutePath());

  // Sort list of images by times
  db->Sort<syd::Image>(images);

  // Get itk images
  std::vector<ImageType::Pointer> itk_images;
  std::vector<double> times;
  for(auto image:images) {
    auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
    itk_images.push_back(itk_image);
    times.push_back(image->GetHoursFromInjection());
  }

  // Build options list
  syd::TimeIntegratedActivityFitOptions options;
  options.SetRestrictedFlag(false);
  options.SetR2MinThreshold(0.8);
  options.SetMaxNumIterations(50);
  options.SetAkaikeCriterion("AICc");
  options.AddModel("f3");
  options.AddModel("f4");
  // options.AddModel("f2");
  // options.AddModel("f4a");
  // options.AddModel("f4b");
  // options.AddModel("f4c");
  // options.AddTimeValue(0,0);
  // options.AddTimeValue(0,0); // FIXME

  // Test with builder
  syd::TimeIntegratedActivityImageBuilder builder;
  builder.SetInput(images);
  builder.SetImageActivityThreshold(15.0);
  builder.SetOptions(options);
  builder.SetDebugOutputFlag(true);
  auto tia = builder.Run();
  std::cout << "tia : " << tia << std::endl;
  auto outputs = tia->outputs;
  auto & filter = builder.GetFilter();

  // Test some pixels (f3)
  {
    std::cout << "Test1 : f3 " << std::endl;
    auto it = filter.GetIteratorAtPoint(-73, 15, 127);
    auto best_model_index = filter.FitOnePixel(it);
    auto tac = filter.GetCurrentTAC();
    auto wtac = filter.GetWorkingTAC();
    auto index = tac->size() - wtac->size();
    auto model = filter.GetModels()[best_model_index];
    std::cout << "Best model = " << model->GetName() << std::endl;
    std::cout << std::setprecision(10) << "lp = " << model->GetLambdaDecayConstantInHours() << std::endl;
    std::cout << std::setprecision(10) << "A1 = " << model->GetA(0) << std::endl;
    std::cout << std::setprecision(10) << "l1 = " << model->GetLambda(0) << std::endl;
    std::cout << std::setprecision(10) << "A2 = " << model->GetA(1) << std::endl;

    // Check
    TestDouble(model->Integrate(), 1465.940751);
    TestDouble(model->ComputeAUC(wtac, index), 1465.022425);
    TestDouble(model->GetA(0), -7.80824759);
    TestDouble(model->GetA(1), 16.09279155);
    TestDouble(model->GetLambda(0), 0.07017864907);
  }

  // Test some pixels (f3) restricted
  {
    std::cout << "Test2 : restricted " << std::endl;
    options.SetRestrictedFlag(true);
    filter.SetOptions(options);
    auto it = filter.GetIteratorAtPoint(-73, 15, 127);
    auto best_model_index = filter.FitOnePixel(it);
    auto tac = filter.GetCurrentTAC();
    auto wtac = filter.GetWorkingTAC();
    auto index = tac->size() - wtac->size();
    auto model = filter.GetModels()[best_model_index];
    std::cout << "Best model = " << model->GetName() << std::endl;
    std::cout << std::setprecision(10) << "lp = " << model->GetLambdaDecayConstantInHours() << std::endl;
    std::cout << std::setprecision(10) << "A1 = " << model->GetA(0) << std::endl;
    std::cout << std::setprecision(10) << "l1 = " << model->GetLambda(0) << std::endl;
    std::cout << std::setprecision(10) << "A2 = " << model->GetA(1) << std::endl;

    // Check
    TestDouble(model->Integrate(), 1567.287436);
    TestDouble(model->ComputeAUC(wtac, index), 1603.14764);
    TestDouble(model->GetA(0),-6.851719051);
    TestDouble(model->GetA(1),18.52429633);
    TestDouble(model->GetLambda(0),0.01925817373);
  }

  // Test some pixels (f4)
  {
    std::cout << "Test3 : f4 " << std::endl;
    auto it = filter.GetIteratorAtPoint(77, 53, 79);
    auto best_model_index = filter.FitOnePixel(it);
    auto tac = filter.GetCurrentTAC();
    auto wtac = filter.GetWorkingTAC();
    auto index = tac->size() - wtac->size();
    auto model = filter.GetModels()[best_model_index];
    std::cout << "Best model = " << model->GetName() << std::endl;
    std::cout << std::setprecision(10) << "lp = " << model->GetLambdaDecayConstantInHours() << std::endl;
    std::cout << std::setprecision(10) << "A1 = " << model->GetA(0) << std::endl;
    std::cout << std::setprecision(10) << "l1 = " << model->GetLambda(0) << std::endl;
    std::cout << std::setprecision(10) << "A2 = " << model->GetA(1) << std::endl;
    std::cout << std::setprecision(10) << "l2 = " << model->GetLambda(1) << std::endl;

    // Check
    TestDouble(model->Integrate(), 439.9174087);
    TestDouble(model->ComputeAUC(wtac, index), 439.6288894);
    TestDouble(model->GetA(0), 1.765629112);
    TestDouble(model->GetA(1), 6.012353489);
    TestDouble(model->GetLambda(0), 0.1914756474);
    TestDouble(model->GetLambda(1), 0.003648075848);
  }

  {
    // Add a mask
    // Insert a roi from a mhd file
    std::string filename = "input/liver_18.mhd";
    auto image = tia->images[0];
    auto roitype = syd::FindRoiType("liver", db);
    auto liver = syd::InsertRoiMaskImageFromFile(filename, image->patient, roitype);
    mask->frame_of_reference_uid = image->frame_of_reference_uid;
    mask->CopyDicomSeries(image);
    mask->acquisition_date = image->acquisition_date;
    syd::AddTag(liver->tags, image->tags);
    db->Update(liver);
    std::cout << "Mask: " << liver << std::endl;
  }

  {
    // Helpers function to use TIA on roi mask
    std::cout << "-------------------------------------" << std::endl
              << "Pixel-based TIA estimation" << std::endl;
    DD(tia);
    db->Insert(tia); // insert in the db
    auto mask = syd::FindOneRoiMaskImage(tia->images[0], "liver");
    DD(mask);
    std::cout << "TIA with mask and success mask";
    auto s1 = syd::NewRoiStatistic(tia, mask);
    DD(s1);
    std::cout << "TIA with mask only (false ; to compare)";
    auto s2 = syd::NewRoiStatistic(tia->GetOutput("auc"), mask);
    DD(s2);
    std::cout << "TIA without mask but using success mask";
    auto s3 = syd::NewRoiStatistic(tia, nullptr);
    DD(s3);

    db->Insert(s1);
    db->Insert(s2);
    db->Insert(s3);

    // Check already exist ? s1 and s2 will have the same image+mask, but
    // different results.
  }

  {
    // roi based estimation
    std::cout << "-------------------------------------" << std::endl
              << "Roi-based TIA estimation" << std::endl;
    auto mask = syd::FindOneRoiMaskImage(tia->images[0], "liver");
    DD(mask);
    syd::RoiStatistic::vector stats;
    for(auto image:tia->images) {
      auto stat = syd::NewRoiStatistic(image, mask);
      stats.push_back(stat);
      DD(stats.back());
    }
    db->Insert(stats);
    /* // IN PROGRESS
    auto tac = syd::NewTac(stats);
    DD(tac);
    // syd::TimeIntegratedActivityFitOptions options;
    // tia->SetToOptions(options);
    auto fittac = syd::NewFitTac(tac, tia->GetOptions());
    DD(fittac);
    */
  }

  std::cout << "Success." << std::endl;
  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void TestDouble(double a, double b)
{
  double diff = fabs(a-b);
  static double tiny = 0.0001;
  if (diff > tiny) {
    LOG(FATAL) << "Error compare: " << std::setprecision(10) << a << " " << b;
  }
}
// --------------------------------------------------------------------


// GNUPLOT TEST

// 2.3 8.94407 6.932034969
// 5.98333 10.7627 6.060602188
// 24.9833 11.0483 4.243060589
// 49.8333 9.51835 3.020119905
// 144.733 4.07888 0.7849646807
// # gp
// # f3(lp,A1,l1,A2,x) = A1*exp(-(lp+l1)*x) + A2*exp(-lp*x)
// # replot f3(lp,A1,l1,A2,x) w l

// # f4(lp,A1,l1,A2,l2,x) = A1*exp(-(lp+l1)*x) + A2*exp(-(lp+l2)*x)
// # replot f4(lp,A1,l1,A2,l2,x) w l
