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

#ifndef SYDINTEGRATEDACTIVITYIMAGEBUILDER_H
#define SYDINTEGRATEDACTIVITYIMAGEBUILDER_H

// syd
#include "sydStandardDatabase.h"
#include "sydImage.h"
#include "sydTimeActivityCurve.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a pixel-based integrated activity.
  class IntegratedActivityImageBuilder {

  public:
    /// Constructor.
    IntegratedActivityImageBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~IntegratedActivityImageBuilder() {}

    void SetInput(syd::Image::vector & images);
    syd::Image::pointer GetOutput() const;
    void CreateIntegratedActivityImage();
    double Integrate();

    syd::TimeActivityCurve & GetTAC() { return current_tac_; }

    void SaveDebugPixel(const std::string & filename) const;
    void AddDebugPixel(int x, int y, int z);

  protected:
    syd::StandardDatabase * db_;

    syd::Image::vector images_;
    syd::Image::pointer output_;
    syd::TimeActivityCurve current_tac_;

    std::vector<int> debug_pixels;
    std::vector<syd::TimeActivityCurve> debug_tac;


  }; // class IntegratedActivityImageBuilder

#include "sydIntegratedActivityImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
