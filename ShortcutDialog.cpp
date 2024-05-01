//
// Created by jws3 on 4/30/24.
//

#include "ShortcutDialog.h"

ShortcutDialog::ShortcutDialog( QWidget *w, QWidget *parent ) {

  refWidget = w;
  vlayout = new QVBoxLayout();
  description = new QLabel( "Vibration Data Viewer" );
  ok = new QPushButton( "Dismiss" );

  std::vector<QString> leftStr
    {
      "Shift left/right click on graph area",
      "Shift left/right click on X/Y scale",
      "Left Click on border",
      "",
      "Shift wheel up/down on graph area",
      "Shift wheel up/down on X/Y scale",
      "Wheel up/down on horizontal border",
      "Wheel up/down on vertical border",
      "",
      "Ctrl Left Click on graph area",
      "",
      "",
      "Right click on graph area"
    };
  
  std::vector<QString> rightStr
    {
      "Zoom in/out",
      "Zoom in/out X/Y",
      "Pan in border direction",
      "",
      "Zoom in/out",
      "Zoom in/out X/Y",
      "Pan up/down",
      "Pan right/left",
      "",
      "Show mouse position",
      "(and (x,y) at max peak for FFT)",
      "",
      "Open menu"
    };

  for ( int i=0; i<leftStr.size(); i++ ) {

    QHBoxLayout *tmpLayout = new QHBoxLayout;
    vlayout->addLayout( tmpLayout );

    QLabel *tmpLabel = new QLabel( leftStr[i] );
    tmpLayout->addWidget( tmpLabel );
    tmpLayout->setAlignment( tmpLabel, Qt::AlignLeft );

    tmpLabel = new QLabel( "                 " );
    tmpLayout->addWidget( tmpLabel );
  
    tmpLabel = new QLabel( rightStr[i] );
    tmpLayout->addWidget( tmpLabel );
    tmpLayout->setAlignment( tmpLabel, Qt::AlignRight );

  }

  QHBoxLayout *tmpLayout = new QHBoxLayout;
  vlayout->addLayout( tmpLayout );

  QLabel *tmpLabel = new QLabel( " " );
  tmpLayout->addWidget( tmpLabel );

  tmpLayout = new QHBoxLayout;
  vlayout->addLayout( tmpLayout );

  tmpLabel = new QLabel( " " );
  tmpLayout->addWidget( tmpLabel );
  tmpLabel = new QLabel( "Note that buttons are disabled when graph is empty." );
  tmpLayout->addWidget( tmpLabel );
  tmpLabel = new QLabel( " " );
  tmpLayout->addWidget( tmpLabel );
  tmpLayout->setAlignment( tmpLabel, Qt::AlignHCenter );

  tmpLayout = new QHBoxLayout;
  vlayout->addLayout( tmpLayout );

  tmpLabel = new QLabel( " " );
  tmpLayout->addWidget( tmpLabel );

  tmpLayout = new QHBoxLayout;
  vlayout->addLayout( tmpLayout );

  tmpLabel = new QLabel( " " );
  tmpLayout->addWidget( tmpLabel );
  tmpLayout->addWidget( ok );
  tmpLabel = new QLabel( " " );
  tmpLayout->addWidget( tmpLabel );
  tmpLayout->setAlignment( tmpLabel, Qt::AlignHCenter );

  this->setLayout( vlayout );

  this->setWindowTitle( "Button/Wheel Shortcuts" );

}

void ShortcutDialog::open ( void ) {

  int parentX = refWidget->x();
  int parentY = refWidget->y();

  this->move( parentX + 100, parentY + 100 );
  this->show();

}

void ShortcutDialog::close ( void ) {

  this->hide();

}
