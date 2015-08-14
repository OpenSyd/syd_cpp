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
namespace syd {

  /* This function consider all pixels in the input image that are in
    the mask image. They replace their value by the mean value of a
    neighborhood, provided that the neighboring pixels are not in the
    mask and does not have value at zero.
  */

  template<class ImageType>
  int FillHoles(ImageType * input, const ImageType * mask, int r)
  {
    typedef itk::NeighborhoodIterator<ImageType> Iterator;
    typedef itk::ConstNeighborhoodIterator<ImageType> CIterator;
    typename Iterator::RadiusType radius;
    for(auto i=0; i<3; i++) radius[i] = r;

    Iterator it_n(radius, input, input->GetLargestPossibleRegion());
    CIterator it_mask(radius, mask, mask->GetLargestPossibleRegion());
    it_n.GoToBegin();
    it_mask.GoToBegin();

    int n = 1;
    for(auto i=0; i<3; i++) n *= it_n.GetSize(i);

    int failure = 0;
    while (!it_mask.IsAtEnd()) {
      if (it_mask.GetCenterPixel() == 1.0) {
        double  s = 0.0;
        int m = 0;
        for(auto i=0; i<n; i++) {
          double v = it_n.GetPixel(i);
          double vm = it_mask.GetPixel(i);
          if (vm != 1 and v != 0) { s+=v; ++m; }
        }
        if (m == 0) {
          LOG(WARNING) << "Too large hole to fill (no pixels in mask around this one). index is "
                       << it_n.GetIndex();
          ++failure;
        }
        else {
          s = s/m; // mean of neighbor pixels
          it_n.SetCenterPixel(s);
        }
      }
      ++it_mask;
      ++it_n;
    } //  end loop
    return failure;
  } // end function
} // end namespace
//--------------------------------------------------------------------
