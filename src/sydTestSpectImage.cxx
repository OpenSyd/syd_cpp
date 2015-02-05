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
#include "sydTestSpectImage_ggo.h"
#include "sydCommon.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydImage.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

using namespace syd;

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydTestSpectImage, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Image type
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;

  // Set the list of times
  std::vector<double> times;
  times.push_back(1.0);
  times.push_back(4.7);
  times.push_back(23.9);
  times.push_back(47.9);
  times.push_back(71.8);
  times.push_back(143.8);

  // Create the images
  ImageType::RegionType region;
  ImageType::SizeType size;
  size[0] = 3; size[1] = 3; size[2] = 3;
  region.SetSize(size);
  DD(region);
  std::vector<ImageType::Pointer> images(times.size());
  for(auto i=0; i<times.size(); i++) {
    images[i] = ImageType::New();
    images[i]->SetRegions(region);
    images[i]->Allocate();
  }
  ImageType::Pointer image_integral = ImageType::New();
  ImageType::Pointer image_A = ImageType::New();
  ImageType::Pointer image_lambda = ImageType::New();
  ImageType::Pointer image_First = ImageType::New();
  ImageType::Pointer image_Middle = ImageType::New();
  ImageType::Pointer image_Final = ImageType::New();
  image_integral->SetRegions(region); image_integral->Allocate();
  image_lambda->SetRegions(region); image_lambda->Allocate();
  image_A->SetRegions(region); image_A->Allocate();
  image_First->SetRegions(region); image_First->Allocate();
  image_Middle->SetRegions(region); image_Middle->Allocate();
  image_Final->SetRegions(region); image_Final->Allocate();
  DD("done");

  // Create image iterators
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  std::vector<IteratorType> iters;
  for(auto s:images) {
    IteratorType iter(s, s->GetLargestPossibleRegion());
    iters.push_back(iter);
  }
  DD("here");
  IteratorType iter_integral(image_integral, image_integral->GetLargestPossibleRegion());
  IteratorType iter_lambda(image_lambda, image_lambda->GetLargestPossibleRegion());
  IteratorType iter_A(image_A, image_A->GetLargestPossibleRegion());
  IteratorType iter_First(image_First, image_First->GetLargestPossibleRegion());
  IteratorType iter_Middle(image_Middle, image_Middle->GetLargestPossibleRegion());
  IteratorType iter_Final(image_Final, image_Final->GetLargestPossibleRegion());

  // Random seed
  std::srand(666); // same seed std::time(0)); // use current time as seed for random generator
  double range_A = args_info.maxA_arg - args_info.minA_arg;
  double min_A = args_info.minA_arg;
  DD(range_A);
  DD(min_A);
  double min_lambda = 1.0/args_info.minHL_arg;
  double range_lambda = 1.0/(args_info.maxHL_arg - args_info.minHL_arg);
  DD(min_lambda);
  DD(range_lambda);
  double range_noise = args_info.noise_arg;
  double min_noise = -range_noise/2.0;
  DD(min_noise);
  DD(range_noise);

  // Fill the images
  while (!iters[0].IsAtEnd()) {

    // Determine random parameters
    double A = ((double)std::rand()/(double)RAND_MAX)*range_A + min_A;
    double lambda = ((double)std::rand()/(double)RAND_MAX)*range_lambda + min_lambda;

    // Set values
    double integral = A/lambda;
    iter_lambda.Set(lambda);
    iter_A.Set(A);
    iter_integral.Set(integral);

   // integrate between a and b
    //double r = -A/lambda * (exp(-lambda*end) - exp(-lambda*start));
    double start = 0.0;
    double end = times[0];
    double temp_first = syd::IntegrateMonoExpo(A, lambda, start, end);
    start = times[0];
    end = times.back(); // last element
    double temp_middle = syd::IntegrateMonoExpo(A, lambda, start, end);
    start = 0.0;
    double temp_final = (A/lambda) - syd::IntegrateMonoExpo(A, lambda, start, end);
    iter_First.Set(temp_first);
    iter_Middle.Set(temp_middle);
    iter_Final.Set(temp_final);
    //DD(temp_first+temp_middle+temp_final - integral) ; => ok == zero
    /*    DD("----");
    for(auto i=0; i<times.size()-1; i++) {
      start = times[i];
      end = times[i+1];
      DD(syd::IntegrateMonoExpo(A, lambda, start, end));
    }
    */

    for(auto i=0; i<times.size(); i++) {
      double v = A * exp(-lambda * times[i]);
      //      std::cout << times[i] << " " << v << std::endl;
      //iters[i].Set(std::max(v,1.0)); // no below 0.0

      // Add additive noise
      v = v+((double)std::rand()/(double)RAND_MAX)*range_noise+min_noise;

      iters[i].Set(std::max(v,1.0)); // no below 0.0
    }

    // Next pixel for each spect and the output
    for(auto i=0; i<iters.size(); i++) ++iters[i];
    ++iter_integral;
    ++iter_A;
    ++iter_lambda;
    ++iter_First;
    ++iter_Middle;
    ++iter_Final;
  }


  // Write images
  for(auto i=0; i<images.size(); i++) syd::WriteImage<ImageType>(images[i], "test-spect"+toString(i)+".mhd");
  syd::WriteImage<ImageType>(image_A, "A.mhd");
  syd::WriteImage<ImageType>(image_lambda, "lambda.mhd");
  syd::WriteImage<ImageType>(image_integral, "integral.mhd");
  syd::WriteImage<ImageType>(image_First, "temp_first.mhd");
  syd::WriteImage<ImageType>(image_Middle, "temp_middle.mhd");
  syd::WriteImage<ImageType>(image_Final, "temp_final.mhd");

  // This is the end, my friend.
}
// --------------------------------------------------------------------
