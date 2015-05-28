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

#ifndef SYDTESTUTILS_H
#define SYDTESTUTILS_H

// syd
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydDatabase.h"

//--------------------------------------------------------------------
namespace syd {

  void TestInitialisation();

  void TestCreateReferenceDB(int argc, char* argv[], syd::Database * db, std::string file, std::string folder);

#include "sydTestUtils.txx"

} // end namespace

#endif // end #define SYDTESTUTILS_H
