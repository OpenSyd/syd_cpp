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

#ifndef SYDCHECKRESULT_H
#define SYDCHECKRESULT_H

// --------------------------------------------------------------------
namespace syd {

  class CheckResult {
  public:
    CheckResult() { success = true; description = ""; }
    bool success;
    std::string description;
    void merge(const syd::CheckResult & c) {
      success = success and c.success;
      if (c.description != "")
        description = description+" "+c.description;
    }
  };

} // end namespace
// --------------------------------------------------------------------

#endif
