//
// Created by jws3 on 5/9/24.
//

#include <QSharedPointer>

#include "ExportDialog.h"
#include "UserPrefsFac.h"

ExportDialog::ExportDialog ( QWidget *parent, const Qt::WindowFlags &f) : QDialog(parent, f ) {

  UserPrefsFac upfac;
  up = upfac.createUserPrefs();

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
  hl2->addWidget( startTimeLineEdit, 1 );

  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 4, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 4, 2, Qt::AlignLeft );

  endTimeLabel = new QLabel( "End Time (sec)" );
  hl1->addWidget( endTimeLabel, 0 );
  endTimeLineEdit = new QLineEdit( this );
  hl2->addWidget( endTimeLineEdit,  1 );

  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 5, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 5, 2, Qt::AlignLeft );

  exportFormatMenuButton = new QPushButton( "Out File Type" );
  exportFormatMenu = new QMenu( exportFormatMenuButton );
  uff58bAction = new QAction( "&UFF58b");
  exportFormatMenu->addAction( uff58bAction );
  csvAction = new QAction( "&CSV");
  exportFormatMenu->addAction( csvAction );
  exportFormatMenuButton->setMenu( exportFormatMenu );
  exportFormatLabel = new QLabel( "..." );

  hl1->addWidget( exportFormatMenuButton );
  hl2->addWidget( exportFormatLabel );
  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 6, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 6, 2, Qt::AlignLeft );

  exportRangeMenuButton = new QPushButton( "Export Range" );
  exportRangeMenu = new QMenu( exportRangeMenuButton );
  fromSelectionAction = new QAction( "&From Selection");
  exportRangeMenu->addAction( fromSelectionAction );
  allAction = new QAction( "&All Data");
  exportRangeMenu->addAction( allAction );
  exportRangeMenuButton->setMenu( exportRangeMenu );
  exportRangeLabel = new QLabel( "..." );

  hl1->addWidget( exportRangeMenuButton );
  hl2->addWidget( exportRangeLabel );
  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 7, 1, Qt::AlignTop );
  gl->addLayout( hl2, 7, 2, Qt::AlignLeft );

  chanSelectLabel = new QLabel( "Channels");
  chanSelectTextEdit = new QTextEdit( this );

  hl1->addWidget( chanSelectLabel, 0 );
  hl2->addWidget( chanSelectTextEdit, 9 );
  //hl2->addStretch( 9 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 8, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 8, 2, Qt::AlignLeft );

  ok = new QPushButton( "OK" );
  cancel = new QPushButton( "Cancel" );

  hl1->addWidget( ok, 0 );
  hl2->addWidget( cancel, 0 );
  hl2->addStretch( 9 );

  // init fields from user prefs
  
  // if item doesn't exist, give it a default value
  if ( up->getString( "ExportFileName", exportFileName ) ) exportFileName = "";
  fileNameLineEdit->setText( exportFileName );

  if ( up->getString( "Description", description ) ) description = "";
  descLineEdit->setText( description );

  if ( up->getString( "StartTime", startTime ) ) startTime = "";
  startTimeLineEdit->setText( startTime );
  startTimeValInSec = startTime.toDouble();

  if ( up->getString( "EndTime", endTime ) ) endTime = "";
  endTimeLineEdit->setText( endTime );
  endTimeValInSec = endTime.toDouble();

  if ( up->getString( "ExportFormat", exportFormat ) ) exportFormat = "";
  exportFormatLabel->setText( exportFormat );

  if ( up->getString( "ExportRange", exportRange ) ) exportRange = "";
  exportRangeLabel->setText( exportRange );
  
  if ( up->getString( "chanSelect", chanSelect ) ) chanSelect = "";
  chanSelectTextEdit->setPlainText( chanSelect );

  // connect to signals  

  connect( fileNameLineEdit, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( textUpdated( const QString & ) ) );

  connect( descLineEdit, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( textUpdated( const QString & ) ) );

  connect( startTimeLineEdit, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( textUpdated( const QString & ) ) );

  connect( endTimeLineEdit, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( textUpdated( const QString & ) ) );

  connect( csvAction, SIGNAL( triggered( bool ) ),
           this, SLOT( performAction( bool ) ) );

  connect( uff58bAction, SIGNAL( triggered( bool ) ),
           this, SLOT( performAction( bool ) ) );

  connect( fromSelectionAction, SIGNAL( triggered( bool ) ),
           this, SLOT( performAction( bool ) ) );

  connect( allAction, SIGNAL( triggered( bool ) ),
           this, SLOT( performAction( bool ) ) );

  connect( chanSelectTextEdit, SIGNAL( textChanged( void ) ),
           this, SLOT( textUpdated( void ) ) );

  connect( ok, SIGNAL( clicked( bool ) ),
           this, SLOT( okButtonClicked( bool ) ) );

  connect( cancel, SIGNAL( clicked( bool ) ),
           this, SLOT( cancelButtonClicked( bool ) ) );

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

void ExportDialog::performAction( bool checked ) {

  QAction *action = qobject_cast<QAction *>( sender() );
  
  if ( action == csvAction ) {
    
    exportFormatLabel->setText( "CSV" );
    up->setString( "ExportFormat", "CSV" );
    up->update();

    //std::cout << "fileName is " << fileNameLineEdit->text().toStdString() << std::endl;
    QString name = FileUtil::extractFileName( fileNameLineEdit->text() );
    QString dir = FileUtil::extractDir( fileNameLineEdit->text() );
    QString newFile = dir + name + "." + Cnst::csvExtension.c_str();
    fileNameLineEdit->setText( newFile );
    fileNameLineEdit->update();
    
  }
  else if ( action == uff58bAction ) {
    
    exportFormatLabel->setText( "UFF58b" );
    up->setString( "ExportFormat", "UFF58b" );
    up->update();

    //std::cout << "fileName is " << fileNameLineEdit->text().toStdString() << std::endl;
    QString name = FileUtil::extractFileName( fileNameLineEdit->text() );
    QString dir = FileUtil::extractDir( fileNameLineEdit->text() );
    QString newFile = dir + name + "." + Cnst::uff58aExtension.c_str();
    fileNameLineEdit->setText( newFile );
    fileNameLineEdit->update();
    
  }

  else if ( action == fromSelectionAction ) {
    
    exportRangeLabel->setText( "From Selection" );
    up->setString( "ExportRange", "From Selection" );
    up->update();

    exportRange = "From Selection";
    
  }

  else if ( action == allAction ) {
    
    exportRangeLabel->setText( "All Data" );
    up->setString( "ExportRange", "All Data" );
    up->update();

    exportRange = "All Data";
    
  }

}
 
void ExportDialog::okButtonClicked( bool ) {

  if ( exportFormatLabel->text() == "CSV" ) {
    emit csvExport();
  }
  else if ( exportFormatLabel->text() == "UFF58b" ) {
    emit uff58bExport();
  }
  close();

}
 
void ExportDialog::cancelButtonClicked( bool ) {

  close();

}
 
void ExportDialog::textUpdated( const QString &s ) {

  if ( sender() == fileNameLineEdit ) {
    exportFileName = s;
    up->setString( "ExportFileName", exportFileName );
    up->update();
  }
  else if ( sender() == descLineEdit ) {
    description = s;
    up->setString( "Description", description );
    up->update();
  }
  else if ( sender() == startTimeLineEdit ) {
    startTime = s;
    startTimeValInSec = startTime.toDouble();
    up->setString( "StartTime", startTime );
    up->update();
  }
  else if ( sender() == endTimeLineEdit ) {
    endTime = s;
    endTimeValInSec = endTime.toDouble();
    up->setString( "EndTime", endTime );
    up->update();
  }

}

void ExportDialog::textUpdated( void ) {

  if ( sender() == chanSelectTextEdit ) {
    QTextEdit *te = qobject_cast<QTextEdit *>( sender() );
    chanSelect = te->toPlainText();
    up->setString( "chanSelect", chanSelect );
    up->update();
  }

}
 
