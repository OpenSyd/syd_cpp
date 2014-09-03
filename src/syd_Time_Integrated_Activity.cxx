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

#include "syd_Time_Integrated_Activity.h"

// --------------------------------------------------------------------
syd::Time_Integrated_Activity::
Time_Integrated_Activity():With_Verbose_Flag()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Time_Integrated_Activity::
Set_Data(const std::vector<double> & _times,
         const std::vector<double> & _activities,
         const std::vector<double> & _std)
{
  times = &_times;
  activities = &_activities;
  std = &_std;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Time_Integrated_Activity::Integrate()
{
  DDS(*times);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::Time_Integrated_Activity::Get_Integrated_Activity()
{
  return 666;
}
// --------------------------------------------------------------------
