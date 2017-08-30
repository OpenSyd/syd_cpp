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
  double diagonalAngles[4];
  double cosAngle(0);
  //1nd: for (0, Ymax)
  //cosAngle = (Xmax - voxel[0])*(0 - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(voxel[0]-0,2)+std::pow(Ymax - voxel[1],2)));
  if (std::pow(voxel[0],2)+std::pow(Ymax - voxel[1],2) == 0)
    diagonalAngles[0] = std::acos(-1)/2;
  else {
    cosAngle = ((double) -voxel[0])/std::sqrt(std::pow(voxel[0],2)+std::pow(Ymax - voxel[1],2));
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
  //3rd: for (Xmax, 0)
  //cosAngle = (Xmax - voxel[0])*(Xmax - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(voxel[1]-0,2)));
  if (std::pow(Xmax - voxel[0],2)+std::pow(voxel[1],2) == 0)
    diagonalAngles[2] = std::acos(-1)/2;
  else {
    cosAngle = ((double) (Xmax - voxel[0]))/std::sqrt(std::pow(Xmax - voxel[0],2)+std::pow(voxel[1],2));
    diagonalAngles[2] = std::acos(cosAngle);
  }
  //4st: for (0, 0)
  //cosAngle = (Xmax - voxel[0])*(0 - voxel[0])/((Xmax - voxel[0])*std::sqrt(std::pow(voxel[0]-0,2)+std::pow(voxel[1]-0,2)));
  if (std::pow(voxel[0],2)+std::pow(voxel[1],2) == 0)
    diagonalAngles[3] = std::acos(-1)/2;
  else {
    cosAngle = ((double) -voxel[0])/std::sqrt(std::pow(voxel[0],2)+std::pow(voxel[1],2));
    diagonalAngles[3] = std::acos(cosAngle);
  }

  for (unsigned int angle = 0; angle < nbAngles; ++angle) {
    //Compute the ray cast angle ]-PI; +PI] (angle 0° along x-axis and counter-clock wise)
    double angleRad;
    angleRad = angle*2.0*std::acos(-1)/nbAngles;
    if (angleRad > std::acos(-1))
      angleRad -= 2*std::acos(-1);

    //Find the lastVoxel in the image according the angleRad
    typename ImageType::IndexType lastVoxel;
    lastVoxel[2] = voxel[2];
    //First find the border to have one part of the coordinate
    //Second, find the last coordinate
    if (-std::acos(-1) < angleRad && angleRad <= diagonalAngles[0]) {
      lastVoxel[0] = 0;
      if (angleRad == -std::acos(-1)/2)
        lastVoxel[1] = Ymax;
      else {
        double tempLastVoxel = voxel[1] + voxel[0]*tan(angleRad);
        lastVoxel[1] = std::round(tempLastVoxel);
      }
    } else if (diagonalAngles[0] < angleRad && angleRad <= diagonalAngles[1]) {
      lastVoxel[1] = Ymax;
      if (angleRad == 0)
        lastVoxel[0] = Xmax;
      else {
        double tempLastVoxel = voxel[0] + (Ymax - voxel[1])*tan(angleRad - std::acos(-1)/2);
        lastVoxel[0] = std::round(tempLastVoxel);
      }
    } else if (diagonalAngles[1] < angleRad && angleRad <= diagonalAngles[2]) {
      lastVoxel[0] = Xmax;
      if (angleRad == std::acos(-1)/2)
        lastVoxel[1] = 0;
      else {
        double tempLastVoxel = voxel[1] - (Xmax - voxel[0])*tan(angleRad);
        lastVoxel[1] = std::round(tempLastVoxel);
      }
    } else if (diagonalAngles[2] < angleRad && angleRad <= diagonalAngles[3]) {
      lastVoxel[1] = 0;
      if (angleRad == std::acos(-1))
        lastVoxel[0] = 0;
      else {
        double tempLastVoxel = voxel[0] - voxel[1]*tan(angleRad - std::acos(-1)/2);
        lastVoxel[0] = std::round(tempLastVoxel);
      }
    } else if (diagonalAngles[3] < angleRad && angleRad <= std::acos(-1)) {
      lastVoxel[0] = 0;
      double tempLastVoxel = voxel[1] + voxel[0]*tan(angleRad);
      lastVoxel[1] = std::round(tempLastVoxel);
    }

    //Compute the real distance between voxel and lastVoxel centers
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
    //And the distanceLastVoxel just to avoid to divide by 0 for border voxels
    //(It is not exactly the distance of the ray in the image - the last part between the center of lastVoxel and the exit is not taking into account)
    value += valueAlongLine*distance;
  }

  //Return the mean for all angles
  return (value/nbAngles);
}
//--------------------------------------------------------------------
