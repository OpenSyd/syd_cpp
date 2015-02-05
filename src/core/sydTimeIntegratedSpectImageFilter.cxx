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
#include "sydTimeIntegratedSpectImageFilter.h"

// --------------------------------------------------------------------
syd::TimeIntegratedSpectImageFilter::TimeIntegratedSpectImageFilter():Superclass()
{
  isInitialised_ = false;
  SetDebugFlag(false);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeIntegratedSpectImageFilter::~TimeIntegratedSpectImageFilter()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedSpectImageFilter::AddInput(double time, ImageType::Pointer spect)
{
  spects_.push_back(spect);
  times_.push_back(time);
  isInitialised_ = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedSpectImageFilter::Initialise()
{
  if (isInitialised_) return;

  // Check size
  if (spects_.size() < 1) {
    LOG(FATAL) << "At least one spect image is needed in TimeIntegratedSpectImageFilter.";
  }
  int n = spects_.size();

  // Sort the times/spects
  std::vector<ImageType::Pointer> temp_spects(n);
  std::vector<double> temp_times(n);
  std::copy(spects_.begin(), spects_.end(), temp_spects.begin());
  std::copy(times_.begin(), times_.end(), temp_times.begin());
  std::vector<int> indices;
  for(auto i=0; i<spects_.size(); i++) indices.push_back(i);
  std::sort(begin(indices), end(indices), [&temp_times](size_t a, size_t b) { return temp_times[a] < temp_times[b]; }  );
  for(auto i=0; i<indices.size(); i++) {
    times_[i] = temp_times[indices[i]];
    spects_[i] = temp_spects[indices[i]];
  }

  // Check spect size
  RegionType region = spects_[0]->GetLargestPossibleRegion();
  for(auto s:spects_) {
    bool b = region.IsInside(s->GetLargestPossibleRegion()) and
      s->GetLargestPossibleRegion().IsInside(region);
    if (!b) {
      LOG(FATAL) << "Image sizes must be the same in TimeIntegratedSpectImageFilter.";
    }
  }

  isInitialised_ = true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TimeIntegratedSpectImageFilter::Update()
{
  if (!isInitialised_) Initialise();

  // Solver and TAC
  syd::TimeActivityCurve tac;
  syd::TimeActivityCurveIntegrate solver;
  solver.SetInput(&tac);
  solver.GetFitSolver().SetUseWeightedFit(false); // FIXME not very convenient
  solver.GetFitSolver().InitIncrementalRun(); // FIXME not very convenient

  // Create image iterators
  std::vector<IteratorType> iters;
  for(auto s:spects_) {
    IteratorType iter(s, s->GetLargestPossibleRegion());
    iters.push_back(iter);
  }

  // Initialise the TAC
  for(auto i=0; i<spects_.size(); i++) tac.AddValue(times_[i], 0.0, 0.0);

  // Create result image and iterator
  output_ = syd::CreateImageLike<ImageType>(spects_[0]);
  RegionType region = spects_[0]->GetLargestPossibleRegion();
  IteratorType itero(output_, region);

  // number of pixel (needed for the progress bar)
  int n = region.GetSize()[0]*region.GetSize()[1]*region.GetSize()[2];

  // DEBUG
  int debug_lambda = AddDebugImage("fit_lambda");
  int debug_A = AddDebugImage("fit_A");
  int debug_first = AddDebugImage("fit_first");
  int debug_middle = AddDebugImage("fit_middle");
  int debug_final = AddDebugImage("fit_final");
  int debug_mask = AddDebugImage("fit_mask");
  int debug_nbiteration = AddDebugImage("fit_nbiterations");
  int debug_nbpoints = AddDebugImage("fit_nbpoints");
  int debug_finalcost = AddDebugImage("fit_finalcost");
  int debug_nbuncciteration = AddDebugImage("fit_nbuncciterations");
  int debug_converge = AddDebugImage("fit_converge");
  int debug_fitratio = AddDebugImage("fit_ratio");

  // Loop over pixels
  int x=0;
  int nb = 0;
  while (!iters[0].IsAtEnd()) {
    // Change the TAC value.
    bool toolow = false;
    double max=0.0;
    for(auto i=0; i<spects_.size(); i++) {
      double v = iters[i].Get();
      tac.SetValue(i, v);
      if (v > max) max = v;
    }
    if (max < min_activity_value_) toolow = true;

    // Display progress bar
    syd::loadbar(x, n);

    // Solver
    if (!toolow) { // do not try to fit if a value is too low
      ++nb;
      solver.IncrementalRun();
      // Store the result if it is useful (not infinite, not nan etc)
      double v = solver.GetIntegratedValueWithConditions(); //FIXME -> to put in filter not in solver
      itero.Set(v);

      // DEBUG
      if (debug_flag_) {
        debug_iterators[debug_lambda].Set(solver.GetFitSolver().GetFitLambda());
        debug_iterators[debug_A].Set(solver.GetFitSolver().GetFitA());
        debug_iterators[debug_first].Set(solver.GetTempFirstPartIntegration());
        debug_iterators[debug_middle].Set(solver.GetTempMiddlePartIntegration());
        debug_iterators[debug_final].Set(solver.GetTempFinalPartIntegration());
        debug_iterators[debug_mask].Set(1.0);
        debug_iterators[debug_nbiteration].Set(solver.GetSolverSummary().num_successful_steps);
        debug_iterators[debug_nbpoints].Set(solver.GetFitSolver().GetFitNbPoints());
        debug_iterators[debug_finalcost].Set(solver.GetSolverSummary().final_cost);
        debug_iterators[debug_nbuncciteration].Set(solver.GetSolverSummary().num_unsuccessful_steps);
        debug_iterators[debug_converge].Set(solver.GetSolverSummary().termination_type);

        double a = solver.GetTempFirstPartIntegration();
        double b = solver.GetTempMiddlePartIntegration();
        double c = solver.GetTempFinalPartIntegration();
        debug_iterators[debug_fitratio].Set(c/(a+b));
      }
    }
    else {
      itero.Set(-1.0);
      debug_iterators[debug_mask].Set(0.0);
    }

    // Next pixel for each spect and the output
    for(auto i=0; i<iters.size(); i++) ++iters[i];
    ++itero;
    ++x;

    // DEBUG
    if (debug_flag_) {
      for(auto i=0; i<debug_iterators.size(); i++) ++debug_iterators[i];
    }
  }

  // DEBUG
  if (debug_flag_) {
    for(auto i=0; i<debug_images.size(); i++) syd::WriteImage<ImageType>(debug_images[i], debug_names[i]+".mhd");
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::TimeIntegratedSpectImageFilter::AddDebugImage(std::string name)
{
  ImageType::Pointer image =  syd::CreateImageLike<ImageType>(spects_[0]);
  IteratorType iter(image, spects_[0]->GetLargestPossibleRegion());
  debug_images.push_back(image);
  debug_iterators.push_back(iter);
  debug_names.push_back(name);
  return debug_images.size()-1;
}
// --------------------------------------------------------------------
