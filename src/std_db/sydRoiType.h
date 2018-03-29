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
  class RoiType:
    public syd::Record {
  public:

    DEFINE_TABLE_CLASS(RoiType);

#pragma db options("UNIQUE")
    /// Name of the roi (unique)
    std::string name;

    /// Description associated with the roitype
    std::string description;

    /// Write the element as a string
    virtual std::string ToString() const;
    virtual std::string AllFieldsToString() const {return ToString();}

    void Set(const std::vector<std::string> & arg);

  protected:
    RoiType();

  }; // end class
} // end namespace
// --------------------------------------------------------------------

#endif
