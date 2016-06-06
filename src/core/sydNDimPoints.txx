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

// --------------------------------------------------------------------
template<class Archive>
void NDimPoints::save(Archive & ar, const unsigned int version) {
  ar & nb_dimensions;
  int n = values.size();
  ar & n;
  for(auto v:values)
    for(auto i=0; i<nb_dimensions; i++)
      ar & v[i];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class Archive>
void NDimPoints::load(Archive & ar, const unsigned int version) {
  ar & nb_dimensions;
  int n;
  ar & n;
  values.resize(n);
  for(auto & v:values) {
    double * x = new double[nb_dimensions];
    for(auto i=0; i<nb_dimensions; i++) ar & x[i];
    v = x;
  }
}
// --------------------------------------------------------------------
