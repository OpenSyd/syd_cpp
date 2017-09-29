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

#ifndef SYDSCOEFFICIENTCALCULATOR_H
#define  SYDSCOEFFICIENTCALCULATOR_H

// syd
#include "sydPluginManager.h"
#include "sydICRP_RadiationData.h"
#include "sydICRP_SpecificAbsorbedFraction.h"

// --------------------------------------------------------------------
namespace syd {

  class SCoefficientCalculator {
  public:

    /// Define pointer type
    typedef std::shared_ptr<SCoefficientCalculator> pointer;

    /// Constructor
    SCoefficientCalculator();

    /// Set the name of the source organ
    void SetSourceOrgan(std::string s) { mSourceOrganName = s; }

    /// Set the name of the target organ
    void SetTargetOrgan(std::string s) { mTargetOrganName = s; }

    /// Set the name of the radionuclide
    void SetRadionuclide(std::string s) { mRadionuclideName = s; }

    /// Set the name of the phantom (AF or AM, Adult Female/Male)
    void SetPhantomName(std::string s);
    std::string GetPhantomName() const { return mPhantomName; }

    /// Return the list of source organs
    std::vector<std::string> GetListOfSourceOrgans();

    /// Return the list of target organs
    std::vector<std::string> GetListOfTargetOrgans();

    /// To print as a string
    std::string ToString() const;

    /// Read ICRP data
    void Initialise(std::string folder="");

    /// Compute the S coefficients
    double Run();

    /// Print streaming
    friend std::ostream& operator<<(std::ostream& os, const SCoefficientCalculator & p) {
      os << p.ToString();
      return os;
    }

    /// Print streaming
    friend std::ostream& operator<<(std::ostream& os, const SCoefficientCalculator * p) {
      os << p->ToString();
      return os;
    }

  protected:
    std::string mSourceOrganName;
    std::string mTargetOrganName;
    std::string mRadionuclideName;
    std::string mPhantomName;

    /// ICRP radiation data
    std::shared_ptr<syd::ICRP_RadiationData> mRadiationData;

    /// ICRP vector of SAF for all radiation type (photon, electron etc)
    std::vector<std::shared_ptr<syd::ICRP_SpecificAbsorbedFractionData>> mSAFData;

    /// Cache for list of organs
    std::vector<std::string> mListOfSourceOrgansNames;
    std::vector<std::string> mListOfTargetOrgansNames;
  };

} // end namespace
// --------------------------------------------------------------------


#endif // SYDSCOEFFICIENTCALCULATOR_H
