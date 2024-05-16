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
// Created by jws3 on 4/2/24.
//

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

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

#include "ViewerMainWin.h"

static QColor qcolorBlack( "black" );
static QColor qcolorWhite( "white" );

ViewerMainWin::ViewerMainWin ( QApplication *_app, double _x, double _y, double _w, double _h, QWidget *parent ) {

  if ( Cnst::NumRows != 2 ) {
    std::cout << "Number of rows must be 2 - abort" << std::endl;
    exit( -1 );
  }

  //this->installEventFilter( this );
  this->exitFlag = false;
  this->x = _x;
  this->y = _y;
  this->w = _w;
  this->h = _h;
  this->resize( _w, _h );
  this->move( _x, _y );
  this->app = _app;

  fileSelect = new QFileDialog( nullptr, "Open File", QString(Cnst::HdrRoot.c_str()),
                                QString(Cnst::Filter.c_str()) );

  // config layouts and child widgets

  lbStartDate = new QLabel( "Start Date:" );
  txStartDate = new QLabel( "" );
  lbEndDate = new QLabel( "End Date:" );
  txEndDate = new QLabel( "" );
  lbId = new QLabel( "Acquisition Id:" );
  txId = new QLabel( "" );
  lbRole1 = new QLabel( "Role1 Name:" );
  txRole1 = new QLabel( "" );
  lbSampRate = new QLabel( "Sample Rate (Hz):" );
  txSampRate = new QLabel( "" );
  working = new QPushButton( "Ready" );
  working->setFlat( true );
  QPalette pal = working->palette();
  pal.setColor(QPalette::Button, QColor(Qt::green));
  working->setAutoFillBackground(true);
  working->setPalette(pal);
  working->update();
  what = new QPushButton( "Idle" );
  what->setFlat( true );
  lbNumPoints = new QLabel( "0 pts   " );
  lbNumFftPoints = new QLabel( "0 FFT pts   " );

  masterLayout = new QVBoxLayout();
  graphLayout = new QVBoxLayout();
  formLayout = new QVBoxLayout();
  formRow1 = new QHBoxLayout();
  formRow2 = new QHBoxLayout();
  vlayout = new QVBoxLayout();
  for ( int i=0; i<Cnst::NumRows; i++ ) { // 0, 3
    hlayout[i] = new QHBoxLayout();
  }

  masterLayout->addLayout( formLayout );
  masterLayout->addLayout( graphLayout );
  formLayout->addLayout( formRow1 );
  formLayout->addLayout( formRow2 );

  formRow1->addWidget( lbId, 0 );
  formRow1->addWidget( txId, 0 );
  formRow1->addSpacing(ViewerMainWin::FieldSpacing);

  formRow1->addWidget( lbStartDate, 0 );
  formRow1->addWidget( txStartDate, 0 );
  formRow1->addSpacing(ViewerMainWin::FieldSpacing);

  formRow1->addWidget( lbEndDate, 0 );
  formRow1->addWidget( txEndDate, 0 );
  formRow1->addSpacing(ViewerMainWin::FieldSpacing);

  formRow1->addWidget( lbSampRate, 0 );
  formRow1->addWidget( txSampRate, 0 );

  formRow1->addStretch( 10 );

  formRow1->addWidget( what, 0 );
  formRow1->addWidget( lbNumPoints, 0 );
  formRow1->addWidget( lbNumFftPoints, 0 );
  formRow1->addWidget( working, 0 );

  formRow2->addWidget( lbRole1, 0 );
  formRow2->addWidget( txRole1, 0 );

  formRow2->addStretch( 10 );

  graphLayout->addLayout( vlayout );
  for ( int i=0; i<Cnst::NumRows; i++ ) { // 0, 3
    //vlayout->addSpacing( LayoutSpacing );
    //vlayout->addLayout( hlayout[i], LayoutSpacing );
    vlayout->addLayout( hlayout[i] );
  }
  //vlayout->addSpacing( LayoutSpacing );

  // central widget
  centralWidget = new QWidget();

  int iGrf = 0;
  int iLay = 0;
  for ( int ir=0; ir<Cnst::NumRows; ir++ ) {
    //hlayout[iLay]->addSpacing(LayoutSpacing);
    // for ( int i=0; i<3; i++ ) {
    for (int i = iGrf; i < (iGrf + Cnst::NumCols); i++) {
      if ( ir == 0 ) {
        vga[i] = new ViewerGraphArea( i, centralWidget );
      }
      else if ( ir == 1 ) {
        vga[i] = new ViewerGraphAreaFft( i, centralWidget );
      }
      vgaList.push_back(vga[i]);
      hlayout[iLay]->addWidget(vga[i]);
      //hlayout[iLay]->addSpacing(LayoutSpacing);
    }
    iGrf += Cnst::NumCols;
    iLay++;
  }

  centralWidget->setLayout( masterLayout );

  this->setCentralWidget( centralWidget );

  auto fileMenu = menuBar()->addMenu(tr("&File"));
  openAction = new QAction( "Open" );
  openAction->setData( "open" );
  fileMenu->addAction( openAction );

  exportAction = new QAction( "Export" );
  exportAction->setData( "export" );
  fileMenu->addAction( exportAction );

  exportDialog = new ExportDialog( this, Qt::Window );

  exitAction = new QAction( "Exit" );
  exitAction->setData( "exit" );
  fileMenu->addAction( exitAction );

  auto helpMenu = menuBar()->addMenu(tr("&Help"));

  shortcutAction = new QAction( "Button/Wheel Shortcuts" );
  shortcutAction->setData( "help-shortcuts" );
  helpMenu->addAction( shortcutAction );

  helpAction = new QAction( "About" );
  helpAction->setData( "help-about" );
  helpMenu->addAction( helpAction );
  
  helpDialog = new HelpAboutDialog(this );

  shortcutDialog = new ShortcutDialog( this );

  this->setWindowTitle( "Vibration Data Viewer" );

  this->connect( this->menuBar(), SIGNAL( triggered( QAction *) ), this, SLOT( performAction( QAction *) ) );

  this->connect( this->helpDialog->ok, SIGNAL( clicked( bool ) ),
                 this, SLOT( helpButtonClicked( bool ) ) );
  
  this->connect( this->shortcutDialog->ok, SIGNAL( clicked( bool ) ),
                 this, SLOT( shortcutButtonClicked( bool ) ) );
  
}

ViewerMainWin::~ViewerMainWin() {

  // no memory management
  
}

std::list<ViewerGraphAreaBase *> ViewerMainWin::getVgaList() {
  return vgaList;
}

void ViewerMainWin::config ( void ) {

}

void ViewerMainWin::setID ( QString& text ) {

  txId->setText( text );

}

void ViewerMainWin::setStartDate ( QString& text ) {

  txStartDate->setText( text );

}

void ViewerMainWin::setEndDate ( QString& text ) {

  txEndDate->setText( text );

}

void ViewerMainWin::setRole1 ( QString& text ) {

    txRole1->setText( text );

}

void ViewerMainWin::setSampleRate ( double val ) {

    QString text;
    text.setNum( val );
    //std::cout << "val = " <<  val << std::endl;
    //std::cout << "text = " << text.toStdString() << std::endl;
    txSampRate->setText( text );

}

void ViewerMainWin::setWorking( QString text ) {

  QPalette pal = working->palette();
  if ( text == "Working..." ) {
    pal.setColor(QPalette::Button, QColor(Qt::darkMagenta));
    pal.setColor(QPalette::ButtonText, QColor(Qt::white));
  }
  else {
    pal.setColor(QPalette::Button, QColor(Qt::green));
    pal.setColor(QPalette::ButtonText, QColor(Qt::black));
  }
  working->setPalette(pal);
  working->setText( text );
  working->update();
  app->processEvents( QEventLoop::AllEvents, 100 );

}

void ViewerMainWin::setWhat( QString text ) {

  what->setText( text );
  what->update();
  app->processEvents( QEventLoop::AllEvents, 100 );

}

void ViewerMainWin::setNumPoints ( unsigned long val ) {

    std::stringstream strm;
    strm << std::left << val << std::left << " pts    ";
    QString text( strm.str().c_str() );
    lbNumPoints->setText( text );
    lbNumPoints->update();
    app->processEvents( QEventLoop::AllEvents, 100 );

}

void ViewerMainWin::setNumFftPoints ( unsigned long val ) {

    std::stringstream strm;
    strm << std::left << val << std::left << " FFT pts    ";
    QString text( strm.str().c_str() );
    lbNumFftPoints->setText( text );
    lbNumFftPoints->update();
    app->processEvents( QEventLoop::AllEvents, 100 );

}

void ViewerMainWin::performAction ( QAction *action ) {

  if ( action->data().toString() == "exit" ) {
    exitFlag = true;
    return;
  }
  else if ( action->data().toString() == "open" ) {
    fileSelect->move( 100, 50 );
    fileSelect->show();
    return;
  }
  else if ( action->data().toString() == "export" ) {
    exportDialog->open();
  }
  else if ( action->data().toString() == "help-about" ) {
    helpDialog->open();
  }
  else if ( action->data().toString() == "help-shortcuts" ) {
    shortcutDialog->open();
  }

  //std::cout << "ViewerMainWin::performAction - text = " << action->text().toStdString() << std::endl;
  //std::cout << "ViewerMainWin::performAction - data = " << action->data().toString().toStdString() << std::endl;

}

void ViewerMainWin::helpButtonClicked( bool flag ) {

  helpDialog->close();

}

void ViewerMainWin::shortcutButtonClicked( bool flag ) {

  shortcutDialog->close();

}

bool ViewerMainWin::eventFilter ( QObject *obj, QEvent *event)  {

  //std::cout << "ev type = " << event->type() << std::endl;
  //std::cout << "obj = " << obj << std::endl;

  if ( event->type() == QEvent::Move ) {
    QMoveEvent *ev = dynamic_cast<QMoveEvent *>( event );
    //std::cout << "ViewerMainWin - moveEvent - x = " << ev->pos().x() << ", y = " << ev->pos().y() << std::endl;
    return false; // let event propagate
  }
  else if ( event->type() == QEvent::Resize ) {
    QResizeEvent *ev1 = dynamic_cast<QResizeEvent *>( event );
    //std::cout << "ViewerMainWin - resizeEvent - w = " << ev1->size().width() << ", h = " << ev1->size().height() << std::endl;
    return false; // let event propagate
  }

  return false; // let event propagate

}

bool ViewerMainWin::event ( QEvent *event ) {

  //std::cout << "event " << event->type() << std::endl;

  bool changed = false;

  if ( event->type() == QEvent::Close ) {
    this->exitFlag = true;
  }
  else if ( event->type() == QEvent::Move ) {

    QMoveEvent *ev = dynamic_cast<QMoveEvent *>( event );
    //std::cout << "ViewerMainWin - moveEvent - x = " << ev->pos().x() << ", y = " << ev->pos().y() << std::endl;
    changed = true;
    x = ev->pos().x();
    y = ev->pos().y();

  }
  else if ( event->type() == QEvent::Resize ) {

    QResizeEvent *ev1 = dynamic_cast<QResizeEvent *>( event );
    //std::cout << "ViewerMainWin - resizeEvent - w = " << ev1->size().width() << ", h = " << ev1->size().height() << std::endl;
    changed = true;
    w = ev1->size().width();
    h = ev1->size().height();

  }

  if ( changed ) {
    emit dimensionChange( x, y, w, h );
  }

  return QWidget::event ( event );

}

void ViewerMainWin::paintEvent (QPaintEvent *event) {

  //std::cout << "ViewerMainWin::paintEvent" << std::endl;

  QPainter p(this);
  QPen pen( p.pen() );
  QBrush brush( p.brush() );
  QRegion xR( 0, 0, w, h );

  p.save();

  // Set clipping region
  p.setClipRegion( xR );
  p.setClipping( true );

  brush.setStyle( Qt::NoBrush );

  pen.setStyle( Qt::SolidLine );

  pen.setWidth( 1);

  pen.setColor( qcolorBlack );

  p.setPen( pen );
  p.setBrush( brush );

  p.drawRect( 0, 0, w-1, h-1 );

  // Remove clippling region
  p.setClipping( false );

  p.restore();

}
