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

// ---------------------------------------------------------------------
template<class T>
DatabaseRegistrar<T>::DatabaseRegistrar(std::string type_name)
{
  // std::cout << "Registering new database type " << type_name;
  // register the class factory function
  syd::DatabaseFactory::GetInstance()->
    RegisterFactoryFunction(type_name,
                            [](std::string a,
                               std::string b) -> Database * { return new T(a,b);});
}
// ---------------------------------------------------------------------
