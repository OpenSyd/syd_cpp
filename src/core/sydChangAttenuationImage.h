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

#ifndef SYDCHANGATTENUATIONIMAGE_H
#define SYDCHANGATTENUATIONIMAGE_H

#include <math.h>

// syd
#include "sydCommon.h"
#include "sydFileUtils.h"

// itk
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itkLineConstIterator.h>

// --------------------------------------------------------------------
namespace syd {

  //Function to iterate on the input
  template<class ImageType>
  typename ImageType::Pointer
  ChangAttenuationImage(const ImageType * input, int nbAngles);

  //Main function to compute Chang for all angles
  template<class ImageType>
  typename ImageType::PixelType
  ChangAttenuation(const ImageType * input, int nbAngles, typename ImageType::IndexType voxel);

  //Compute the angles between voxel and the corners (for voxels or the image)
  template<class T>
  std::vector<double>
  ComputeDiagonalAngles(const double Xmin, const double Xmax, const double Ymin, const double Ymax, const T voxel);

  //Compute the exit point (for voxels or the image)
  template<class ImageType, class T>
  typename ImageType::PointType
  ComputeExitPoint(const double Xmin, const double Xmax, const double Ymin, const double Ymax, T voxel, double angleRad, std::vector<double>& diagonalAngles);

} // end namespace

#include "sydChangAttenuationImage.txx"

// --------------------------------------------------------------------

#endif
