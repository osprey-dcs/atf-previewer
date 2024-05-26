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

#include "ViewStack.h"

ViewStack::ViewStack () : ErrHndlr( NumErrs, errMsgs ) {}

void ViewStack::pushView ( double x0, double y0, double x1, double y1 ) {
  this->views.push_back( std::make_tuple( x0, y0, x1, y1 ) );
}

int ViewStack::popView ( double& x0, double& y0, double& x1, double& y1 ) {

  if (!(this->views.empty())) {

    auto tpl = this->views.back();
    x0 = std::get<0>(tpl);
    y0 = std::get<1>(tpl);
    x1 = std::get<2>(tpl);
    y1 = std::get<3>(tpl);
    this->views.pop_back();
    return 0;

  } else {

    x0 = y0 = x1 = y1 = 0;
    return ERRINFO(EUnderflow,"");

  }

}

bool ViewStack::empty ( void ) {
  return this->views.empty();
}

void ViewStack::clear ( void ) {
  views.clear();
}

void ViewStack::show ( void ) {

  for ( auto v : this->views ) {

    double x0 = std::get<0>( v );
    double y0 = std::get<1>( v );
    double x1 = std::get<2>( v );
    double y1 = std::get<3>( v );

    std::cout << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;
  }
}
