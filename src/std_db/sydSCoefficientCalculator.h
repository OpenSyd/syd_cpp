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
#include "sydPluginManager.h"
#include "sydICRP_RadiationData.h"
#include "sydICRP_SpecificAbsorbedFraction.h"

// --------------------------------------------------------------------
namespace syd {

 class SCoefficientCalculator {
  public:

    SCoefficientCalculator();
    void SetSourceOrgan(std::string s) { mSourceOrganName = s; }
    void SetTargetOrgan(std::string s) { mTargetOrganName = s; }
    void SetRadionuclide(std::string s) { mRadionuclideName = s; }
    void SetPhantomName(std::string s) { mPhantomName = s; }
    std::string ToString() const;
    void Initialise(std::string folder="");
    double Run();

    friend std::ostream& operator<<(std::ostream& os, const SCoefficientCalculator & p) {
      os << p.ToString();
      return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const SCoefficientCalculator * p) {
      os << p->ToString();
      return os;
    }

  protected:
    double GetSCoefficient();
    std::string mSourceOrganName;
    std::string mTargetOrganName;
    std::string mRadionuclideName;
    std::string mPhantomName;

    std::vector<std::shared_ptr<syd::ICRP_SpecificAbsorbedFractionData>> mSAFData;
    std::shared_ptr<syd::ICRP_RadiationData> mRadiationData;
  };

} // end namespace
// --------------------------------------------------------------------
