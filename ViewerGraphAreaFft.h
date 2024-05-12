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

#ifndef VIEWER_VIEWERGRAPHAREAFFT_H
#define VIEWER_VIEWERGRAPHAREAFFT_H

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
#include <QComboBox>
#include <QLabel>
#include <QPalette>
#include <QDebug>
#include <QVBoxLayout>
#include <QFont>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include "ViewerGraph.h"
#include "Combo.h"
#include "ViewerGraphAreaBase.h"
#include "ViewerCtlrPushButton.h"

class ViewerGraphAreaFft : public ViewerGraphAreaBase {

  Q_OBJECT

  public:
    explicit ViewerGraphAreaFft( int _id, QWidget *parent = nullptr );
    virtual ~ViewerGraphAreaFft();
    ViewerGraphAreaFft( const ViewerGraphAreaFft& ) = delete;
    ViewerGraphAreaFft( ViewerGraphAreaFft& ) = delete;
    ViewerGraphAreaFft( ViewerGraphAreaFft&& ) = delete;
    ViewerGraphAreaFft& operator=( const ViewerGraphAreaFft& ) = delete;
    ViewerGraphAreaFft& operator=( ViewerGraphAreaFft& ) = delete;
    ViewerGraphAreaFft& operator=( ViewerGraphAreaFft&& ) = delete;
    //virtual void setCurInfo( QString, int );
    //virtual void getCurInfo( QString&, int& );
    virtual void clear();
    virtual void updateMousePosition( double x, double y );
    virtual void updatePeakInfo( double x, double y );

    // use no-op from base class
    //virtual void updateSelectionRange( double xStart, double xEnd );

    // int id, w, h;
    // QVBoxLayout *vlayout;
    // QHBoxLayout *hlayout;
    // ViewerGraph *graph;
    QSharedPointer<QLabel> mousePos;
    QSharedPointer<QLabel> peakInfo;

};


#endif //VIEWER_VIEWERGRAPHAREAFFT_H
