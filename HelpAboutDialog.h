//
// Created by jws3 on 4/30/24.
//

#ifndef VIEWER_HELPABOUTDIALOG_H
#define VIEWER_HELPABOUTDIALOG_H

#include <iostream>

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class HelpAboutDialog : public QDialog {

public:
  HelpAboutDialog(QWidget *w, QWidget *parent=nullptr );
  void open ( void );
  void close ( void );
  QWidget *refWidget;
  QVBoxLayout *vlayout;
  QLabel *description;
  QLabel *version;
  QPushButton *ok;

};

#endif //VIEWER_HELPABOUTDIALOG_H
