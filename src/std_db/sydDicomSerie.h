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

#ifndef SYDDICOMSERIE_H
#define SYDDICOMSERIE_H

// syd
#include "sydInjection.h"
#include "sydDicomBase.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::DicomSerie") callback(Callback)
  /// Store basic information about a dicom image (serie).
  class DicomSerie:
    public syd::Record,
    public syd::DicomBase {
  public:

      DEFINE_TABLE_CLASS(DicomSerie);

      /// Foreign key, can be null
      syd::Injection::pointer injection;

      /// Date when the image has been acquired. Dicom tag =
      /// AcquisitionTime & AcquisitionDate
      std::string dicom_acquisition_date;

      /// Date when the image has been reconstructed. Dicom tag =
      /// ContentDate/Time or InstanceCreationDate/Tiem
      std::string dicom_reconstruction_date;

      /// Dicom tags related to image size/spacing
      std::vector<int> dicom_size;
      std::vector<double> dicom_spacing;

      /// Dicom tags related to pixel scaling and window display
      double dicom_pixel_scale;
      double dicom_pixel_offset;
      double dicom_window_center;
      double dicom_window_width;
      double dicom_real_world_value_slope;
      double dicom_real_world_value_intercept;

      /// Specific NM
      std::string dicom_radionuclide_name;
      double dicom_counts_accumulated;
      double dicom_actual_frame_duration_in_msec;
      int    dicom_number_of_frames_in_rotation;
      int    dicom_number_of_rotations;
      double dicom_table_traverse_in_mm;
      double dicom_table_height_in_mm;
      double dicom_rotation_angle;

      /// Compute the folder to store the dicom
      virtual std::string ComputeRelativeFolder() const;

      /// Database callback
      void Callback(odb::callback_event, odb::database&) const;

      /// Database callback
      void Callback(odb::callback_event, odb::database&);

      /// Write the element as a string
      virtual std::string ToString() const;

      /// Check if the associated files exist on disk
      virtual syd::CheckResult Check() const;

  protected:
      DicomSerie();

    }; // end class
}
// --------------------------------------------------------------------

#endif
