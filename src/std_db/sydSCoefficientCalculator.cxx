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
#include "sydSCoefficientCalculator.h"

// std
#include <sstream>

// boost
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


// --------------------------------------------------------------------
syd::SCoefficientCalculator::SCoefficientCalculator()
{
  mRadiationData = nullptr;
  mSAFData.clear();
  SetPhantomName("am");
  SetSourceOrgan("Liver");
  SetTargetOrgan("Liver");
  SetRadionuclide("Y-90");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SCoefficientCalculator::SetPhantomName(std::string s)
{
  mPhantomName = s;
  // lower case
  std::transform(mPhantomName.begin(), mPhantomName.end(), mPhantomName.begin(), ::tolower);
  if (mPhantomName != "am" and mPhantomName != "af") {
    EXCEPTION("Dont know phantom: " << mPhantomName << " (use only 'am' or 'af', adult male/female).");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SCoefficientCalculator::Initialise(std::string folder)
{
  if (folder == "") {
    char * env = getenv ("SYD_PLUGIN");
    if (!env) {
      EXCEPTION("Could not find SYD_PLUGIN. Please set this variable to the folder to look for ICRP data. (ICRP_133_SAF and ICRP_2017_Nuclear_Data folders)");
    }
    std::vector<std::string> ll;
    std::string senv(env);
    std::stringstream ss (senv);
    std::string tok;
    char delimiter = ':';
    while(std::getline(ss, tok, delimiter)) {
      ll.push_back(tok);
    }
    folder = ll[0];
  }

  // Read radionuclide yields data
  fs::path rad_path(folder);
  rad_path = rad_path/"ICRP_2017_Nuclear_Data"/"ICRP-07.RAD";
  mRadiationData = std::make_shared<syd::ICRP_RadiationData>();
  mRadiationData->Read(rad_path.string());
  LOG(5) << "Reading Radiation data in " << rad_path.string();

  // Prepare SAF filenames
  fs::path saf_folder = fs::path(folder)/"ICRP_133_SAF";
  mSAFData.resize(8);
  mSAFData[0] = nullptr; // No data with ICODE==0
  fs::path saf_path(folder);
  std::string filename = "rcp-am_photon_2016-08-12.SAF";
  if (mPhantomName == "af") syd::Replace(filename, "rcp-am_", "rcp-af_");
  saf_path = saf_folder/filename;

  // Read SAF data (photon)
  saf_path = saf_folder/filename;
  LOG(5) << "Reading SAF data in " << saf_path.string();
  auto saf = std::make_shared<syd::ICRP_SpecificAbsorbedFractionData>();
  saf->Read(saf_path.string());
  mSAFData[1] = saf; // G PG DG (Gamma rays, Prompt Gamma rays, Delayed Gamma rays)
  mSAFData[2] = saf; // X (X rays)
  mSAFData[3] = saf; // AQ (Annihilation photons)

  // Read SAF data (electron)
  syd::Replace(filename, "photon", "electron");
  saf_path = saf_folder/filename;
  LOG(5) << "Reading SAF data in " << saf_path.string();
  saf = std::make_shared<syd::ICRP_SpecificAbsorbedFractionData>();
  saf->Read(saf_path.string());
  mSAFData[4] = saf; // B+ (Beta+ particles
  mSAFData[5] = saf; // B- (Beta- particles, Delayed Beta particles)
  mSAFData[6] = saf; // IE (IC electrons)
  mSAFData[7] = saf; // AE (Auger electrons)

  // FIXME no auger + neutron yet

  // Read mass
  filename = "sregions_2016-08-12.NDX";
  saf_path = saf_folder/filename;
  LOG(5) << "Reading source mass data in " << saf_path.string();
  saf = std::make_shared<syd::ICRP_SpecificAbsorbedFractionData>();
  saf->ReadSourceMass(saf_path.string());
  mSourceMass_AM = saf->mSourceMass_AM;
  mSourceMass_AF = saf->mSourceMass_AF;

  filename = "torgans_2016-08-12.NDX";
  saf_path = saf_folder/filename;
  LOG(5) << "Reading target mass data in " << saf_path.string();
  saf = std::make_shared<syd::ICRP_SpecificAbsorbedFractionData>();
  saf->ReadTargetMass(saf_path.string());
  mTargetMass_AM = saf->mTargetMass_AM;
  mTargetMass_AF = saf->mTargetMass_AF;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::SCoefficientCalculator::ToString() const
{
  std::ostringstream ss;
  ss << "Phantom      " << mPhantomName << std::endl
     << "Source organ " << mSourceOrganName << std::endl
     << "Target organ " << mTargetOrganName << std::endl
     << "Radionuclide " << mRadionuclideName << std::endl
     << "Target mass  " << GetTargetMassInKg() << " kg" << std::endl
     << "Source mass  " << GetSourceMassInKg() << " kg";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::SCoefficientCalculator::Run()
{
  if (mRadiationData == nullptr or mSAFData.size() == 0) Initialise();

  // Consider the radionuclide and get the saf for the source/target
  const auto & rad = mRadiationData->GetData(mRadionuclideName);
  std::vector<std::shared_ptr<ICRP_SpecificAbsorbedFraction>> safd;
  safd.resize(8);
  for(auto i=0; i<safd.size(); i++) {
    if (mSAFData[i] != nullptr) {
      try{
        safd[i] = mSAFData[i]->Get(mSourceOrganName, mTargetOrganName);
      } catch(std::exception & e) {
        LOG(WARNING) << "Cannot find SAF for T/S: "
                     << mTargetOrganName << "/"
                     << mSourceOrganName << " (ignored)";
        safd[i] = nullptr;
        return 0;
      }
    }
  }

  // Loop on the energies list
  auto n = rad.size();
  double s = 0.0;
  for(int i=0; i<n; ++i) {
    auto & r = rad[i];
    auto energy = r.mEnergy;
    auto yield = r.mYield;
    auto ss = safd[r.mId];
    if (ss == nullptr) {
      LOG(WARNING) << "Error, ICODE " << i << " not read in SCoefficientCalculator::Run.";
      continue;
    }
    auto saf = ss->Compute(energy);
    auto ds = (energy * yield * saf);

    // Add to current
    s += ds;

  }
  // Conversion from Joule Gy/Bq.s in mGy/MBq.h
  // J    -> Gy   = 1.6e-13
  // Gy   -> mGy  = 1e3
  // Bq   -> MBq  = 1e6
  // Bq.s -> Bq.h = 3600
  s = s*1.6e-13*1e3*1e6*3600.0;
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::vector<std::string> syd::SCoefficientCalculator::GetListOfSourceOrgans()
{
  if (mListOfSourceOrgansNames.size() != 0) return mListOfSourceOrgansNames;
  if (mRadiationData == nullptr or mSAFData.size() == 0) Initialise();
  auto m = mSAFData[1];
  auto map = m->mSourcesMap;
  for(auto m:map) mListOfSourceOrgansNames.push_back(m.first);
  return mListOfSourceOrgansNames;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::vector<std::string> syd::SCoefficientCalculator::GetListOfTargetOrgans()
{
  if (mListOfTargetOrgansNames.size() != 0) return mListOfTargetOrgansNames;
  if (mRadiationData == nullptr or mSAFData.size() == 0) Initialise();
  auto m = mSAFData[1];
  auto smap = m->mSourcesMap;
  std::set<std::string> l;
  for(auto m:smap) {
    auto tmap = m.second;
    for(auto t:tmap) l.insert(t.first);
  }
  std::copy(l.begin(), l.end(), std::back_inserter(mListOfTargetOrgansNames));
  return mListOfTargetOrgansNames;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::SCoefficientCalculator::GetTargetMassInKg() const
{
  double m;
  // Get target mass
  if (mPhantomName == "am") {
    auto it = mTargetMass_AM.find(mTargetOrganName);
    if (it == mTargetMass_AM.end()) {
      EXCEPTION("Cannot find the target organ mass (AM): "<< mTargetOrganName);
    }
    m = it->second;
  }
  else {
    auto it = mTargetMass_AF.find(mTargetOrganName);
    if (it == mTargetMass_AF.end()) {
      EXCEPTION("Cannot find the target organ mass (AF): "<< mTargetOrganName);
    }
    m = it->second;
  }
  return m;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::SCoefficientCalculator::GetSourceMassInKg() const
{
  double m;
  // Get source mass
  if (mPhantomName == "am") {
    auto it = mSourceMass_AM.find(mSourceOrganName);
    if (it == mSourceMass_AM.end()) {
      EXCEPTION("Cannot find the source organ mass (AM): "<< mSourceOrganName);
    }
    m = it->second;
  }
  else {
    auto it = mSourceMass_AF.find(mSourceOrganName);
    if (it == mSourceMass_AF.end()) {
      EXCEPTION("Cannot find the source organ mass (AF): "<< mSourceOrganName);
    }
    m = it->second;
  }
  return m;
}
// --------------------------------------------------------------------

