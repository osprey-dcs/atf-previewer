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

#include <iostream>
#include <sstream>
#include <iomanip>

#include "ViewerGraphArea.h"

static QColor qcolorBlack( "black" );
static QColor qcolorWhite( "white" );

ViewerGraphArea::ViewerGraphArea( int _id, QWidget *_parent ) : ViewerGraphAreaBase( _id, _parent ) {

  //std::cout << "ViewerGraphArea created" << std::endl;
  
  vlayout = QSharedPointer<QVBoxLayout>( new QVBoxLayout() );
  hlayout = QSharedPointer<QHBoxLayout>( new QHBoxLayout() );
  hlayout2 = QSharedPointer<QHBoxLayout> ( new QHBoxLayout() );
  slider = QSharedPointer<ViewerSlider>( new ViewerSlider( this ) );
  graph = QSharedPointer<ViewerGraphBase>( new ViewerGraph( id, ViewerGraph::DataType::TimeSeries, this ) );
  mousePos = QSharedPointer<QLabel>( new QLabel( "          " ) );
  exportSelection = QSharedPointer<QLabel>( new QLabel( " " ) );
  selectionXMin = QSharedPointer<QLabel>( new QLabel( " " ) );
  selectionXMax = QSharedPointer<QLabel>( new QLabel( " " ) );
  selectionDeltaX = QSharedPointer<QLabel>( new QLabel( " " ) );
  sigLabel = QSharedPointer<QLabel>( new QLabel( "Signal" ) );
  sigName = QSharedPointer<Combo>( new Combo( this ) );
  sigName->setEditable( false );
  sigName->clear();
  curSigIndex = 0;
  curFileName = "";
  calcFft = QSharedPointer<ViewerCtlrPushButton>( new ViewerCtlrPushButton(this, "FFT" ) );
  lockTimeScale = QSharedPointer<QPushButton>( new QPushButton( "Lock Time Scale" ) );
  lockTimeScale->setCheckable( true );
  lockTimeScale->setDown( false );
  lockTimeScaleState = false;
  useRoiIndicators = QSharedPointer<QPushButton>( new QPushButton( "Show Selectors" ) );
  useRoiIndicators->setCheckable( true );
  useRoiIndicators->setDown( false );
  useRoiIndicatorsState = false;
  enableFftButton( false );
  curSliderValue = 0;
  roiSelect = QSharedPointer<RoiSelector>( new RoiSelector( this->graph.data()) );

  sigName->addItem( QString( "                    " ), -1 );

  w = 200;
  h = 100;
  this->setMinimumSize( w, h );
  this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  vlayout->addLayout( hlayout.data() );
  hlayout->addWidget( sigLabel.data(), 0 );
  hlayout->addWidget( sigName.data(), 0 );
  sigName->setMinimumWidth( 250 );
  hlayout->addWidget( calcFft.data(), 0 );
  hlayout->addWidget( lockTimeScale.data(), 0 );
  hlayout->addWidget( useRoiIndicators.data(), 0 );
  
  hlayout->addSpacing( 100 );
  hlayout->addWidget( exportSelection.data(), 0 );
  hlayout->addSpacing( 20 );
  hlayout->addWidget( selectionXMin.data(), 0 );
  hlayout->addSpacing( 20 );
  hlayout->addWidget( selectionXMax.data(), 0 );
  hlayout->addSpacing( 20 );
  hlayout->addWidget( selectionDeltaX.data(), 0 );
  hlayout->addStretch( 1000 );
  hlayout->addWidget( mousePos.data(), 0 );
  
  vlayout->addLayout( hlayout2.data() );
  hlayout2->addWidget( slider.data() );
  
  vlayout->addWidget( graph.data() );
  this->setLayout( vlayout.data() );

  curSliderValue = 0;
  slider->setValue( 0 );

  connect( lockTimeScale.data(), SIGNAL( clicked( bool ) ),
           this, SLOT( performAction( bool ) ) );

  connect( useRoiIndicators.data(), SIGNAL( clicked( bool ) ),
           this, SLOT( performAction( bool ) ) );

  this->connect( roiSelect.data(), SIGNAL( roiRange( int, int, int ) ),
                 this, SLOT( roiSelectorChange( int, int, int ) ) );

  this->connect( graph.data(), SIGNAL( scaleToSelection( void ) ),
                 this, SLOT( doScaleToSelection( void ) ) );

  this->connect( graph.data(), SIGNAL( leftInc ( int ) ),
                 this, SLOT( doLeftInc( int ) ) );
    
  this->connect( graph.data(), SIGNAL( leftBigInc ( int ) ),
                 this, SLOT( doLeftBigInc( int ) ) );
    
  this->connect( graph.data(), SIGNAL( rightInc ( int ) ),
                 this, SLOT( doRightInc( int ) ) );
    
  this->connect( graph.data(), SIGNAL( rightBigInc ( int ) ),
                 this, SLOT( doRightBigInc( int ) ) );
    
}

ViewerGraphArea::~ViewerGraphArea( ) {

}

void ViewerGraphArea::setCurInfo( QString _curFileName, int _curSigIndex ) {
  curFileName = _curFileName;
  curSigIndex = _curSigIndex;
  //qDebug() << "vga id: " << this->id << "  -  " << curFileName << "  -  " << curSigIndex;
}

void ViewerGraphArea::getCurInfo( QString& _curFileName, int& _curSigIndex ) {
  _curFileName = curFileName;
  _curSigIndex = curSigIndex;
}

void ViewerGraphArea::setInitialState( void ) {

  //roiSelect->setInitialState();
  
  if ( graph ) {
    graph->views.clear();
    graph->prevViewAction->setEnabled( false );
  }
  curSliderValue = 0;
  slider->setValue( 0 );

}

void ViewerGraphArea::clear ( void ) {
  
  curSigIndex = 0;
  curFileName = "";
  if ( graph ) graph->clear();

}

void ViewerGraphArea::enableFftButton( bool ena ) {
  calcFft->setEnabled( ena );
}

void ViewerGraphArea::updateMousePosition( double x, double y ) {

  std::stringstream strm;
  strm << "X, Y: [ " << std::left << std::setprecision(8) << x << ", " << std::left << std::setprecision(8) << y << " ]";
  QString text( strm.str().c_str() );
  mousePos->setText( text );
  mousePos->update();

}

void ViewerGraphArea::updateSelectionRange( double xMin, double xMax ) {

  selectionXMinValue = xMin;
  selectionXMaxValue = xMax;
  selectionDeltaXValue = xMax - xMin;
  
  std::stringstream strm1, strm2, strm3;

  strm1 << "[ minT: " << std::setw(9) << std::left << std::setprecision(8) << xMin;
  QString text( strm1.str().c_str() );
  text = text.simplified();
  selectionXMin->setText( text );
  selectionXMin->update();

  strm2 << " maxT: " << std::setw(9) << std::left << std::setprecision(8) << xMax << " ]";
  text = strm2.str().c_str();
  text = text.simplified();
  selectionXMax->setText( text );
  selectionXMax->update();

  strm3 << " deltaT: " << std::setw(9) << std::left << std::setprecision(8) << selectionDeltaXValue << " ]";
  text = strm3.str().c_str();
  text = text.simplified();
  selectionDeltaX->setText( text );
  selectionDeltaX->update();

  text = "Export Selection";
  exportSelection->setText( text );
  exportSelection->update();

}

void ViewerGraphArea::performAction ( bool flag ) {

  QPushButton *b = qobject_cast<QPushButton *>(sender());

  if ( b == lockTimeScale.data() ) {

    lockTimeScaleState = !lockTimeScaleState;

    if ( lockTimeScaleState ) {
      b->setDown( true );
      emit enableLockTimeScale( true );
    }
    else {
      b->setDown( false );
      emit enableLockTimeScale( false );
    }

  }
  else if ( b == useRoiIndicators.data() ) {

    useRoiIndicatorsState = !useRoiIndicatorsState;

    if ( useRoiIndicatorsState ) {
      b->setDown( true );
      //emit enableRoiIndicators( true );
      roiSelect->setActive( true );
    }
    else {
      b->setDown( false );
      //emit enableRoiIndicators( false );
      roiSelect->setActive( false );
    }

  }
     
}

void ViewerGraphArea::roiSelectorChange( int id, int left, int right ) {

  QRectF r = graph->chart->plotArea();
  double x = r.x();
  double y = r.y();
  double w = r.size().width();
  if ( w <= 0 ) w = 1;
  double h = r.size().height();

  double chartXMin, chartXMax, chartYMin, chartYMax;
  
  //graph->getAxesLimits( chartXMin, chartYMin,
  //                      chartXMax, chartYMax );
  
  chartXMin = graph->lastXMin;
  chartYMin = graph->lastYMin;
  chartXMax = graph->lastXMax;
  chartYMax = graph->lastYMax;

  double dleft = ( left - x ) * ( chartXMax - chartXMin ) / w + chartXMin;

  double dright = ( right - x ) * ( chartXMax - chartXMin ) / w + chartXMin;

  updateSelectionRange( dleft, dright );
  
}

void ViewerGraphArea::doLeftInc( int dir ) {

  roiSelect->incLeft( dir );

}

void ViewerGraphArea::doLeftBigInc( int dir ) {

  roiSelect->incLeftBig( dir );

}

void ViewerGraphArea::doRightInc( int dir ) {

  roiSelect->incRight( dir );

}

void ViewerGraphArea::doRightBigInc( int dir ) {

  roiSelect->incRightBig( dir );

}

void ViewerGraphArea::doScaleToSelection ( void ) {

  //std::cout << "ViewerGraphArea::doScaleToSelection" << std::endl;

  if ( !( roiSelect->isActive() ) ) return;

  QRectF r = graph->chart->plotArea();
  double x = r.x();
  double y = r.y();
  double w = r.size().width();
  if ( w <= 0 ) w = 1;
  double h = r.size().height();

  int left = roiSelect->getLeft();
  int right = roiSelect->getRight();
  
  //std::cout << "x = " << x << ", w = " << w << std::endl;  
  
  double chartXMin, chartXMax, chartYMin, chartYMax;

  try {
    graph->getAxesLimits( chartXMin, chartYMin,
                          chartXMax, chartYMax );
  }
  catch ( const std::exception &e ) {
    std::cout << "ViewerGraphArea::doScaleToSelection - e = " << e.what() << std::endl;
    return;
  }

  //std::cout << "left = " << left << ", right = " << right << std::endl;  
  //std::cout << "chartXMin = " << chartXMin << ", chartXMax = " << chartXMax << std::endl; 

  double dleft = ( left - x ) * ( chartXMax - chartXMin ) / w + chartXMin;

  double dright = ( right - x ) * ( chartXMax - chartXMin ) / w + chartXMin;

  //std::cout << "dleft = " << dleft << ", dright = " << dright << std::endl;  

  emit vgaRoiSelectScale( this, graph->curSigIndex, graph->curFileName, dleft, dright );

}
