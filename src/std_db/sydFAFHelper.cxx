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
#include "sydFAFHelper.h"

//--------------------------------------------------------------------
double syd::ComputeFafIntegral(const syd::Image::pointer input_SPECT)
{
  //Get the different important value
  double injectedActivity = input_SPECT->injection->activity_in_MBq*1000000.0; //injected activity in Bq
  double lambdaDecay = input_SPECT->injection->GetLambdaDecayConstantInHours(); //lambda decay in 1/h
  double timeInjectionSPECT = input_SPECT->GetHoursFromInjection(); //Time between injection and the beginning of the SPECT acquisition in h

  return (0);
}
//--------------------------------------------------------------------

