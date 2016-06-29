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
#include "sydNDimPoints.h"

// --------------------------------------------------------------------
syd::NDimPoints::NDimPoints()
{
  SetPointDimension(1);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::NDimPoints::pointer syd::NDimPoints::New()
{
  return pointer(new syd::NDimPoints);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::SetPointDimension(int d)
{
  nb_dimensions = d;
  values.clear(); // when changing the dim, clear the data !
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::NDimPoints::GetValue(int p, int d) const
{
  return values[p*GetNumberOfDimensions() + d];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::SetValue(double v, int p, int d)
{
  values[p*GetNumberOfDimensions() + d] = v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double * syd::NDimPoints::GetPointer()
{
  return &values[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
//void syd::NDimPoints::push_back(double *v)
double* syd::NDimPoints::push_back()
{
  for(auto i=0; i<GetNumberOfDimensions(); i++)
    values.push_back(0.0);
  return &(values[GetNumberOfPoints()*GetNumberOfDimensions()-GetNumberOfDimensions()]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::Save(std::string filename)
{
  std::ofstream ofs(filename);//, std::ofstream::binary);
  //boost::archive::binary_oarchive oa(ofs);
  //oa << *this;
  ofs << GetNumberOfDimensions() << " "
      << GetNumberOfPoints() << std::endl;
  for(auto & v:values)
    //    for(auto i=0; i<nb_dimensions; i++)
    ofs << v << std::endl;
  ofs.close();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::Load(std::string filename)
{
  std::ifstream ifs(filename);//, std::ifstream::binary);
  //boost::archive::binary_iarchive ia(ifs);
  //ia >> *this;
  ifs >> nb_dimensions;
  int n;
  ifs >> n;
  DD(n);
  values.resize(n*nb_dimensions);
  for(auto & v:values) {
    // double * x = new double[nb_dimensions];
    // for(auto i=0; i<nb_dimensions; i++)
    ifs >> v;//x[i];
    //    v = x;
  }
  ifs.close();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::GetMinMax(std::vector<double> & mins,
                                std::vector<double> & maxs)
{
  int N = GetNumberOfPoints();
  int D = GetNumberOfDimensions();
  mins.resize(D);
  maxs.resize(D);
  for(auto j=0; j<D; j++) {
    mins[j] = std::numeric_limits<double>::max();
    maxs[j] = std::numeric_limits<double>::lowest();
    for(auto i=0; i<N; i++) {
      double v = GetValue(i,j);
      if (v < mins[j]) mins[j] = v;
      if (v > maxs[j]) maxs[j] = v;
    }
  }
}
// --------------------------------------------------------------------
