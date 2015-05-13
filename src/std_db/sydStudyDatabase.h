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

#ifndef SYDSTUDYDATABASE_H
#define SYDSTUDYDATABASE_H

// syd
#include "sydClinicDatabase.h"
#include "sydTimepoint-odb.hxx"
#include "sydTag-odb.hxx"

// --------------------------------------------------------------------
namespace syd {

  /// FIXME
  class StudyDatabase: public ClinicDatabase {
  public:

    /// Insert a new element. Special case for some tables (Injection)
    virtual TableElement * InsertFromArg(const std::string & table_name, std::vector<std::string> & arg);

    /// Insert a new Timepoint
    syd::Timepoint InsertTimepoint(syd::Injection & injection,
                                   syd::Tag & tag,
                                   std::vector<syd::DicomSerie> & dicoms,
                                   bool replaceTimepointFlag);

    /// Specific Dump for Timepoint
    virtual void DumpTimepoint(const std::vector<std::string> & args, std::ostream & os, bool verboseFlag=false);

  protected:
    /// Insert the tables
    virtual void CreateTables();


  }; // class StudyDatabase

} // namespace syd

// --------------------------------------------------------------------

#endif
