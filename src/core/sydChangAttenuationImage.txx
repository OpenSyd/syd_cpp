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

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::ChangAttenuationImage(const ImageType * input, int nbAngles)
{
  //Create the output, just a copy of the input
  typename ImageType::Pointer output = ImageType::New();
  output->SetRegions(input->GetLargestPossibleRegion());
  output->SetSpacing(input->GetSpacing());
  output->SetOrigin(input->GetOrigin());
  output->SetDirection(input->GetDirection());
  output->Allocate();

  itk::ImageRegionConstIterator<ImageType> inputIterator(input, input->GetLargestPossibleRegion());
  itk::ImageRegionIterator<ImageType> outputIterator(output, output->GetLargestPossibleRegion());

  while(!inputIterator.IsAtEnd()) {
    // Get the Chang Attenuation value for the current pixel
    auto val = ChangAttenuation<ImageType>(input, nbAngles, inputIterator.GetIndex());
    outputIterator.Set(val);

    ++inputIterator;
    ++outputIterator;
  }
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::PixelType
syd::ChangAttenuation(const ImageType * input, int nbAngles, typename ImageType::IndexType voxel)
{
  typename ImageType::PixelType value(0);
  for (unsigned int angle = 0; angle < nbAngles; ++angle) {
    //Compute the ray cast direction according the angle (angle 0° along x-axis and counter-clock wise)
    double direction[2];
    direction[0] = std::cos(angle*360.0/nbAngles); //direction along x
    direction[1] = std::sin(angle*360.0/nbAngles); //direction along y

    //Find the lastVoxel in the image along direction
    typename ImageType::IndexType lastVoxel;
    lastVoxel[0] = voxel[0];
    lastVoxel[1] = voxel[1];
    lastVoxel[2] = voxel[2];

    //Compute the real distance between voxel and lastVoxel
    typename ImageType::PointType point;
    typename ImageType::PointType lastPoint;
    input->TransformIndexToPhysicalPoint(voxel, point);
    input->TransformIndexToPhysicalPoint(lastVoxel, lastPoint);
    double distance = point.EuclideanDistanceTo(lastPoint);

    //Iterate between voxel and lastVoxel over a line (Bresenham line)
    //Add all attenuation value along the line
    itk::LineConstIterator<ImageType> lineIterator(input, voxel, lastVoxel);
    lineIterator.GoToBegin();
    typename ImageType::PixelType valueAlongLine(0);
    while (!lineIterator.IsAtEnd()) {
      valueAlongLine += lineIterator.Get();
      ++lineIterator;
    }

    //Divide the sum of attenuation value along the line by the real distance between voxel and lastVoxel
    //(It is not exactly the distance of the ray in the image - the last part between the center of lastVoxel and the exit is not taking into account)
    //(Be sure to avoid to divide by 0, how to compute it for border voxel)
    if (distance != 0)
      value += valueAlongLine/distance;
  }

  //Return the mean for all angles
  return (value/nbAngles);
}
//--------------------------------------------------------------------
