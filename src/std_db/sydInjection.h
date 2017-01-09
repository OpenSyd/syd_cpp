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
#include "sydPatient.h"
#include "sydRadionuclide.h"

// --------------------------------------------------------------------
namespace syd {

  class InjectionStat;

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Injection") callback(Callback)
  /// Store information about a radionuclide injection (date, etc).
  class Injection :
    public syd::Record,
    public syd::RecordWithComments {
  public:

      DEFINE_TABLE_CLASS(Injection);

#pragma db not_null
      /// Foreign key, linked to Patient table.
      syd::Patient::pointer patient;

#pragma db not_null
      /// Foreign key, linked to Radionuclide table.
      syd::Radionuclide::pointer radionuclide;

      /// Date of the injection
      std::string date;

      /// Injected activity in MBq
      double activity_in_MBq;

      /// Write the element as a string
      virtual std::string ToString() const;

      void Set(const std::vector<std::string> & args);

      virtual void Callback(odb::callback_event, odb::database&) const;
      virtual void Callback(odb::callback_event, odb::database&);

      virtual void DumpInTable(syd::PrintTable & table) const;

      double GetLambdaDecayConstantInHours() const;

  protected:
      Injection();

    }; // end of class

} // end of namespace
// --------------------------------------------------------------------

#endif
