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

#ifndef SYD_ROI_PEAK_COMMAND_H
#define SYD_ROI_PEAK_COMMAND_H

// syd
#include "sydCommand.h"

// clitk
#include <clitkImageCommon.h>

namespace syd {

  // --------------------------------------------------------------------
  class ROI_Peak_Command: public syd::Command {

  public:
    ROI_Peak_Command(sydQuery & db);

    // Image types
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef unsigned char MaskPixelType;
    typedef itk::Image<MaskPixelType, 3> MaskImageType;

    void SetArgs(char ** inputs, int n);
    void Run();
    void Run(RoiStudy roistudy);

  protected:
    std::vector<RoiStudy> roistudies;
    double m_Gaussian_Variance;
    double m_Peak_Volume_In_CC;
    ImageType::Pointer m_CachedAASpect;
    unsigned long m_PreviousAASpectStudyId;

  };
  // --------------------------------------------------------------------

} // end namespace

#endif
