//
// Created by jws3 on 5/10/24.
//

#include <iostream>

#include <QDebug>
#include <utility>

#include "ChanSelector.h"
#include "Cnst.h"

ChanSelector::ChanSelector() : ErrHndlr( NumErrs, errMsgs ) {

  this->haveList = false;

}

ChanSelector::~ChanSelector() {

}

int ChanSelector::setText( QString s ) {
  //qWarning()<<__func__<<"expr"<<s;

  const int FindStartDigit = 1;
  const int FindDigitOrCommaOrDashOrEnd = 2;
  const int FindEndDigit = 3;
  const int FindDigitOrCommaOrEnd = 4;
  const int ElimTextEditNewLine = 5;

  int state, startDigit, endDigit, error;

  text = std::move(s);

  chans.clear();
  error = 0;
  state = FindStartDigit;

  //std::cout<<  "ChanSelector::setText, text = " << text.toStdString() << std::endl;

  for ( auto c : text ) {

    if ( error ) break;

    //std::cout << "state = " << state << std::endl;

    switch ( state ) {

      case FindStartDigit:

        startDigit = 0;
        endDigit = 0;

        if ( c == "\\" ) {
          state = ElimTextEditNewLine;
          break;
        }
        else if ( c.isDigit() ) {
          startDigit = c.digitValue();
          state = FindDigitOrCommaOrDashOrEnd;
          break;
        }
        else if ( !c.isSpace() ) {
          error = 1;
          break;
        }

        break;

      case FindDigitOrCommaOrDashOrEnd:

        if ( c == "\\" ) {
          state = ElimTextEditNewLine;
          break;
        }
        else if ( c.isDigit() ) {
          startDigit = startDigit * 10 + c.digitValue();
          break;
        }
        else if ( c == "," ) {
          if ( ( startDigit > 0 ) && ( startDigit <= Cnst::MaxSigIndex+1 ) ) {
            chanSet.insert( startDigit );
          }
          else {
            error = 3;
          }
          state = FindStartDigit;
          break;
        }
        else if ( c == "-" ) {
          endDigit = 0;
          state = FindEndDigit;
          break;
        }
        else {
          error = 1;
          break;
        }

        break;

      case FindEndDigit:

        if ( c == "\\" ) {
          state = ElimTextEditNewLine;
          break;
        }
        else if ( c.isDigit() ) {
          endDigit = c.digitValue();
          state = FindDigitOrCommaOrEnd;
          break;
        }
        else if ( !c.isSpace() ) {
          error = 1;
          break;
        }

        break;

      case FindDigitOrCommaOrEnd:

        if ( c == "\\" ) {
          state = ElimTextEditNewLine;
          break;
        }
        else if ( c.isDigit() ) {
          endDigit = endDigit * 10 + c.digitValue();
          break;
        }
        if ( c == "," ) {
          //std::cout << "startDigit = " << startDigit << std::endl;
          //std::cout << "endDigit = " << endDigit << std::endl;
          for ( int i=startDigit; i<=endDigit; i++ ) {
            if ( ( i > 0 ) && ( i <= Cnst::MaxSigIndex+1 ) ) {
              chanSet.insert( i );
            }
            else {
              error = 3;
            }
          }
          state = FindStartDigit;
          break;
        }
        //else if ( !c.isSpace() ) {
        else {
          error = 1;
          break;
        }

        break;

    case ElimTextEditNewLine:

      if ( c != "n" ) {
        error = 4;
        break;
      }

      break;

    default:
      // do nothing
      break;

    }

  }

  // this is the end of data state

  if ( state == ElimTextEditNewLine ) {
    error = 4;
  }
  else if ( state == FindStartDigit )  {
    chanSet.clear();
    chans.clear();
    error = 2; // list is empty
  }
  else if ( state == FindEndDigit ) {
    error = 1;
  }
  else if ( state == FindDigitOrCommaOrDashOrEnd ) {
    if ( ( startDigit > 0 ) && ( startDigit <= Cnst::MaxSigIndex+1 ) ) {
      chanSet.insert( startDigit );
    }
    else {
      error = 3;
    }
  }
  else if ( state == FindDigitOrCommaOrEnd ) {
    for ( int i=startDigit; i<=endDigit; i++ ) {
      if ( ( i > 0 ) && ( i <= Cnst::MaxSigIndex+1 ) ) {
        chanSet.insert( i );
      }
      else {
        error = 3;
      }
    }
  }

  if ( error ) {
    //qWarning() << __func__ << "error = " << error;
    return ERRINFO(error,"");
  }

  for ( auto c : chanSet ) {
    chans.push_back( c );
  }

  chans.sort();
  //qWarning()<<__func__<<"selected"<<chans;

  return 0;

}

const std::list<int>& ChanSelector::getList( void ) {

    return chans;

}

void ChanSelector::report( void ) {

   std::cout << "report" << std::endl;
   std::cout << "text = " << text.toStdString() << std::endl;
   std::cout << "list:" << std::endl;
   for ( auto itme : chans ) {
     std::cout << itme << " ";
   }
   std::cout << std::endl;

}
