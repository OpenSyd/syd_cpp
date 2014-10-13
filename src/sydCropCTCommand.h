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

#ifndef SYDCROPCTCOMMAND_H
#define SYDCROPCTCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydDicomCommon.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  class CropCTCommand: public syd::DatabaseCommand
  {
  public:

    CropCTCommand(std::string d);
    CropCTCommand(StudyDatabase * d);
    ~CropCTCommand();

    virtual void Run(std::string patient_name, const std::vector<std::string> & arg);

    void set_ignore_md5_flag(bool b) { ignore_md5_flag_ = b; }
    bool get_ignore_md5_flag() const { return ignore_md5_flag_; }

  protected:
    void Initialization();
    void Run(const Timepoint & t);
    bool ignore_md5_flag_;

    std::shared_ptr<syd::ClinicDatabase> cdb_;
    std::shared_ptr<syd::StudyDatabase>  sdb_;

  };


} // end namespace
// --------------------------------------------------------------------

#endif
