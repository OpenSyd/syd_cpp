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
  mSAFData = std::make_shared<syd::SpecificAbsorbedFractionData>();
  mSAFData->Read();

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
  const auto & energy = mRadiationData->GetEnergies(mRadionuclideName);
  const auto & yield  = mRadiationData->GetYields(mRadionuclideName);
  const auto & safd = mSAFData->Get(mSourceOrganName, mTargetOrganName);
  // electron only for the moment FIXME --> saf will need type of particle

  auto nb_E = energy.size();
  for(int i=0; i<nb_E; ++i) {
    DD(i);
    auto saf = safd->Compute(energy[i]);
    DD(saf);
    s += saf * yield[i];
  }
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
  DDF();

  auto search = std::lower_bound(mEnergies.begin(), mEnergies.end(), energy);
  if (search == mEnergies.end()) {
    LOG(FATAL) << "too large " << energy;
  }

  if (search == mEnergies.begin()) { // first element
    return mSAFvalues[0];
  }

  double before = *(search-1);
  double after = *(search);
  int index_after = search-mEnergies.begin();
  int index_before = index_after-1;
  DD(index_before);
  DD(index_after);

  // Interpolation
  double saf_before = mSAFvalues[index_before];
  double saf_after = mSAFvalues[index_after];
  DD(saf_before);
  DD(saf_after);
  double w = after-before;
  DD(w);
  double w1 = (energy-before)/w;
  double w2 = 1.0-w1;
  DD(w1);
  DD(w2);
  double saf = saf_before*w2 + saf_after*w1;

  return saf;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SpecificAbsorbedFractionData::Read()
{
  DDF();

  std::string folder = "/home/dsarrut/src/syd/syd-devel/icrp/ICRP_133_SAF";
  std::string filename = "rcp-am_photon_2016-08-12.SAF";
  DD(folder);
  DD(filename);

  fs::path p(folder);
  p = p/filename;
  DD(p.string());

  // Read first lines header
  std::ifstream is(p.string());
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
  std::string filename = "ICRP-07.BET";
  DD(folder);
  DD(filename);

  fs::path p(folder);
  p = p/filename;
  DD(p.string());

  std::ifstream is(p.string());
  std::string rad;
  int n;
  double e;
  double y;
  while (is >> rad) {
    // Read nb of values
    is >> n;
    mEnergiesMap[rad].resize(n);
    mYieldsMap[rad].resize(n);
    auto & energies = mEnergiesMap[rad];
    auto & yields = mYieldsMap[rad];
    for(auto i=0; i<n; ++i) {
      is >> energies[i];
      is >> yields[i];
    }
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
const std::vector<double> & syd::RadiationData::GetEnergies(std::string rad)
{
  DDF();
  auto search = mEnergiesMap.find(rad);
  if (search == mEnergiesMap.end()) {
    LOG(FATAL) << "Cannot find radiation energy data for " << rad;
  }
  return search->second;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
const std::vector<double> & syd::RadiationData::GetYields(std::string rad)
{
  DDF();
  auto search = mYieldsMap.find(rad);
  if (search == mYieldsMap.end()) {
    LOG(FATAL) << "Cannot find radiation yields data for " << rad;
  }
  return search->second;
}
// --------------------------------------------------------------------
