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

#ifndef SYDTIMEPOINTS_H
#define SYDTIMEPOINTS_H

// std
#include <array>

// syd
#include "sydRoiMaskImage.h"
#include "sydRecordWithTags.h"
#include "sydRecordWithHistory.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::TimePoints") callback(Callback)
  /// Store information about a time activity curve (TimePoints).
  class TimePoints : public syd::Record,
                     public syd::RecordWithHistory,
                     public syd::RecordWithTags {
  public:

    /// List of times
    std::vector<double> times;

    /// List values
    std::vector<double> values;

    /// Associated std dev (not required)
    std::vector<double> std_deviations;

    /// Associated images (not required)
    syd::Image::vector images;

    /// Associated mask (not required)
    syd::RoiMaskImage::pointer mask;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(TimePoints, syd::TimePoints);

  // static void InitInheritance() {
  //   syd::RecordWithHistory::InitInheritance(); // add record
  //   //    syd::RecordWithTags::InitInheritance(); // add nothing
  //   inherit_sql_tables_map_["TimePoints"].push_back(syd::RecordWithHistory::GetStaticSQLTableName());
  //   //    inherit_sql_tables_map_["TimePoints"].push_back(syd::RecordWithTags::GetStaticSQLTableName());
  // }

    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    /// Callback (const)
    void Callback(odb::callback_event, odb::database&) const;

    /// Callback
    void Callback(odb::callback_event, odb::database&);

    /// Initialise a PrintTable
    virtual void InitTable(syd::PrintTable & table) const;

    /// Add a line in the given PrintTable
    virtual void DumpInTable(syd::PrintTable & table) const;

    /// Check if the history are ok
    virtual syd::CheckResult Check() const; //FIXME

  protected:
    TimePoints();

  }; // end class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
