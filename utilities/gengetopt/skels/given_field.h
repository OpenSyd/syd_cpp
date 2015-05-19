/*
 * File automatically generated by
 * gengen 1.2 by Lorenzo Bettini 
 * http://www.gnu.org/software/gengen
 */

#ifndef GIVEN_FIELD_GEN_CLASS_H
#define GIVEN_FIELD_GEN_CLASS_H

#include <string>
#include <iostream>

using std::string;
using std::ostream;

class given_field_gen_class
{
 protected:
  string arg_name;
  bool group;
  string long_opt;

 public:
  given_field_gen_class() :
    group (false)
  {
  }
  
  given_field_gen_class(const string &_arg_name, bool _group, const string &_long_opt) :
    arg_name (_arg_name), group (_group), long_opt (_long_opt)
  {
  }

  static void
  generate_string(const string &s, ostream &stream, unsigned int indent)
  {
    if (!indent || s.find('\n') == string::npos)
      {
        stream << s;
        return;
      }

    string::size_type pos;
    string::size_type start = 0;
    string ind (indent, ' ');
    while ( (pos=s.find('\n', start)) != string::npos)
      {
        stream << s.substr (start, (pos+1)-start);
        start = pos+1;
        if (start+1 <= s.size ())
          stream << ind;
      }
    if (start+1 <= s.size ())
      stream << s.substr (start);
  }

  void set_arg_name(const string &_arg_name)
  {
    arg_name = _arg_name;
  }

  void set_group(bool _group)
  {
    group = _group;
  }

  void set_long_opt(const string &_long_opt)
  {
    long_opt = _long_opt;
  }

  void generate_given_field(ostream &stream, unsigned int indent = 0);
  
};

#endif // GIVEN_FIELD_GEN_CLASS_H
