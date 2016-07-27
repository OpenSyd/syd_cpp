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

#ifndef SYDDICOMFILE_H
#define SYDDICOMFILE_H

// syd
#include "sydFile.h"
#include "sydDicomSerie.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::DicomFile") callback(Callback)
  /// Store information about a dicom file (unique dicom identifier
  /// dicom_sop_uid). Also contains a link to the DicomSerie it
  /// belongs to.
  class DicomFile: public syd::Record {
  public:

    virtual ~DicomFile() {}

    /// Foreign key, it must exist in the File table.
#pragma db not_null on_delete(cascade)
    syd::File::pointer file;

    /// Foreign key, it must exist in the DicomSerie table.
#pragma db not_null on_delete(cascade)
    syd::DicomSerie::pointer dicom_serie;

#pragma db options("UNIQUE")
    /// Dicom SOPInstanceUID. Unique dicom file identifier. Unique in db.
    std::string dicom_sop_uid;

    /// Dicom InstanceNumber.
    int dicom_instance_number;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(DicomFile, syd::DicomFile);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

    virtual void DumpInTable(syd::PrintTable & table) const;

    /// Check if the associated files exist on disk
    virtual syd::CheckResult Check() const;

  protected:
    DicomFile();

  }; // end class

} // end syd namespace
// --------------------------------------------------------------------

#endif
