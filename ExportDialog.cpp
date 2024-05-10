//
// Created by jws3 on 5/9/24.
//

#include <QSharedPointer>

#include "ExportDialog.h"

ExportDialog::ExportDialog ( QWidget *parent, const Qt::WindowFlags &f) : QDialog(parent, f ) {

  this->setMinimumWidth( 900 );
  this->refWidget = parent;
  this->setWindowTitle( "File Export" );

  gl = new QGridLayout( this );

  QHBoxLayout *hl1 = new QHBoxLayout();
  QHBoxLayout *hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 1, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 1, 2, Qt::AlignLeft );

  fileNameLabel = new QLabel( "Out File Name");
  hl1->addWidget( fileNameLabel, 0 );

  fileNameLineEdit = new QLineEdit( this );
  fileNameLineEdit->setMaxLength( 78 );
  hl2->addWidget( fileNameLineEdit, 9 );

  hl2->addStretch( 12 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 2, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 2, 2, Qt::AlignLeft );

  descLabel = new QLabel( "Description");
  hl1->addWidget( descLabel, 0 );

  descLineEdit = new QLineEdit( this );
  descLineEdit->setMaxLength( 78 );
  hl2->addWidget( descLineEdit, 9 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 3, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 3, 2, Qt::AlignLeft );

  startTimeLabel = new QLabel( "Start Time (sec)" );
  hl1->addWidget( startTimeLabel, 0 );
  startTimeLineEdit = new QLineEdit( this );
  hl2->addWidget( startTimeLineEdit,  1 );

  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 4, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 4, 2, Qt::AlignLeft );

  startTimeLabel = new QLabel( "End Time (sec)" );
  hl1->addWidget( startTimeLabel, 0 );
  startTimeLineEdit = new QLineEdit( this );
  hl2->addWidget( startTimeLineEdit,  1 );

  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 5, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 5, 2, Qt::AlignLeft );

  menuButton = new QPushButton( "Out File Type" );
  exportFormatMenu = new QMenu( menuButton );
  uff58bAction = new QAction( "&UFF58b");
  exportFormatMenu->addAction( uff58bAction );
  csfAction = new QAction( "&CSF");
  exportFormatMenu->addAction( csfAction );
  menuButton->setMenu( exportFormatMenu );
  exportFormatLabel = new QLabel( "..." );

  hl1->addWidget( menuButton );
  hl2->addWidget( exportFormatLabel );
  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 6, 1, Qt::AlignTop );
  gl->addLayout( hl2, 6, 2, Qt::AlignLeft );

  chanSelectLabel = new QLabel( "Channels");
  chanSelectTextEdit = new QTextEdit( this );

  hl1->addWidget( chanSelectLabel, 0 );
  hl2->addWidget( chanSelectTextEdit, 9 );
  //hl2->addStretch( 9 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 7, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 7, 2, Qt::AlignLeft );

  ok = new QPushButton( "OK" );
  cancel = new QPushButton( "Cancel" );

  hl1->addWidget( ok, 0 );
  hl2->addWidget( cancel, 0 );
  hl2->addStretch( 9 );

}



ExportDialog::~ExportDialog() {

}

void ExportDialog::open ( void ) {

  int parentX = this->refWidget->pos().x();
  int parentY = this->refWidget->pos().y();

  this->move( parentX + 100, parentY + 100 );
  this->show();

}

void ExportDialog::close ( void ) {

  this->hide();

}
