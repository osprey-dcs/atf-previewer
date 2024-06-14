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
// Created by jws3 on 4/5/24.
//

#include <list>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "BinData.h"

BinData::BinData() {

}

BinData::~BinData() {

}

int64_t BinData::readTraceData (
 int *buf,
 int64_t readSizeInbytes ) {

  if ( !isOpenRead ) {
    return ENoFile;
  }

  int64_t n = oneFb.sgetn( reinterpret_cast<char *>( buf ), readSizeInbytes );
  return n;

}

int64_t BinData::readTraceData (
 std::filebuf& fb,
 int *buf,
 int64_t readSizeInbytes ) {

  int64_t n = fb.sgetn( reinterpret_cast<char *>( buf ), readSizeInbytes );
  return n;

}
