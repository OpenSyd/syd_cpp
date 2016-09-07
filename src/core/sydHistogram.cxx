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
#include "sydHistogram.h"

// --------------------------------------------------------------------
syd::Histogram::Histogram::Histogram()
{
  SetMinMaxBins(0,1,100);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Histogram::SetMinMaxWidth(double vmin, double vmax, double vwidth)
{
  min_value = vmin;
  max_value = vmax;
  width = vwidth;
  nb_bins = (max_value-min_value)/width;
  Init();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Histogram::SetMinMaxBins(double vmin, double vmax, int vnb_bins)
{
  min_value = vmin;
  max_value = vmax;
  nb_bins = vnb_bins;
  width = (max_value-min_value)/(double)nb_bins;
  Init();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Histogram::Fill(double x)
{
  if (x < min_value) {
    ++nb_below_min_limit;
    return;
  }
  if (x > max_value) {
    ++nb_below_min_limit;
    return;
  }
  if (x == max_value) { // special case: bounds min and max are included
    ++values.back();
    return;
  }
  int bin = (int)floor((double)(x-min_value)/(double)width);
  ++values[bin];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Histogram::Init()
{
  values.resize(nb_bins);
  nb_below_min_limit = 0;
  nb_above_max_limit = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Histogram::ToString() const
{
  std::stringstream ss;
  ss << "h " << min_value << " "
     << max_value << " "
     << nb_bins << " "
     << width << std::endl;
  int n=0;
  for(auto v:values) {
    ss << min_value+n*width << " "
       << min_value+(n+1)*width
       << " -> " << v << std::endl;
    ++n;
  }
  ss << "out " << nb_below_min_limit << " " << nb_above_max_limit << std::endl;
  return ss.str();
}
// --------------------------------------------------------------------
