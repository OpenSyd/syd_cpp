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
#include <typeinfo>

// --------------------------------------------------------------------
syd::Exception::Exception(const std::string& message)
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
  std::ostringstream os;
  os << message_ << std::endl
     << "In file: " << file_
     << ":" << function_
     << ":" << line_;
  std::string * s = new std::string(os.str());
  return s->c_str();
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
    catch (syd::Exception & ex) {
      // --> this work on linux (gcc) but not on osx (clang). Don't know why.
      LOG(FATAL) << ex.what() << std::endl
                 << "(Exception may be caught by a try catch)";
    }
    catch (std::exception & ex) {
      LOG(FATAL) << ex.what() << std::endl
                 << "(Exception may be caught by a try catch)";
    }
    catch (...) {
      LOG(FATAL) << "Terminated due to unknown exception.";
    }
  }
  LOG(FATAL) << "Terminated due to unknown reason :(";
}
// --------------------------------------------------------------------
