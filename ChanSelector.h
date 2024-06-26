//
// Created by jws3 on 5/10/24.
//

#ifndef VIEWER_CHANSELECTOR_H
#define VIEWER_CHANSELECTOR_H

#include <set>
#include <list>

#include <QString>
#include "ErrHndlr.h"

class ChanSelector : public ErrHndlr {

public:

  static const int NumErrs = 5;
  static const int E_Success = 0;
  static const int E_Syntax = 1;
  static const int E_Empty = 2;
  static const int E_Bounds = 3;
  static const int E_Expected_n = 4;
  inline static const std::string errMsgs[NumErrs] {
    { "Success" },
      { "Syntax error" },
      { "Channel list is empty" },
      { "Channel value out of range" },
      { "Missing new line marker" }
  };

  ChanSelector();
  virtual ~ChanSelector();
  int setText( QString s );
  const std::list<int>& getList( void );
  void report( void );

private:
  std::set<int> chanSet;
  std::list<int> chans;
  QString text;
  bool haveList;

};


#endif //VIEWER_CHANSELECTOR_H
