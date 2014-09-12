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

#ifndef SYDDATABASECOMMAND_H
#define SYDDATABASECOMMAND_H

// syd
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  class DatabaseCommand
  {
  public:

    virtual void AddDatabase(Database * db);
    virtual void SetArgs(char ** inputs, int n) = 0;
    virtual void Run() = 0;

  protected:
    std::vector<syd::Database*> databases_;

  };

} // end namespace
// --------------------------------------------------------------------

#endif
