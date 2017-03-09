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

#ifndef SYDIMAGETYPE_H
#define SYDIMAGETYPE_H

// syd
#include "sydDicomSerie.h"
#include "sydPixelUnit.h"

// --------------------------------------------------------------------
namespace syd {

  /// Information about an image type
  class ImageType {
  public:

    /// Define pointer type
    typedef std::shared_ptr<ImageType> pointer;

    /// Define vectortype
    typedef std::vector<pointer> vector;
    typedef const std::vector<pointer> vector_const;

    // type for properties
    enum PropertiesValue {Yes, No, Unknown};

    /// Constructor
    ImageType();

    /// Destructor
    virtual ~ImageType();

    /// Get (or build if this is the first time) the list of known ImageType
    static vector_const GetImageTypes();

    /// return the "not_found" image type
    static pointer GetNotFoundImageType();

    /// try to identify the given dicom, return a value [0-1]
    double Identify(syd::DicomSerie::pointer dicom) const;

    /// Write information as a string
    std::string ToString() const;

    /// make a copy
    pointer Clone() const;

    /// Default function to print (must be inline here).
    friend std::ostream& operator<<(std::ostream& os, const ImageType & p) {
      os << p.ToString();
      return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const ImageType::pointer & p) {
      os << p->ToString();
      return os;
    }

  protected:
    static vector all_types;
    std::string name;
    std::string description;
    syd::PixelUnit::pointer pixel_unit;
    std::string preferred_pixel_type;
    std::map<std::string, PropertiesValue> properties;
    std::function<double (syd::DicomSerie::pointer)> identifier;
    static pointer New();

    // List here the known type
    static pointer BuildImageType_NotFound();
    static pointer BuildImageType_CT();
    static pointer BuildImageType_SPECT();
    static pointer BuildImageType_PlanarSPECT();
    static pointer BuildImageType_ProjectionSPECT();
    static pointer BuildImageType_AttMap();

  }; // end class
  // --------------------------------------------------------------------

  /// Identify the type of an image
  syd::ImageType::pointer IdentifyImageType(syd::DicomSerie::pointer dicom);

} // end namespace
// --------------------------------------------------------------------

#endif
