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

#ifndef SYDPRINTTABLECOLUMNINFO_H
#define SYDPRINTTABLECOLUMNINFO_H

// syd
#include "sydCommon.h"

//--------------------------------------------------------------------
namespace syd {

  static const char * resetColor = "\x1b[0m";
  static const char * redColor = "\x1b[31m";
  static const char * greenColor = "\x1b[32m"; // Green
  static const char * magentaColor = "\x1b[35m"; // Magenta

  /// Manage a row of a print table
  class PrintTableColumnInfo {
  public:

    // Constructor
    PrintTableColumnInfo(int index);

    /// typedef and pointer
    typedef std::shared_ptr<PrintTableColumnInfo> pointer;
    static pointer New(int index);

    // Set the column name
    void SetName(std::string n);

    // Set the column max width
    void SetMaxWidth(int w) { max_width_ = w; }

    // Set the precision for floating point values
    void SetPrecision(int i);

    // Get the column name
    std::string GetName() const { return name_; }

    // Get the column index
    int GetIndex() const { return index_; }

    // Get the precision for floating point values
    int GetPrecision() const { return precision_; }

    // Return the current width of the column
    int GetWidth() const { return width_; }

    // Get the precision for floating point values
    std::string GetStringValue(double value, int precision=-1);

    // Put column parameters to the stream
    void InstallStreamParameters(std::ostream & os) const;

    // Truncate the string if larger than column width
    std::string TruncateStringIfNeeded(const std::string & s) const;

    // Change the column width according to the value
    void UpdateWidth(const std::string & value);

    // Print the column header
    void DumpHeader(std::ostream & os);

    // Get the color for header
    const char * GetHeaderColor() const { return headerColor_; }

  protected:
    std::string name_;
    int index_;
    int width_;
    int max_width_;
    int precision_;
    int use_user_precision_;
    const char * color_;
    const char * indexColor_;
    const char * headerColor_;

  };

} // end namespace

#endif /* end #define SYDPRINTTABLE2_H */
