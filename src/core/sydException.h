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

#ifndef SYDEXCEPTION_H
#define SYDEXCEPTION_H

// std
#include <string>
#include <exception>

namespace syd {

  // --------------------------------------------------------------------
  /// Base class for exception in syd. By default print message, file,
  /// line and function.
  class Exception : public std::exception
  {
  public:

    /// Standard constructor
    Exception(const std::string& message);

    /// Main class to get the error message
    virtual const char* what() const noexcept;

    /// The error message
    std::string message_;

    /// Store the file where the exception is thrown (see macro)
    std::string file_;

    /// Store the line where the exception is thrown (see macro)
    int line_;

    /// Store the function where the exception is thrown (see macro)
    std::string function_;
  };
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  /// Main macro for exception (store function, line and file)
#define EXCEPTION(message)                      \
  {                                             \
    std::ostringstream os;                      \
    os << message;                              \
    syd::Exception se(os.str());                \
    se.file_ = __FILE__;                        \
    se.line_ = __LINE__;                        \
    se.function_ = __FUNCTION__;                \
    throw se;                                   \
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  /// Function called when a seg signal occur, for uncaugh exception
  void terminateHandler();
  // --------------------------------------------------------------------

} // end namespace

#endif
