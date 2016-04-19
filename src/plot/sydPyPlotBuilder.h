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

#ifndef SYDPYPLOTBUILDER_H
#define SYDPYPLOTBUILDER_H

// Python (must be before dcmtk includes to avoid warning)
#include <Python.h>

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  /// Simple wraper to create python plot
  class PyPlotBuilder {

  public:
    /// Constructor.
    PyPlotBuilder();

    void AddCurve(const std::vector<double> & x,
                  const std::vector<double> & y,
                  const std::string & plot_type,
                  const std::string & label);
    void Add(const std::string & line);
    std::string GetOutput() const;
    void Show();
    void Initialize();

  protected:
    int current_curve_nb_;
    std::stringstream script_;


  }; // class PyPlotBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
