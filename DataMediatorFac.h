//
// Created by jws3 on 6/12/24.
//

#ifndef VIEWER_DATAMEDIATORFAC_H
#define VIEWER_DATAMEDIATORFAC_H

#include <memory>
#include <string>

#include "DataMediator.h"

static std::shared_ptr<DataMediator> dm = nullptr;

class DataMediatorFac {

public:
  DataMediatorFac();
  std::shared_ptr<DataMediator> createDataMediator( const std::string& name );

};


#endif //VIEWER_DATAMEDIATORFAC_H
