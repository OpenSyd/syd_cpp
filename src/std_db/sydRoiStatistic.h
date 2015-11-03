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

#ifndef SYDROISTATISTIC_H
#define SYDROISTATISTIC_H

// syd
#include "sydRoiMaskImage.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::RoiStatistic")
  /// Simple table to store a label and a description
  class RoiStatistic : public syd::Record {
  public:

#pragma db not_null
    /// Linked image
    syd::Image::pointer image;

    /// Linked mask. Maybe null (no mask)
    syd::RoiMaskImage::pointer mask;

    /// Stat values
    double mean;
    double std_dev;
    double n;
    double min;
    double max;
    double sum;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(RoiStatistic);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(RoiStatistic);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(RoiStatistic);
    // ------------------------------------------------------------------------

    //    static void GetCount(syd::Database * db); // FIXME

  protected:
    RoiStatistic();

  }; // end of class


#pragma db view object(RoiStatistic)
  struct RoiStatistic_count {
#pragma db column("count(" + RoiStatistic::id + ")")
    std::size_t count; };

  /*
  template<class Table>
  class Table_count {
  public:
#pragma db column("count(" + Table::id + ")")
    std::size_t count;
  };
  */

  /*
  static void syd::RoiStatistic::GetCount(syd::Database * db)
  {
    auto db_odb = db->GetODB_DB();
    typedef odb::query<syd::RoiStatistic> query;
    odb::transaction t (db_odb->begin ());
    syd::RoiStatistic_count ec (db_odb->query_value<syd::RoiStatistic_count> (odb::query<syd::RoiStatistic>::id != 0));
    DD(ec.count);
    t.commit ();
  }
  */

  /*  template<>
  class Table_count<RoiStatistic>;

  typedef Table_count<RoiStatistic> R;

#pragma db view object(RoiStatistic)
  template<>
  class Table_count<RoiStatistic> {
  public:
#pragma db column("count(" + RoiStatistic::id + ")")
    std::size_t count;
  };
  */

  //#pragma db member(Table_count<RoiStatistic>::count) column("count(" + RoiStatistic::id + ")")


}
// --------------------------------------------------------------------

#endif
