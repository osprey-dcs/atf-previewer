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

#ifndef VIEWER_COMBO_H
#define VIEWER_COMBO_H

#include <iostream>

#include <QObject>
#include <QWidget>
#include <QComboBox>

class Combo : public QComboBox {

  Q_OBJECT

  public:
  
  Combo( QWidget *w );
  QWidget *callbackW;

  public slots:
  
  void relayActivate( int index );

  signals:
  
  void activated1( int index, int sigIndex, QWidget *w );

};


#endif //VIEWER_COMBO_H
