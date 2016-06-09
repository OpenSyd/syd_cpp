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

#ifndef SYDPATIENT_H
#define SYDPATIENT_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

  class Injection;
  class PatientStat;
  class StandardDatabase;

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Patient") callback(Callback)
  /// Store information about a patient (id, study_id, name etc).
  class Patient: public syd::Record {
  public:

#pragma db options("UNIQUE")
    /// Patient name (unique)
    std::string name;

#pragma db options("UNIQUE")
    /// Patient number in the study (unique)
    IdType study_id;

    /// Patient weight_in_kg
    double weight_in_kg;

    /// Patient dicom ID. Not unique because could be unknown.
    std::string dicom_patientid;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(Patient, syd::Patient);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    /// Additional Set function to shorter patient inclusion
    virtual void Set(const std::vector<std::string> & args);
    virtual void Set(const std::string & pname,
                     const IdType & pstudy_id, const double pweight_in_kg=-1,
                     const std::string pdicom_patientid="unset");

    virtual bool CheckIdentity(std::string vdicom_patientid, std::string vdicom_name) const;
    virtual std::string ComputeRelativeFolder() const;

    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

    virtual void DumpInTable(syd::PrintTable2 & table) const;

    // FIXME
    //    virtual void InitTableDescription(syd::TableDescription * description) const;

  protected:
    Patient();

  }; // end of class
}
// --------------------------------------------------------------------

#endif
