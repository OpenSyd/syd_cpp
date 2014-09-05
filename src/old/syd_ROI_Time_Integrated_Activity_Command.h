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

#ifndef SYD_ROI_TIME_INTEGRATED_ACTIVITY_COMMAND_H
#define SYD_ROI_TIME_INTEGRATED_ACTIVITY_COMMAND_H

// syd
#include "sydCommand.h"

namespace syd {

  // --------------------------------------------------------------------
  class ROI_Time_Integrated_Activity_Command: public syd::Command {

  public:
    ROI_Time_Integrated_Activity_Command(sydQuery & db);

    void SetArgs(char ** inputs, int n);
    void Run();
    void Run(RoiStudy roistudy);

  protected:
    std::vector<RoiStudy> roistudies;
    int m_Nb_Of_Points_For_Fit;
  };
  // --------------------------------------------------------------------

} // end namespace

#endif
