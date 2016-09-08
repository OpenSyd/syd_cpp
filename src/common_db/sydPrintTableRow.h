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

#ifndef SYDPRINTTABLEROW_H
#define SYDPRINTTABLEROW_H

// syd
#include "sydPrintTableColumnInfo.h"

//--------------------------------------------------------------------
namespace syd {

  class PrintTable;

  /// Manage a row of a print table
  class PrintTableRow {
  public:

    // Constructor
    PrintTableRow(syd::PrintTable * table);

    /// Typedef, pointer creation
    typedef std::shared_ptr<PrintTableRow> pointer;
    typedef std::vector<pointer> vector;
    static pointer New(syd::PrintTable * table);

    // Set the value for column col
    void Set(int col, std::string value);

    // Get the value at column col
    std::string GetValue(int col) const;

    // Dump values in the stream
    void Dump(const std::vector<int> & indices, std::ostream & ss) const;

  protected:
    syd::PrintTable * table_;
    std::vector<std::string> values_;

  };

} // end namespace

#endif /* end #define SYDPRINTTABLE_H_H */