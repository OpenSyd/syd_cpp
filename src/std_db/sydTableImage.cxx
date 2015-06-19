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
#include "sydTableImage.h"

// --------------------------------------------------------------------
template<>
void syd::Table<syd::Image>::Dump(std::ostream & os,
                                  const std::string & format,
                                  const std::vector<syd::IdType> & ids)
{
  std::vector<syd::Image> images;
  Query(ids, images); // (no sort)
    std::sort(begin(images), end(images),
            [images](Image & a, Image & b) {
              if (a.dicoms.size() == 0) return true;
              if (b.dicoms.size() == 0) return false;
              return syd::IsDateBefore(a.dicoms[0]->acquisition_date, b.dicoms[0]->acquisition_date); });
  Dump(os, format, images);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void syd::Table<syd::Image>::Dump(std::ostream & os,
                                  const std::string & format,
                                  const std::vector<syd::Image> & images)
{

  if (format == "help") {
    std::cout << "Default format: id p date tags type size" << std::endl
              << "format '1'    : id p date tags type size dicom_description" << std::endl;
    return;
  }

  syd::PrintTable table;
  table.AddColumn("#id", 4);
  table.AddColumn("p", 5);
  table.AddColumn("date", 18);
  table.AddColumn("tags", 25);
  table.AddColumn("type", 8);
  table.AddColumn("size",12);
  if (format == "1") table.AddColumn("dicom",110);
  table.Init();

  for(auto s:images) {
    std::ostringstream size;
    size << s.size[0] << "x" << s.size[1] << "x" << s.size[2];
    table << s.id;
    table << s.patient->name;
    table << s.GetAcquisitionDate()
          << GetTagLabels(s.tags)
          << s.pixel_type
          << size.str();
    if (format == "1") table << s.dicoms[0]->dicom_description;
  }
  table.Print(std::cout);
}
// --------------------------------------------------------------------
