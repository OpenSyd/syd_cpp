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
#include "sydImageUtils.h"

// --------------------------------------------------------------------
itk::Image<float,3>::Pointer
syd::ReadDicomFromSingleFile(std::string filename)
{
  // Open the dicom
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(filename.c_str(), dfile);
  if (!b) {
    EXCEPTION("Could not open the dicom file '" << filename
              << "' maybe this is not a dicom ?");
  }
  DcmObject *dset = dfile.getDataset();

  // Read the image data
  LOG(2) << "Converting dicom file (" << filename << ") to itk image.";
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer image = ReadImage<ImageType>(filename);

  // Check nb of slices
  typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
  ushort nbslices = GetTagValueUShort(dset, "NumberOfSlices");
  if (nbslices != size[2]) {
    EXCEPTION("Error image spacing is " << size
              << " while in the dicom NumberOfSlices = " << nbslices);
  }

  // Remove meta information (if not : garbage in the mhd)
  itk::MetaDataDictionary d;
  image->SetMetaDataDictionary(d);

  // Offset
  std::string ImagePositionPatient = GetTagValueString(dset, "ImagePositionPatient");
  if (ImagePositionPatient == "") {
    EXCEPTION("Error while reading tag ImagePositionPatient in the dicom ");
  }
  int n = ImagePositionPatient.find("\\");
  std::string sx = ImagePositionPatient.substr(0,n);
  ImagePositionPatient = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  n = ImagePositionPatient.find("\\");
  std::string sy = ImagePositionPatient.substr(0,n);
  std::string sz = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  typename ImageType::PointType origin;
  origin[0] = ToDouble(sx);
  origin[1] = ToDouble(sy);
  origin[2] = ToDouble(sz);
  image->SetOrigin(origin);

  // Correct for negative SpacingBetweenSlices
  double s = GetTagValueDouble(dset, "SpacingBetweenSlices");
  // change spacing z
  typename ImageType::SpacingType spacing = image->GetSpacing();
  if (s<0) spacing[2] = -s;
  else spacing[2] = s;
  image->SetSpacing(spacing);
  // Direction
  if (s<0) {
    LOG(2) << "Negative spacing, I flip the image.";
    typename ImageType::DirectionType direction = image->GetDirection();
    direction.Fill(0.0);
    direction(0,0) = 1; direction(1,1) = 1; direction(2,2) = -1;
    image->SetDirection(direction);

    // Change orientation
    itk::OrientImageFilter<ImageType,ImageType>::Pointer orienter =
      itk::OrientImageFilter<ImageType,ImageType>::New();
    orienter->UseImageDirectionOn();
    orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
    orienter->SetInput(image);
    orienter->Update();
    image = orienter->GetOutput();
  }

  // Correct for pixel scale --> dont know why it does not work.
  // DcmDictEntry * e = new DcmDictEntry(0x0011, 0x103b, EVR_UN, "PixelScale", 0, DcmVariableVM, NULL, true, NULL);
  // DcmDataDictionary &globalDataDict = dcmDataDict.wrlock();
  // globalDataDict.addEntry(e);
  // DcmElement * ee = GetTagValue(dset, "PixelScale");
  // double f;
  // ee->getFloat64(f);
  // DD(f);
  /*
  double scale = GetTagValueDouble(dset, "PixelScale");
  DD(scale);
  if (scale != 1.0 and scale != 0.0) {
    typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(image);
    filter->SetConstant(scale);
    filter->Update();
    image = filter->GetOutput();
  }
  */

  // End write the result
  // syd::WriteImage<ImageType>(image, mhd_filename);

  // // Compute md5
  // return ComputeImageMD5<ImageType>(image);
  return image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
itk::Image<short,3>::Pointer
syd::ReadDicomSerieFromFolder(std::string folder, std::string serie_uid)
{
  typedef short PixelType;
  typedef itk::Image<PixelType, 3> ImageType;

  // Read itk image
  try {
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::GDCMImageIO ImageIOType;
    typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
    InputNamesGeneratorType::Pointer nameGenerator = InputNamesGeneratorType::New();
    nameGenerator->SetInputDirectory(folder);
    const std::vector<std::string> & temp = nameGenerator->GetFileNames(serie_uid);
    typename ReaderType::Pointer reader = ReaderType::New();
    LOG(2) << "Looking for " << temp.size() << " files of serie " << serie_uid << " in " << folder << ".";
    reader->SetFileNames(temp );
    reader->Update();
    return reader->GetOutput();
  }
  catch (itk::ExceptionObject &excp) {
   EXCEPTION("Error while reading the dicom serie in " << folder << ", itk exception is: " << excp);
  }
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
/*
  Rename a mhd image, composed of 2 files XXX.mhd and XXX.raw. This
  function moves the two files and change the header to be linked with
  the renamed .raw file.
 */
void syd::RenameOrCopyMHDImage(std::string old_path, std::string new_path, int verbose_level, bool erase)
{
  // Check if not the same
  if (old_path == new_path) {
    LOG(WARNING) << "Try to rename " << old_path << " to the same file. Do nothing.";
    return; // do nothing
  }

  // Check extension old filename
  size_t n = old_path.find_last_of(".");
  std::string extension = old_path.substr(n+1);
  if (extension != "mhd") {
    LOG(FATAL) << "Rename MHD : Error the filename must have mhd as extension : " << old_path;
  }
  std::string old_path_raw = old_path.substr(0,n)+".raw";

  // Check extension new filename
  n = new_path.find_last_of(".");
  extension = new_path.substr(n+1);
  if (extension != "mhd") {
    LOG(FATAL) << "Rename MHD : Error the new filename must have mhd as extension : " << new_path;
  }
  std::string new_path_raw = new_path.substr(0,n)+".raw";

  // Check files
  if (!syd::FileExists(old_path)) {
    LOG(FATAL) << "Rename MHD : Error path (mhd) not exist : " << old_path;
  }
  if (!syd::FileExists(old_path_raw)) {
    LOG(FATAL) << "Rename MHD : Error path (raw) not exist : " << old_path_raw;
  }
  if (syd::FileExists(new_path)) {
    LOG(WARNING) << "Rename MHD : path (mhd) to rename already exist : " << new_path;
  }
  if (syd::FileExists(new_path_raw)) {
    LOG(WARNING) << "Rename MHD : path (raw) to rename already exist : " << new_path_raw;
  }

  // verbose
  if (erase) {
    LOG(verbose_level) << "Rename header " << old_path << " to " << new_path;
  }
  else {
    LOG(verbose_level) << "Copy header " << old_path << " to " << new_path;
  }

  // header part : change ElementDataFile in the header
  std::ifstream in(old_path);
  //  std::ofstream out(new_path);
  std::vector<std::string> outlines;
  // std::ofstream out;
  OFString r;
  OFStandard::getFilenameFromPath(r, old_path_raw.c_str());
  std::string wordToReplace(r.c_str());
  OFStandard::getFilenameFromPath(r, new_path_raw.c_str());
  std::string wordToReplaceWith(r.c_str());
  size_t len = wordToReplace.length();
  std::string line;
  // Delete old path (before writing new)
  if (erase) std::remove(old_path.c_str());
  while (std::getline(in, line)) {
    while (line != "") {
      size_t pos = line.find(wordToReplace);
      if (pos != std::string::npos) {
        line.replace(pos, len, wordToReplaceWith);
        break;
      }
      else
        break;
    }
    outlines.push_back(line+"\n");
  }
  in.close();

  // Write
  std::ofstream out(new_path);
  for(auto i:outlines) out << i;
  out.close();

  // Rename or copy .raw part
  if (erase) {
    LOG(verbose_level) << "Rename raw " << old_path_raw << " to " << new_path_raw;
    int result = std::rename(old_path_raw.c_str(), new_path_raw.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << old_path_raw << " to " << new_path_raw;
    }
  }
  else {
    LOG(verbose_level) << "Copy raw " << old_path_raw << " to " << new_path_raw;
    // std::ifstream  src(old_path_raw.c_str(), std::ios::binary);
    // std::ofstream  dst(new_path_raw.c_str(), std::ios::binary);
    // dst << src.rdbuf();

    // I need to do that way to avoid overwriting if old/new are the same files.
    std::ifstream source(old_path_raw, std::ios::binary);

    // file size
    source.seekg(0, std::ios::end);
    std::ifstream::pos_type size = source.tellg();
    source.seekg(0);
    // allocate memory for buffer
    char* buffer = new char[size];

    // copy file
    source.read(buffer, size);
    source.close();
    std::ofstream dest(new_path_raw, std::ios::binary);
    dest.write(buffer, size);

    // clean up
    delete[] buffer;
    dest.close();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DeleteMHDImage(std::string path)
{
  OFStandard::deleteFile(path.c_str());
  size_t n = path.find_last_of(".");
  std::string path_raw = path.substr(0,n)+".raw";
  OFStandard::deleteFile(path_raw.c_str());
}
// --------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::CopyMHDImage(std::string from , std::string to, int verbose_level)
{
  RenameOrCopyMHDImage(from, to, verbose_level, false);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::RenameMHDImage(std::string from , std::string to, int verbose_level)
{
  RenameOrCopyMHDImage(from, to, verbose_level, true);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Convert PointType
std::string syd::PointToString(const itk::Point<double,3> & t)
{
  std::ostringstream myStream;
  myStream << t[0] << ";" << t[1] << ";" << t[2] << std::flush;
  return(myStream.str());
}
//--------------------------------------------------------------------
