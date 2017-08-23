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

#ifndef SYDTHUMBNAIL_H
#define SYDTHUMBNAIL_H

// syd
#include "sydCommonDatabase.h"
#include "sydImage.h"
#include "itkImage.h"

//--------------------------------------------------------------------
namespace syd {

  /// Helpers class to dump a table (line/column) of values,
  /// controling the precision and the column size
  class Thumbnail {
  public:

    // Constructor
    Thumbnail();

    typedef std::shared_ptr<Thumbnail> pointer;
    typedef syd::RecordTraitsBase::RecordBasePointer RecordBasePointer;
    typedef syd::RecordTraitsBase::RecordBaseVector RecordBaseVector;
    //    typedef std::function<std::string(RecordBasePointer)> FieldFunc;
    //    typedef syd::FieldBase::pointer FieldFunc;

    //Set the input image from syd::Image or syd::DicomSerie
    void setInputImage(const syd::DicomSerie::pointer images);
    void setInputImage(const syd::Image::pointer image);
    void setThumbnailPath(const std::string& thumbnailPath);

    //Compute the png image with all concatenated thumbnails
    void computeThumbnail();
    void computeThumbnail(const itk::Image<float,3>::Pointer image, const std::string& thumbnailPath);

  protected:
    //Window/Level values
    double m_window;
    double m_level;
    std::string m_thumbnailPath;
    itk::Image<float,3>::Pointer m_image;

  };


} // end namespace

#endif /* end #define SYDTHUMBNAIL_H_H */
