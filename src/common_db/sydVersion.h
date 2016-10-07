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

#ifndef SYDVERSION_H
#define SYDVERSION_H

// Change the version here everytime the database scheme is changed.
// http://www.codesynthesis.com/products/odb/doc/manual.xhtml#13.2

// We keep the model version close to prevent compiling changes. If
// the change is legitimate: 1) open and 2) increment the
// version. 3) Once checked, close again.
// It  means that we can migrate from base version to current version)
#define SYD_VERSION      0117 // version 1.17 (current version)
#define SYD_BASE_VERSION 0101 // version 1.1 (base version)

// Log:
// 1.17 : add comments
// 1.16 :
// 1.15 : add table Elastix
// 1.14 : change table Image (acqui date etc)
// 1.13 : add tags to RoiStatistic
// 1.11 :
// 1.10 : md5 unique field
// 1.9 : fitresult with md5
// 1.8 : (md5 unique)
// 1.7 : md5
// 1.6 : FitResult
// 1.5 : TimePoints
// 1.3 : add pixel_scale, pixel_offset in table DicomSerie
// 1.2 : add 'duration' in table DicomSerie
// 1.1 : initial version


// Ugly trick below ... (you have been warn)

// Concatenate 0x before the version number
// First argument of BUILD_VERSION is ignored here. It will be used in derived schema
// Example :
// #pragma db model version(BUILD_VERSION(SYD_BASE_VERSION,NEW_BASE_VERSION), BUILD_VERSION(SYD_VERSION,NEW_VERSION), closed)
// http://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
#define PASTER(x,y) 0x ## x ## ## y
#define BUILD_VERSION(x,y)  PASTER(x,y)

#ifdef ODB_COMPILER
#pragma db model version(BUILD_VERSION(,SYD_BASE_VERSION), BUILD_VERSION(,SYD_VERSION), close)
#endif

#endif
