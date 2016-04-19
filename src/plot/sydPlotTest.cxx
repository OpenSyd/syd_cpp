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

// Python (must be before dcmtk includes to avoid warning)
#include <Python.h>

// syd
#include "sydPlotTest_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydFitModels.h"

// Init syd
SYD_STATIC_INIT

void sydPyRun_SimpleString(const std::string var_name,
                           const std::vector<double> & values)
{
  std::stringstream ss;
  ss << var_name << " = [";
  for(auto v:values) ss <<v << ",";
  ss << "]";
  PyRun_SimpleString(ss.str().c_str());
}

template<typename T>
std::vector<T> arange(T start, T stop, T step = 1)
{
  std::vector<T> values;
  for (T value = start; value < stop; value += step)
    values.push_back(value);
  return values;
}

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydPlotTest, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // start
  Py_Initialize();

  // init
  {
    std::stringstream ss;
    ss << "import numpy as np" << std::endl
       << "import matplotlib"  << std::endl
       << "import platform" << std::endl
       << "if (platform.system() == 'Linux'):" << std::endl
       << "\tmatplotlib.use('Qt5Agg') # only for linux, not on mac" << std::endl
       << "import matplotlib.pyplot as plt" << std::endl
       << "import matplotlib.pyplot as plt" << std::endl
       << "fig, ax = plt.subplots()" << std::endl;
    PyRun_SimpleString(ss.str().c_str());
  }

  // Get tac data
  syd::FitResult::pointer fr;
  db->QueryOne(fr, atoi(args_info.inputs[0]));

  syd::Timepoints::pointer tp = fr->timepoints;

  // data
  sydPyRun_SimpleString("times", tp->times);
  sydPyRun_SimpleString("values", tp->values);
  PyRun_SimpleString("plt.plot(times, values, '-o')");

  // Fit
  double last = tp->times[tp->times.size()-1];
  double l = last-tp->times[0];
  auto times = arange<double>(tp->times[0], last, l/100.0);

  syd::FitModelBase * model;
  if (fr->model_name == "f2") model = new syd::FitModel_f2;
  if (fr->model_name == "f3") model = new syd::FitModel_f3;
  if (fr->model_name == "f4a") model = new syd::FitModel_f4a;
  if (fr->model_name == "f4") model = new syd::FitModel_f4;
  model->SetParameters(fr->params);

  syd::Injection::pointer injection = tp->injection;
  model->SetLambdaPhysicHours(injection->GetLambdaInHours());
  DD(injection);

  std::vector<double> values;
  for(auto t:times) {
    values.push_back(model->GetValue(t));
  }
  syd::TimeActivityCurve tac;
  tp->GetTAC(tac);
  DD(tac);
  DDS(model->GetParameters());
  DD(model->ComputeR2(tac));
  DD(model->GetName());

  // data
  sydPyRun_SimpleString("t", times);
  sydPyRun_SimpleString("v", values);
  std::stringstream label;
  label << tp->mask->roitype->name << " "
        << fr->model_name << " " << fr->id << " " << tp->id;
  std::stringstream ss;
  ss << "plt.plot(t, v, '-', label='" << label.str() << "')";
  PyRun_SimpleString(ss.str().c_str());

  {
    std::stringstream ss;
    ss //<< "plt.tight_layout()" << std::endl
       //<< "plt.savefig('foo.pdf',dpi=180)" << std::endl
      << "plt.legend(loc='upper right', frameon=False)" << std::endl
      << "plt.xlabel('Times from injection in hours')" << std::endl
      << "plt.ylabel('Activity')" << std::endl
      << "plt.show()";
    PyRun_SimpleString(ss.str().c_str());
  }


  // ------------------------------------------------------------------
  // This is the end, my friend.
}
// --------------------------------------------------------------------
