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

#ifndef SYDTIMEPOINTSHELPER_H
#define SYDTIMEPOINTSHELPER_H

// syd
#include "sydRoiTimepoints.h"
#include "sydFitTimepoints.h"
#include "sydTimeActivityCurve.h"

// --------------------------------------------------------------------
namespace syd {

  /// Create a new timepoints from a set of RoiStatistics
  syd::RoiTimepoints::pointer NewTimepoints(const syd::RoiStatistic::vector stats);

  /// Retrive similar RoiTimepoints sharing the same list of stats
  syd::RoiTimepoints::vector FindRoiTimepoints(const syd::RoiStatistic::vector stats);

  /// Retrive similar FitTimepoints sharing the same tp and options
  syd::FitTimepoints::vector FindFitTimepoints(const syd::Timepoints::pointer tp,
                                               const syd::TimeIntegratedActivityFitOptions & options);

  /// Create a TAC from a tp
  syd::TimeActivityCurve::pointer GetTAC(const syd::Timepoints::pointer tp);
  void GetTAC(const syd::Timepoints::pointer tp, syd::TimeActivityCurve::pointer tac);
  /// Set timepoints from a given TAC //FIXME 

  // Copy patient + times + values + std
  void CopyTimepoints(const syd::Timepoints::pointer from, syd::Timepoints::pointer to);

  /// Fit timepoints
  syd::FitTimepoints::pointer
    NewFitTimepoints(const syd::Timepoints::pointer tp,
                     syd::TimeIntegratedActivityFitOptions & options);

  /// Fit timepoints
  void ComputeFitTimepoints(syd::FitTimepoints::pointer fit);

} // end namespace
// --------------------------------------------------------------------

#endif
