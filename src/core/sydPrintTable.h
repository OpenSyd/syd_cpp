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

#ifndef SYDPRINTTABLE_H
#define SYDPRINTTABLE_H

// syd
#include "sydCommon.h"

//--------------------------------------------------------------------
namespace syd {

  /// Helpers class to dump a table (line/column) of values, controling the precision and the column size
  class PrintTable {
  public:
    PrintTable();
    void AddColumn(std::string name, int width=-1, int digit=0);
    void Init();
    PrintTable & operator<<(const double & value);
    PrintTable & operator<<(const std::string & value);
    void SkipLine();
    void Print(std::ostream & out);
    void SetColumnWidth(int col, int width);
  protected:
    std::vector<std::vector<std::string>> values;
    std::vector<std::string> headers;
    std::vector<int> width;
    std::vector<int> precision;
    int current_line;
    int current_column;
  };

} // end namespace

#endif /* end #define SYDPRINTTABLE_H_H */
