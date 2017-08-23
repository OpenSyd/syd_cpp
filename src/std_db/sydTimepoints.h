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

#ifndef SYDTIMEPOINTS_H
#define SYDTIMEPOINTS_H

// syd
#include "sydRoiMaskImage.h"
#include "sydRecordWithTags.h"
#include "sydRecordWithHistory.h"
#include "sydRecordWithMD5Signature.h"
#include "sydRecordWithComments.h"
#include "sydTimeActivityCurve.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Timepoints") callback(Callback)
  /// Store information about a time activity curve (Timepoints).
  class Timepoints :
    public syd::Record,
    public syd::RecordWithHistory,
    public syd::RecordWithTags,
    public syd::RecordWithMD5Signature,
    public syd::RecordWithComments{
  public:

      DEFINE_TABLE_CLASS(Timepoints);

#pragma db not_null on_delete(cascade)
      /// Foreign key
      syd::Patient::pointer patient;

#pragma db not_null on_delete(cascade)
      /// Foreign key. Timepoints must be associated with an injection (to
      /// consider the decay when fit)
      syd::Injection::pointer injection;

      /// List of times
      std::vector<double> times;

      /// List values
      std::vector<double> values;

      /// Associated std dev (not required)
      std::vector<double> std_deviations;

      /// Write the element as a string
      virtual std::string ToString() const;

      /// Build a string to compute MD5
      virtual std::string ToStringForMD5() const;

      /// Callback (const)
      void Callback(odb::callback_event, odb::database&) const;

      /// Callback
      void Callback(odb::callback_event, odb::database&);

  protected:
      Timepoints();

    }; // end class
  // ------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
