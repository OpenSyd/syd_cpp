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

#ifndef SYDUPDATERADIONUCLIDEFILTER_H
#define SYDUPDATERADIONUCLIDEFILTER_H

// syd
#include "sydImageBuilder.h"

// --------------------------------------------------------------------
namespace syd {

  /// Update or create Radionuclide
  class UpdateRadionuclideFilter: public syd::DatabaseFilter {

  public:
    /// Constructor.
    UpdateRadionuclideFilter(syd::StandardDatabase * db);

    /// Set the url + path where to find the data file
    /// (www.nucleide.org, /DDEP_WG/Nuclides/)
    void SetURL(const std::string & url, const std::string & path);

    syd::Radionuclide::vector Update(const std::vector<std::string> & rad_names);
    syd::Radionuclide::pointer Update(const std::string & rad_name);
    syd::Radionuclide::pointer GetOrCreate(const std::string & rad_name);
    void GetDataFromWeb(syd::Radionuclide::pointer rad);

  protected:
    std::string url_;
    std::string path_;

  }; // class UpdateRadionuclideFilter

} // namespace syd
// --------------------------------------------------------------------

#endif
