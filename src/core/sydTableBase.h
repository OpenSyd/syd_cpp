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

#ifndef SYDTABLEBASE_H
#define SYDTABLEBASE_H

// syd
#include "sydCommon.h"
//#include "sydRecord.h"
//#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  //class Database;
  class Record;

  /// Base class for the functions that are common to all tables
  class TableBase {
  public:

    typedef std::shared_ptr<syd::Record> record_pointer;

    virtual record_pointer New() const = 0;

    virtual void Insert(record_pointer record) const = 0;

    virtual void QueryOne(record_pointer & r, const IdType & id) const = 0;

  };

} // end namespace
// --------------------------------------------------------------------

#endif
