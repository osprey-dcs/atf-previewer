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
// Created by jws3 on 4/16/24.
//

#ifndef VIEWER_VIEWERCHART_H
#define VIEWER_VIEWERCHART_H

#include <Qt>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QPushButton>
#include <QCheckBox>
#include <QPalette>
#include <QDebug>
#include <QFont>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QGraphicsWidget>

#include "Cnst.h"


class ViewerChart : public QtCharts::QChart {

  Q_OBJECT

public:
  ViewerChart();
  virtual ~ViewerChart();
  void paint( QPainter *p );
  void paintEvent( QPaintEvent *event );
};


#endif //VIEWER_VIEWERCHART_H
