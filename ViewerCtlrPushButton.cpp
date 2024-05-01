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
// Created by jws3 on 4/21/24.
//

#include "ViewerCtlrPushButton.h"

ViewerCtlrPushButton::ViewerCtlrPushButton ( ) {}
ViewerCtlrPushButton::ViewerCtlrPushButton (QString _text ) {
  this->setText( _text );
}
ViewerCtlrPushButton::ViewerCtlrPushButton(QWidget *_w, QString _text ) {
  this->setText( _text );
  w = _w;
  connect( this, SIGNAL( clicked( bool ) ), this, SLOT( clicked0( bool ) ) );
}

ViewerCtlrPushButton::~ViewerCtlrPushButton() {}

void ViewerCtlrPushButton::clicked0(bool flag ) {

  emit this->pressed1 ( w );

};

