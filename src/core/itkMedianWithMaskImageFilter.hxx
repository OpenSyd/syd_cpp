/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef ITKMEDIANWITHMASKIMAGEFILTER_HXX
#define ITKMEDIANWITHMASKIMAGEFILTER_HXX

#include "itkMedianWithMaskImageFilter.h"

#include <vector>
#include <algorithm>

namespace itk
{

  // --------------------------------------------------------------------
  template< typename TInputImage, typename TOutputImage, typename TMaskImage>
  MedianWithMaskImageFilter< TInputImage, TOutputImage, TMaskImage>
  ::MedianWithMaskImageFilter()
  {
    mask_ = NULL;
    mask_backgroundvalue_ = 0;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  template< typename TInputImage, typename TOutputImage, typename TMaskImage>
  void
  MedianWithMaskImageFilter< TInputImage, TOutputImage, TMaskImage>
  ::ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                         ThreadIdType threadId)
  {
    if (!mask_) {
      itkExceptionMacro(<< "Set the mask with SetMask before using the filter");
    }

    // Allocate output
    typename OutputImageType::Pointer output = this->GetOutput();
    typename  InputImageType::ConstPointer input  = this->GetInput();

    // Find the data-set boundary "faces"
    NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType > bC;
    typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType >::FaceListType
      faceList = bC( input, outputRegionForThread, this->GetRadius() );

    // support progress methods/callbacks
    ProgressReporter progress( this, threadId, outputRegionForThread.GetNumberOfPixels() );

    // All of our neighborhoods have an odd number of pixels, so there is
    // always a median index (if there where an even number of pixels
    // in the neighborhood we have to average the middle two values).

    ZeroFluxNeumannBoundaryCondition< InputImageType > nbc;
    std::vector< InputPixelType >                      pixels;
    // Process each of the boundary faces.  These are N-d regions which border
    // the edge of the buffer.
    for ( typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator< InputImageType >::FaceListType::iterator
            fit = faceList.begin(); fit != faceList.end(); ++fit )
      {
        ImageRegionIterator< OutputImageType > it = ImageRegionIterator< OutputImageType >(output, *fit);

        ConstNeighborhoodIterator< InputImageType > bit =
          ConstNeighborhoodIterator< InputImageType >(this->GetRadius(), input, *fit);
        bit.OverrideBoundaryCondition(&nbc);
        bit.GoToBegin();

        // Second iterator for mask
        ConstNeighborhoodIterator< MaskImageType > bit_mask =
          ConstNeighborhoodIterator< MaskImageType >(this->GetRadius(), mask_, *fit);
        bit_mask.OverrideBoundaryCondition(&nbc);
        bit_mask.GoToBegin();

        const unsigned int neighborhoodSize = bit.Size();
        unsigned int medianPosition = neighborhoodSize / 2;
        while ( !bit.IsAtEnd() )
          {
            // If the current pixel is outside the mask, we do nothing
            if (bit_mask.GetCenterPixel() == mask_backgroundvalue_) it.Set(bit.GetCenterPixel());
            else {
              // collect all the pixels in the neighborhood, note that we use
              // GetPixel on the NeighborhoodIterator to honor the boundary conditions
              pixels.resize(0); // Different nb of neighborh pixel at each point (according to mask)
              for ( unsigned int i = 0; i < neighborhoodSize; ++i )
                {
                  if (bit_mask.GetPixel(i) != mask_backgroundvalue_) pixels.push_back(bit.GetPixel(i));
                }

              if (pixels.size() > 0) {
                unsigned int medianPosition = pixels.size() / 2;
                // get the median value
                const typename std::vector< InputPixelType >::iterator medianIterator = pixels.begin() + medianPosition;
                std::nth_element( pixels.begin(), medianIterator, pixels.end() );
                if (pixels.size() % 2 == 0) {
                  // If the nb of neighborhood if even, mean of the two median ones.
                  it.Set( (static_cast< typename OutputImageType::PixelType >( *medianIterator ) +
                           static_cast< typename OutputImageType::PixelType >( *(medianIterator-1) )) /2.0 );
                }
                else
                  it.Set( static_cast< typename OutputImageType::PixelType >( *medianIterator ) );
              }
              else it.Set(bit.GetCenterPixel());
            }

            ++bit;
            ++bit_mask;
            ++it;
            progress.CompletedPixel();
          }
      }
  }
  // --------------------------------------------------------------------


} // end namespace syd

#endif
