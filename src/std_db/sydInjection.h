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

#ifndef SYDINJECTION_H
#define SYDINJECTION_H

// syd
//#include "sydRecord.h"
//#include "sydCommon.h"
//#include "sydStandardDatabase.h"
#include "sydPatient.h"
// #include "sydRadionuclide.h"
//#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Injection")
  /// Store information about a radionuclide injection (date, etc).
  class Injection : public syd::Record {
  public:

    //#pragma db not_null // FIXME to put when Set is ok
    /// Foreign key, linked to Patient table.
    std::shared_ptr<syd::Patient> patient;

    friend class odb::access;

    virtual ~Injection() { }

    typedef std::shared_ptr<Injection> pointer;
    static pointer New() { return pointer(new Injection); }

    virtual std::string GetTableName() const { return "Injection"; }
    static std::string GetStaticTableName() { return "Injection"; }

    virtual void Set(const syd::Database * db, const std::vector<std::string> & args);

    /// Foreign key, linked to Radionuclide table.
    //    std::shared_ptr<Radionuclide> radionuclide;

    /// Date of the injection
    std::string date;

    /// Injected activity in MBq
    double activity_in_MBq;

    // ------------------------------------------------------------------------
    // SET_TABLE_NAME("Injection")
    // Injection();

    virtual std::string ToString() const;
    // virtual void Set(std::vector<std::string> & arg) { DD("TODO"); }
    // void Set(Patient & p, Radionuclide & pr, const std::string & pdate, double activity);

    // bool operator==(const Injection & p);
    // bool operator!=(const Injection & p) { return !(*this == p); }

    // virtual void OnDelete(syd::Database * db);
   protected:
    Injection():Record("") { DD("const injection"); }

  }; // end of class

} // end of namespace
// --------------------------------------------------------------------

#endif
