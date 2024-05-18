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

#ifndef VIEWER_VIEWERGRAPHAREA_H
#define VIEWER_VIEWERGRAPHAREA_H

#include <Qt>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
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
#include "ViewerSlider.h"

class ViewerGraphArea : public ViewerGraphAreaBase {

  Q_OBJECT

  public:
    explicit ViewerGraphArea( int _id, QWidget *parent = nullptr );
    virtual ~ViewerGraphArea();
    ViewerGraphArea( const ViewerGraphArea& ) = delete;
    ViewerGraphArea( ViewerGraphArea& ) = delete;
    ViewerGraphArea( ViewerGraphArea&& ) = delete;
    ViewerGraphArea& operator=( const ViewerGraphArea& ) = delete;
    ViewerGraphArea& operator=( ViewerGraphArea& ) = delete;
    ViewerGraphArea& operator=( ViewerGraphArea&& ) = delete;
    virtual void setCurInfo( QString, int );
    virtual void getCurInfo( QString&, int& );
    virtual void clear();
    virtual void enableFftButton( bool );
    virtual void updateMousePosition( double x, double y );
    virtual void updateSelectionRange( double xStart, double xEnd );
    virtual void setInitialState( void );

    // int id, w, h;
    // QVBoxLayout *vlayout;
    // QHBoxLayout *hlayout;
    // ViewerGraphBase *graph;

    QSharedPointer<QHBoxLayout> hlayout2;
    QSharedPointer<ViewerSlider> slider;
    QSharedPointer<QLabel> sigLabel;
    QSharedPointer<Combo> sigName;
    QSharedPointer<QLabel> mousePos;
    QSharedPointer<QLabel> exportSelection;
    QSharedPointer<QLabel> selectionXMin;
    double selectionXMinValue;
    QSharedPointer<QLabel> selectionXMax;
    double selectionXMaxValue;
    QSharedPointer<QLabel> selectionDeltaX;
    double selectionDeltaXValue;
    int curSigIndex;
    QString curFileName;
    QSharedPointer<ViewerCtlrPushButton> calcFft;
    QSharedPointer<QPushButton> lockTimeScale;
    bool lockTimeScaleState;
    QSharedPointer<QPushButton> useRoiIndicators;
    bool useRoiIndicatorsState;
    QSharedPointer<RoiSelector> roiSelect;
    int curSliderValue;

public slots:
  void performAction ( bool flag );
  void roiSelectorChange( int id, int left, int right );
  void doLeftInc( int dir );
  void doLeftBigInc( int dir );
  void doRightInc( int dir );
  void doRightBigInc( int dir );
  void doScaleToSelection( void );
  
signals:
  void enableLockTimeScale ( bool );
  void vgaRoiSelectScale ( QWidget *vga, int, QString&, double, double );
  
};


#endif //VIEWER_VIEWERGRAPHAREA_H
