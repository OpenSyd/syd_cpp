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

// Change the version here everytime the database scheme is changed
#define SYD_VERSION      0x0002 // 00.02
#define SYD_BASE_VERSION 0x0001 // 00.01

#ifdef ODB_COMPILER
#pragma db model version(SYD_BASE_VERSION, SYD_VERSION)
#endif

#endif
