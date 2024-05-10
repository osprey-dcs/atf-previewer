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

#ifndef VIEWER_VIEWERMAINWIN_H
#define VIEWER_VIEWERMAINWIN_H

#include <iostream>
#include <list>

#include <QApplication>
#include <QGuiApplication>
#include <QObject>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QFileDialog>

#include "Cnst.h"
#include "ViewerGraphArea.h"
#include "ViewerGraphAreaFft.h"
#include "HelpAboutDialog.h"
#include "ShortcutDialog.h"
#include "ExportDialog.h"

    class ViewerMainWin : public QMainWindow {

    Q_OBJECT

    public:

      static const int FieldSpacing = 25;
      static const int LayoutSpacing = 5;

      explicit ViewerMainWin ( QApplication *app, double x, double y, double w, double h, QWidget *parent = nullptr );
      ~ViewerMainWin();

      ViewerMainWin ( ViewerMainWin&& ) = delete;
      ViewerMainWin ( ViewerMainWin& ) = delete;
      ViewerMainWin ( const ViewerMainWin& ) = delete;

      ViewerMainWin& operator=( ViewerMainWin&& other ) = delete;
      ViewerMainWin& operator=( ViewerMainWin& other ) = delete;
      ViewerMainWin& operator=( const ViewerMainWin& other ) = delete;

      std::list<ViewerGraphAreaBase *>vgaList;
      std::list<ViewerGraphAreaBase *> getVgaList();

      void config ( void );
      void setID ( QString& text );
      void setStartDate ( QString& text );
      void setEndDate ( QString& text );
      void setRole1 ( QString& text );
      void setSampleRate ( double val );
      void setWorking ( QString text );
      void setWhat( QString text );
      void setNumPoints ( unsigned long val );
      void setNumFftPoints ( unsigned long val );

      static const int NumRows {Cnst::NumRows};
      static const int NumCols {Cnst::NumCols};
      static const int NumGraphs {Cnst::NumGraphs};

      QApplication *app;
      bool exitFlag;
      QLabel *lbId;
      QLabel *txId;
      QLabel *lbStartDate;
      QLabel *txStartDate;
      QLabel *lbEndDate;
      QLabel *txEndDate;
      QLabel *lbRole1;
      QLabel *txRole1;
      QLabel *lbSampRate;
      QLabel *txSampRate;
      QPushButton *working;
      QPushButton *what;
      QLabel *lbNumPoints;
      QLabel *lbNumFftPoints;
      QVBoxLayout *masterLayout;
      QVBoxLayout *graphLayout;
      QVBoxLayout *formLayout;
      QHBoxLayout *formRow1;
      QHBoxLayout *formRow2;
      QHBoxLayout *hlayout[NumRows];
      QVBoxLayout *vlayout;
      QWidget *centralWidget;
      ViewerGraphAreaBase *vga[NumGraphs];
      QFileDialog *fileSelect;
      double x, y, w, h;
      QAction *openAction;
      QAction *exportAction;
      QAction *exitAction;
      QAction *helpAction;
      QAction *shortcutAction;
      ExportDialog *exportDialog;
      HelpAboutDialog *helpDialog;
      ShortcutDialog *shortcutDialog;

    signals:
      void dimensionChange( double x, double y, double w, double h );

    public slots:
      void performAction( QAction *action );
      void helpButtonClicked( bool );
      void shortcutButtonClicked( bool );

      // event handlers
      virtual bool eventFilter(QObject *obj, QEvent *event) override;
      bool event( QEvent *event );
      void paintEvent(QPaintEvent *event);

    };

#endif //VIEWER_VIEWERMAINWIN_H
