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

#ifndef SYDFIT_H
#define SYDFIT_H

// Ceres
#include <ceres/ceres.h>
#include <glog/logging.h>

// syd
#include "sydTimeActivityCurve.h"
#include "sydQuery.h"

#include "clitkCommon.h"

// --------------------------------------------------------------------
namespace syd {

  struct MonoExponentialResidual;
  struct BiExponentialResidual;
  struct BiExponentialResidual2;
  struct BiExponentialResidual3;

  void Fit(std::vector<double> & times, std::vector<double> & activities);

  void Fit(std::vector<double> & times,
           std::vector<double> & activities,
           std::vector<double> & variances);

  void FitTest(std::vector<double> & times,
               std::vector<double> & activities,
               std::vector<double> & variances);

}

#endif
