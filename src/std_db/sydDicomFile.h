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

#pragma db object
  /// Store information about a dicom file (unique dicom identifier
  /// dicom_sop_uid). Also contains a link to the DicomSerie it
  /// belongs to.
  class DicomFile : public syd::TableElement {
  public:

#pragma db id auto
    /// Id of the DicomFile
    IdType id;

    /// Foreign key, it must exist in the File table
    std::shared_ptr<syd::File> file;

    /// Foreign key, it must exist in the DicomSerie table
    std::shared_ptr<syd::DicomSerie> dicom_serie;

#pragma db options("UNIQUE")
    /// Dicom SOPInstanceUID. Unique dicom file identifier. Unique in db.
    std::string dicom_sop_uid;

    /// Dicom InstanceNumber.
    int dicom_instance_number;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("DicomFile")
    DicomFile();
    ~DicomFile();
    DicomFile(const DicomFile & other);
    DicomFile & operator= (const DicomFile & other);
    void copy(const DicomFile & t);

    virtual std::string ToString() const;
    virtual void SetValues(std::vector<std::string> & arg);

  }; // end class
}
// --------------------------------------------------------------------

#endif
