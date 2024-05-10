//
// Created by jws3 on 5/9/24.
//

#ifndef VIEWER_EXPORTDIALOG_H
#define VIEWER_EXPORTDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QAction>
#include <QMenu>
#include <QTextEdit>

class ExportDialog : QDialog {

public:
  ExportDialog(QWidget *parent, const Qt::WindowFlags &f);
  virtual ~ExportDialog() override;
  void open ( void );
  void close ( void );

  QWidget *refWidget;
  QGridLayout *gl;

  QLabel *fileNameLabel;
  QLineEdit *fileNameLineEdit;
  QString exportFileName;
  QLabel *descLabel;
  QLineEdit *descLineEdit;
  QString description;
  QPushButton *menuButton;
  QLabel *startTimeLabel;
  QLineEdit *startTimeLineEdit;
  QString startTime;
  double startTimeInSecVal;
  QLabel *endTimeLabel;
  QLineEdit *endTimeLineEdit;
  QString endTime;
  double endTimeInSecVal;
  QLabel *exportFormatLabel;
  QMenu *exportFormatMenu;
  QAction *csfAction;
  QAction *uff58bAction;
  QLabel *chanSelectLabel;
  QTextEdit *chanSelectTextEdit;
  QString chanSelect;
  QPushButton *ok;
  QPushButton *cancel;

};


#endif //VIEWER_EXPORTDIALOG_H
