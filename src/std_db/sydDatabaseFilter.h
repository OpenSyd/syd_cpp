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

#ifndef SYDDATABASEFILTERBASE_H
#define SYDDATABASEFILTERBASE_H

// syd
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This base class is used to create class that perform operation on database
  class DatabaseFilter {

  public:
    /// Constructor.
    DatabaseFilter(syd::StandardDatabase * db) { SetDatabase(db); }

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

  protected:
    /// Protected constructor. No need to use directly.
    DatabaseFilter() {}

    /// Pointer to the database
    syd::StandardDatabase * db_;

  }; // class DatabaseFilter

} // namespace syd
// --------------------------------------------------------------------

#endif
