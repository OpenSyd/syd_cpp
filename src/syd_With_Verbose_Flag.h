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

#ifndef SYD_CLASS_WITH_VERBOSE_FLAG_H
#define SYD_CLASS_WITH_VERBOSE_FLAG_H

// syd
#include "sydCommon.h"

namespace syd {

  // --------------------------------------------------------------------
  class With_Verbose_Flag {

  public:
    With_Verbose_Flag();

    void Set_VerboseFlag(bool b);
    bool Get_VerboseFlag() const;

  protected:
    bool m_VerboseFlag;

  };
  // --------------------------------------------------------------------

} // end namespace

#endif
