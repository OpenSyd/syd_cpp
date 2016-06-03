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

#ifndef SYDNDIMPOINTS_H
#define SYDNDIMPOINTS_H

// syd
#include "sydCommon.h"


#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

// --------------------------------------------------------------------
namespace syd {

  /// TODO
  class NDimPoints {

  public:
    /// Constructor.
    NDimPoints();

    void SetPointDimension(int d);
    int GetNumberOfDimensions() const { return nb_dimensions; }

    void push_back(double *v);
    typedef std::vector<double*> T;
    T::iterator begin() { return values.begin(); }
    T::iterator end() { return values.end(); }
    size_t size() const { return values.size(); }

    void Save(std::string filename);
    void Load(std::string filename);

  protected:
    std::vector<double*> values;
    int nb_dimensions;

    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int version) {
      ar & nb_dimensions;
      int n = values.size();
      ar & n;
      for(auto v:values)
        for(auto i=0; i<nb_dimensions; i++)
          ar & v[i];
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version) {
      ar & nb_dimensions;
      int n;
      ar & n;
      DD(n);
      values.resize(n);
      for(auto & v:values) {
        double * x = new double[nb_dimensions];
        for(auto i=0; i<nb_dimensions; i++) ar & x[i];
        v = x;
      }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
  }; // class NDimPoints

} // namespace syd
// --------------------------------------------------------------------

#endif
