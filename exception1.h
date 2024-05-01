/*
This file is part of viewer.

viewer is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

viewer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with viewer.
If not, see <https://www.gnu.org/licenses/>. 
*/

#ifndef __exception1_h
#define __exception1_h 1

#include <typeinfo>

// ------------------------------------------------------------------------------------------------------

// To use this exception scheme in your class, add the following to the class header file
//
// #include <stdexcept>
// #include <iostream>
// #include <string>
// #include <string.h>
// #include "exception1.h"
//
// class Myclass ... {
//
//   static const int ESuccess = 0; // error numeric codes must start with zero and increment by one
//   static const int ESuccess = 1;
//   static const int EOne = 2;
//   static const int ETwo = 3;
//   .
//   .
//   .
//
//   static const int ErrLast = N; // where N = is the largest error number (from the above example, 3
//   static const std::string getErrMsg ( int st );
//   Exception1<Myclass> myclassE;

//   before c++17
//   static const std::string ErrMsg[ErrLast+1];
//
//   after c++17
//   inline static const std::string Myclass::ErrMsg[Myclass::ErrLast+1] = {
//   "Unknown",
//   "Success",
//   "Msg 1",
//   "Msg 2",
//   .
//   .
//   .
//   "Msg N" };
//
// }
//
// Then, in the class implementation file (myclass.cc or whatever):
//
//    In the constructor, add a line that reads
//    OVERRIDE_FACILITY(Myclass,"Myclass");
//
//    before c++17
//    // error msg array - declared in the h file as static
//    static const std::string Myclass::ErrMsg[Myclass::ErrLast+1] = {
//   "Unknown",
//   "Success",
//   "Msg 1",
//   "Msg 2",
//   .
//   .
//   .
//   "Msg N" };
//
//   // function - declared in the h file as static
//   const std::string Myclass::getErrMsg ( int st ) {
//
//   if ( ( st < 0 ) || ( st > Myclass::ErrLast ) ) {
//     return Myclass::ErrMsg[0];
//   }
//   else {
//     return Myclass::ErrMsg[st];
//   }
//
// }
//
// To use the following macros, you need to use the implied naming convention shown above, i.e.:
// If your class name is Myclass, the execption object name must be myclassE
//
// To throw an exception:
//   THROWE(myclassE,fuctionName,Eone);
//
// After catching ( &e ), display exception info:
//   SHOWE(e);
//
// After catching ( &e ), take some action based on a specific exception:
//   if ( EXCEPTION_IS(myclass,Eone) ) {
//     do-whatever;
//   }
//
//
// If Myclass is a template class then the exeception facility name displayed may be
// overridden as follows:
//
// Myclass<sometype> obj;
// obj.MyclassE.overrideFacility( "Myclass<sometype>" );
//
// ------------------------------------------------------------------------------------------------------

#define THROW1(exceptionobj,funcname,facility,errorname)\
  { exceptionobj.set(__FILE__,__LINE__,funcname,facility,errorname);throw exceptionobj; }

#define THROWE(obj,funcname,errorname)\
  { obj##E.set(__FILE__,__LINE__, #funcname, typeid(obj##E).name(), errorname);throw obj##E; }

#define SHOWE(e)\
  { std::cout << "-------------------------------------------------------------------------------------"\
    << std::endl; }\
  { std::cout << "In " << __FILE__ << " at line " << std::dec << __LINE__ << ":" << std::endl; \
    std::cout << "  Exception from " << e.what() <<  std::endl; }\
  { std::cout << "-------------------------------------------------------------------------------------"\
    << std::endl; }

#define FACILITY(e)\
  ( dynamic_cast<Exception1Base *>(&e)?(dynamic_cast<Exception1Base *>(&e))->facility():std::string("?") )

#define OVERRIDE_FACILITY(Etype,str)\
  ( Exception1<Etype>::overrideFacility(std::string(str)) )

#define ERRNUM(e)                                                       \
  ( dynamic_cast<Exception1Base *>(&e)?(dynamic_cast<Exception1Base *>(&e))->error():-1 )

#define EXCEPTION_IS(fac,num)\
  ( ( FACILITY(e) == #fac ) && ( ERRNUM(e) == num ) )

#define DBGE(fac,num)\
  std::cout << "facility " << FACILITY(e) << std::endl;      \
  std::cout << "     fac " << #fac << std::endl;


class Exception1Base : public std::exception {

public:

  static const unsigned int StrSize = 255;
  static const unsigned int FacSize = 31;

  Exception1Base () { m_err = 0; strcpy(m_facility,""); strcpy(m_str,""); }

  void set ( std::string file, int line, std::string func, std::string oneFacility, int err  ) {
    strncpy( m_facility, oneFacility.c_str(), FacSize );
    m_facility[FacSize] = 0;
    snprintf( m_str, StrSize, "%s, at line %-d, function %s::%s ",
              file.c_str(), line, facility().c_str(), func.c_str() );
    m_str[StrSize]=0;
    m_err=err;
  }

  int error ( void ) {
    return m_err;
  }

  virtual std::string facility ( void ) {
    return std::string(m_facility);
  }

protected:

  char m_facilityOverride[FacSize+1];

  int m_err;
  char m_facility[FacSize+1];
  char m_str[StrSize+1];
    
};

template <class T>
class Exception1 : public Exception1Base {

public:

  static const unsigned int StrSize = 255;
  static const unsigned int FacSize = 31;

  static bool s_init;
  static char s_facilityOverride[FacSize+1];
  
  Exception1 () : Exception1Base() {
    m_err = 0;
    strcpy(m_facility,"");
    strcpy(m_str,"");
  }

  virtual const char* what() const throw () {

    static std::string s;

    if ( ( m_err >= 0 ) && ( m_err <= T::ErrLast ) ) {
      s = std::string(m_str) + std::string(T::getErrMsg(m_err));
      return s.c_str();
    }
    else {
      s = std::string(m_str) + "Unknown";
      return s.c_str();
    }
      
  }

  virtual std::string facility ( void ) {
    if ( s_init ) {
      s_init = false;
      strcpy( s_facilityOverride, "" );
    }
    if ( strcmp( s_facilityOverride, "" ) ) return std::string(s_facilityOverride);
    return std::string(m_facility);
  }

  static void overrideFacility ( std::string facility ) {
    if ( s_init ) {
      s_init = false;
      strcpy( s_facilityOverride, "" );
    }
    strncpy( s_facilityOverride, facility.c_str(), FacSize );
  }
  
};

template <typename T>
bool Exception1<T>::s_init = true;

template <typename T>
char Exception1<T>::s_facilityOverride[FacSize+1];

#endif
