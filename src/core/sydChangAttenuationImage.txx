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

typedef std::chrono::high_resolution_clock Clock;
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
  for (unsigned int angle = 0; angle < nbAngles; ++angle) {
    //Compute the ray cast angle ]-PI; +PI] (angle 0° along x-axis and counter-clock wise)
    //Approximation : nbAngles is a multiple of 2
    anglesRad.push_back(angle*2.0*PI/nbAngles);
    if (anglesRad[angle] > PI)
      anglesRad[angle] -= 2*PI;
    tanAngles.push_back(tan(anglesRad[angle]));
    tanAnglesPi.push_back(tan(anglesRad[angle] - PI/2));
  }
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
  std::vector<double> diagonalAngles = syd::ComputeDiagonalAngles<typename ImageType::IndexType>(-0.5, Xmax + 0.5, -0.5, Ymax +0.5, voxel);
  for (unsigned int angle = 0; angle < nbAngles; ++angle) {
    //Compute the exit voxel for the iterator
    typename ImageType::PointType tempLastVoxel = ComputeExitPoint<ImageType, typename ImageType::IndexType>(-0.5, Xmax + 0.5, -0.5, Ymax + 0.5, voxel, angle, diagonalAngles);
    typename ImageType::IndexType lastVoxel;
    itk::ContinuousIndex<double, ImageType::ImageDimension> lastContinuousVoxel;
    for (unsigned int i=0; i < ImageType::ImageDimension; ++i) {
      if (tempLastVoxel[i] == -0.5)
        lastVoxel[i] = 0;
      else if ((std::round(tempLastVoxel[i]) - tempLastVoxel[i]) <= 0.5001)
        lastVoxel[i] = std::floor(tempLastVoxel[i]);
      else
        lastVoxel[i] = std::round(tempLastVoxel[i]);

      if (lastVoxel[i] < 0)
        lastVoxel[i] = 0;
      else if (lastVoxel[i] > (input->GetLargestPossibleRegion().GetSize()[i] -1))
        lastVoxel[i] = input->GetLargestPossibleRegion().GetSize()[i] -1;

      lastContinuousVoxel[i] = tempLastVoxel[i];
    }

    //Iterate between voxel and lastVoxel over a line (Bresenham line)
    //Add all attenuation value along the line
    //For the first pixel compute it alone because the start point is the center (and divide the attenuation by 2)

    //Approximation: Multiply by the mean distance of voxel at the end,
    //ie. mulitpliy by the distance and divide by the number of voxel (-0.5 because for the first voxel, we start at the center)
    //This approximation is exact for nbAngles <= 8 and nbAngles%2=0

    itk::LineConstIterator<ImageType> lineIterator(input, voxel, lastVoxel);
    lineIterator.GoToBegin();
    typename ImageType::PointType entryVoxelPoint;
    typename ImageType::PointType exitVoxelPoint;
    input->TransformIndexToPhysicalPoint(voxel, entryVoxelPoint);
    input->TransformContinuousIndexToPhysicalPoint(lastContinuousVoxel, exitVoxelPoint);
    double distance = entryVoxelPoint.EuclideanDistanceTo(exitVoxelPoint);
    typename ImageType::PixelType valueAlongLine(lineIterator.Get()/2);
    int numberVoxel(1);
    ++lineIterator;
    while (!lineIterator.IsAtEnd()) {

      /* non-approximated method to compute distance:
      typename ImageType::PointType centerVoxel;
      input->TransformIndexToPhysicalPoint(lineIterator.GetIndex(), centerVoxel);
      std::vector<double> diagonalVoxelAngles = syd::ComputeDiagonalAngles<typename ImageType::PointType>(centerVoxel[0] - input->GetSpacing()[0]/2.0, centerVoxel[0] + input->GetSpacing()[0]/2.0, centerVoxel[1] - input->GetSpacing()[1]/2.0, centerVoxel[1] + input->GetSpacing()[1]/2.0, entryVoxelPoint);
      exitVoxelPoint = ComputeExitPoint<ImageType, typename ImageType::PointType>(centerVoxel[0] - input->GetSpacing()[0]/2.0, centerVoxel[0] + input->GetSpacing()[0]/2.0, centerVoxel[1] - input->GetSpacing()[1]/2.0, centerVoxel[1] + input->GetSpacing()[1]/2.0, entryVoxelPoint, angle, diagonalVoxelAngles);
      double distance = entryVoxelPoint.EuclideanDistanceTo(exitVoxelPoint);*/

      valueAlongLine += lineIterator.Get();
      ++lineIterator;
      ++numberVoxel;
    }

    //Add the attenuation x mean distance into each voxel. Compute the attenuation factor (with exp)
    value += std::exp(-valueAlongLine*distance/(numberVoxel-0.5));
  }
  //Return the inverse of the mean for all angles
  return (nbAngles/value);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class T>
std::vector<double>
syd::ComputeDiagonalAngles(const double Xmin, const double Xmax, const double Ymin, const double Ymax, const T& voxel)
{
  std::vector<double> diagonalAngles(4, 0.0);
  //1nd: for (Xmin, Ymax)
  //cosAngle = (Xmax - voxel[0])*(Xmin - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(Xmin - voxel[0],2)+std::pow(Ymax - voxel[1],2)));
  if ((Xmin == voxel[0]) && (Ymax == voxel[1]))
    diagonalAngles[0] = -PI/2;
  else {
    double cosAngle = (Xmin-voxel[0])/std::sqrt(std::pow(Xmin - voxel[0],2)+std::pow(Ymax - voxel[1],2));
    diagonalAngles[0] = -std::acos(cosAngle);
  }
  //2rd: for (Xmax, Ymax)
  //cosAngle = (Xmax - voxel[0])*(Xmax - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(Ymax - voxel[1],2)));
  if ((Xmax == voxel[0]) && (Ymax == voxel[1]))
    diagonalAngles[1] = -PI/2;
  else {
    double cosAngle = (Xmax - voxel[0])/std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(Ymax - voxel[1],2));
    diagonalAngles[1] = -std::acos(cosAngle);
  }
  //3rd: for (Xmax, Ymin)
  //cosAngle = (Xmax - voxel[0])*(Xmax - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(voxel[1] - Ymin,2)));
  if ((Xmax == voxel[0]) && (voxel[1] == Ymin) == 0)
    diagonalAngles[2] = PI/2;
  else {
    double cosAngle = (Xmax - voxel[0])/std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(voxel[1] - Ymin,2));
    diagonalAngles[2] = std::acos(cosAngle);
  }
  //4st: for (Xmin, Ymin)
  //cosAngle = (Xmax - voxel[0])*(Xmin - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(voxel[0] - Xmin,2)+std::pow(voxel[1] - Ymin,2)));
  if ((voxel[0] == Xmin) && (voxel[1] == Ymin) == 0)
    diagonalAngles[3] = PI/2;
  else {
    double cosAngle = (Xmin - voxel[0])/std::sqrt(std::pow(voxel[0] - Xmin,2)+std::pow(voxel[1] - Ymin,2));
    diagonalAngles[3] = std::acos(cosAngle);
  }
  return(diagonalAngles);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType, class T>
typename ImageType::PointType
syd::ComputeExitPoint(const double Xmin, const double Xmax, const double Ymin, const double Ymax, const T& voxel, const int angle, const std::vector<double>& diagonalAngles)
{
  //tanAngle = tan(angleRad);
  //tanAnglePI = tan(angleRad - PI/2);
  typename ImageType::PointType lastVoxel;
  lastVoxel[2] = voxel[2];
  //First find the border to have one part of the coordinate
  //Second, find the last coordinate
  if (-PI < anglesRad[angle] && anglesRad[angle] <= diagonalAngles[0]) {
    lastVoxel[0] = Xmin;
    if (anglesRad[angle] == -PI/2)
      lastVoxel[1] = Ymax;
    else {
      lastVoxel[1] = voxel[1] + (voxel[0] - Xmin)*tanAngles[angle];
    }
  } else if (diagonalAngles[0] < anglesRad[angle] && anglesRad[angle] <= diagonalAngles[1]) {
    lastVoxel[1] = Ymax;
    if (anglesRad[angle] == 0)
      lastVoxel[0] = Xmax;
    else {
      lastVoxel[0] = voxel[0] + (Ymax - voxel[1])*tanAnglesPi[angle];
    }
  } else if (diagonalAngles[1] < anglesRad[angle] && anglesRad[angle] <= diagonalAngles[2]) {
    lastVoxel[0] = Xmax;
    if (anglesRad[angle] == PI/2)
      lastVoxel[1] = Ymin;
    else {
      lastVoxel[1] = voxel[1] - (Xmax - voxel[0])*tanAngles[angle];
    }
  } else if (diagonalAngles[2] < anglesRad[angle] && anglesRad[angle] <= diagonalAngles[3]) {
    lastVoxel[1] = Ymin;
    if (anglesRad[angle] == PI)
      lastVoxel[0] = Xmin;
    else {
      lastVoxel[0] = voxel[0] - (voxel[1] - Ymin)*tanAnglesPi[angle];
    }
  } else if (diagonalAngles[3] < anglesRad[angle] && anglesRad[angle] <= PI) {
    lastVoxel[0] = Xmin;
    lastVoxel[1] = voxel[1] + (voxel[0] - Xmin)*tanAngles[angle];
  }
  return(lastVoxel);
}
//--------------------------------------------------------------------
