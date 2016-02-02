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
template<class T, int N>
std::string ArrayToString(const std::array<T, N> & t, int precision)
{
  std::ostringstream myStream;
  for(auto i=0; i<N-1; i++) myStream << std::fixed << std::setprecision(precision) << t[i] << "x";
  myStream << std::setprecision(precision) << t[N-1] << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------
