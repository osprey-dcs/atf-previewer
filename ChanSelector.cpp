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

  int state, startDigit, endDigit, error;

  text = std::move(s);

  chans.clear();
  error = 0;
  state = FindStartDigit;

  //std::cout<<  "ChanSelector::setText, text = " << text.toStdString() << std::endl;

  QChar c;
  for ( auto c1 : text ) {

    //std::cout << "state = " << state << "  c1 = " << c1.toLatin1() << "  error = " << error << std::endl;

    if ( c1.isSpace() ) continue;
    
    c = c1;
    //std::cout << "state = " << state << "  c = " << c.toLatin1() << "  error = " << error << std::endl;
    
    if ( error ) break;

    switch ( state ) {

      case FindStartDigit:

        startDigit = 0;
        endDigit = 0;

        if ( c.isDigit() ) {
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

        if ( c.isDigit() ) {
          startDigit = startDigit * 10 + c.digitValue();
          break;
        }
        else if ( c.toLatin1() == ',' ) {
          if ( ( startDigit > 0 ) && ( startDigit <= Cnst::MaxSigIndex+1 ) ) {
            chanSet.insert( startDigit );
            startDigit = 0;
            endDigit = 0;
          }
          else {
            error = 3;
          }
          state = FindStartDigit;
          break;
        }
        else if ( c.toLatin1() == '-' ) {
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

        if ( c.isDigit() ) {
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

        if ( c.isDigit() ) {
          endDigit = endDigit * 10 + c.digitValue();
          break;
        }
        if ( c.toLatin1() == ',' ) {
          for ( int i=startDigit; i<=endDigit; i++ ) {
            if ( ( i > 0 ) && ( i <= Cnst::MaxSigIndex+1 ) ) {
              chanSet.insert( i );
            }
            else {
              error = 3;
            }
          }
          startDigit = 0;
          endDigit = 0;
          state = FindStartDigit;
          break;
        }
        else {
          error = 1;
          break;
        }

        break;

    default:
      // do nothing
      break;

    }

    //std::cout << "end switch, state = " << state << "  error = " << error << std::endl; 

  }

  // this is the end of data state

  //std::cout << "end state = " << state << std::endl;

  if ( state == FindStartDigit )  {
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
      startDigit = 0;
      endDigit = 0;
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
    startDigit = 0;
    endDigit = 0;
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
