//
// Created by jws3 on 6/12/24.
//

#include "DataMediatorFac.h"

DataMediatorFac::DataMediatorFac() {

}

std::shared_ptr<DataMediator> DataMediatorFac::createDataMediator( const std::string& name ) {
  return std::shared_ptr<DataMediator>( new DataMediator( name ) );
}
