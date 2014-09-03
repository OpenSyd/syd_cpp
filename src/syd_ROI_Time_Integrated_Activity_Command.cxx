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
#include "syd_Time_Integrated_Activity.h"

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
  DD(roistudies.size());

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
  DD(roistudy);
  // Get all roiseries for this study
  std::vector<RoiSerie> roiseries;
  db.Get_RoiSeries_Sorted_by_Time(roistudy, roiseries);

  // Get times / activities values
  // (LATER : read from a different db ?)
  std::vector<double> times;
  std::vector<double> activities;
  std::vector<double> std;
  for(auto i=roiseries.begin(); i<roiseries.end(); i++) {
    double t = db.GetById<Serie>(i->SerieId).TimeFromInjectionInHours;
    times.push_back(t);
    activities.push_back(i->MeanActivity); // mean activity in the ROI //FIXME
    std.push_back(i->StdActivity);  // std deviation activity in the ROI
  }

  // Compute the integrated activity
  syd::Time_Integrated_Activity a;
  a.Set_Data(times, activities, std);
  // a->Set_Number_Of_Points_For_Final_Fit(); // FIXME integrate / fit options
  a.Integrate();

  // Update the db (now : the same db, @LATER@ : a specific result db
  /*ResultRoiStudy r = db_results.Get_ResultRoiStudy(roistudy);
    r.IntegratedActivity = 0.0; // etc
    db_results.Update(r);
  */
  roistudy.CumulatedActivity = a.Get_Integrated_Activity();
  DD(roistudy.CumulatedActivity);

  // Update
  db.Update<RoiStudy>(roistudy);

  // Verbose
  if (GetVerboseFlag()) {
    Study study = db.GetById<Study>(roistudy.StudyId);
    Patient patient = db.GetById<Patient>(study.PatientId);
    RoiType roitype = db.GetById<RoiType>(roistudy.RoiTypeId);
    std::cout << patient.SynfrizzId << " " << study.Number << " "
              << roitype.Name << " "
              << roistudy.CumulatedActivity << " "
              << roistudy.FitLambda << " "
              << roistudy.FitA << " "
              << roistudy.FitRMS << " "
              << roistudy.FitPeakTime << std::endl;
  }
}
// --------------------------------------------------------------------
