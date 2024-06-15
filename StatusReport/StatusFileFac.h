//
// Created by jws3 on 6/15/24.
//

#ifndef STATUSREPORT_STATUSFILEFAC_H
#define STATUSREPORT_STATUSFILEFAC_H

#include <memory>
#include <string>

#include "PsnStatusFile.h"

static std::shared_ptr<StatusFileBase> sf = nullptr;

class StatusFileFac {

  public:
    StatusFileFac();
    std::shared_ptr<StatusFileBase> createStatusFile( std::string name );

};


#endif //STATUSREPORT_STATUSFILEFAC_H
