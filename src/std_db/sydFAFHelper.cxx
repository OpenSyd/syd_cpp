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
  double injectedActivity = input_SPECT->injection->activity_in_MBq; //injected activity in MBq
  double lambdaDecay = input_SPECT->injection->GetLambdaDecayConstantInHours()/3600.0; //lambda decay in 1/s
  double timeInjectionSPECT = input_SPECT->GetHoursFromInjection()*3600.0; //Time between injection and the beginning of the SPECT acquisition in s
  double totalAcquisitionTime = 0.2*3600.0; //Total acquisition time in s

  //Compute A0
  double A0 = injectedActivity*std::exp(-lambdaDecay*timeInjectionSPECT);
  std::cout << "A0 " << A0 << std::endl;
  //Compute the integral between 0 and totalAcquisitionTime of exp(-lambdaDecay*t)
  double integral = (1 - std::exp(-lambdaDecay*totalAcquisitionTime))/lambdaDecay;
  std::cout << "integral " << integral << std::endl;

  return (A0*integral);
}
//--------------------------------------------------------------------

