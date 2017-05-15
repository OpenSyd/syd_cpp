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

#ifndef SYDDICOMSTRUCT_H
#define SYDDICOMSTRUCT_H

// syd
#include "sydInjection.h"
#include "sydDicomBase.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::DicomStruct") callback(Callback)
  /// Store basic information about a dicom image (serie).
  class DicomStruct:
    public syd::Record,
    public syd::DicomBase {
  public:

      DEFINE_TABLE_CLASS(DicomStruct);

      // Dicom tag elements
      std::string dicom_station_name;
      std::string dicom_protocol_name;
      std::string dicom_structure_set_label;
      std::string dicom_structure_set_name;
      std::string dicom_structure_set_date; // (date+time)

      /// Write the element as a string
      virtual std::string ToString() const;

      /// Compute the folder to store the dicom
      virtual std::string ComputeRelativeFolder() const;

      /// Database callback (MUST be called in derived class)
      void Callback(odb::callback_event, odb::database&) const;

      /// Database callback (MUST be called in derived class)
      void Callback(odb::callback_event, odb::database&);

      /// Check if the associated files exist on disk
      virtual syd::CheckResult Check() const;

  protected:
      DicomStruct();

    }; // end class
}
// --------------------------------------------------------------------

#endif