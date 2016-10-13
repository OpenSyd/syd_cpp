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

#ifndef SYDRECORDWITHCOMMENTS_H
#define SYDRECORDWITHCOMMENTS_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object abstract pointer(std::shared_ptr)
  class RecordWithComments  {
  public:

    virtual ~RecordWithComments() {}

    /// Define pointer type
    typedef std::shared_ptr<RecordWithComments> pointer;

    /// Define vectortype
    typedef std::vector<pointer> vector;

    /// List of comments
    std::vector<std::string> comments;

    /// Concatenate all comments in a single string
    std::string GetAllComments() const;

  protected:
    RecordWithComments();

  };

} // end namespace
// --------------------------------------------------------------------

#endif
