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

#ifndef SYDHISTOGRAM_H
#define SYDHISTOGRAM_H

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  /// TODO
  class Histogram {

  public:
    /// Constructor.
    Histogram();

    void SetMinMaxWidth(double min, double max, double width);
    void SetMinMaxBins(double min, double max, int nb_bins);
    void Fill(double x);

    std::string ToString() const ;

    friend std::ostream& operator<<(std::ostream& os, const Histogram & h) {
      os << h.ToString();
      return os;
    }

  protected:
    void Init();
    std::vector<int> values;
    double min_value;
    double max_value;
    double width;
    int nb_bins;
    int nb_below_min_limit;
    int nb_above_max_limit;

  }; // class Histogram

} // namespace syd
// --------------------------------------------------------------------

#endif
