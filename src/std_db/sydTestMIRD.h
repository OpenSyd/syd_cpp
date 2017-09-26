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

  class SpecificAbsorbedFraction {
  public:

    SpecificAbsorbedFraction(std::vector<double> & v);
    double Compute(double energy);

    std::vector<double> & mEnergies;
    std::vector<double> mSAFvalues;

  };

  class SpecificAbsorbedFractionData {
  public:

    void Read(std::string filename);
    std::shared_ptr<SpecificAbsorbedFraction>
      Get(std::string source, std::string target);

    typedef std::map<std::string, std::shared_ptr<SpecificAbsorbedFraction>> TargetMapType;
    typedef std::map<std::string, TargetMapType> SourceMapType;
    SourceMapType mSourcesMap;
    std::vector<double> mEnergies;

  };

  class RadiationTypeData
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

 class RadiationData {
  public:
    void Read();
    const std::vector<RadiationTypeData> & GetData(std::string rad);
    typedef std::map<std::string, std::vector<RadiationTypeData>> ValuesMapType;
    ValuesMapType mRadiationMap;
  };


 class AbsorbedDoseMIRDCalculator {
  public:

    AbsorbedDoseMIRDCalculator();
    void SetActivity(double a) { mActivity_in_MBq = a; }
    void SetSourceOrgan(std::string s) { mSourceOrganName = s; }
    void SetTargetOrgan(std::string s) { mTargetOrganName = s; }
    void SetRadionuclide(std::string s) { mRadionuclideName = s; }
    void SetPhantomName(std::string s) { mPhantomName = s; }

    std::string ToString() const;
    void Run();

    double GetSCoefficient();

    friend std::ostream& operator<<(std::ostream& os, const AbsorbedDoseMIRDCalculator & p) {
      os << p.ToString();
      return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const AbsorbedDoseMIRDCalculator * p) {
      os << p->ToString();
      return os;
    }


  protected:
    double mActivity_in_MBq;
    std::string mSourceOrganName;
    std::string mTargetOrganName;
    std::string mRadionuclideName;
    std::string mPhantomName;

    std::vector<std::shared_ptr<syd::SpecificAbsorbedFractionData>> mSAFData;
    std::shared_ptr<syd::RadiationData> mRadiationData;
  };




} // end namespace
// --------------------------------------------------------------------
