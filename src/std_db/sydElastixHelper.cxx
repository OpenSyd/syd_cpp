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
#include "sydElastixHelper.h"
#include "sydFileHelper.h"
#include "sydImageHelper.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
int syd::ExecuteElastix(syd::Elastix::pointer elastix,
                        std::string options,
                        int verbose)
{
  auto db = elastix->GetDatabase();
  auto output_dir = db->GetUniqueTempFilename("");
  fs::create_directories(output_dir);

  // Create command line
  std::ostringstream cmd;
  cmd << "elastix -f " << elastix->fixed_image->GetAbsolutePath()
      << " -m " << elastix->moving_image->GetAbsolutePath()
      << " -out " << output_dir
      << " -p " << elastix->config_file->GetAbsolutePath()
      << " " << options;

  // Execute the cmd line
  LOG(1) << cmd.str();
  int r = syd::ExecuteCommandLine(cmd.str(), verbose);

  // test return
  std::string filename = elastix->ComputeDefaultFilename();//"TransformParameters.0.txt";
  if (r!=0) { // fail
    LOG(WARNING) << "Command elastix fail";
    fs::remove(output_dir);
  }
  else  {
    std::string result_file = output_dir+PATH_SEPARATOR+filename;
    if (!fs::exists(result_file)) {
      LOG(WARNING) << "Error could not find the output file: " << result_file;
      fs::remove(output_dir);
    }
    else { // only create the files if ok
      if (!elastix->transform_file) {
        auto folder = elastix->ComputeDefaultFolder();
        elastix->transform_file = syd::NewFile(db, folder, filename);
        db->Insert(elastix->transform_file);
      }
      fs::copy_file(result_file,
                    elastix->transform_file->GetAbsolutePath(),
                    fs::copy_option::overwrite_if_exists);
      db->Update(elastix);
      LOG(1) << "Registration computed. Result: " << elastix;
    }
  }
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::InsertTransformixImage(const syd::Elastix::pointer elastix,
                                                const syd::Image::pointer image,
                                                double default_pixel_value,
                                                std::string options,
                                                int verbose)
{
  // Get the input and the transform
  std::string transform_path = elastix->transform_file->GetAbsolutePath();
  std::string input_image_path = image->GetAbsolutePath();

  // Modify the transform file according to the input image spacing
  auto db = elastix->GetDatabase();
  std::ifstream in(transform_path);
  auto temp_folder = db->GetUniqueTempFilename("");
  fs::create_directories(temp_folder);
  auto temp = temp_folder+PATH_SEPARATOR+elastix->ComputeDefaultFilename();
  std::ofstream out(temp);
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
      << image->size[0] << " "
      << image->size[1] << " "
      << image->size[2] << ")" << std::endl;
  out << "(Spacing "
      << image->spacing[0] << " "
      << image->spacing[1] << " "
      << image->spacing[2] << ")" << std::endl;
  // out << "(FixedInternalImagePixelType " << input_image->pixel_type << ")" << std::endl;
  // out << "(MovingInternalImagePixelType " << input_image->pixel_type << ")" << std::endl;
  out << "(DefaultPixelValue " << default_pixel_value << ")" << std::endl;
  out << "(ResultImagePixelType " << image->pixel_type << ")" << std::endl;
  out.close();

  // Create command line
  std::ostringstream cmd;
  cmd << "transformix -in " << input_image_path
      << " -tp " << temp
      << " -out " << temp_folder
      << " " << options; // additional options to transformix

  // Execute transformix
  LOG(1) << cmd.str();
  int r = syd::ExecuteCommandLine(cmd.str(), verbose);

  // Test end of command --> we dont test because sometimes fail, only because warning.
  /*
    if (r!=0) { // fail
      LOG(WARNING) << "Command transformix fail";
      fs::remove_all(temp_folder);
      }
      else {
  */
  std::string f = temp_folder+PATH_SEPARATOR+"result.mhd";
  if (!fs::exists(f)) { // fail
    LOG(1) << "Command fail, cannot find " << f;
    fs::remove_all(temp_folder);
  }
  else  {
    // Create warp image
    auto output = syd::InsertImageFromFile(f, image->patient, image->modality);
    // Copy information from the deformed image ...
    syd::SetImageInfoFromImage(output, image);
    // ... but change the frame_of_reference_uid as the target image.
    output->frame_of_reference_uid = elastix->fixed_image->frame_of_reference_uid;
    db->Update(output);
    fs::remove_all(temp_folder);
    return output;
  }

  return NULL;
}
// --------------------------------------------------------------------
