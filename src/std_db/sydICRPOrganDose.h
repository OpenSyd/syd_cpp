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

#ifndef SYDICRPORGANDOSE_H
#define SYDICRPORGANDOSE_H

// syd
#include "sydFitTimepoints.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::ICRPOrganDose") callback(Callback)
  /// Table to store the result of a fit process on a TAC time curve activity
  class ICRPOrganDose:
    public syd::Record,
    public syd::RecordWithHistory,
    public syd::RecordWithTags,
    public syd::RecordWithMD5Signature {
  public:

      DEFINE_TABLE_CLASS(ICRPOrganDose);

#pragma db not_null on_delete(cascade)
      /// If the tp is deleted, the ICRPOrganDose also.
      syd::FitTimepoints::pointer target_fit_timepoints;

#pragma db not_null on_delete(cascade)
      /// If the tp is deleted, the ICRPOrganDose also.
      syd::FitTimepoints::vector sources_fit_timepoints;

      /// Radionuclide
      syd::Radionuclide::pointer radionuclide;

      /// The computed S coefficients
      std::vector<double> S_coefficients;

      /// The computed final dose result
      double absorbed_dose_in_Gy;

      /// Phantom name that was used to compute the dose (AM or AF)
      std::string phantom_name;

      /// Name of the target organ used in the ICRP
      std::string target_organ_name;

      /// Corresponding target RoiType
      syd::RoiType::pointer target_roitype;

      /// Name of the source organs used in the ICRP
      std::vector<std::string> source_organ_names;

      /// Corresponding source RoiType
      syd::RoiType::vector source_roitypes;

      /// Write the element as a string
      virtual std::string ToString() const;

      /// Build a string to compute MD5
      virtual std::string ToStringForMD5() const;

      /// Callback
      void Callback(odb::callback_event, odb::database&) const;
      void Callback(odb::callback_event, odb::database&);

  protected:
      ICRPOrganDose();

    }; // end of class

} // end namespace
// --------------------------------------------------------------------

#endif
