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
#include "sydTestMIRD.h"

// std
#include <sstream>

// boost
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


// https://stackoverflow.com/questions/10376199/how-can-i-use-non-default-delimiters-when-reading-a-text-file-with-stdfstream
class my_ctype : public
std::ctype<char>
{
  mask my_table[table_size];
public:
  my_ctype(size_t refs = 0)
    : std::ctype<char>(&my_table[0], false, refs)
  {
    std::copy_n(classic_table(), table_size, my_table);
    my_table['<'] = (mask)space;
  }
};



// --------------------------------------------------------------------
syd::AbsorbedDoseMIRDCalculator::AbsorbedDoseMIRDCalculator()
{
  DDF();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AbsorbedDoseMIRDCalculator::Run()
{
  DDF();
  DD(ToString());

  // Read radionuclide yields data
  mRadiationData = std::make_shared<syd::RadiationData>();
  mRadiationData->Read();

  // Read SAF data
  mSAFData.resize(8);
  mSAFData[0] = nullptr;
  std::string folder = "/home/dsarrut/src/syd/syd-devel/icrp/ICRP_133_SAF";
  {
    std::string filename = "rcp-am_photon_2016-08-12.SAF";
    fs::path p(folder);
    p = p/filename;
    DD(p.string());
    auto saf = std::make_shared<syd::SpecificAbsorbedFractionData>();
    saf->Read(p.string());
    mSAFData[1] = saf; // G PG DG
    mSAFData[2] = saf; // X
    mSAFData[3] = saf; // X
  }
  {
    std::string filename = "rcp-am_electron_2016-08-12.SAF";
    fs::path p(folder);
    p = p/filename;
    DD(p.string());
    auto saf = std::make_shared<syd::SpecificAbsorbedFractionData>();
    saf->Read(p.string());
    mSAFData[4] = saf; // B+
    mSAFData[5] = saf; // B- BD
    mSAFData[6] = saf; // IC electrons
    mSAFData[7] = saf; // Auger electrons
  }
  DD(mSAFData.size());
  // {
  //   std::string filename = "rcp-am_alpha_2016-08-12.SAF";
  //   fs::path p(folder);
  //   p = p/filename;
  //   DD(p.string());
  //   auto saf = std::make_shared<syd::SpecificAbsorbedFractionData>();
  //   saf->Read(p.string());
  //   mSAFData[8] = saf; // Alpha particles
  //   mSAFData[9] = saf; // Alpha recoil nuclei
  // }


  // Get S Coefficients
  auto s = GetSCoefficient();
  DD(s);

  auto dose = mActivity_in_MBq * s;
  DD(dose);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::AbsorbedDoseMIRDCalculator::ToString() const
{
  std::ostringstream ss;
  ss << "Activity     " << mActivity_in_MBq << " MBq" << std::endl
     << "Phantom      " << mPhantomName << std::endl
     << "Source organ " << mSourceOrganName << std::endl
     << "Target organ " << mTargetOrganName << std::endl
     << "Radionuclide " << mRadionuclideName << std::endl;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::AbsorbedDoseMIRDCalculator::GetSCoefficient()
{
  DDF();

  double s = 0.0;
  // const auto & energies = mRadiationData->GetEnergies(mRadionuclideName);
  // const auto & yields  = mRadiationData->GetYields(mRadionuclideName);
  const auto & rad = mRadiationData->GetData(mRadionuclideName);

  std::vector<std::shared_ptr<SpecificAbsorbedFraction>> safd;
  safd.resize(8);
  for(auto i=0; i<safd.size(); i++) {
    if (mSAFData[i] != nullptr)
      safd[i] = mSAFData[i]->Get(mSourceOrganName, mTargetOrganName);
  }

  auto n = rad.size();
  for(int i=0; i<n; ++i) {
    auto & r = rad[i];
    //if (r.mId != 1 and r.mId != 2 and r.mId != 5) continue;
    auto energy = r.mEnergy;
    auto yield = r.mYield;
    auto ss = safd[r.mId];
    auto saf = ss->Compute(energy);
    s += (energy * yield * saf);
    if (yield>0.05) {
      DD("----------");
      DD(i);
      DD(energy);
      DD(yield);
      DD(rad[i].mId);
      DD(saf);
      DD(energy * yield * saf);
      DD(s);
    }
  }
  DD(s);
  s = s*1.6e-13*1e3*1e6*3600;
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::SpecificAbsorbedFraction::SpecificAbsorbedFraction(std::vector<double> & v)
  :mEnergies(v)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::SpecificAbsorbedFraction::Compute(double energy)
{
  // Search for first energy larger than the given energy
  auto search = std::lower_bound(mEnergies.begin(), mEnergies.end(), energy);
  if (search == mEnergies.end()) {
    LOG(FATAL) << "too large " << energy;
  }

  // If this is the first, no interpolation
  if (search == mEnergies.begin()) return mSAFvalues[0];

  // Interpolation interpolation of SAF values according to energy interval
  double before = *(search-1);
  double after = *(search);
  int index_after = search-mEnergies.begin();
  int index_before = index_after-1;
  double saf_before = mSAFvalues[index_before];
  double saf_after = mSAFvalues[index_after];
  double w = after-before;
  double w1 = (energy-before)/w;
  double w2 = 1.0-w1;
  double saf = saf_before*w2 + saf_after*w1;

  // if (energy > 0.14 and energy <0.16) {
  //   DD(energy);
  //   DD(before);
  //   DD(after);
  //   DD(saf_before);
  //   DD(saf_after);
  //   DD(saf);
  // }

  return saf;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SpecificAbsorbedFractionData::Read(std::string filename)
{
  DDF();

  // Read first lines header
  std::ifstream is(filename);
  std::string line;
  std::getline(is, line);
  std::getline(is, line);
  std::getline(is, line); // First word must be "Photons"

  // Read nb of target and source organs
  int nb_target;
  is >> nb_target;
  int nb_source;
  is >> nb_source;

  // Read the list of energies
  std::string w;
  is >> w;
  mEnergies.clear();
  while (w != "Ecutoff") {
    double e = atof(w.c_str());
    mEnergies.push_back(e);
    is >> w;
  }
  DDS(mEnergies);
  DD(mEnergies.size());

  // Skip lines
  std::getline(is, line);
  std::getline(is, line);

  // Read target <- source values
  std::string target;
  std::string source;
  double Ecut;
  int Icut;

  std::locale x(std::locale::classic(), new my_ctype);
  is.imbue(x);

  auto ne = mEnergies.size();
  while (is) {
    is >> target;

    is >> source;
    if (source[0]=='-') source.erase(0,1);
    else source.erase(0,2);

    auto saf = std::make_shared<SpecificAbsorbedFraction>(mEnergies);

    saf->mSAFvalues.resize(ne);
    for(auto i=0; i<ne; i++)
      is >> saf->mSAFvalues[i];

    is >> Ecut; // to put in saf FIXME
    is >> Icut;
    std::getline(is, line);

    mSourcesMap[source][target] = saf;
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RadiationData::Read()
{
  DDF();
  std::string folder = "/home/dsarrut/src/syd/syd-devel/icrp/ICRP_2017_Nuclear_Data";
  std::string filename = "ICRP-07.RAD";
  DD(folder);
  DD(filename);

  fs::path p(folder);
  p = p/filename;
  DD(p.string());

  std::ifstream is(p.string());
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
    std::vector<RadiationTypeData> radiations(n);
    for(auto i=0; i<n; ++i) {
      RadiationTypeData d;
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
std::shared_ptr<syd::SpecificAbsorbedFraction>
syd::SpecificAbsorbedFractionData::Get(std::string source, std::string target)
{
  DDF();
  auto search_source = mSourcesMap.find(source);
  if (search_source == mSourcesMap.end()) {
    LOG(FATAL) << "Cannot find source " << source;
  }
  auto search_target = search_source->second.find(target);
  if (search_target == search_source->second.end()) {
    LOG(FATAL) << "Cannot find target " << target;
  }
  return search_target->second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*const std::vector<double> & syd::RadiationData::GetEnergies(std::string rad)
  {
  DDF();
  auto search = mEnergiesMap.find(rad);
  if (search == mEnergiesMap.end()) {
  LOG(FATAL) << "Cannot find radiation energy data for " << rad;
  }
  return search->second;
  }*/
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*const std::vector<double> & syd::RadiationData::GetYields(std::string rad)
  {
  DDF();
  auto search = mYieldsMap.find(rad);
  if (search == mYieldsMap.end()) {
  LOG(FATAL) << "Cannot find radiation yields data for " << rad;
  }
  return search->second;
  }*/
// --------------------------------------------------------------------


// --------------------------------------------------------------------
const std::vector<syd::RadiationTypeData> &
syd::RadiationData::GetData(std::string rad)
{
  DDF();
  auto search = mRadiationMap.find(rad);
  if (search == mRadiationMap.end()) {
    LOG(FATAL) << "Cannot find radiation data for " << rad;
  }
  return search->second;
}
// --------------------------------------------------------------------
