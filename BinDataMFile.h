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

#ifndef VIEWER_BINDATAMFILE_H
#define VIEWER_BINDATAMFILE_H

#include <utility>
#include <memory>

#include "VDisk.h"
#include "BinDataBase.h"

class BinDataMFile : public BinDataBase {

  public:

  VDisk vdisk;

  BinDataMFile ();
  virtual ~BinDataMFile ();
  BinDataMFile ( const BinDataMFile& ) = delete;
  BinDataMFile ( BinDataMFile& ) = delete;
  BinDataMFile ( BinDataMFile&& ) = delete;
  BinDataMFile& operator= ( const BinDataMFile& ) = delete;
  BinDataMFile& operator= ( BinDataMFile& ) = delete;
  BinDataMFile& operator= ( BinDataMFile&& ) = delete;

  int newFile ( std::string fileName );

  virtual int readHeader ( void );
  
  virtual int readVersion ( int64_t& major, int64_t& minor, int64_t& release );

  virtual int readNumBytes ( int64_t& num );

  void initMaxBufSize( int64_t max );

  int getMaxElements ( std::string filename, int64_t& max );

  int64_t readTraceData( int *buf, int64_t readSizeInbytes );
  
  int64_t readTraceData( std::filebuf& fb, int *buf, int64_t readSizeInbytes );

};

#endif //VIEWER_BINDATAMFILE_H
