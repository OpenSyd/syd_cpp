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

#include "sydImage.h"

// --------------------------------------------------------------------
std::string syd::ConvertDicomSPECTFileToImage(std::string dicom_filename, std::string mhd_filename)
{
  // Open the dicom
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(dicom_filename.c_str(), true, dfile);
  DcmObject *dset = dfile.getDataset();
  if (!b) {
    LOG(FATAL) << "Could not open the dicom file " << dicom_filename;
  }

  // Read the image data
  VLOG(2) << "Converting SPECT dicom to mhd (" << mhd_filename << ") ...";
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer image = ReadImage<ImageType>(dicom_filename);

  // Check nb of slices
  typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
  ushort nbslices = GetTagValueUShort(dset, "NumberOfSlices");
  if (nbslices != size[2]) {
    LOG(FATAL) << "Error image spacing is " << size
               << " while in the dicom NumberOfSlices = " << nbslices;
  }

  // Remove meta information (if not : garbage in the mhd)
  itk::MetaDataDictionary d;
  image->SetMetaDataDictionary(d);

  // Offset
  std::string ImagePositionPatient = GetTagValueString(dset, "ImagePositionPatient");
  if (ImagePositionPatient == "") {
    LOG(FATAL) << "Error while reading tag ImagePositionPatient in the dicom ";
  }
  int n = ImagePositionPatient.find("\\");
  std::string sx = ImagePositionPatient.substr(0,n);
  ImagePositionPatient = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  n = ImagePositionPatient.find("\\");
  std::string sy = ImagePositionPatient.substr(0,n);
  std::string sz = ImagePositionPatient.substr(n+1,ImagePositionPatient.size());
  typename ImageType::PointType origin;
  origin[0] = toDouble(sx);
  origin[1] = toDouble(sy);
  origin[2] = toDouble(sz);
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
  syd::WriteImage<ImageType>(image, mhd_filename);

  // Compute md5
  return ComputeImageMD5<ImageType>(image);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::ConvertDicomCTFolderToImage(std::string dicom_path, std::string mhd_filename)
{
  // read all dicom in the folder
  typedef itk::Image<signed short, 3> ImageType;
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory(dicom_path);
  const ReaderType::FileNamesContainer & filenames = inputNames->GetInputFileNames();
  VLOG(2) << "Converting CT dicom (with " << filenames.size() << " files) to mhd (" << mhd_filename << ") ...";
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames );
  try { reader->Update(); }
  catch (itk::ExceptionObject &excp) {
    std::cerr << excp << std::endl;
    LOG(FATAL) << "Error while reading the dicom serie in " << dicom_path << " ";
  }

  // convert to mhd and write
  ImageType::Pointer ct = reader->GetOutput();
  syd::WriteImage<ImageType>(ct, mhd_filename);

  // Compute md5
  return ComputeImageMD5<ImageType>(ct);
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
    VLOG(verbose_level) << "Rename header " << old_path << " to " << new_path;
  }
  else {
    VLOG(verbose_level) << "Copy header " << old_path << " to " << new_path;
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
    VLOG(verbose_level) << "Rename raw " << old_path_raw << " to " << new_path_raw;
    int result = std::rename(old_path_raw.c_str(), new_path_raw.c_str());
    if (result != 0) {
      LOG(FATAL) << "Error while renaming " << old_path_raw << " to " << new_path_raw;
    }
  }
  else {
    VLOG(verbose_level) << "Copy raw " << old_path_raw << " to " << new_path_raw;
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
