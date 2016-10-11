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

#ifndef SYDTIA_H
#define SYDTIA_H

#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::TIA") //callback(Callback)
  /// Store information about an image stored in a db (file, size etc)
  class TIA: public syd::Record,
             public syd::RecordWithHistory,
             public syd::RecordWithTags,
             public syd::RecordWithComments
  {
  public:

    virtual ~TIA();

 #pragma db on_delete(cascade)
   /// List of input images to compute the TIA
    syd::Image::vector images;

    /// Initial threshold : do not compute TIA when activity is lower
    /// than this value
    double min_activity;

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

 #pragma db on_delete(cascade)
    /// List of output images
    syd::Image::vector outputs;

    /// Name of the output images
    std::vector<std::string> output_names;

    /// Resulting nb of tested pixels
    int nb_pixels;

    /// Resulting nb of successful fit pixels
    int nb_success_pixels;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(TIA, syd::TIA);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    /// Add a line in the given PrintTable
    virtual void DumpInTable(syd::PrintTable & table) const;
    virtual void DumpInTable_default(syd::PrintTable & table) const;

    /// Return a string that describe all associated output (id + name)
    std::string GetOutputNames() const;

    /// Return a string with all model names
    std::string GetModelsName() const;

    /// Add an output (image + name)
    void AddOutput(syd::Image::pointer output, std::string name);

    /// Get the output by name (nullptr if not found)
    syd::Image::pointer GetOutput(std::string name);

  protected:
    TIA();

  }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
