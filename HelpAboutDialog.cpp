//
// Created by jws3 on 4/30/24.
//

#include <sstream>
#include <iomanip>

#include "HelpAboutDialog.h"

// version.h declares static ints: g_major, g_minor, g_release
#include "version.h"

HelpAboutDialog::HelpAboutDialog(QWidget *w, QWidget *parent ) {

  std::stringstream strm;
  strm << "Version " << g_major << "." << g_minor << "." << g_release;

  refWidget = w;
  vlayout = new QVBoxLayout();
  //hlayout = new QHBoxLayout();
  description = new QLabel( "Vibration Data Viewer" );
  version = new QLabel( strm.str().c_str() );
  ok = new QPushButton( "Dismiss" );
  //vlayout->addLayout( hlayout );
  vlayout->addWidget( description );
  vlayout->addSpacing( 10 );
  vlayout->addWidget( version );
  vlayout->setAlignment( version, Qt::AlignHCenter );
  vlayout->addSpacing( 20 );
  vlayout->addWidget( ok );
  this->setLayout( vlayout );

  this->setWindowTitle( "Viewer Info" );

}

void HelpAboutDialog::open (void ) {

  int parentX = refWidget->x();
  int parentY = refWidget->y();

  this->move( parentX + 100, parentY + 100 );
  this->show();

}

void HelpAboutDialog::close (void ) {

  this->hide();

}

