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
#include "sydICRP_SpecificAbsorbedFraction.h"
#include "sydException.h"

// std
#include <fstream>
#include <algorithm>

// --------------------------------------------------------------------
// Convenient function to read stream with additional token delimitor. Here we
// add "<" because the target<-source maybe separated by <- without space.
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


// --------------------------------------------------------------------
syd::ICRP_SpecificAbsorbedFraction::ICRP_SpecificAbsorbedFraction(std::vector<double> & v)
  :mEnergies(v)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ICRP_SpecificAbsorbedFraction::Compute(double energy)
{
  // Search for first energy larger than the given energy
  auto search = std::lower_bound(mEnergies.begin(), mEnergies.end(), energy);
  if (search == mEnergies.end()) {
    EXCEPTION("too large " << energy);
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

  return saf;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ICRP_SpecificAbsorbedFractionData::Read(std::string filename)
{
  // Read first lines header
  std::ifstream is(filename);
  if (!is) {
    EXCEPTION("Error, cannot open " << filename);
  }
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
    // Read target and source name
    is >> target;
    is >> source;
    // Sometimes the source start with a '-', remove it
    if (source[0]=='-') source.erase(0,1);

    auto saf = std::make_shared<ICRP_SpecificAbsorbedFraction>(mEnergies);

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
std::shared_ptr<syd::ICRP_SpecificAbsorbedFraction>
syd::ICRP_SpecificAbsorbedFractionData::Get(std::string source, std::string target)
{
  auto search_source = mSourcesMap.find(source);
  if (search_source == mSourcesMap.end()) {
    EXCEPTION("Cannot find SAF with source. Source/Target: " << source << "/" << target);
  }
  auto search_target = search_source->second.find(target);
  if (search_target == search_source->second.end()) {
    EXCEPTION("Cannot find SAF with target. Source/Target: " << source << "/" << target);
  }
  return search_target->second;
}
// --------------------------------------------------------------------


