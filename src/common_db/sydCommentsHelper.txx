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
  if (args_info.add_comment_given) {
    for(auto i=0; i<args_info.add_comment_given; i++) {
      auto c = args_info.add_comment_arg[i];
      comments.push_back(c);
    }
  }
  if (args_info.rm_comment_given) {
    for(auto i=0; i<args_info.rm_comment_given; i++) {
      auto index = args_info.rm_comment_arg[i];
      if ((index < 0) or (index >= comments.size())) {
        LOG(WARNING) << "Cannot erase comments " << index
                     << " because there are " << comments.size()
                     << " comments.";
      }
      else comments.erase(comments.begin() + index);
    }
  }
}
// --------------------------------------------------------------------
