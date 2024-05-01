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
// Created by jws3 on 4/22/24.
//

#ifndef VIEWER_VIEWSTACK_H
#define VIEWER_VIEWSTACK_H

#include <iostream>
#include <tuple>
#include <list>

class ViewStack {

public:
  ViewStack ();
  void pushView ( double x0, double y0, double x1, double y1 );
  int popView ( double& x0, double& y0, double& x1, double& y1 );
  bool empty ( void );
  void clear ( void );
  void show ( void );

  std::list<std::tuple<double, double, double, double>> views;

};

#endif //VIEWER_VIEWSTACK_H
