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
#include "sydPyPlotBuilder.h"

// --------------------------------------------------------------------
syd::PyPlotBuilder::PyPlotBuilder()
{
  Initialize();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PyPlotBuilder::Initialize()
{
  current_curve_nb_ = 0;
  script_.str("");
  script_ << "import numpy as np" << std::endl
          << "import matplotlib"  << std::endl
          << "import platform" << std::endl
          << "if (platform.system() == 'Linux'):" << std::endl
          << "\tmatplotlib.use('Qt5Agg') # only for linux, not on mac" << std::endl
          << "import matplotlib.pyplot as plt" << std::endl
          << "import matplotlib.pyplot as plt" << std::endl
          << "fig, ax = plt.subplots()" << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PyPlotBuilder::Add(const std::string & line)
{
  script_ << line << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PyPlotBuilder::AddCurve(const std::vector<double> & x,
                                  const std::vector<double> & y,
                                  const std::string & plot_type,
                                  const std::string & label)
{
  script_ << "curve_" << current_curve_nb_ << "_x = [";
  for(auto v:x) script_ << v << ",";
  script_ << "]" << std::endl;
  script_ << "curve_" << current_curve_nb_ << "_x = [";
  for(auto v:y) script_ << v << ",";
  script_ << "]" << std::endl;
  script_ << "plt.plot("
     << "curve_" << current_curve_nb_ << "_x,"
     << "curve_" << current_curve_nb_ << "_y, "
     << plot_type << ", " << label << ")" << std::endl;
  DD(script_.str());
  ++current_curve_nb_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::PyPlotBuilder::GetOutput() const
{
  return script_.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PyPlotBuilder::Show()
{
  DD("show");
  script_ << "plt.legend(loc='upper right', frameon=False)" << std::endl
          <<  "plt.show()" << std::endl;
  PyRun_SimpleString(script_.str());
}
// --------------------------------------------------------------------
