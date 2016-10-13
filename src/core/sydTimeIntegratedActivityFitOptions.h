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

#ifndef SYDTIMEINTEGRATEDACTIVITYFITOPTIONS_H
#define SYDTIMEINTEGRATEDACTIVITYFITOPTIONS_H

// syd
#include "sydFitModels.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to store the options for TAC fitting process
  class TimeIntegratedActivityFitOptions {

  public:
    /// Constructor.
    TimeIntegratedActivityFitOptions();

    /// Destructor (empty)
    ~TimeIntegratedActivityFitOptions();

    // Input
    void SetR2MinThreshold(double r) { R2_min_threshold_ = r; }
    void SetRestrictedFlag(bool b) { restricted_flag_ = b; }
    void AddModel(const std::string & model_name);
    // void AddModels(const std::vector<std::string> & model_names);
    // void AddModels(const std::string & model_names);
    void SetAkaikeCriterion(const std::string & n);
    void AddTimeValue(double time, double value);
    void SetMaxNumIterations(int i) { max_num_iterations_ = i; }

    // Get model object
    syd::FitModelBase::vector GetModels() const;
    double GetR2MinThreshold() const { return R2_min_threshold_; }
    bool GetRestrictedFlag() const { return restricted_flag_; }
    int GetMaxNumIterations() const { return max_num_iterations_; }
    std::string GetAkaikeCriterion() const { return akaike_criterion_; }

  protected:

    bool restricted_flag_;
    double R2_min_threshold_;
    std::set<std::string> model_names_;
    syd::FitModelBase::vector all_models_;
    int max_num_iterations_;
    std::string akaike_criterion_;

  }; // class TimeIntegratedActivityFitOptions

} // namespace syd
// --------------------------------------------------------------------

#endif
