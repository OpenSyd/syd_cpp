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
  output->FillBuffer(0);

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
  //Return value of the voxel
  typename ImageType::PixelType value(0);

  //Size of the image:
  double Xmax = input->GetLargestPossibleRegion().GetSize()[0] -1;
  double Ymax = input->GetLargestPossibleRegion().GetSize()[1] -1;

  //Compute the 4 angles for diagonals
  //Compute it using scalar product, between ]-PI; +PI]:
  //
  //   ______________________________ Corner
  //  |                             /|
  //  |                            / |
  //  |                           /  |
  //  |                          V---| Border
  //  |______________________________|
  //
  //  (V) : Voxel = (voxel[0], voxe[1])
  //
  //   cos(Angle) = scalarProduct(Corner Voxel Border)/(dist(Border, Voxel)*dist(Corner, Voxel))
  //
  //
  // std::acos(-1) = PI
  std::vector<double> diagonalAngles = syd::ComputeDiagonalAngles<typename ImageType::IndexType>(0, Xmax, 0, Ymax, voxel);

  for (unsigned int angle = 0; angle < nbAngles; ++angle) {
    //Compute the ray cast angle ]-PI; +PI] (angle 0° along x-axis and counter-clock wise)
    double angleRad;
    angleRad = angle*2.0*std::acos(-1)/nbAngles;
    if (angleRad > std::acos(-1))
      angleRad -= 2*std::acos(-1);

    //Compute the exit voxel for the iterator
    typename ImageType::PointType tempLastVoxel = ComputeExitPoint<ImageType, typename ImageType::IndexType>(0, Xmax, 0, Ymax, voxel, angleRad, diagonalAngles);
    typename ImageType::IndexType lastVoxel;
    for (unsigned int i=0; i < ImageType::ImageDimension; ++i)
      lastVoxel[i] = std::round(tempLastVoxel[i]);

    //Iterate between voxel and lastVoxel over a line (Bresenham line)
    //Add all attenuation value along the line
    itk::LineConstIterator<ImageType> lineIterator(input, voxel, lastVoxel);
    lineIterator.GoToBegin();
    typename ImageType::PointType entryVoxelPoint;
    typename ImageType::PointType exitVoxelPoint;
    input->TransformIndexToPhysicalPoint(lineIterator.GetIndex(), entryVoxelPoint);
    typename ImageType::PixelType valueAlongLine(0);
    while (!lineIterator.IsAtEnd()) {
      typename ImageType::IndexType centerVoxel = lineIterator.GetIndex();
      std::vector<double> diagonalVoxelAngles = syd::ComputeDiagonalAngles<typename ImageType::PointType>(centerVoxel[0] - input->GetSpacing()[0], centerVoxel[0] + input->GetSpacing()[0], centerVoxel[1] - input->GetSpacing()[1], centerVoxel[1] + input->GetSpacing()[1], entryVoxelPoint);
      exitVoxelPoint = ComputeExitPoint<ImageType, typename ImageType::PointType>(centerVoxel[0] - input->GetSpacing()[0], centerVoxel[0] + input->GetSpacing()[0], centerVoxel[1] - input->GetSpacing()[1], centerVoxel[1] + input->GetSpacing()[1], entryVoxelPoint, angleRad, diagonalVoxelAngles);
      double distance = entryVoxelPoint.EuclideanDistanceTo(exitVoxelPoint);
      valueAlongLine += lineIterator.Get()*distance;
      entryVoxelPoint = exitVoxelPoint;
      ++lineIterator;
    }

    //Add the attenuation x distance into each voxel
    value += valueAlongLine;
  }

  //Return the mean for all angles
  return (value/nbAngles);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class T>
std::vector<double>
syd::ComputeDiagonalAngles(const double Xmin, const double Xmax, const double Ymin, const double Ymax, const T voxel)
{
  std::vector<double> diagonalAngles(4, 0.0);
  double cosAngle(0);
  //1nd: for (Xmin, Ymax)
  //cosAngle = (Xmax - voxel[0])*(Xmin - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(Xmin - voxel[0],2)+std::pow(Ymax - voxel[1],2)));
  if (std::pow(voxel[0],2)+std::pow(Ymax - voxel[1],2) == 0)
    diagonalAngles[0] = std::acos(-1)/2;
  else {
    cosAngle = ((double) Xmin-voxel[0])/std::sqrt(std::pow(Xmin - voxel[0],2)+std::pow(Ymax - voxel[1],2));
    diagonalAngles[0] = std::acos(cosAngle);
  }
  if (diagonalAngles[0] != std::acos(-1))
    diagonalAngles[0] = -diagonalAngles[0];
  //2rd: for (Xmax, Ymax)
  //cosAngle = (Xmax - voxel[0])*(Xmax - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(Ymax - voxel[1],2)));
  if (std::pow(Xmax - voxel[0],2)+std::pow(Ymax - voxel[1],2) == 0)
    diagonalAngles[1] = std::acos(-1)/2;
  else {
    cosAngle = ((double) (Xmax - voxel[0]))/std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(Ymax - voxel[1],2));
    diagonalAngles[1] = std::acos(cosAngle);
  }
  if (diagonalAngles[1] != 0)
    diagonalAngles[1] = -diagonalAngles[1];
  //3rd: for (Xmax, Ymin)
  //cosAngle = (Xmax - voxel[0])*(Xmax - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(voxel[1]-Ymin,2)));
  if (std::pow(Xmax - voxel[0],2)+std::pow(voxel[1] - Ymin,2) == 0)
    diagonalAngles[2] = std::acos(-1)/2;
  else {
    cosAngle = ((double) (Xmax - voxel[0]))/std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(voxel[1],2));
    diagonalAngles[2] = std::acos(cosAngle);
  }
  //4st: for (Xmin, Ymin)
  //cosAngle = (Xmax - voxel[0])*(Xmin - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(voxel[0]-Xmin,2)+std::pow(voxel[1]-Ymin,2)));
  if (std::pow(voxel[0],2)+std::pow(voxel[1],2) == 0)
    diagonalAngles[3] = std::acos(-1)/2;
  else {
    cosAngle = ((double) Xmin - voxel[0])/std::sqrt(std::pow(voxel[0] - Xmin,2)+std::pow(voxel[1] - Ymin,2));
    diagonalAngles[3] = std::acos(cosAngle);
  }
  return(diagonalAngles);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType, class T>
typename ImageType::PointType
syd::ComputeExitPoint(const double Xmin, const double Xmax, const double Ymin, const double Ymax, T voxel, double angleRad, std::vector<double>& diagonalAngles)
{
  typename ImageType::PointType lastVoxel;
  lastVoxel[2] = voxel[2];
  //First find the border to have one part of the coordinate
  //Second, find the last coordinate
  if (-std::acos(-1) < angleRad && angleRad <= diagonalAngles[0]) {
    lastVoxel[0] = Xmin;
    if (angleRad == -std::acos(-1)/2)
      lastVoxel[1] = Ymax;
    else {
      lastVoxel[1] = voxel[1] + (voxel[0]-Xmin)*tan(angleRad);
    }
  } else if (diagonalAngles[0] < angleRad && angleRad <= diagonalAngles[1]) {
    lastVoxel[1] = Ymax;
    if (angleRad == 0)
      lastVoxel[0] = Xmax;
    else {
      lastVoxel[0] = voxel[0] + (Ymax - voxel[1])*tan(angleRad - std::acos(-1)/2);
    }
  } else if (diagonalAngles[1] < angleRad && angleRad <= diagonalAngles[2]) {
    lastVoxel[0] = Xmax;
    if (angleRad == std::acos(-1)/2)
      lastVoxel[1] = Ymin;
    else {
      lastVoxel[1] = voxel[1] - (Xmax - voxel[0])*tan(angleRad);
    }
  } else if (diagonalAngles[2] < angleRad && angleRad <= diagonalAngles[3]) {
    lastVoxel[1] = Ymin;
    if (angleRad == std::acos(-1))
      lastVoxel[0] = Xmin;
    else {
      lastVoxel[0] = voxel[0] - (voxel[1]-Ymin)*tan(angleRad - std::acos(-1)/2);
    }
  } else if (diagonalAngles[3] < angleRad && angleRad <= std::acos(-1)) {
    lastVoxel[0] = Xmin;
    lastVoxel[1] = voxel[1] + (voxel[0]-Xmin)*tan(angleRad);
  }
  return(lastVoxel);
}
//--------------------------------------------------------------------
