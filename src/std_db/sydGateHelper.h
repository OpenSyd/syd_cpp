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

#ifndef SYDGATEHELPER_H
#define SYDGATEHELPER_H

// syd
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  /// Manage a Gate macro alias
  class GateAlias {
  public:
    typedef std::shared_ptr<GateAlias> pointer;
    std::string alias;
    std::string value;
    std::string GetMacro() const {
      std::ostringstream oss;
      oss << "/control/alias " << alias << " " << value;
      return oss.str();
    }
  };

  /// Add a new alias
  GateAlias::pointer AddAlias(std::vector<GateAlias::pointer> & aliases, std::string alias_name);

  /// Create a macro from the template
  std::string CreateGateMacroFile(std::string mac_filename,
                                  syd::Image::pointer ct,
                                  syd::Image::pointer source,
                                  syd::Radionuclide::pointer rad,
                                  int N,
                                  std::string output="");

  /// Run Gate simulations
  std::string RunGate(std::string folder,
                      std::string mac_filename,
                      int nb_thread,
                      std::string & error_output,
                      std::string & output);

} // namespace syd

// --------------------------------------------------------------------

#endif
