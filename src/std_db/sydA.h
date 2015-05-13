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

#ifndef SYDA_H
#define SYDA_H

// syd
#include "sydCommon.h"
#include "sydPrintTable.h"

// --------------------------------------------------------------------
namespace syd {

  //#pragma db model version(1, 3)

#pragma db object
  class A  {
  public:

#pragma db id auto
    IdType        id;
    std::string   name;

    A();

  }; // end of class

  // class B : public A {
  // public:

  //   std::string date;

  // }; // end of class


}
// --------------------------------------------------------------------

#endif
