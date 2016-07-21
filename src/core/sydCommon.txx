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


//--------------------------------------------------------------------
// Convert float, double ... to string
template<class T>
std::string ToString(const T & t)
{
  std::ostringstream myStream;
  myStream << t << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Convert array to string with 'x'
template<class T>
std::string ArrayToString(const std::vector<T> & t, int precision)
{
  if (t.size() == 0) return "";
  std::ostringstream os;
  os << std::fixed << std::setprecision(precision);
  for(auto i=0; i<t.size()-1; i++) os << t[i] << "x";
  os << t[t.size()-1];// << std::flush;
  return(os.str());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<typename T>
std::vector<T> arange(T start, T stop, T step)
{
  std::vector<T> values;
  for (T value = start; value < stop; value += step)
    values.push_back(value);
  return values;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// http://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of
template <typename T, typename Compare>
std::vector<std::size_t> sort_permutation(const std::vector<T>& vec,
                                          Compare compare)
{
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
        [&](std::size_t i, std::size_t j){ return compare(vec[i], vec[j]); });
    return p;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <typename T>
std::vector<T> apply_permutation(const std::vector<T>& vec,
                                 const std::vector<std::size_t>& p)
{
    std::vector<T> sorted_vec(p.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(),
        [&](std::size_t i){ return vec[i]; });
    return sorted_vec;
}
//--------------------------------------------------------------------
