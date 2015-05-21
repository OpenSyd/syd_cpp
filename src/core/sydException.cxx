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

#include "sydException.h"

// --------------------------------------------------------------------
syd::Exception::Exception(const std::string& message) throw()
  :std::exception(), message_(message)
{
  file_ = "unknown_file";
  function_ = "unknown_function";
  line_ = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
const char* syd::Exception::what() const noexcept
{
  return message_.c_str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::terminateHandler()
{
  std::exception_ptr exptr = std::current_exception();
  if (exptr != 0) {
    // the only useful feature of std::exception_ptr is that it can be rethrown
    try {
      std::rethrow_exception(exptr);
    }
    catch (syd::Exception &ex) {
      LOG(FATAL) << ex.what() << std::endl
                 << "Exception not caught: " << ex.file_
                 << ":" << ex.function_
                 << ":" << ex.line_;
    }
    catch (std::exception &ex) {
      LOG(FATAL) << "(std exception): " << ex.what();
    }
    catch (...) {
      LOG(FATAL) << "Terminated due to unknown exception.";
    }
  }
  LOG(FATAL) << "Terminated due to unknown reason :(";
}
// --------------------------------------------------------------------
