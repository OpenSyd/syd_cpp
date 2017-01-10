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
#include "sydCommonDatabase.h"
#include "sydImageUtils.h"

// syd tables
#include "sydPatient-odb.hxx"
#include "sydInjection-odb.hxx"
#include "sydRadionuclide-odb.hxx"
#include "sydDicomSerie-odb.hxx"
#include "sydDicomFile-odb.hxx"
#include "sydPixelUnit-odb.hxx"
#include "sydImage-odb.hxx"
#include "sydRoiType-odb.hxx"
#include "sydRoiMaskImage-odb.hxx"
#include "sydRoiStatistic-odb.hxx"
#include "sydElastix-odb.hxx"
#include "sydFitImages-odb.hxx"
#include "sydTimepoints-odb.hxx"
#include "sydRoiTimepoints-odb.hxx"
#include "sydFitTimepoints-odb.hxx"

// itk
#include <itkImage.h>

// --------------------------------------------------------------------
namespace syd {

  /// This database manages a set of 'standard' tables: Patient, Injection etc.
  class StandardDatabase: public CommonDatabase {
  public:
    StandardDatabase();
    virtual ~StandardDatabase();

    // Search for a patient by name (or id)
    syd::Patient::pointer FindPatient(const std::string & name_or_study_id) const;

    // Search images for this patient name
    syd::Image::vector FindImages(const std::string & patient_name) const;

    // Search images for this patient
    syd::Image::vector FindImages(const syd::Patient::pointer patient) const;

    /// Automatically insert some default records
    void InsertDefaultRecords(const std::string & def);

    /// Insert a new tag
    Tag::pointer NewTag(const std::string & name, const std::string & description);
    PixelUnit::pointer NewPixelUnit(const std::string & name, const std::string & description);
    RoiType::pointer NewRoiType(const std::string & name, const std::string & description);

    /// Find a record with the same MD5 than the input
    template<class RecordType>
      bool FindSameMD5(const typename RecordType::pointer input,
                       typename RecordType::pointer & output);

  protected:
    /// Insert the tables
    virtual void CreateTables();

  }; // class StandardDatabase

  /*
  template<> void syd::Table<syd::Image>::Sort(syd::Image::vector & records,
                                               const std::string & type) const;
  template<> void syd::Table<syd::Injection>::Sort(syd::Injection::vector & records,
                                                   const std::string & type) const;
  template<> void syd::Table<syd::RoiMaskImage>::Sort(syd::RoiMaskImage::vector & records,
                                                      const std::string & type) const;
  template<> void syd::Table<syd::DicomSerie>::Sort(syd::DicomSerie::vector & records,
                                                    const std::string & type) const;
  template<> void syd::Table<syd::Patient>::Sort(syd::Patient::vector & records,
                                                 const std::string & type) const;
  template<> void syd::Table<syd::Radionuclide>::Sort(syd::Radionuclide::vector & records,
                                                      const std::string & type) const;
  // template<> void syd::Table<syd::Calibration>::Sort(syd::Calibration::vector & records,
  //                                                    const std::string & type) const;
  template<> void syd::Table<syd::PixelUnit>::Sort(syd::PixelUnit::vector & records,
                                                   const std::string & type) const;
  template<> void syd::Table<syd::RoiStatistic>::Sort(syd::RoiStatistic::vector & records,
                                                      const std::string & type) const;
  */

  DEFINE_TABLE_TRAITS_HEADER(Patient);
  DEFINE_TABLE_TRAITS_HEADER(Injection);
  DEFINE_TABLE_TRAITS_HEADER(Radionuclide);

  DEFINE_TABLE_TRAITS_HEADER(DicomFile);
  DEFINE_TABLE_TRAITS_HEADER(DicomSerie);

  DEFINE_TABLE_TRAITS_HEADER(PixelUnit);
  DEFINE_TABLE_TRAITS_HEADER(Image);
  DEFINE_TABLE_TRAITS_HEADER(RoiType);
  DEFINE_TABLE_TRAITS_HEADER(RoiMaskImage);
  DEFINE_TABLE_TRAITS_HEADER(RoiStatistic);

  DEFINE_TABLE_TRAITS_HEADER(Elastix);
  DEFINE_TABLE_TRAITS_HEADER(FitImages);
  DEFINE_TABLE_TRAITS_HEADER(Timepoints);
  DEFINE_TABLE_TRAITS_HEADER(RoiTimepoints);
  DEFINE_TABLE_TRAITS_HEADER(FitTimepoints);


  template<>
    void syd::RecordTraits<syd::Image>::Sort(const syd::Database * db,
                                             syd::Image::vector & v,
                                             const std::string & type) const;


} // namespace syd

#include "sydStandardDatabase.txx"

// --------------------------------------------------------------------
#endif
