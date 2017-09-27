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

// std
#include <map>
#include <vector>

// --------------------------------------------------------------------
namespace syd {

  class ICRP_RadiationTypeData
  {
  public:
    int mId;
    // std::string mTypeName;
    double mYield;
    double mEnergy;
    /* ICODE radiation type
     1 G, PG, DG (Gamma rays)
     2 X
     3 AQ Annihilation photons
     4 B+ Beta + particles
     5 B- BD Beta – particles Delayed beta particles*
     6 IE IC electrons
     7 AE Auger electrons
     8 A Alpha particles
     9 AR Alpha recoil nuclei
     10 FF Fission fragments
     11 N Neutrons
    */
  };

 class ICRP_RadiationData {
  public:
   void Read(std::string filename);
    const std::vector<ICRP_RadiationTypeData> & GetData(std::string rad);
    typedef std::map<std::string, std::vector<ICRP_RadiationTypeData>> ValuesMapType;
    ValuesMapType mRadiationMap;
  };


} // end namespace
// --------------------------------------------------------------------
