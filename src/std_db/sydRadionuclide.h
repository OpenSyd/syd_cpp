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

#ifndef SYDRADIONUCLIDE_H
#define SYDRADIONUCLIDE_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Radionuclide")
  /// Store information about a radionuclide
  class Radionuclide : public syd::Record {
  public:

#pragma db options("UNIQUE")
    std::string name;

    /// Element name such as Y-90 or Lu-177
    std::string element;

    /// Atomic number Z
    double atomic_number;

    /// Mass number A
    double mass_number;

    /// If metastable
    bool metastable;

    /// Half life in hours
    double half_life_in_hours;

    /// Max beta- energy (Q-)
    double max_beta_minus_energy_in_kev;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(Radionuclide, "syd::Radionuclide");
    TABLE_DECLARE_MANDATORY_FUNCTIONS(Radionuclide);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(Radionuclide);
    // ------------------------------------------------------------------------

    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;

  protected:
    Radionuclide();

  }; // end of class
} // end of namespace
// --------------------------------------------------------------------

#endif
