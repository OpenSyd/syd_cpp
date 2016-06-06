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

    /// types
    typedef std::shared_ptr<NDimPoints> pointer;
    typedef std::vector<pointer> vector;
    static pointer New();

    void SetPointDimension(int d);
    int GetNumberOfDimensions() const { return nb_dimensions; }

    void push_back(double *v);
    typedef std::vector<double*> T;
    T::iterator begin() { return values.begin(); }
    T::iterator end() { return values.end(); }
    T::const_iterator begin() const { return values.begin(); }
    T::const_iterator end() const { return values.end(); }
    size_t size() const { return values.size(); }

    void Save(std::string filename);
    void Load(std::string filename);

  protected:
    std::vector<double*> values;
    int nb_dimensions;


    // For serialization
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int version);
    template<class Archive>
    void load(Archive & ar, const unsigned int version);

    BOOST_SERIALIZATION_SPLIT_MEMBER()
  }; // class NDimPoints

#include "sydNDimPoints.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
