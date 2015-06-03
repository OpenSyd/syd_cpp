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
#include "sydPatient.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Store information about a radionuclide
  class Radionuclide : public TableElementBase {
  public:

#pragma db id auto
    IdType id;

#pragma db options("UNIQUE")
    std::string name;

    /// Half life of the radionuclide in hours
    double half_life_in_hours;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("Radionuclide")
    Radionuclide();

    virtual std::string ToString() const;
    virtual void Set(std::vector<std::string> & arg);
    void Set(const std::string & vname, double hl);

    bool operator==(const Radionuclide & p);
    bool operator!=(const Radionuclide & p) { return !(*this == p); }

    virtual void OnDelete(syd::Database * db);

  }; // end of class
} // end of namespace
// --------------------------------------------------------------------

#endif
