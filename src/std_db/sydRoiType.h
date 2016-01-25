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

#ifndef SYDROITYPE_H
#define SYDROITYPE_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::RoiType")
  /// Store information about a type of ROI (liver, kidney etc)
  class RoiType : public syd::Record {
  public:

#pragma db options("UNIQUE")
    /// Name of the roi (unique)
    std::string name;

    /// Description associated with the roitype
    std::string description;

     // ------------------------------------------------------------------------
    TABLE_DEFINE(RoiType, syd::RoiType);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(RoiType);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(RoiType);
    // ------------------------------------------------------------------------

    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;

  protected:
    RoiType();

  }; // end class
}
// --------------------------------------------------------------------

#endif
