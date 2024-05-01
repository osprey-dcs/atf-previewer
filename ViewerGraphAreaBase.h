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

#ifndef VIEWER_VIEWERGRAPHAREABASE_H
#define VIEWER_VIEWERGRAPHAREABASE_H

#include <memory>

#include <Qt>
#include <QWidget>
#include <QColor>
#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QPalette>
#include <QDebug>
#include <QVBoxLayout>
#include <QFont>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLineSeries>
#include <QSharedPointer>

#include "ViewerGraph.h"

class ViewerGraphAreaBase : public QWidget {

  Q_OBJECT

  public:
    ViewerGraphAreaBase();
    explicit ViewerGraphAreaBase( int _id, QWidget *parent = nullptr );
    virtual ~ViewerGraphAreaBase();
    ViewerGraphAreaBase( const ViewerGraphAreaBase& ) = delete;
    ViewerGraphAreaBase( ViewerGraphAreaBase& ) = delete;
    ViewerGraphAreaBase( ViewerGraphAreaBase&& ) = delete;
    ViewerGraphAreaBase& operator=( const ViewerGraphAreaBase& ) = delete;
    ViewerGraphAreaBase& operator=( ViewerGraphAreaBase& ) = delete;
    ViewerGraphAreaBase& operator=( ViewerGraphAreaBase&& ) = delete;
    virtual void setCurInfo( QString, int );
    virtual void getCurInfo( QString&, int& );
    virtual void clear();
    virtual void enableFftButton( bool );
    virtual void updateMousePosition( double x, double y );
    virtual void updatePeakInfo( double x, double y );
    virtual void setInitialState( void );

    int id, w, h;
    QSharedPointer<QVBoxLayout> vlayout;
    QSharedPointer<QHBoxLayout> hlayout;
    QSharedPointer<ViewerGraph> graph;

    int curSigIndex;
    QString curFileName;

};


#endif //VIEWER_VIEWERGRAPHAREABASE_H
