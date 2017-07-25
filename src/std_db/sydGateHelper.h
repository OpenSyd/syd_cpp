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
  void GateCreateMacroFile(std::string mac_filename,
                                  syd::Image::pointer ct,
                                  syd::Image::pointer source,
                                  syd::Radionuclide::pointer rad,
                                  int N,
                                  std::string output);

  /// Run Gate simulations
  std::string GateRun(std::string folder,
                      std::string mac_filename,
                      int nb_thread,
                      std::string & error_output,
                      std::string & output);

  /// Get the simulation name from a folder results.XXYZ
  std::string GateGetSimulationNameFromFolder(std::string folder);

  /// Retrieve Gate output images
  syd::Image::vector GateInsertOutputImages(std::string folder,
                                            syd::Image::pointer source);

  /// Parse the filename and retrive the type (edep, dose etc)
  std::string GateGetFileType(std::string filename);

  /// Create an image from a mhd Gate output file
  syd::Image::pointer GateInsertImage(std::string filename,
                                      syd::Image::pointer source);

  /// Compute the scaling factor for a dose image, according to input activity
  /// and nb of events in the simulations. Results will be in Gy by injected
  /// MBq.
  double GateComputeDoseScalingFactor(syd::Image::pointer source, double nb_events);

  /// Read Gate output folder and search for stat file
  syd::File::pointer GateInsertStatFile(std::string folder_name, syd::Patient::pointer patient);

  /// From a stat file, determine the NumberOfEvents
  double GateGetNumberOfEvents(syd::File::pointer stat_file);

} // namespace syd

// --------------------------------------------------------------------

#endif
