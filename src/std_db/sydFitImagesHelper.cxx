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

// syd
#include "sydFitImagesHelper.h"

// --------------------------------------------------------------------
syd::Image::pointer syd::NewOutputFitImages(const syd::FitImages::pointer tia,
                                            const std::string fit_output_name)
{
  DDF();
  DD(tia);
  DD(fit_output_name);

  auto params = tia->GetOutput("fit_p");
  auto success = tia->GetOutput("fit_1");
  auto models = tia->GetOutput("fit_mo");
  DD(params);
  DD(success);
  DD(models);

  return nullptr;
}
// --------------------------------------------------------------------
