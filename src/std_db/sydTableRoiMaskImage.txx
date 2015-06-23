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
template<class RoiMaskImage>
void FindRoiMaskImage(RoiMaskImage & mask, syd::Database * db, syd::Patient & patient,
                      syd::RoiType & roitype, syd::Image & image)
{
  if (image.dicoms.size() < 1) {
    EXCEPTION("This image does not have a single dicom, cannot find the mask: " << image);
  }
  FindRoiMaskImage(mask, db, patient, roitype, *image.dicoms[0]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RoiMaskImage>
void FindRoiMaskImage(RoiMaskImage & mask, syd::Database * db, syd::Patient & patient,
                      syd::RoiType & roitype, syd::DicomSerie & dicom)
{
  // Get all mask for this patient and this roitype
  std::vector<RoiMaskImage> masks;
  db->Query<RoiMaskImage>(odb::query<RoiMaskImage>::image->patient == patient.id and
                      odb::query<RoiMaskImage>::roitype == roitype.id, masks);

  // Select the one associated with the dicom
  bool found = false;
  std::vector<RoiMaskImage> results;
  for(auto m:masks) {
    if (m.image->dicoms.size() != 1) {
      LOG(WARNING) << "Warning the image of this mask does not have a single dicom (ignoring): " << m;
      continue;
    }
    if (dicom.dicom_frame_of_reference_uid == m.image->dicoms[0]->dicom_frame_of_reference_uid)
      results.push_back(m);
  }
  if (results.size() == 0) {
    EXCEPTION("No RoiMaskImage found for " << patient.name << " " << roitype.name
              << " dicom " << dicom.id << " frame_of_reference_uid = " << dicom.dicom_frame_of_reference_uid);
  }
  if (results.size() > 1) {
    std::string s;
    for(auto r:results) s += "\n"+r.ToString();
    EXCEPTION("Several RoiMaskImage found for " << patient.name << ", " << roitype.name
              << ", dicom " << dicom.id << ", frame_of_reference_uid = " << dicom.dicom_frame_of_reference_uid
              << s;);
  }
  mask = results[0];
}
// --------------------------------------------------------------------
