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

//
// Created by jws3 on 4/2/24.
//

#ifndef VIEWER_BinDataFAC_H
#define VIEWER_BinDataFAC_H

#include <memory>
#include <string>

#include "BinData.h"

static std::shared_ptr<BinDataBase> bd = nullptr;
//static BinDataBase *bd = nullptr;

class BinDataFac {

  public:
    BinDataFac();
    std::shared_ptr<BinDataBase> createBinData( std::string name );

  };

#endif //VIEWER_BinDataFAC_H
