//
// Created by jws3 on 4/30/24.
//

#ifndef VIEWER_SHORTCUTDIALOG_H
#define VIEWER_SHORTCUTDIALOG_H

#include <iostream>

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class ShortcutDialog : public QDialog {

public:
  ShortcutDialog( QWidget *w, QWidget *parent=nullptr );
  void open ( void );
  void close ( void );
  QWidget *refWidget;
  QVBoxLayout *vlayout;
  QHBoxLayout *hlayout1;
  QLabel *description;
  QLabel *version;
  QPushButton *ok;

};


#endif //VIEWER_SHORTCUTDIALOG_H
