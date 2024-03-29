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
#include "sydCommonDatabase.h"

//--------------------------------------------------------------------
namespace syd {

  /// Helpers class to dump a table (line/column) of values,
  /// controling the precision and the column size
  class PrintTable {
  public:

    // Constructor
    PrintTable();

    typedef syd::RecordTraitsBase::RecordBasePointer RecordBasePointer;
    typedef syd::RecordTraitsBase::RecordBaseVector RecordBaseVector;
    //    typedef std::function<std::string(RecordBasePointer)> FieldFunc;
    //    typedef syd::FieldBase::pointer FieldFunc;

    void Build(std::string table_name, const RecordBaseVector records, std::string columns);
    //    void Build(const RecordBaseVector & records, const std::vector<FieldFunc> & f);
    void Build(const RecordBaseVector & records, syd::FieldBase::vector & f);
    void SetHeaderFlag(bool b);
    void SetFooterFlag(bool b);
    void SetDefaultColumnsSize();
    void SetPrecision(int p);
    void SetSingleLineFlag(bool b);
    void Print(std::ostream & os);
    void PrintHeader(std::ostream & os);

  protected:
    std::vector<std::vector<std::string>> values_;
    std::vector<int> column_widths_;
    std::vector<std::string> header_;
    bool header_flag_;
    bool footer_flag_;
    int precision_;
    bool single_line_flag_;
    void PrintRow(std::ostream & os, std::vector<std::string> & row);
  };


} // end namespace

#endif /* end #define SYDPRINTTABLE_H_H */
