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

#ifndef SYDFITIMAGES_H
#define SYDFITIMAGES_H

#include "sydImage.h"
#include "sydFitOptions.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::FitImages") callback(Callback)
  /// Store information about pixel-based time activity fitting to compute Time
  /// Integrated Activity image
  class FitImages:
    public syd::Record,
    public syd::RecordWithHistory,
    public syd::RecordWithTags,
    public syd::RecordWithComments,
    public syd::FitOptions {
  public:

      DEFINE_TABLE_CLASS(FitImages);

#pragma db on_delete(cascade)
      /// List of input images to compute the FitImages
      syd::Image::vector images;

      /// Initial threshold : do not compute FitImages when activity is lower
      /// than this value
      double min_activity;

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

      /// Write the element as a string
      virtual std::string ToString() const;

      /// Return a string that describe all associated output (id + name)
      std::string GetOutputNames() const;

      /// Callback : delete the associated files when the image is deleted.
      void Callback(odb::callback_event, odb::database&) const;
      void Callback(odb::callback_event, odb::database&);

      /// Add an output (image + name)
      void AddOutput(syd::Image::pointer output, std::string name);

      /// Get the output by name (nullptr if not found)
      syd::Image::pointer GetOutput(std::string name);

      /// Return an options objects with the values from the record
      virtual syd::TimeIntegratedActivityFitOptions GetOptions() const;

  protected:
      FitImages();

    }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
