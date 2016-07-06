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

#include "sydImageUtils.h"

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::GeometricalMean(const ImageType * ant_em,
                     const ImageType * post_em,
                     const ImageType * ant_sc,
                     const ImageType * post_sc,
                     double k)
{
  auto ant = syd::RemoveScatter<ImageType>(ant_em, ant_sc, k);
  auto post = syd::RemoveScatter<ImageType>(post_em, post_sc, k);
  post = syd::FlipImage<ImageType>(post, 0);
  return syd::GeometricalMean<ImageType>(ant, post);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::RemoveScatter(const ImageType * em, const ImageType * sc, double k)
{
  auto output = syd::CreateImageLike<ImageType>(em);
  output->Allocate();
  itk::ImageRegionConstIterator<ImageType> iter_em(em, em->GetLargestPossibleRegion());
  itk::ImageRegionConstIterator<ImageType> iter_sc(sc, sc->GetLargestPossibleRegion());
  itk::ImageRegionIterator<ImageType> iter_output(output, output->GetLargestPossibleRegion());
  iter_em.GoToBegin();
  iter_sc.GoToBegin();
  iter_output.GoToBegin();
  while (!iter_em.IsAtEnd()) {
    iter_output.Set(iter_em.Get()*k*iter_sc.Get());
    ++iter_em;
    ++iter_sc;
    ++iter_output;
  }
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::GeometricalMean(const ImageType * ant, const ImageType * post)
{
  typename ImageType::Pointer output = syd::CreateImageLike<ImageType>(ant);
  output->Allocate();
  itk::ImageRegionConstIterator<ImageType> iter_ant(ant, ant->GetLargestPossibleRegion());
  itk::ImageRegionConstIterator<ImageType> iter_post(post, post->GetLargestPossibleRegion());
  itk::ImageRegionIterator<ImageType> iter_output(output, output->GetLargestPossibleRegion());
  iter_ant.GoToBegin();
  iter_post.GoToBegin();
  iter_output.GoToBegin();
  while (!iter_ant.IsAtEnd()) {
    iter_output.Set(sqrt(iter_ant.Get()*iter_post.Get()));
    ++iter_ant;
    ++iter_post;
    ++iter_output;
  }
  return output;
}
//--------------------------------------------------------------------
