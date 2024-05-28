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
// Created by jws3 on 4/8/24.
//

#include "Combo.h"

Combo::Combo( QWidget *w ) : QComboBox( w ) {

  this->setEditable( false );

  callbackW = w;

  //std::cout << "Combo - call connect" << std::endl;
  this->connect( this, SIGNAL( activated( int ) ),
                 this, SLOT( relayActivate( int ) ) );

}

void Combo::relayActivate( int index ) {
  //std::cout << "relayActivate, index = " << index << std::endl;
  emit activated1( index, currentData().toInt(), callbackW );
}
