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

#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydActivityDatabase.h"

namespace syd {

  // static
  std::map<std::string, syd::FunctionType> syd::DatabaseFactory::factoryFunctionRegistry;
  std::map<std::string, std::string> syd::DatabaseFactory::map_of_database_types_;
  std::map<std::string, std::string> syd::DatabaseFactory::map_of_database_param_;

  // Type of DB
  SYD_DECLARE_DATABASE_TYPE(ClinicDatabase);
  SYD_DECLARE_DATABASE_TYPE(StudyDatabase);
  SYD_DECLARE_DATABASE_TYPE(ActivityDatabase);

} // namespace syd
