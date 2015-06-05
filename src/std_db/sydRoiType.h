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
#include "sydImage.h"
#include "sydRoiType.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Store information about a type of ROI (liver, kidney etc)
  class RoiType : public syd::TableElementBase {
  public:

#pragma db id auto
    /// Id of the RoiType
    IdType id;

#pragma db options("UNIQUE")
    /// Name of the roi (unique)
    std::string name;

    /// Description associated with the roitype
    std::string description;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("RoiType")
    RoiType();

    virtual std::string ToString() const;

    bool operator==(const RoiType & p);
    bool operator!=(const RoiType & p) { return !(*this == p); }

    virtual void Set(std::vector<std::string> & arg);

    virtual void OnDelete(syd::Database * db);

  }; // end class
}
// --------------------------------------------------------------------

#endif
