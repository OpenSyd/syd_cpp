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

#ifndef SYDTIAFITOPTIONS_H
#define SYDTIAFITOPTIONS_H

#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  /// Store common information on TAC fit options
  class TiaFitOptions {
  public:

    /// Constructor
    TiaFitOptions();

    /// Fit is fail if r2 is lower than this value
    double r2_min;

    /// Max nb of iteration
    int max_iteration;

    /// Compute fit with the end of the TAC only
    bool restricted_tac;

    /// List of fitting models
    std::vector<std::string> models_name;

    /// Which Akaike criterion is used : AIC or AICc
    std::string akaike_criterion;

    // FIXME additional points
    // FIXME post processes

    /// Return a string with all model names
    std::string GetModelsName() const;

  }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
