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
void FillHoles(ImageType * input,
              const ImageType * mask,
              int r,
              double foreground,
              int & nb_failures,
              int & nb_changed)
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

  nb_failures = 0;
  nb_changed = 0;
  int warning = 0;
  while (!it_mask.IsAtEnd()) {
    if (it_mask.GetCenterPixel() == foreground) {
      double s = 0.0;
      int m = 0;
      for(auto i=0; i<n; i++) {
        auto v = it_n.GetPixel(i);
        auto vm = it_mask.GetPixel(i);
        if (vm != foreground) { s+=v; ++m; }
      }
      if (m == 0) ++nb_failures;
      else {
        s = s/(double)m; // mean of neighbor pixels
        it_n.SetCenterPixel(s);
        ++nb_changed;
      }
    }
    ++it_mask;
    ++it_n;
  } //  end loop
} // end function
//--------------------------------------------------------------------
