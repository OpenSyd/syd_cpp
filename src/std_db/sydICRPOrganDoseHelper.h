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
#include "sydStandardDatabase.h"
#include "sydSCoefficientCalculator.h"

// --------------------------------------------------------------------
namespace syd {

  /// Create a ICRPOrganDose for a target/sources FitTimepoint
  syd::ICRPOrganDose::pointer
    NewICRPOrganDose(syd::SCoefficientCalculator::pointer c,
                     syd::FitTimepoints::pointer ftp,
                     syd::FitTimepoints::vector ftps);

  /// Create a ICRPOrganDose for a target/sources FitTimepoint
  syd::ICRPOrganDose::pointer
    NewICRPOrganDose(syd::SCoefficientCalculator::pointer c,
                     syd::FitTimepoints::pointer target_ft,
                     std::string target_name,
                     syd::FitTimepoints::vector source_fts,
                     std::vector<std::string> source_names);

  /// Try to guess the ICRP name from the RoiTimepoints roitype
  std::string GuessTargetRoiName(syd::SCoefficientCalculator::pointer c,
                                 syd::Timepoints::pointer tp);

  /// Try to guess the ICRP name from the RoiTimepoints roitype
  std::string GuessSourceRoiName(syd::SCoefficientCalculator::pointer c,
                                 syd::Timepoints::pointer tp);

  /// FIXME
  void GetICRPNamesFromComments(const syd::FitTimepoints::vector & ftps,
                                syd::FitTimepoints::vector & tftps,
                                std::vector<std::string> & target_names,
                                syd::FitTimepoints::vector & sftps,
                                std::vector<std::string> & source_names);

  static const std::string AssociatedSourceNameComment = "AssociatedSource: ";
  static const std::string AssociatedTargetNameComment = "AssociatedTarget: ";

  void SetAssociatedSourceName(std::vector<std::string> & com, std::string name);
  void SetAssociatedTargetName(std::vector<std::string> & com, std::string name);
  void RemoveAssociatedSourceName(std::vector<std::string> & com);
  void RemoveAssociatedTargetName(std::vector<std::string> & com);
  std::string GetAssociatedSourceName(std::vector<std::string> & com);
  std::string GetAssociatedTargetName(std::vector<std::string> & com);

} // end namespace
// --------------------------------------------------------------------

#endif
