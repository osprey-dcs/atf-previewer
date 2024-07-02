//
// Created by jws3 on 6/15/24.
//

#include "StatusFileFac.h"

StatusFileFac::StatusFileFac() {

}

std::shared_ptr<StatusFileBase> StatusFileFac::createStatusFile(std::string name) {

  if ( name == "PSN File" ) {
    
    if ( !sf ) {
      sf = std::shared_ptr<StatusFileBase>( new PsnStatusFile() );
    }

  }
  else {
    
    if ( !sf ) {
      sf = std::shared_ptr<StatusFileBase>( new PsnStatusFile() );
    }

  }
  
  return sf;

}
