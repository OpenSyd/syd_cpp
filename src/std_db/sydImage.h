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

#ifndef SYDIMAGE_H
#define SYDIMAGE_H

// std
#include <array>

// syd
#include "sydPatient.h"
#include "sydInjection.h"
#include "sydFile.h"
#include "sydTag.h"
#include "sydDicomSerie.h"
#include "sydPixelUnit.h"
#include "sydRecordWithHistory.h"
#include "sydRecordWithTags.h"
#include "sydRecordWithComments.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Image") callback(Callback)
  /// Store information about an image stored in a db (file, size etc)
  class Image :
    public syd::Record,
    public syd::RecordWithHistory,
    public syd::RecordWithTags,
    public syd::RecordWithComments
    {
    public:

      DEFINE_TABLE_CLASS(Image);

#pragma db not_null
      /// Foreign key, it must exist in the Patient table.
      syd::Patient::pointer patient;

      /// Foreign key, it must exist in the Injection table. Could be null.
      syd::Injection::pointer injection;

      /// List of associated files (will be deleted when the image is deleted)
      syd::File::vector files;

      /// Dicoms related to the image (could be empty). Allow to
      /// retrieve Injection, date. A mask image is not necessarily
      /// converted from a dicom, but associated with dicom information.
      syd::DicomSerie::vector dicoms;

      /// Type of the image (mhd by default)
      std::string type;

      /// Pixel type: short, float, unsigned char
      std::string pixel_type;

      /// The unit of the pixel value (counts, suv, etc)
      syd::PixelUnit::pointer pixel_unit;

      /// Frame of reference : images with same frame_of_reference_uid
      /// are in the same coordinate system. Imported from dicom
      std::string frame_of_reference_uid;

      /// Date when the image has been acquired. Imported from dicom
      std::string acquisition_date;

      /// Modality as indicated in the dicom tag Modality. Imported from dicom
      std::string modality;

      /// Dimension is 3 or 2.
      unsigned short int dimension;

      /// Image size (in pixels)
      std::vector<int> size;

      /// Image spacing aka size of the pixel (in mm)
      std::vector<double> spacing;

      /// Write the element as a string
      virtual std::string ToString() const;
      virtual std::string ToShortString() const;
      virtual std::string AllFieldsToString() const {return ToString();}

      /// Return the file path
      std::string GetAbsolutePath() const;

      /// Return the acquisition date of the first dicom linked to this image
      std::string GetAcquisitionDate() const;

      /// Compute the nb of hours between the injection and the
      /// acquisition date.
      double GetHoursFromInjection() const;

      /// Convenient fct (check if patient is set)
      std::string GetPatientName() const;

      /// Convenient fct (check if injection is set)
      std::string GetInjectionName() const;

      /// Add a DicomSerie to the list (check if already exists) ; do not
      /// update in the db.
      void AddDicomSerie(const syd::DicomSerie::pointer dicom);

      /// Remove a DicomSerie from the list ; do not update in the
      /// db. Do nothing it not found
      void RemoveDicomSerie(const syd::DicomSerie::pointer dicom);

      /// Copy DicomSeries from another image
      void CopyDicomSeries(const syd::Image::pointer image);

      /// Callback : delete the associated files when the image is deleted.
      void Callback(odb::callback_event, odb::database&) const;
      void Callback(odb::callback_event, odb::database&);

      /// Return size as string such like 128x128x64
      std::string SizeAsString() const { return syd::ArrayToString(size); }

      /// Return spacing as string such like 0.5x0.5x0.3
      std::string SpacingAsString() const { return syd::ArrayToString(spacing); }

      /// Check if the image has a dicom. If not fail
      void FatalIfNoDicom() const;

      /// Check if the associated files exist on disk
      virtual syd::CheckResult Check() const;

      /// Compute the default image path (based on the patient's name)
      virtual std::string ComputeDefaultRelativePath();

      /// Compute the default image mhd filename (based on id + modality)
      virtual std::string ComputeDefaultMHDFilename();

    protected:
      Image();

    }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
