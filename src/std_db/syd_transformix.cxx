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
#include "syd_transformix_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydImageBuilderBase.h"

#include "boost/date_time/gregorian/gregorian.hpp" //include all types plus i/o
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::gregorian;
namespace pt = boost::posix_time;

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_transformix, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Read the ImageTransform
  std::vector<syd::IdType> tids;
  std::vector<std::string> stids;
  syd::GetWords(stids, args_info.transform_arg);
  for(auto s:stids) tids.push_back(atoi(s.c_str()));
  syd::ImageTransform::vector initial_transforms;
  db->Query(initial_transforms, tids);

  // Read ids from the command line and the pipe
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));
  if (ids.size() < 1) {
    LOG(FATAL) << "Please provide at least one image id.";
  }
  syd::Image::vector initial_images;
  db->Query(initial_images, ids);

  // Match
  syd::Image::vector images;
  syd::ImageTransform::vector transforms;
  for(auto i=0; i<initial_images.size(); i++) {
    syd::Image::pointer image = initial_images[i];
    // Find the corresponding transforms
    int found = -1;
    for(auto j=0; j<initial_transforms.size(); j++) {
      if (initial_transforms[j]->moving_image->frame_of_reference_uid == image->frame_of_reference_uid) {
        if (found != -1) {
          LOG(FATAL) << "Error two transform could be used with the image: " << image << std::endl
                     << "First  is: " << initial_transforms[found] << std::endl
                     << "Second is: " << initial_transforms[j] << std::endl;
        }
        else found = j;
      }
    }
    if (found == -1) {
      LOG(FATAL) << "Error, could not find adequate ImageTransform for image:" << image;
    }
    images.push_back(image);
    transforms.push_back(initial_transforms[found]);
  }

  // Loop
  for(auto i=0; i<images.size(); i++) {

    // Get the input and the transform
    syd::Image::pointer input_image = images[i];
    syd::ImageTransform::pointer transform = transforms[i];

    std::string transform_path = db->GetAbsolutePath(transform->transform_file);
    std::string input_image_path = db->GetAbsolutePath(input_image);

    // Modify the transform file according to the input image spacing
    std::ifstream in(transform_path);
    transform_path = transform_path+"_temp.txt";
    std::ofstream out(transform_path);
    std::string line;
    while (std::getline(in, line)) {
      syd::Replace(line, "(Size ", "//(Size ");
      syd::Replace(line, "(Spacing ", "//(Spacing ");
      syd::Replace(line, "(FixedInternalImagePixelType ", "//(FixedInternalImagePixelType ");
      syd::Replace(line, "(MovingInternalImagePixelType ", "//(MovingInternalImagePixelType ");
      syd::Replace(line, "(DefaultPixelValue ", "//(DefaultPixelValue ");
      syd::Replace(line, "(ResultImagePixelType ", "//(ResultImagePixelType ");
      out << line << std::endl;
    }
    out << "(Size "
        << input_image->size[0] << " "
        << input_image->size[1] << " "
        << input_image->size[2] << ")" << std::endl;
    out << "(Spacing "
        << input_image->spacing[0] << " "
        << input_image->spacing[1] << " "
        << input_image->spacing[2] << ")" << std::endl;
    // out << "(FixedInternalImagePixelType " << input_image->pixel_type << ")" << std::endl;
    // out << "(MovingInternalImagePixelType " << input_image->pixel_type << ")" << std::endl;
    if (input_image->pixel_type == "short") out << "(DefaultPixelValue -1000)" << std::endl; // FIXME
    else out << "(DefaultPixelValue 0)" << std::endl;
    out << "(ResultImagePixelType " << input_image->pixel_type << ")" << std::endl;
    out.close();


    // Check frame_of_reference_uid
    if (input_image->frame_of_reference_uid != transform->moving_image->frame_of_reference_uid) {
      LOG(WARNING) << "Error the frame_of_reference_uid of the input image is different from the one of the moving_image of the transform" << std::endl
                   << "input : " << input_image << std::endl
                   << "moving_image : " << transform->moving_image  << std::endl;
      continue;
    }

    // Create output image
    syd::ImageBuilderBase builder(db);
    syd::Image::pointer output_image = builder.InsertNewMHDImageLike(input_image);
    // db->New(output_image);
    // output_image->CopyFrom(input_image);
    //output_image->tags.clear(); // remove the tags
    // output_image->frame_of_reference_uid = transform->fixed_image->frame_of_reference_uid; // copy the ref from the fixed_image
    //    db->Insert(output_image);
    // std::ostringstream oss;
    // if (input_image->dicoms.size() == 0) oss << "IMAGE_" << output_image->id << ".mhd";
    // else oss << input_image->dicoms[0]->dicom_modality << "_" << output_image->id << ".mhd";
    // std::string mhd_filename = oss.str();
    // std::string mhd_relative_path = output_image->ComputeRelativeFolder()+PATH_SEPARATOR;
    // std::string mhd_path = db->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);
    // output_image->UpdateFile(db, mhd_filename, mhd_relative_path);
    // std::string output_image_path = db->GetAbsolutePath(output_image);

    // builder.UpdateImageToFile

    // Tag ? ct spect mask to copy ; transform to add
    syd::Tag::vector tags;
    db->FindTags(tags, args_info.tags_arg);
    for(auto t:tags) output_image->AddTag(t);

    db->Update(output_image);

    // Create command line
    std::ostringstream cmd;
    cmd << "transformix -in " << input_image_path
        << " -tp " << transform_path
        << " -out " << db->ConvertToAbsolutePath(output_image->ComputeRelativeFolder());
    //  cmd << options; // additional options to transformix

    // Execute transformix
    LOG(1) << cmd.str();
    int r = syd::ExecuteCommandLine(cmd.str(), args_info.verbose_arg);

    // Get result path
    std::string f = db->ConvertToAbsolutePath(output_image->ComputeRelativeFolder()+PATH_SEPARATOR+"result.mhd");

    if (r!=0 || !fs::exists(f)) { // fail
      LOG(1) << "Command fail, removing temporary image";
      db->Delete(output_image);
    }
    else  {
      std::string output_image_path = db->GetAbsolutePath(output_image);
      syd::RenameMHDImage(f, output_image_path);
      LOG(1) << "Image computed. Result: " << output_image;
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
