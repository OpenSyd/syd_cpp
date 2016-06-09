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

#ifndef SYDRECORDWITHMD5SIGNATURE_H
#define SYDRECORDWITHMD5SIGNATURE_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object abstract pointer(std::shared_ptr)
  class RecordWithMD5Signature {
  public:

    /// Define pointer type
    typedef std::shared_ptr<RecordWithMD5Signature> pointer;

    /// Define vectortype
    typedef std::vector<pointer> vector;

#pragma db options("UNIQUE")
    /// Store the MD5 signature. It is 'mutable' because is changed in the const Callback.
    mutable std::string md5;

    /// Helper function to compute the MD5 signature
    std::string ComputeMD5() const;

    /// Need to be overloaded: compute a string for the MD5 signature
    virtual std::string ToStringForMD5() const = 0;

    /// Complete the print format "md5"
    virtual void DumpInTable(syd::PrintTable2 & table) const;

    /// Add and update the MD5 signature
    virtual void Callback(odb::callback_event,
                          odb::database & odb,
                          syd::Database * db) const;

  protected:
    RecordWithMD5Signature();

  };

} // end namespace
// --------------------------------------------------------------------

#endif
