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
#include "sydDD.h"
#include "sydLog.h"
using namespace sydlog;

#include <memory>
#include <map>
#include <vector>

// --------------------------------------------------------------------
namespace syd {

  // For a given source-target, store the list of energies and SAF values
  class ICRP_SpecificAbsorbedFraction {
  public:

    ICRP_SpecificAbsorbedFraction(std::vector<double> & v);
    double Compute(double energy);

    std::vector<double> & mEnergies;
    std::vector<double> mSAFvalues;

  };

  /*
    Store all the data for every couple source-target.
   */
  class ICRP_SpecificAbsorbedFractionData {
  public:

    void Read(std::string filename);
    std::shared_ptr<ICRP_SpecificAbsorbedFraction> Get(std::string source, std::string target);

    typedef std::map<std::string, std::shared_ptr<ICRP_SpecificAbsorbedFraction>> TargetMapType;
    typedef std::map<std::string, TargetMapType> SourceMapType;
    SourceMapType mSourcesMap;
    std::vector<double> mEnergies;

  };


} // end namespace
// --------------------------------------------------------------------
