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

#ifndef SYDSTANDARDDATABASE_H
#define SYDSTANDARDDATABASE_H

// syd
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydImageUtils.h"

// syd tables
#include "sydPatient-odb.hxx"
#include "sydInjection-odb.hxx"
#include "sydRadionuclide-odb.hxx"
#include "sydTag-odb.hxx"
#include "sydFile-odb.hxx"
#include "sydDicomSerie-odb.hxx"
#include "sydDicomFile-odb.hxx"
#include "sydImage-odb.hxx"
#include "sydRoiType-odb.hxx"
#include "sydRoiMaskImage-odb.hxx"
#include "sydImageTransform-odb.hxx"

// itk
#include <itkImage.h>

// --------------------------------------------------------------------
namespace syd {

  /// This database manages a set of 'standard' tables: Patient, Injection etc.
  class StandardDatabase: public Database {
  public:

    virtual ~StandardDatabase() { }

    syd::Patient::pointer FindPatient(const std::string & name_or_study_id) const;
    syd::Injection::pointer FindInjection(const syd::Patient::pointer patient,
                                          const std::string & name_or_study_id) const;
    void FindTags(syd::Tag::vector & tags, const std::string & names) const;
    syd::Tag::pointer FindOrInsertTag(const std::string & label, const std::string & description);

    std::string GetAbsolutePath(const syd::Image::pointer image) const;
    std::string GetAbsolutePath(const syd::DicomFile::pointer df) const;
    std::string GetAbsolutePath(const syd::File::pointer file) const;


    // template<class RecordType>
    // void Sort(std::vector<std::shared_ptr<RecordType>> & records, const std::string & type="") const;
    // template<>
    // void Sort(std::vector<std::shared_ptr<syd::Patient>> & records, const std::string & type="") const;

    template<class PixelType>
    typename itk::Image<PixelType,3>::Pointer
    ReadImage(const syd::DicomSerie::pointer dicom) const;

    /// Update image information from this itk image (type, size, spacing)
    template<class PixelType>
    void UpdateImageInfo(syd::Image::pointer image,
                         typename itk::Image<PixelType,3>::Pointer & itk_image,
                         bool computeMD5Flag);

  protected:
    /// Insert the tables
    virtual void CreateTables();

  }; // class StandardDatabase

#include "sydStandardDatabase.txx"

} // namespace syd

// --------------------------------------------------------------------

#endif
