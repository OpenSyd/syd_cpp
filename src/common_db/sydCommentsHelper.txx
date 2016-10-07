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

// --------------------------------------------------------------------
template<class ArgsInfo>
void syd::SetCommentsFromCommandLine(std::vector<std::string> & comments,
                                     const syd::Database * db,
                                     ArgsInfo & args_info)
{
  DDF();
  if (args_info.add_comment_given) {
    for(auto i=0; i<args_info.add_comment_given; i++) {
      auto c = args_info.add_comment_arg[i];
      DD(c);
      comments.push_back(c);
    }
  }
}
// --------------------------------------------------------------------
