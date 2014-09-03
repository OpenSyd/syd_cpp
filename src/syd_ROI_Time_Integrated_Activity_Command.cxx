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
#include "syd_ROI_Time_Integrated_Activity_Command.h"

// --------------------------------------------------------------------
syd::ROI_Time_Integrated_Activity_Command::
ROI_Time_Integrated_Activity_Command(sydQuery & _db):Command(_db)
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ROI_Time_Integrated_Activity_Command::
SetArgs(char ** inputs, int n)
{
  // FIXME check nb of args
  DD(n);

  // Get all roistudies (patient / study=all / roi)
  db.GetRoiStudies(inputs[0], "all", inputs[1], roistudies);

  // Get parameters for Fit
  //FIXME

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ROI_Time_Integrated_Activity_Command::
Run()
{
  for(auto i=roistudies.begin(); i<roistudies.end(); i++) Run(*i);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ROI_Time_Integrated_Activity_Command::
Run(RoiStudy roistudy)
{
   // Get all objects associated with this roistudy
  Study study = db.GetById<Study>(roistudy.StudyId);
  Patient patient = db.GetById<Patient>(study.PatientId);
  RoiType roitype = db.GetById<RoiType>(roistudy.RoiTypeId);

  DD(roistudy);
  DD(study);
  DD(patient);
  DD(roitype);

}
// --------------------------------------------------------------------
