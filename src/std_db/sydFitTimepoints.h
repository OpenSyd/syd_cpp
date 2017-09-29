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

#ifndef SYDFITTIMEPOINTS_H
#define SYDFITTIMEPOINTS_H

// syd
#include "sydFitModels.h"
#include "sydTimepoints.h"
#include "sydFitOptions.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::FitTimepoints") callback(Callback)
  /// Table to store the result of a fit process on a TAC time curve activity
  class FitTimepoints:
    public syd::Record,
    public syd::RecordWithHistory,
    public syd::RecordWithTags,
    public syd::RecordWithMD5Signature,
    public syd::FitOptions {
  public:

      DEFINE_TABLE_CLASS(FitTimepoints);

#pragma db not_null on_delete(cascade)
      /// Linked Timepoints. If the tp is deleted, the FitTimepoints also.
      syd::Timepoints::pointer timepoints;

      /// Values of the parameters for the fit
      std::vector<double> params;

      /// Area under the curve (integral or restricted computation)
      double auc;

      /// Residual (after the fit)
      double r2;

      /// Resulting model name (f3, f4a etc)
      std::string model_name;

      /// Index of the first value used for the fit (not 0 if
      /// "restricted" option is used)
      int first_index;

      /// Number of iterations
      int iterations;

      /// Unity (e.g. MBq.h)
      syd::PixelUnit::pointer unit;

      /// Write the element as a string
      virtual std::string ToString() const;

      /// Build a string to compute MD5
      virtual std::string ToStringForMD5() const;

      /// Callback
      void Callback(odb::callback_event, odb::database&) const;
      void Callback(odb::callback_event, odb::database&);

      /// Create a model from the current result
      syd::FitModelBase::pointer NewModel() const;

      /// Return an options objects with the values from the record
      virtual syd::TimeIntegratedActivityFitOptions GetOptions() const;

  protected:
      FitTimepoints();

    }; // end of class

} // end namespace
// --------------------------------------------------------------------

#endif
