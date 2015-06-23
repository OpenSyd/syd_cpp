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
template<class Image>
void FindImages(std::vector<Image> & images, syd::Database * db, syd::Patient & patient, std::vector<syd::Tag> & tags)
{
  // Get all images for this patient
  std::vector<Image> temp;
  odb::query<Image> q = odb::query<Image>::patient == patient.id;
  db->Query<Image>(q, temp);

  // Only sort those that match ALL tags (suppose not two times the same tags);
  for(auto i:temp) {
    int n=0;
    for(auto t:tags) {
      syd::IdType id = t.id;
      auto it = std::find_if(i.tags.begin(), i.tags.end(),
                             [id](const std::shared_ptr<syd::Tag> & tag) { return tag->id == id; });
      if (it != i.tags.end()) { //found !
        ++n;
      }
    }
    if (n == tags.size()) images.push_back(i);
  }
}
// --------------------------------------------------------------------
