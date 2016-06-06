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
  for(auto v:values) delete v;
  values.clear();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::push_back(double *v)
{
  values.push_back(v);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::Save(std::string filename)
{
  std::ofstream ofs(filename);
  boost::archive::binary_oarchive oa(ofs);
  oa << *this;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::NDimPoints::Load(std::string filename)
{
  std::ifstream ifs(filename);
  boost::archive::binary_iarchive ia(ifs);
  ia >> *this;
}
// --------------------------------------------------------------------
