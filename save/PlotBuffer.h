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
// Created by jws3 on 4/6/24.
//
// This call allocates a plot buffer - nothing special for now but future use may be to manage lookaside lists
//

#ifndef VIEWER_PLOTBUFFER_H
#define VIEWER_PLOTBUFFER_H


class PlotBuffer {

public:

  PlotBuffer();
  PlotBuffer( const PlotBuffer& ) = delete;
  PlotBuffer( PlotBuffer& ) = delete;
  PlotBuffer( PlotBuffer&& ) = delete;
  PlotBuffer& operator=( const PlotBuffer& ) = delete;
  PlotBuffer& operator=( PlotBuffer& ) = delete;
  PlotBuffer& operator=( PlotBuffer&& ) = delete;

  void delBuf( void );
  int *getBuffer();

};


#endif //VIEWER_PLOTBUFFER_H
