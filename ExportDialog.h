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
#include <QString>
#include <QFileDialog>

#include "UserPrefsFac.h"
#include "Cnst.h"
#include "FileUtil.h"

class ExportDialog : public QDialog {

  Q_OBJECT

public:
  ExportDialog(QWidget *parent, const Qt::WindowFlags &f);
  virtual ~ExportDialog() override;
  void open ( void ) override;
  void close ( void );

  UserPrefs *up;

  QWidget *refWidget;
  QGridLayout *gl;

  QFileDialog *fileSelect;
  QString exportFileName;
  QLabel *descLabel;
  QLineEdit *descLineEdit;
  QString description;
  QLabel *startTimeLabel;
  QLineEdit *startTimeLineEdit;
  QString startTime;
  double startTimeValInSec;
  QLabel *endTimeLabel;
  QLineEdit *endTimeLineEdit;
  QString endTime;
  double endTimeValInSec;
  QPushButton *exportFormatMenuButton;
  QLabel *exportFormatLabel;
  QMenu *exportFormatMenu;
  QAction *csvAction;
  QAction *uff58bAction;
  QString exportFormat;
  QPushButton *exportRangeMenuButton;
  QLabel *exportRangeLabel;
  QMenu *exportRangeMenu;
  QAction *fromSelectionAction;
  QAction *allAction;
  QString exportRange;
  QLabel *chanSelectLabel;
  QTextEdit *chanSelectTextEdit;
  QString chanSelect;
  QPushButton *ok;
  QPushButton *cancel;

signals:
  void uff58bExport( void );
  void csvExport( void );

private slots:
  void performAction( bool );
  void cancelButtonClicked( bool );
  void textUpdated( const QString &s );
  void textUpdated( void );
  void openFileSelect( bool ) const;
  void exportFileSelected( const QString& file );
  
};


#endif //VIEWER_EXPORTDIALOG_H
