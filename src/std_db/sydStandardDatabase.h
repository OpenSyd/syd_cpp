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
#include "sydDicomStruct-odb.hxx"
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
#include "sydICRPOrganDose-odb.hxx"

#include "sydPatientTraits.h"
#include "sydInjectionTraits.h"
#include "sydRadionuclideTraits.h"
#include "sydDicomFileTraits.h"
#include "sydDicomSerieTraits.h"
#include "sydDicomStructTraits.h"
#include "sydPixelUnitTraits.h"
#include "sydRoiTypeTraits.h"
#include "sydImageTraits.h"
#include "sydRoiMaskImageTraits.h"
#include "sydRoiStatisticTraits.h"
#include "sydFitImagesTraits.h"
#include "sydTimepointsTraits.h"
#include "sydRoiTimepointsTraits.h"
#include "sydFitTimepointsTraits.h"
#include "sydElastixTraits.h"
#include "sydICRPOrganDoseTraits.h"

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

} // namespace syd

#include "sydStandardDatabase.txx"

// --------------------------------------------------------------------
#endif
