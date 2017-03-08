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

#ifndef SYDFITOPTIONS_H
#define SYDFITOPTIONS_H

#include "sydImage.h"
#include "sydTimeIntegratedActivityFitOptions.h"

// --------------------------------------------------------------------
namespace syd {

  /// Store common information on TAC fit options
#pragma db object abstract pointer(std::shared_ptr)
  class FitOptions {
  public:

    /// Constructor
    FitOptions();

    /// Fit is fail if r2 is lower than this value
    double r2_min;

    /// Max nb of iteration
    int max_iteration;

    /// Compute fit with the end of the TAC only
    bool restricted_tac;

    /// List of fitting models
    std::vector<std::string> model_names;

    /// Which Akaike criterion is used : AIC or AICc
    std::string akaike_criterion;

    // FIXME additional points
    // FIXME post processes

    // Not stored in the db (set/get with SetToOptions and SetFromOptions)
#pragma db transient
    bool fit_verbose_flag;

    /// Specific case for RecordWithComments (composition not inheritance)
    typedef std::shared_ptr<FitOptions> pointer;
    typedef std::function<std::string(pointer)> SpecificFieldFunc;
    typedef std::map<std::string, SpecificFieldFunc> FieldFunctionMap;
    static void BuildMapOfFieldsFunctions(FieldFunctionMap & map);

    /// Return a string with all model names
    std::string GetModelsName() const;

    /// Return an options objects with the values from the record
    void SetToOptions(syd::TimeIntegratedActivityFitOptions & options) const;

    /// Return an options objects with the values from the record
    virtual syd::TimeIntegratedActivityFitOptions GetOptions() const;
 
    /// Set values from the options object
    void SetFromOptions(const syd::TimeIntegratedActivityFitOptions & options);

    /// Write the element as a string
    virtual std::string ToString() const;

  }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
