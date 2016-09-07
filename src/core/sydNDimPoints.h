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
    size_t GetNumberOfPoints() const { return values.size()/nb_dimensions; }
    void SetNumberOfPoints(int n);
    void clear() { values.clear(); }

    // Add an empty ND points
    double * push_back();

    // Return point p, dimension d
    double GetValue(int p, int d) const;

    // Return raw pointer
    double * GetPointer();

    // Set the value point p dim d
    void SetValue(double v, int p, int d);

    // Compute min max value by dim
    void GetMinMax(std::vector<double> & mins, std::vector<double> & maxs);

    void ComputeMedians(std::vector<double> & medians) const;
    void ComputeMeans(std::vector<double> & means) const;
    void ComputeMedianAbsDeviations(const std::vector<double> & medians,
                                    std::vector<double> & mads) const;
    void Rescale(const std::vector<double> & inputMin,
                 const std::vector<double> & inputMax,
                 const double outputMin,
                 const double outputMax);

    void Save(std::string filename);
    void Load(std::string filename);

  protected:
    std::vector<double> values;
    int nb_dimensions;

  }; // class NDimPoints

} // namespace syd
// --------------------------------------------------------------------

#endif
