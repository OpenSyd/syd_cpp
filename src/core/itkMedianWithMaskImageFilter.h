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
#ifndef ITKMEDIANWITHMASKIMAGEFILTER_H
#define ITKMEDIANWITHMASKIMAGEFILTER_H

#include "itkMedianImageFilter.h"

namespace itk
{
  template< typename TInputImage, typename TOutputImage, typename TMaskImage>
  class MedianWithMaskImageFilter: public MedianImageFilter<TInputImage, TOutputImage>
  {
  public:

    /** Convenient typedefs for simplifying declarations. */
    typedef TInputImage  InputImageType;
    typedef TMaskImage   MaskImageType;
    typedef TOutputImage OutputImageType;

    /** Standard class typedefs. */
    typedef MedianWithMaskImageFilter                             Self;
    typedef MedianImageFilter< InputImageType, OutputImageType >  Superclass;
    typedef SmartPointer< Self >                                  Pointer;
    typedef SmartPointer< const Self >                            ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(MedianWithMaskImageFilter, MedianImageFilter);

    /** Image typedef support. */
    typedef typename InputImageType::PixelType  InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;

    typedef typename InputImageType::RegionType  InputImageRegionType;
    typedef typename OutputImageType::RegionType OutputImageRegionType;

    typedef typename InputImageType::SizeType InputSizeType;

    void SetMask(typename MaskImageType::Pointer mask) { mask_ = mask; }
    void SetMaskBackgroundValue(typename MaskImageType::PixelType value) { mask_backgroundvalue_ = value; }

  protected:
    MedianWithMaskImageFilter();
    virtual ~MedianWithMaskImageFilter() {}

    virtual void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                                      ThreadIdType threadId) override;

    typename MaskImageType::Pointer mask_;
    typename MaskImageType::PixelType mask_backgroundvalue_;

  private:
    MedianWithMaskImageFilter(const Self &); //purposely not implemented
    void operator=(const Self &);    //purposely not implemented
  };
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMedianWithMaskImageFilter.hxx"
#endif

#endif
