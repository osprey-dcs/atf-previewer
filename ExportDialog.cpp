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
  exportFormatLabel = new QLabel( "UFF58b" );

  hl1->addWidget( exportFormatMenuButton );
  hl2->addWidget( exportFormatLabel );
  hl2->addStretch( 7 );

  // ----------------------------------------------

  hl1 = new QHBoxLayout();
  hl2 = new QHBoxLayout();

  gl->addLayout( hl1, 6, 1, Qt::AlignLeft );
  gl->addLayout( hl2, 6, 2, Qt::AlignLeft );

  exportRangeMenuButton = new QPushButton( "Time Range" );
  exportRangeMenu = new QMenu( exportRangeMenuButton );
  fromSelectionAction = new QAction( "&From Start/End");
  exportRangeMenu->addAction( fromSelectionAction );
  allAction = new QAction( "&All Time");
  exportRangeMenu->addAction( allAction );
  exportRangeMenuButton->setMenu( exportRangeMenu );
  exportRangeLabel = new QLabel( "All Time" );

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

  fileSelect = new QFileDialog( nullptr, "Select File", "", "" );
  fileSelect->setAcceptMode( QFileDialog::AcceptSave );
  fileSelect->setLabelText(QFileDialog::Accept, "Export" );
  fileSelect->setFileMode( QFileDialog::AnyFile );
  fileSelect->setDefaultSuffix( "uff58b" );
  fileSelect->setNameFilters( { "Export files ( *.csv *.uff58b )",
                                "All files ( *.* )" } );

  // init fields from user prefs

  // if item doesn't exist, give it a default value

  if ( up->getString( "Description", description ) ) description = "";
  descLineEdit->setText( description );

  if ( up->getString( "StartTime", startTime ) ) startTime = "";
  startTimeLineEdit->setText( startTime );
  startTimeValInSec = startTime.toDouble();

  if ( up->getString( "EndTime", endTime ) ) endTime = "";
  endTimeLineEdit->setText( endTime );
  endTimeValInSec = endTime.toDouble();

  if ( up->getString( "ExportFormat", exportFormat ) ) exportFormat = "UFF58b";
  exportFormatLabel->setText( exportFormat );

  if ( up->getString( "ExportRange", exportRange ) ) exportRange = "All Time";
  exportRangeLabel->setText( exportRange );
  
  if ( up->getString( "chanSelect", chanSelect ) ) chanSelect = "";
  chanSelectTextEdit->setPlainText( chanSelect );

  // connect to signals  

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

  //connect( ok, SIGNAL( clicked( bool ) ),
  //         this, SLOT( okButtonClicked( bool ) ) );

  connect( cancel, SIGNAL( clicked( bool ) ),
           this, SLOT( cancelButtonClicked( bool ) ) );

  connect( ok, SIGNAL( clicked( bool ) ),
           this, SLOT( openFileSelect( bool ) ) );
  
  connect( fileSelect, SIGNAL( fileSelected( const QString& ) ),
           this, SLOT( exportFileSelected( const QString& ) ) );

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

    fileSelect->setDefaultSuffix( "csv" );
    fileSelect->update();
    
  }
  else if ( action == uff58bAction ) {
    
    exportFormatLabel->setText( "UFF58b" );
    up->setString( "ExportFormat", "UFF58b" );
    up->update();

    fileSelect->setDefaultSuffix( "uff58b" );
    fileSelect->update();

  }
  else if ( action == fromSelectionAction ) {
    
    exportRangeLabel->setText( "From Start/End" );
    up->setString( "ExportRange", "From Start/End" );
    up->update();

    exportRange = "From Start/End";
    
  }

  else if ( action == allAction ) {
    
    exportRangeLabel->setText( "All Time" );
    up->setString( "ExportRange", "All Time" );
    up->update();

    exportRange = "All Time";
    
  }

}
 
void ExportDialog::cancelButtonClicked( bool ) {

  up->update();
  
  close();

}
 
void ExportDialog::textUpdated( const QString &s ) {

  if ( sender() == descLineEdit ) {
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
 
void ExportDialog::openFileSelect( bool ) {

  fileSelect->move( 100, 50 );
  fileSelect->show();

}

void ExportDialog::exportFileSelected( const QString& file ) {

  exportFileName = file;

  if ( exportFormatLabel->text() == "CSV" ) {
    emit csvExport();
  }
  else if ( exportFormatLabel->text() == "UFF58b" ) {
    emit uff58bExport();
  }
  
  up->update();
  
  close();

}

