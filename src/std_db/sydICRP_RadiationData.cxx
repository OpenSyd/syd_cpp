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
#include "sydICRP_RadiationData.h"
#include "sydException.h"

// std
#include <fstream>

// --------------------------------------------------------------------
void syd::ICRP_RadiationData::Read(std::string filename)
{
  std::ifstream is(filename);
  if (!is) {
    EXCEPTION("Error, cannot open " << filename);
  }
  std::string rad;
  int n;
  std::string hl;
  double e;
  double y;
  std::string type;
  while (is >> rad) {
    is >> hl; // half life (as string because unit)
    // Read nb of values
    is >> n;
    std::vector<ICRP_RadiationTypeData> radiations(n);
    for(auto i=0; i<n; ++i) {
      ICRP_RadiationTypeData d;
      is >> d.mId;
      is >> d.mYield;
      is >> d.mEnergy;
      is >> type;
      radiations[i] = d;
    }
    mRadiationMap[rad] = radiations;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
const std::vector<syd::ICRP_RadiationTypeData> &
syd::ICRP_RadiationData::GetData(std::string rad)
{
  DDF();
  auto search = mRadiationMap.find(rad);
  if (search == mRadiationMap.end()) {
    LOG(FATAL) << "Cannot find radiation data for " << rad;
  }
  return search->second;
}
// --------------------------------------------------------------------
