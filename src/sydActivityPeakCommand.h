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

#ifndef SYDACTIVITYPEAKCOMMAND_H
#define SYDACTIVITYPEAKCOMMAND_H

// syd
#include "sydActivityCommandBase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class ActivityPeakCommand: public syd::ActivityCommandBase
  {
  public:

    ActivityPeakCommand();
    ~ActivityPeakCommand();

    void set_mean_radius(double v) { mean_radius_ = v;}

  protected:
    virtual void SetOptions(args_info_sydActivity & args_info);
    virtual void Run(const Patient & p, const RoiType & roitype, std::vector<std::string> & args);
    double mean_radius_;

  };

}  // namespace syd
// --------------------------------------------------------------------

#endif
