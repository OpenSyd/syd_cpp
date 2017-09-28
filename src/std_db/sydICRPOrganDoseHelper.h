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

#ifndef SYDORGANICRPDOSEHELPER_H
#define SYDORGANICRPDOSEHELPER_H

// syd
//#include "sydICRPOrganDose.h"
#include "sydStandardDatabase.h"
#include "sydSCoefficientCalculator.h"

// --------------------------------------------------------------------
namespace syd {

  /// FIXME
  //  syd::ICRPOrganDose::pointer
  void NewICRPOrganDose(syd::SCoefficientCalculator::pointer c,
                        syd::FitTimepoints::pointer ftp,
                        syd::FitTimepoints::vector ftps);

  std::string GuessTargetRoiName(syd::SCoefficientCalculator::pointer c,
                                 syd::Timepoints::pointer tp);
  std::string GuessSourceRoiName(syd::SCoefficientCalculator::pointer c,
                                 syd::Timepoints::pointer tp);


} // end namespace
// --------------------------------------------------------------------

#endif
