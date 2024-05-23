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
// Created by jws3 on 4/3/24.
//

#include <unistd.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

#include "ViewerCtlr.h"
#include "FileUtil.h"
#include "ChanSelector.h"
#include "Uff58bExport.h"
#include "CsvExport.h"

static void setAll( int *ary, int n, int v ) {
  for ( int i=0; i<n; i++ ) {
    ary[i] = v;
  }
}

static bool isTimeSeries( ViewerGraphAreaBase *vga ) {
  if ( vga->id < Cnst::NumCols ) {
    return true;
  }
  else {
    return false;
  }

}

static bool isTimeSeries( int id ) {
  if ( id < Cnst::NumCols ) {
    return true;
  }
  else {
    return false;
  }

}

static bool isFFT( ViewerGraphAreaBase *vga ) {
  if ( vga->id >= Cnst::NumCols ) {
    return true;
  }
  else {
    return false;
  }

}

static bool isFFT( int id ) {
  if ( id >= Cnst::NumCols ) {
    return true;
  }
  else {
    return false;
  }

}

ViewerCtlr::ViewerCtlr( QSharedPointer<ViewerMainWin> mainw, const QString& file ) {

  this->up = upfac.createUserPrefs();

  this->mainWindow = mainw;
  this->haveFile = false;
  this->hdrFileName = "";
  this->fileName = "";
  this->haveHeader = false;
  this->readyForData = false;
  this->curMaxElements = 0;
  setAll( this->prevSigIndex, Cnst::NumGraphs, -1 );
  this->sampleRate = 0.0;
  lastDataRequestGraphArea = nullptr;
  fftVga = nullptr;

  QRectF r { 0, 0, 0, 0 };
  for ( int i=0; i<Cnst::NumGraphs; i++ ) {
    this->plotAreaDimensions[i] = r;
  }

  this->dh = this->dhf.createDataHeader();
  //this->dh = std::shared_ptr<DataHeader>( new DataHeader() );

  this->bd = this->bdf.createBinData( Cnst::BinDataName );
  this->bd->initMaxBufSize( Cnst::MaxMFileBufSize);
  //this->bd = std::shared_ptr<BinData>( new BinData() );

  this->fftIn = new fftw_complex[Cnst::MaxFftSize+1];
  this->fftOut = nullptr;
  this->numPts = 0ul;
  this->numFft = 0ul;
  this->maxFft = Cnst::MaxFftSize;
  this->haveDataForFft = false;

  this->haveCurTimeRange = false;
  this->curTimeMinimum = 0;
  this->curTimeMaximum = 0;

  this->lockTimeScale = false;

  roiX0 = roiX1 = 0.0;

  // Main window dimension change requires replotting all signals
  // (I might not need this)
  this->connect( mainw.data(), SIGNAL( dimensionChange( double, double, double, double ) ),
                 this, SLOT( dimension( double, double, double, double ) ) );

  this->connect( mainw->fileSelect, SIGNAL( fileSelected( const QString& ) ),
                 this, SLOT( fileSelected1( const QString& ) ) );

  this->connect( mainw->exportDialog, SIGNAL( csvExport( void ) ),
                 this, SLOT( doCsvExport( void ) ) );

  this->connect( mainw->exportDialog, SIGNAL( uff58bExport( void ) ),
                 this, SLOT( doUff58bExport( void ) ) );

  // Signal name change or plot area change requires replotting that signal
  // (I might not need this)
  for ( ViewerGraphAreaBase *w : mainw->getVgaList() ) {

    if ( isTimeSeries( w ) ) {
      
      ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>( w );
      this->connect( vga->sigName.data(), SIGNAL( activated1( int, int, QWidget * ) ),
                     this, SLOT( sigNameChange1( int, int, QWidget * ) ) );
      
      this->connect( vga->graph.data(), SIGNAL( fullScale( int, int, QString& ) ),
                     this, SLOT( haveFullScale( int, int, QString& ) ) );

      this->connect( vga, SIGNAL( enableLockTimeScale( bool ) ),
                     this, SLOT( lockTimeScaleEvent( bool ) ) );
      
    }

    this->connect( w->graph.data(), SIGNAL( horizontalPan( int, int, QString&, double, double, double, double ) ),
                   this, SLOT( haveHorizontalPan( int, int, QString&, double, double, double, double ) ) );

    this->connect( w->graph.data(), SIGNAL( scale( int, int, QString&, double, double, double, double ) ),
                   this, SLOT( haveScale( int, int, QString&, double, double, double, double ) ) );
    
    this->connect( w->graph.data(), SIGNAL( scale( int, int, QString&, double, double, double, double ) ),
                   this, SLOT( haveScale( int, int, QString&, double, double, double, double ) ) );
    
    this->connect( w->graph.data(), SIGNAL( reset( int, int, QString& ) ),
                   this, SLOT( haveReset( int, int, QString& ) ) );
    
    this->connect( w->graph.data(), SIGNAL( rubberBandScale( int, int, QString& ) ),
                   this, SLOT( haveRubberBandScale( int, int, QString& ) ) );

    this->connect( w->graph.data(), SIGNAL( prevView( int, int, QString& ) ),
                   this, SLOT( havePrevViewRequest( int, int, QString& ) ) );

    this->connect( w->graph.data(), SIGNAL( mousePos( int, double, double ) ),
                   this, SLOT( mousePosition( int, double, double ) ) );

    this->connect( w->graph.data(), SIGNAL( rubberBandRange( int, double, double ) ),
                   this, SLOT( selectionRange( int, double, double ) ) );

    if ( isTimeSeries( w ) ) {
      
      ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>( w );
      
      this->connect( vga->calcFft.data(), SIGNAL( pressed1( QWidget * ) ),
                     this, SLOT( haveDoCalcFft( QWidget * ) ) );
      
      this->connect( vga->slider.data(), SIGNAL( valueChanged1( QWidget *, int ) ),
                     this, SLOT( sliderValue( QWidget *, int ) ) );

      this->connect( vga, SIGNAL( vgaRoiSelectScale ( QWidget *, int, QString&, double, double ) ),
                     this, SLOT( haveVgaRoiSelectScale ( QWidget *, int, QString&, double, double ) ) );
      
    }

    // Plot area change is the result of window resize
    // (I might not need this)
    //this->connect( w->graph->chart, SIGNAL( plotAreaChanged( const QRectF & ) ),
    //               this, SLOT( sigAreaChanged( const QRectF & ) ) );

  }

  if ( !file.isEmpty() ) {
    fileToOpen = file;
  }

}

ViewerCtlr::~ViewerCtlr() {

  if ( fftIn ) {
    fftw_free( fftIn );
    fftIn = nullptr;
  }

}

void ViewerCtlr::enableFftButton( ViewerGraphAreaBase *vga ) {

  if ( isTimeSeries(vga) ) {
    
    for ( ViewerGraphAreaBase *w : mainWindow->getVgaList() ) {
      if ( w == vga ) {
        w->enableFftButton( true );
      }
      else {
        w->enableFftButton( false );
      }
    }

  }

}

ViewerGraphAreaBase *ViewerCtlr::companionVga ( ViewerGraphAreaBase *primaryVga ) {

  if ( isFFT( primaryVga ) ) {
    return nullptr;
  }
  
  int companionId = primaryVga->id + Cnst::NumCols;
  for ( ViewerGraphAreaBase *vga : mainWindow->getVgaList() ) {
    if ( vga->id == companionId ) {
      return vga;
    }
  }

  return nullptr;

}

static int count = 0;
static int delayCount = 0;
static int secCount = 0;
static int count250ms = 0;
static double value = 0;
static bool plotAreasAllNonZero = false;
static bool doNothing = false;
static bool once = true;

int ViewerCtlr::processHeaderFile (void ) {

  int stat = this->dh->readContents( this->hdrFileName );
  if ( stat ) return stat;

  QString str;
  // get acquisition ID
  stat = this->dh->getString( "AcquisitionId", str );
  if ( !stat ) {
    this->mainWindow->setID( str );
  }
  else {
    std::cout << "Failed to get Acquisition ID" << std::endl;
  }
  
  // get start date
  stat = this->dh->getString( "AcquisitionStartDate", str );
  if ( !stat ) {
    this->mainWindow->setStartDate( str );
  }
  else {
    std::cout << "Failed to get start date" << std::endl;
  }
  
  // get end date
  stat = this->dh->getString( "AcquisitionEndDate", str );
  if ( !stat ) {
    this->mainWindow->setEndDate( str );
  }
  else {
    std::cout << "Failed to get end date" << std::endl;
  }
  
  // get role 1
  stat = this->dh->getString( "Role1Name", str );
  if ( !stat ) {
    this->mainWindow->setRole1( str );
  }
  else {
    std::cout << "Failed to get role 1 name" << std::endl;
  }
  
  // get get sample rate
  sampleRate = 0;
  stat = this->dh->getDouble( "SampleRate", sampleRate );
  if ( !stat ) {
    //std::cout << "sr = " << sampleRate << std::endl;
    if ( sampleRate == 0.0 ) sampleRate = 1.0;
    this->mainWindow->setSampleRate( sampleRate );
  }
  else {
    std::cout << "Failed to get sample rate" << std::endl;
  }
  
  // populate combo boxes for the first row graphs
  auto nameList = this->dh->getNameList();
  auto nameMap = this->dh->getNameMap();
  std::list<ViewerGraphAreaBase *> vgaList = this->mainWindow->getVgaList();
  for ( ViewerGraphAreaBase *w : vgaList ) {
    if ( isTimeSeries( w ) ) {
      ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>( w );
      vga->sigName->clear();
      vga->sigName->addItem( "", -1 );
      setAll( this->prevSigIndex, Cnst::NumGraphs, -1 );
      for ( QString name : nameList ) {
        int sigIndex = std::get<DataHeader::SIGINDEX>( nameMap[name] );
        QString namePlusNum = name + QStringLiteral( "  (%1)" ).arg( sigIndex );
        vga->sigName->addItem( namePlusNum, sigIndex );
      }
    }   
  }

  return 0;

}

void ViewerCtlr::process(void ) {

  int stat;
  double slope = 1.0, intercept = 0.0;
  QString binFile;
  
  // process all handled events here

  if ( doNothing ) return;

  if ( once ) {
    once = false;
    if ( !fileToOpen.isEmpty() ) {
      fileSelected1( fileToOpen );
    }
  }

  // wait until plot area dimension are all non-zero (or max 10 seconds)
  if ( !plotAreasAllNonZero ) {
    std::list<ViewerGraphAreaBase *> vgalist = this->mainWindow->getVgaList();
    int maxCount = Cnst::LoopsPerSec * 10;
    while ( !plotAreasAllNonZero && (delayCount < maxCount) ) {

      // check about once per sec
      secCount += 1;
      if ( secCount > Cnst::LoopsPerSec ) {
        secCount = 0;
        //std::cout << "check plot areas" << std::endl;
        for ( ViewerGraphAreaBase *vga: vgalist ) {
          if ( ( vga->id >= 0 ) && ( vga->id < Cnst::NumGraphs ) ) {
            this->plotAreaDimensions[ vga->id ] = vga->graph->chart->plotArea();
          }
        }
        plotAreasAllNonZero = this->arrayAllNonZero(plotAreaDimensions, Cnst::NumGraphs);
      }

      delayCount++;

      // do nothing more until we have all non-zero plot areas (or timeout)
      return;

    }

    if (!plotAreasAllNonZero) {
      std::cout << "ViewerCtl.cpp - internal error - plot areas are not configured" << std::endl;
      this->mainWindow->exitFlag = true;
      doNothing = true;
    }

    count250ms = 0;

  }

  count++;
  if ( count >= (Cnst::LoopsPerSec/4) ) {

    count = 0;
    count250ms++;
    value += 5.0;

    //std::cout << "event tick " << count250ms << std::endl;

    if ( this->dataRequestList.empty() ) {

      this->mainWindow->setWorking( QString("Ready") );

    }

    //while ( !(this->dataRequestList.empty()) ) {
    if ( !(this->dataRequestList.empty()) ) {

      this->mainWindow->setWorking( QString("Working...") );

      auto dataReq = this->dataRequestList.back();
      this->dataRequestList.clear();

      int request = std::get<ViewerCtlr::Req>( dataReq );
      ViewerGraphAreaBase *grArea = std::get<ViewerCtlr::Vga>( dataReq );
      int grAreaId;
      if ( grArea ) {
        grAreaId = grArea->id;
      }
      int sigIndex = std::get<ViewerCtlr::SigIndex>( dataReq );
      QString reqFileName = std::get<ViewerCtlr::FileName>( dataReq );

      //std::cout << "\nprocess - request = " << request << "\n\n";

      // for fft calc and display, input data is from the time series graph
      // and displayed on the graph positioned under the time series one ( companion vga ).

      if ( request == ViewerCtlr::HaveUff58bExportRequest ) {
        
        this->mainWindow->setWorking( QString("Working...") );
        this->mainWindow->setWhat( "UFF58b Export..." );
        stat = uff58bExport();
        if ( stat ) {
          exportFail.showMessage(QString("Export fails (%1)").arg(stat));
          std::cout << "Error " << stat << " from uff58bExport" << std::endl;
        }
        this->mainWindow->setWorking( QString("Ready") );
        this->mainWindow->setWhat( "Idle" );
        
      }
      else if ( request == ViewerCtlr::HaveCsvExportRequest ) {
        
        this->mainWindow->setWorking( QString("Working...") );
        this->mainWindow->setWhat( "CSV Export..." );
        stat = csvExport();
        if ( stat ) {
          exportFail.showMessage(QString("Export fails (%1)").arg(stat));
          std::cout << "Error " << stat << " from csvExport" << std::endl;
        }
        this->mainWindow->setWorking( QString("Ready") );
        this->mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HaveFftRequest ) && this->readyForData &&
          ( numFft > 0 ) && ( numFft < Cnst::MaxFftSize ) && ( this->haveDataForFft ) ) {

        //std::cout << "have fft request - numFft = " << numFft << std::endl;

        this->mainWindow->setWhat( "FFT..." );

        if ( lastDataRequestGraphArea && ( lastDataRequestGraphArea == grArea ) ) {
        
          ViewerGraphAreaBase *companVga = this->companionVga( grArea );
          if ( !companVga ) {
           std::cout << "companion vga id not found" << std::endl;
          }

          if ( companVga ) {

            QString sigInfoName, sigInfoEgu;
            double sigInfoSlope, sigInfoIntercept;
            int stat = this->dh->getSigInfoBySigIndex ( sigIndex, sigInfoName, sigInfoEgu, sigInfoSlope, sigInfoIntercept );
            std::stringstream sstitle;
            sstitle << sigInfoName.toStdString() << "  (" << sigInfoEgu.toStdString() << ")";
            std::string s = sstitle.str();
            companVga->graph->setYTitle( s );

            if ( fftOut ) {
              fftw_free( fftOut );
            }
            fftOut = new fftw_complex[numFft];
            fftw_plan p = fftw_plan_dft_1d( numFft, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE );
            fftw_execute( p );

            fftVga = companVga;
        
            // Viewer graph object, setSeries function, manages qls
            QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();

            // get num of pixels in x
            QSizeF size = companVga->graph->chart->size();
        
            double minx, maxx, miny, maxy;

            stat = this->bd->genFftLineSeriesFromBuffer( numFft, fftOut, sampleRate, size.width(),
                                                      *qls, minx, maxx, miny, maxy, true );
            if ( !stat ) {
              companVga->setInitialState();
              companVga->graph->setSeries( qls, sigIndex, reqFileName, minx, maxx, miny, maxy );
              //std::cout << "after - minx, maxx, miny, maxy = "
              //          << minx << ", " << maxx << ", " << miny << ", " << maxy << std::endl;
              mainWindow->setNumFftPoints( numFft );
            }
            else {
              mainWindow->setNumPoints( 0ul );
              std::cout << "genFftLineSeriesFromBuffer failure" << std::endl;
              delete qls; qls = nullptr;
            }
        
            fftw_destroy_plan(p);

          }
          
        }
        else {
          
          std::cout << "No FFT buffer for this graph" << std::endl;

        }

        mainWindow->setWhat( "Idle" );

      }
      else if ( ( ( request == ViewerCtlr::HaveDataRequest ) || ( request == ViewerCtlr::HaveFullScaleRequest ) )
                && isTimeSeries( grArea ) && this->readyForData ) {
          
        // stop locking scales between initial signal display
        if ( ( request == HaveFullScaleRequest ) || ( !lockTimeScale ) ) this->haveCurTimeRange = false;

        enableFftButton( grArea );
        lastDataRequestGraphArea = grArea;

        // get min/max time values
        double x0, x1, y0, y1;

        // get num of pixels in x
        QSizeF size = grArea->graph->chart->size();

        // save reqFileName before appending extension
        grArea->setCurInfo( reqFileName, sigIndex );

        binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

        double minTime=0, maxTime=1;
        x0 = Cnst::InitialMinTime;
        x1 = Cnst::InitialMaxTime;
        y0 = Cnst::InitialMinSig;
        y1 = Cnst::InitialMaxSig;

        if ( x1 == 0.0 ) { // use all data
          double minTime=0, maxTime=1;
          int st = bd->getDataFullTimeRange( binFile, sampleRate, minTime, maxTime );
          if ( !st ) {
            x0 = minTime;
            x1 = maxTime;
          }
          else {
            std::cout << "Error " << st << " from getDataFullTimeRange" << std::endl;
            x1 = x0 + 1.0;
          }
        }
        
        // Viewer graph object, setSeries function,  manages qls
        QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
        double miny, maxy;
        double dataTimeIncrementInSec = 1.0 / sampleRate;

        numFft = 0ul;
        numPts = 0ul;
        haveDataForFft = false;

        mainWindow->setWhat( "Reading file..." );

        stat = this->bd->getMaxElements( binFile, sigIndex, this->curMaxElements );
        if ( stat ) {
          this->curMaxElements = 0;
        }

        QString qsdiscard, egudiscard;
        stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
        if ( stat ) {
          std::cout << "getSigInfoBySigIndex failure" << std::endl;
        }

        //  file name, sig index, x scale width in pixels, start time in sec,
        //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)

        stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                        dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
        if ( !stat ) {

          // if we have performed some kind of time scale adjustment and time scales are locked...
          if ( this->haveCurTimeRange ) {
            nonSlotHaveScale( grArea, sigIndex, reqFileName, this->curTimeMinimum,
                              this->curTimeMaximum, miny, maxy );
          }

          grArea->curSigIndex = sigIndex;
          grArea->setInitialState();
          
          mainWindow->setNumPoints( numPts );

          // don't draw the graph if we are going to rescale...
          if ( !this->haveCurTimeRange ) {

            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, miny, maxy );

          }

          if ( numFft ) {
            haveDataForFft = true;
          }

          // clear associated fft graph
          ViewerGraphAreaBase *companVga = this->companionVga( grArea );
          if ( companVga ) {
            companVga->setInitialState();
            companVga->clear();
          }

        }
        else {
          
          mainWindow->setNumPoints( 0ul );
          delete qls; qls = nullptr;
          std::cout << "genLineSeries failure" << std::endl;

        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HaveResetRequest ) && this->readyForData ) {

        if ( isFFT( grArea ) ) {

          if ( fftVga ) {
            //std::cout << "fftVga id = " << fftVga->id << ", grArea id = " << grArea->id << std::endl;
          }

          if ( !fftVga || ( fftVga != grArea ) ) {
            mainWindow->setWhat( "Idle" );
            return;
          }

          //std::cout << "haveResetRequest for FFT" << std::endl;

          double x0, x1, y0, y1;
          x0 = Cnst::InitialMinFreq;
          x1 = sampleRate / 2;

          //std::cout << "x0, x1 = " << x0 << ", " << x1 << std::endl;

          QSizeF size = grArea->graph->chart->size();

          // Viewer graph object, setSeries function,  manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
          double minx, maxx, miny, maxy;

          if ( x1 > sampleRate / 2 ) x1 = sampleRate / 2;

          mainWindow->setWhat( "Reseting..." );

          stat = this->bd->genFftLineSeriesFromBufferByFreq
            ( numFft, fftOut, sampleRate, size.width(),
              x0, x1, *qls, minx, maxx, miny, maxy, true );
          if ( !stat ) {

            //std::cout << "after - minx, maxx, miny, maxy = "
            //          << minx << ", " << maxx << ", " << miny << ", " << maxy << std::endl;

            grArea->setInitialState();

            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, miny, maxy );

          }
          else {
          
            delete qls; qls = nullptr;
            std::cout << "genFftFillUnderLineSeriesFromBuffer failure" << std::endl;
          
          }

        }
        else {
          
          //std::cout << "haveResetRequest" << std::endl;

          enableFftButton( grArea );
          lastDataRequestGraphArea = grArea;
        
          // get min/max time values
          double x0, x1, y0, y1;
          //grArea->graph->getAxesLimits( x0, y0, x1, y1 );
          //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();
          //std::cout << "x pixels = " << size.width() << std::endl;

          // save reqFileName before appending extension
          grArea->setCurInfo( reqFileName, sigIndex );
        
          QString binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

          x0 = Cnst::InitialMinTime;
          x1 = Cnst::InitialMaxTime;
          y0 = Cnst::InitialMinSig;
          y1 = Cnst::InitialMaxSig;

          if ( x1 == 0.0 ) { // use all data
            double minTime=0, maxTime=1;
            int st = bd->getDataFullTimeRange( binFile, sampleRate, minTime, maxTime );
            if ( !st ) {
              x0 = minTime;
              x1 = maxTime;
              //std::cout << "use full data range " << x0 << " to " << x1 << std::endl;
            }
            else {
              x1 = x0 + 1.0;
            }
          }

          // Viewer graph object, setSeries function,  manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
          double miny, maxy;
          double dataTimeIncrementInSec = 1.0 / sampleRate;

          numFft = 0ul;
          numPts = 0ul;
          haveDataForFft = false;
       
          mainWindow->setWhat( "Reading file..." );
    
          QString qsdiscard, egudiscard;
          stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
          if ( stat ) {
            std::cout << "getSigInfoBySigIndex failure" << std::endl;
          }
        
          //  file name, sig index, x scale width in pixels, start time in sec,
          //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
          stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                          dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
          if ( !stat ) {

            this->haveCurTimeRange = true;
            this->curTimeMinimum = x0;
            this->curTimeMaximum = x1;
          
            grArea->curSigIndex = sigIndex;
            grArea->setInitialState();
          
            mainWindow->setNumPoints( numPts );

            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, miny, maxy );

            if ( numFft ) {
              haveDataForFft = true;
            }

            // clear associated fft graph
            ViewerGraphAreaBase *companVga = this->companionVga( grArea );
            if ( companVga ) {
              companVga->setInitialState();
              companVga->clear();
            }

          }
          else {
          
            mainWindow->setNumPoints( 0ul );
            delete qls; qls = nullptr;
            std::cout << "genLineSeries failure" << std::endl;
          
          }

        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HaveHorizontalPanRequest ) && this->readyForData ) {

        if ( isFFT( grArea ) ) {

          if ( fftVga ) {
            //std::cout << "fftVga id = " << fftVga->id << ", grArea id = " << grArea->id << std::endl;
          }

          if ( !fftVga || ( fftVga != grArea ) ) {
            mainWindow->setWhat( "Idle" );
            return;
          }

          //std::cout << "HaveHorizontalPanRequest for FFT" << std::endl;

          double x0, x1, y0, y1;
          grArea->graph->getAxesLimits( x0, y0, x1, y1 );
          //std::cout << "x0, y0, x1, y1 = " << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();

          double minx, maxx, miny, maxy;
          // Viewer graph object, setSeries function, manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();

          mainWindow->setWhat( "Panning..." );

          stat = this->bd->genFftLineSeriesFromBufferByFreq
            ( numFft, fftOut, sampleRate, size.width(),
              x0, x1, *qls, minx, maxx, miny, maxy, true );
          if ( !stat ) {

            //std::cout << "after - minx, maxx, miny, maxy = "
            //          << minx << ", " << maxx << ", " << miny << ", " << maxy << std::endl;

            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1 );

          }
          else {

            delete qls; qls = nullptr;
            std::cout << "genFftFillUnderLineSeriesFromBuffer failure" << std::endl;
          
          }

        }
        else {

          enableFftButton( grArea );
          lastDataRequestGraphArea = grArea;
        
          //std::cout << "haveHorizontalPanRequest" << std::endl;

          // get min/max time values
          double x0, x1, y0, y1;
          grArea->graph->getAxesLimits( x0, y0, x1, y1 );
          //std::cout << "x0, y0, x1, y1 = " << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();
          //std::cout << "x pixels = " << size.width() << std::endl;

          // save reqFileName before appending extension
          grArea->setCurInfo( reqFileName, sigIndex );
        
          QString binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );
        
          // Viewer graph object, setSeries function,  manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
          double miny, maxy;
          double dataTimeIncrementInSec = 1.0 / sampleRate;
          
          numFft = 0ul;
          numPts = 0ul;
          haveDataForFft = false;
        
          mainWindow->setWhat( "Reading file..." );
    
          QString qsdiscard, egudiscard;
          stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
          if ( stat ) {
            std::cout << "getSigInfoBySigIndex failure" << std::endl;
          }
          
          //  file name, sig index, x scale width in pixels, start time in sec,
          //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
          stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                        dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
          if ( !stat ) {
          
            this->haveCurTimeRange = true;
            this->curTimeMinimum = x0;
            this->curTimeMaximum = x1;
          
            mainWindow->setNumPoints( numPts );
          
            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1 );

            if ( numFft ) {
              haveDataForFft = true;
            }

            setSlider( grArea );

            // clear associated fft graph
            ViewerGraphAreaBase *companVga = this->companionVga( grArea );
            if ( companVga ) {
              companVga->setInitialState();
              companVga->clear();
            }

          }
          else {
          
            mainWindow->setNumPoints( 0ul );
            delete qls; qls = nullptr;
            std::cout << "genLineSeries failure" << std::endl;
          
          }

        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HaveScaleRequest ) && this->readyForData ) {

        if ( isFFT( grArea ) ) {

          if ( fftVga ) {
            //std::cout << "fftVga id = " << fftVga->id << ", grArea id = " << grArea->id << std::endl;
          }

          if ( !fftVga || ( fftVga != grArea ) ) {
            mainWindow->setWhat( "Idle" );
            return;
          }

          //std::cout << "HaveScaleRequest for FFT" << std::endl;

          double x0, x1, y0, y1;
          grArea->graph->getAxesLimits( x0, y0, x1, y1 );
          //std::cout << "x0, y0, x1, y1 = " << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();

          double minx, maxx, miny, maxy;
          // Viewer graph object, setSeries function, manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();

          mainWindow->setWhat( "Scaling..." );
          
          stat = this->bd->genFftLineSeriesFromBufferByFreq
            ( numFft, fftOut, sampleRate, size.width(),
              x0, x1, *qls, minx, maxx, miny, maxy, true );
          if ( !stat ) {

            //std::cout << "after - minx, maxx, miny, maxy = "
            //          << minx << ", " << maxx << ", " << miny << ", " << maxy << std::endl;

            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

          }
          else {

            delete qls; qls = nullptr;
            std::cout << "genFftLineSeriesFromBuffer failure" << std::endl;
          
          }
            
        }
        else {

          enableFftButton( grArea );
          lastDataRequestGraphArea = grArea;
        
          //std::cout << "haveScaleRequest" << std::endl;

          // get min/max time values
          double x0, x1, y0, y1;
          grArea->graph->getAxesLimits( x0, y0, x1, y1 );
        
          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();
          //std::cout << "x pixels = " << size.width() << std::endl;

          // save reqFileName before appending extension
          grArea->setCurInfo( reqFileName, sigIndex );
        
          binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );
        
          // Viewer graph object, setSeries function,  manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
          double miny, maxy;
          double dataTimeIncrementInSec = 1.0 / sampleRate;
          
          numFft = 0ul;
          numPts = 0ul;
          haveDataForFft = false;
        
          mainWindow->setWhat( "Reading file..." );
    
          QString qsdiscard, egudiscard;
          stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
          if ( stat ) {
            std::cout << "getSigInfoBySigIndex failure" << std::endl;
          }
          
          //  file name, sig index, x scale width in pixels, start time in sec,
          //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
          stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                            dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
          if ( !stat ) {
          
            this->haveCurTimeRange = true;
            this->curTimeMinimum = x0;
            this->curTimeMaximum = x1;
          
            mainWindow->setNumPoints( numPts );

            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

            if ( numFft ) {
              haveDataForFft = true;
            }

            setSlider( grArea );

            // clear associated fft graph
            ViewerGraphAreaBase *companVga = this->companionVga( grArea );
            if ( companVga ) {
              companVga->setInitialState();
              companVga->clear();
            }
            
          }
          else {
          
            mainWindow->setNumPoints( 0ul );
            delete qls; qls = nullptr;
            std::cout << "genLineSeries failure" << std::endl;
          
          }

        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HaveRubberBandScaleRequest ) && this->readyForData ) {

        //std::cout << "ViewerCtlr::HaveRubberBandScaleRequest" << std::endl;

        if ( isFFT( grArea ) ) {

          if ( !fftVga || ( fftVga != grArea ) ) {
            mainWindow->setWhat( "Idle" );
            return;
          }

          // rubber band does not cause rescale with signal emission hence the following kludge
          int maxMs = 100;
          for ( int i=0; i<25; i++ ) {
            this->mainWindow->app->processEvents( QEventLoop::AllEvents, maxMs );
            QThread::usleep( 10000 ); // 1/100 sec
          }

          double x0, x1, y0, y1;
          grArea->graph->getAxesLimits( x0, y0, x1, y1 );
          //std::cout << "111   x0, y0, x1, y1 = " << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;
          //grArea->graph->setAxesLimits( x0, y0, x1, y1 );
          //grArea->graph->getAxesLimits( x0, y0, x1, y1 );
          //std::cout << "112   x0, y0, x1, y1 = " << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();

          double minx, maxx, miny, maxy;
          // Viewer graph object, setSeries function, manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();

          mainWindow->setWhat( "Scaling..." );
          
          //stat = this->bd->genFftLineSeriesFromBufferByFreq
          //  ( numFft, fftOut, sampleRate, size.width(),
          //    x0, x1, *qls, minx, maxx, miny, maxy, true );
          stat = this->bd->genFftLineSeriesFromBufferByFreq
            ( numFft, fftOut, sampleRate, size.width(),
              x0, x1, *qls, minx, maxx, miny, maxy, true );
          if ( !stat ) {

            //std::cout << "after - minx, maxx, miny, maxy = "
            //          << minx << ", " << maxx << ", " << miny << ", " << maxy << std::endl;

            // Viewer graph object manages qls
          grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

            //grArea->graph->getAxesLimits( x0, y0, x1, y1 );
            //std::cout << "222   x0, y0, x1, y1 = " << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;

          }
          else {

            delete qls; qls = nullptr;
            std::cout << "genFftLineSeriesFromBuffer failure" << std::endl;
          
          }
            
        }
        else {

          enableFftButton( grArea );
          lastDataRequestGraphArea = grArea;
        
          // rubber band does not cause rescale with signal emission hence the following kludge
          int maxMs = 100;
          for ( int i=0; i<25; i++ ) {
            this->mainWindow->app->processEvents( QEventLoop::AllEvents, maxMs );
            QThread::usleep( 10000 ); // 1/100 sec
          }

          double  x0, x1, y0, y1;
          grArea->graph->getAxesLimits( x0, y0, x1, y1 );

          binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

          double minTime, maxTime;
          int st = bd->getDataFullTimeRange( binFile, sampleRate, minTime, maxTime );
          if ( !st ) {
            x0 = std::fmax( x0, minTime );
            x1 = std::fmin( x1, maxTime );
          }
          else {
            std::cout << "Error " << st << " from getDataFullTimeRange\n";
          }

          this->haveCurTimeRange = true;
          this->curTimeMinimum = x0;
          this->curTimeMaximum = x1;

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();

          // save reqFileName before appending extension
          grArea->setCurInfo( reqFileName, sigIndex );

          binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

          // Viewer graph object, setSeries function,  manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
          double miny, maxy;
          double dataTimeIncrementInSec = 1.0 / sampleRate;

          numFft = 0ul;
          numPts = 0ul;
          haveDataForFft = false;
        
          mainWindow->setWhat( "Reading file..." );
    
          QString qsdiscard, egudiscard;
          stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
          if ( stat ) {
            std::cout << "getSigInfoBySigIndex failure" << std::endl;
          }
          
          //  file name, sig index, x scale width in pixels, start time in sec,
          //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
          stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                          dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
          if ( !stat ) {
          
            mainWindow->setNumPoints( numPts );
          
            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

            if ( numFft ) {
              haveDataForFft = true;
            }

            setSlider( grArea );
            
            // clear associated fft graph
            ViewerGraphAreaBase *companVga = this->companionVga( grArea );
            if ( companVga ) {
              companVga->setInitialState();
              companVga->clear();
            }
            
          }
          else {
          
            mainWindow->setNumPoints( 0ul );
            delete qls; qls = nullptr;
            std::cout << "genLineSeries failure" << std::endl;
          
          }

        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HaveRoiSelectScaleRequest ) && this->readyForData ) {

        //std::cout << "ViewerCtlr::HaveRoiSelectScaleRequest" << std::endl;

        enableFftButton( grArea );
        lastDataRequestGraphArea = grArea;

        double  x0, x1, y0, y1;
        grArea->graph->getAxesLimits( x0, y0, x1, y1 );

        x0 = this->roiX0;
        x1 = this->roiX1;
        
        binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

        double minTime, maxTime;
        int st = bd->getDataFullTimeRange( binFile, sampleRate, minTime, maxTime );
        if ( !st ) {
          x0 = std::fmax( x0, minTime );
          x1 = std::fmin( x1, maxTime );
        }
        else {
          std::cout << "Error " << st << " from getDataFullTimeRange\n";
        }

        this->haveCurTimeRange = true;
        this->curTimeMinimum = x0;
        this->curTimeMaximum = x1;

        // get num of pixels in x
        QSizeF size = grArea->graph->chart->size();

        // save reqFileName before appending extension
        grArea->setCurInfo( reqFileName, sigIndex );

        binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

        // Viewer graph object, setSeries function,  manages qls
        QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
        double miny, maxy;
        double dataTimeIncrementInSec = 1.0 / sampleRate;

        numFft = 0ul;
        numPts = 0ul;
        haveDataForFft = false;
        
        mainWindow->setWhat( "Reading file..." );
    
        QString qsdiscard, egudiscard;
        stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
        if ( stat ) {
          std::cout << "getSigInfoBySigIndex failure" << std::endl;
        }
          
        //  file name, sig index, x scale width in pixels, start time in sec,
        //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
        stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                        dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
        if ( !stat ) {
          
          mainWindow->setNumPoints( numPts );
          
          // Viewer graph object manages qls
          grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

          if ( numFft ) {
            haveDataForFft = true;
          }

          setSlider( grArea );

          // clear associated fft graph
          ViewerGraphAreaBase *companVga = this->companionVga( grArea );
          if ( companVga ) {
            companVga->setInitialState();
            companVga->clear();
          }

        }
        else {
          
          mainWindow->setNumPoints( 0ul );
          delete qls; qls = nullptr;
          std::cout << "genLineSeries failure" << std::endl;
          
        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HaveNonSlotScaleRequest ) && this->readyForData ) {

        //std::cout << "ViewerCtlr::HaveNonSlotScaleRequest" << std::endl;

          enableFftButton( grArea );
          lastDataRequestGraphArea = grArea;
        
          // non slot scale does not cause rescale with signal emission hence the following kludge
          //int maxMs = 100;
          //for ( int i=0; i<25; i++ ) {
          //  this->mainWindow->app->processEvents( QEventLoop::AllEvents, maxMs );
          //  QThread::usleep( 10000 ); // 1/100 sec
          //}

          double  x0, x1, y0, y1;
          grArea->graph->getAxesLimits( x0, y0, x1, y1 );
          x0 = this->nonSlotHavScaleX0;
          x1 = this->nonSlotHavScaleX1;
          
          binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

          double minTime, maxTime;
          int st = bd->getDataFullTimeRange( binFile, sampleRate, minTime, maxTime );
          if ( !st ) {
            x0 = std::fmax( x0, minTime );
            x1 = std::fmin( x1, maxTime );
          }
          else {
            std::cout << "Error " << st << " from getDataFullTimeRange\n";
          }

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();

          // save reqFileName before appending extension
          grArea->setCurInfo( reqFileName, sigIndex );

          binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

          // Viewer graph object, setSeries function,  manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
          double miny, maxy;
          double dataTimeIncrementInSec = 1.0 / sampleRate;

          numFft = 0ul;
          numPts = 0ul;
          haveDataForFft = false;
        
          mainWindow->setWhat( "Reading file..." );
    
          QString qsdiscard, egudiscard;
          stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
          if ( stat ) {
            std::cout << "getSigInfoBySigIndex failure" << std::endl;
          }
          
          //  file name, sig index, x scale width in pixels, start time in sec,
          //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
          stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                          dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
          
          if ( !stat ) {
          
            mainWindow->setNumPoints( numPts );
          
            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, miny, maxy );

            if ( numFft ) {
              haveDataForFft = true;
            }

            setSlider( grArea );
            
            // clear associated fft graph
            ViewerGraphAreaBase *companVga = this->companionVga( grArea );
            if ( companVga ) {
              companVga->setInitialState();
              companVga->clear();
            }
            
          }
          else {
          
            mainWindow->setNumPoints( 0ul );
            delete qls; qls = nullptr;
            std::cout << "genLineSeries failure" << std::endl;
          
          }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtlr::HavePrevViewRequest ) && this->readyForData ) {

        if ( isFFT( grArea ) ) {

          if ( !fftVga || ( fftVga != grArea ) ) {
            mainWindow->setWhat( "Idle" );
            return;
          }

          //std::cout << "HaveScaleRequest for FFT" << std::endl;

          double x0, x1, y0, y1;
          stat = grArea->graph->views.popView( x0, y0, x1, y1 );
          if ( grArea->graph->views.empty() ) {
            grArea->graph->prevViewAction->setEnabled( false );
          }
          //std::cout << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;
          if ( stat ) {
            mainWindow->setWhat( "Idle" );
            return;
          }

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();

          double minx, maxx, miny, maxy;
          // Viewer graph object, setSeries function, manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();

          mainWindow->setWhat( "Getting prev view..." );
          
          stat = this->bd->genFftLineSeriesFromBufferByFreq
            ( numFft, fftOut, sampleRate, size.width(),
              x0, x1, *qls, minx, maxx, miny, maxy, true );
          if ( !stat ) {

            //std::cout << "after - minx, maxx, miny, maxy = "
            //          << minx << ", " << maxx << ", " << miny << ", " << maxy << std::endl;

            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

          }
          else {

            delete qls; qls = nullptr;
            std::cout << "genFftLineSeriesFromBuffer failure" << std::endl;
          
          }
            
        }
        else {

          enableFftButton( grArea );
          lastDataRequestGraphArea = grArea;
        
          //std::cout << "request == ViewerCtlr::HavePrevViewRequest" << std::endl;

          double  x0, x1, y0, y1;
          stat = grArea->graph->views.popView( x0, y0, x1, y1 );
          if ( grArea->graph->views.empty() ) {
            grArea->graph->prevViewAction->setEnabled( false );
          }
          //std::cout << x0 << ", " << y0 << ", " << x1 << ", " << y1 << std::endl;
          if ( stat ) {
            mainWindow->setWhat( "Idle" );
            return;
          }

          // get num of pixels in x
          QSizeF size = grArea->graph->chart->size();
          //std::cout << "x pixels = " << size.width() << std::endl;

          // save reqFileName before appending extension
          grArea->setCurInfo( reqFileName, sigIndex );

          binFile = FileUtil::makeBinFileName( dh.get(), reqFileName, sigIndex );

          // Viewer graph object, setSeries function,  manages qls
          QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
          double miny, maxy;
          double dataTimeIncrementInSec = 1.0 / sampleRate;

          numFft = 0ul;
          numPts = 0ul;
          haveDataForFft = false;
        
          mainWindow->setWhat( "Reading file..." );
    
          QString qsdiscard, egudiscard;
          stat = this->dh->getSigInfoBySigIndex( sigIndex, qsdiscard, egudiscard, slope, intercept );
          if ( stat ) {
            std::cout << "getSigInfoBySigIndex failure" << std::endl;
          }
          
          //  file name, sig index, x scale width in pixels, start time in sec,
          //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
          stat = this->bd->genLineSeries( binFile, sigIndex, slope, intercept, size.width(), x0, x1,
                                          dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
          if ( !stat ) {
          
            this->haveCurTimeRange = true;
            this->curTimeMinimum = x0;
            this->curTimeMaximum = x1;
          
            mainWindow->setNumPoints( numPts );
          
            // Viewer graph object manages qls
            grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

            if ( numFft ) {
              haveDataForFft = true;
            }

            setSlider( grArea );
            
            // clear associated fft graph
            ViewerGraphAreaBase *companVga = this->companionVga( grArea );
            if ( companVga ) {
              companVga->setInitialState();
              companVga->clear();
            }
            
          }
          else {
          
            mainWindow->setNumPoints( 0ul );
            delete qls; qls = nullptr;
            std::cout << "genLineSeries failure" << std::endl;
          
          }

        }

        mainWindow->setWhat( "Idle" );
        
      }
      
    }
    
    if ( this->haveFile ) {
      
      this->readyForData = false;
      
      stat = processHeaderFile();
      if ( !stat ) {
        
        this->haveDataForFft = false;
        this->haveHeader = true;
        this->dataRequestList.clear();
        setAll( this->prevSigIndex, Cnst::NumGraphs, -1 );
        this->readyForData = true;
        for ( ViewerGraphAreaBase *vga : mainWindow->getVgaList() ) {
          vga->setInitialState();
        }
        this->mainWindow->setNumPoints( 0 );
        this->mainWindow->setNumFftPoints( 0 );
        this->haveCurTimeRange = false;
        this->curTimeMinimum = 0;
        this->curTimeMaximum = 0;

      }
      else {

        this->haveHeader = false;
        this->readyForData = false;
        
      }
      
      this->haveFile = false;
      
    }

  }

}

bool ViewerCtlr::allZero(const QRectF& r ) {
  if ( ( r.right() - r.left()) != 0.0 ) return false;
  if ( ( r.bottom() - r.top()) != 0.0 ) return false;
  return true;
}

bool ViewerCtlr::allNonZero(const QRectF& r ) {
  if ( ( r.right() - r.left()) == 0.0 ) return false;
  if ( ( r.bottom() - r.top()) == 0.0 ) return false;
  return true;
}

bool ViewerCtlr::arrayAllNonZero(const QRectF r[], int n ) {

  for ( int i=0; i<n; i++ ) {
    //qDebug() << n << "   " << i << " : " << r[i];
    if ( ( r[i].right() - r[i].left()) == 0.0 ) return false;
    if ( ( r[i].bottom() - r[i].top()) == 0.0 ) return false;

  }

  return true;

}


void ViewerCtlr::dimension(double _x, double _y, double _w, double _h ) {

  BinData::TwoDIntPtr arry;

  up->setDouble( "x", _x );
  up->setDouble( "y", _y );
  up->setDouble( "w", _w );
  up->setDouble( "h", _h );

  up->update();

}

void ViewerCtlr::sigNameChange(int index ) {
  //std::cout << "ViewerCtrl::sigNameChange - index = " << index << std::endl;
}

void ViewerCtlr::sigNameChange1(int index, int sigIndex, QWidget *w ) {
  
  ViewerGraphAreaBase *vga = dynamic_cast<ViewerGraphAreaBase *>( w );

  if ( ( sigIndex >= 0 ) && ( sigIndex <= Cnst::MaxSigIndex ) &&
   ( sigIndex != prevSigIndex[vga->id] ) ) {

    vga->setInitialState();
    prevSigIndex[vga->id] = sigIndex;
    
    int request = ViewerCtlr::HaveDataRequest;

    QString binFile = FileUtil::makeBinFileName( dh.get(), this->fileName, sigIndex );
    int st = bd->newFile( binFile );
    if ( st ) {
      std::cout << "Error " << st << " from newFile" << std::endl;
    }
    
    dataRequestList.push_back( std::make_tuple( request, vga, sigIndex, this->fileName ) );
    
    auto nameList = this->dh->getNameList();
    auto nameMap = this->dh->getNameMap();
    int sigInfoSigIndex;
    QString sigInfoName, sigInfoEgu;
    double sigInfoSlope, sigInfoIntercept;

    int stat = this->dh->getSigInfoBySigIndex ( sigIndex, sigInfoName, sigInfoEgu, sigInfoSlope, sigInfoIntercept );

    std::stringstream sstitle;
    sstitle << sigInfoName.toStdString() << "  (" << sigInfoEgu.toStdString() << ")";
    std::string s = sstitle.str();
    vga->graph->setYTitle( s );

  }

}

void ViewerCtlr::sigAreaChanged(const QRectF& plotArea ) {
  //std::cout << "sigAreaChange" << std::endl;
  QtCharts::QChart *chart = qobject_cast<QtCharts::QChart *>(sender());
  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(chart->parent());
  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>(vg->parent());

  if ( ( vga->id < 0 ) || ( vga->id > Cnst::NumGraphs) ) {
    std::cout << "Graph id out of bounds";
  }
  else {
    plotAreaDimensions[vga->id] = plotArea;
  }

}

void ViewerCtlr::fileSelected1(const QString& file ) {

  //qDebug() << "file is " << file;

  int l, count;

  l = count = file.length();
  
  QString::const_reverse_iterator it = file.rbegin();
  while ( it != file.rend() ) {
    if ( *it == '.' ) {
      count--;
      break;
    }
    if ( *it != '.' ) count--;
    (it)++;
  }

  if ( count > 1 ) {
    
    this->fileName = this->hdrFileName = file;
    this->fileName.replace( count, l-count+1, "" );
    this->haveFile = true;
    for ( ViewerGraphAreaBase *w : this->mainWindow->getVgaList() ) {
      w->clear();
    }

  }
  
}

static void closeAll ( std::filebuf *fb, int n ) {
  for ( int i=0; i<n; i++ ) {
    fb[i].close();
  }
}


static void zero( int *val, int n ) {
  for ( int i=0; i<n; i++ ) val[i] = n;
}

int ViewerCtlr::csvExport ( void ) {

  //std::cout << "ViewerCtlr::doCsvExport" << std::endl;

  if ( !haveHeader ) {
    std::cout << "No header file is open\n";
    return -1;
  }

  int st, i, ii, numSignals;
  QString str;
  std::filebuf fbInput[Cnst::MaxSignals+1];
  std::ofstream fbExport;
  int signalIndices[Cnst::MaxSignals+1];

  QString simpleName = FileUtil::extractFileName( this->fileName ) + "." + Cnst::HdrExtension.c_str();
  
  // add extension to get header file name
  QString hdrFile = this->fileName + "." + Cnst::HdrExtension.c_str();
  //std::cout << "hdrFile = " << hdrFile.toStdString() << std::endl;

  // create csv object
  std::unique_ptr<CsvExport> csv{new CsvExport( hdrFile )};

  // get map of channel/signal properties by signal index
  DataHeader::DataHeaderIndexMapType indexMap = dh->getIndexMap();

  // get list of channels/signals to export
  std::unique_ptr<ChanSelector> chans{new ChanSelector()};
  chans->setText( mainWindow->exportDialog->chanSelect );
  std::list<int> sigNumList = chans->getList();

  // make sure we have at least one valid signal number in the list

  bool atLeastOneSignal = false;
  int numGoodSignalsFound = 0;
  
  for ( int sigIndex : sigNumList ) {

    QString sigNameFromMap = std::get<DataHeader::SIGNAME>( indexMap[sigIndex] );

    if ( !sigNameFromMap.isEmpty() ) {

      QString bfname = FileUtil::makeBinFileName( dh.get(), this->fileName, sigIndex );
      std::filebuf fb;
      auto result = fb.open( bfname.toStdString(), std::ios::in | std::ios::binary );
      if ( result ) {
        atLeastOneSignal = true;
        fb.close();
        numGoodSignalsFound++;
      }
      
    }

  }

  if ( !atLeastOneSignal ) {
    return -1;
  }

  std::cout << "found " << numGoodSignalsFound << " signals" << std::endl;

  // open export file
  QString exportFileName = mainWindow->exportDialog->exportFileName;
  fbExport.open( exportFileName.toStdString() );

  //std::cout << "export file " << exportFileName.toStdString() << " is open\n";

  QString binFile;
  
  numSignals = 0;
  bool once = true;
  unsigned long recRange;
  double minT=0, maxT=0;
  unsigned long minByte=0, maxByte=0;
  double timeInc = 1.0 / sampleRate;
    
  // write header and data to the csv file for each signal in list
  for ( int sigIndex : sigNumList ) {

    QString sigNameFromMap = std::get<DataHeader::SIGNAME>( indexMap[sigIndex] );

    //std::cout << "sigIndex = " << sigIndex << std::endl;

    // get binary file name from fileName and signal + extension
    binFile = FileUtil::makeBinFileName( dh.get(), fileName, sigIndex );

    //std::cout << "binFile = " << binFile.toStdString() << std::endl;
    
    signalIndices[numSignals] = sigIndex;

    if ( once ) {
      
      once = false;

      if ( mainWindow->exportDialog->exportRange == "From Start/End" ) {

        // get min and max time
        minT = mainWindow->exportDialog->startTimeValInSec;
        maxT = mainWindow->exportDialog->endTimeValInSec;

        // get min and max rec number corresponding to min and max time
        st = bd->getRecordRangeForTime( binFile, sampleRate, minT, maxT,
                                        minByte, maxByte );

        recRange = maxByte - minByte + 1;

      }
      else {

        unsigned long maxEle;
        bd->getMaxElements ( binFile, 0, maxEle );
        minByte = 0;
        maxByte = maxEle * sizeof(int) - 1;
        recRange = maxByte - minByte + 1;

      }

      minT = minByte / sizeof(int) / sampleRate;

      //std::cout << "minT = " << minT << std::endl;
      //std::cout << "maxT = " <<  maxT << std::endl;
      //std::cout << "minByte = " <<  minByte << std::endl;
      //std::cout << "maxByte = " <<  maxByte << std::endl;
      //std::cout << "recRange (ele) = " << recRange/sizeof(int) << std::endl;
      //std::cout << "numSignals = " << numSignals << std::endl;

    }
    
    //std::fbin = open input bin files and count numSignals
    auto result2 = fbInput[numSignals].open( binFile.toStdString(), std::ios::in | std::ios::binary );
    if ( !result2 ) {
      fbExport.close();
      closeAll( fbInput, numSignals-1 );
      std::cout << "Open file " << binFile.toStdString() << " failure"  << std::endl;
      return -1;
    }
    //std::cout << "input file " << binFile.toStdString() << " is open\n";

    numSignals++;
    
  }

  //std::cout << "numSignals = " << numSignals << std::endl;

  QString id = dh->getString( "AcquisitionId" );
  QString desc = mainWindow->exportDialog->description;
  QString startTime = dh->getString( "AcquisitionStartDate" );
  QString endTime = dh->getString( "AcquisitionEndDate" );
  QString inputCsvFileName = "InputCsvFileName";
  st = csv->writeHeader( fbExport, id, desc, startTime, endTime, inputCsvFileName, simpleName );
  if ( st ) {
    fbExport.close();
    closeAll( fbInput, numSignals );
    return -1;
  }

  //std::cout << "export header is written\n";

  int (*intBuf)[100];
  intBuf = new int[numSignals][100];
  double outBuf[100];
  int numFullOps = recRange / 100;
  int numRemaining = recRange % 100;
  int *nr, nw;
  nr = new int[numSignals];
  QString *names = new QString[numSignals];

  //std::cout << "get signal names" << std::endl;
  for ( int i=0; i<numSignals; i++ ) {
    int ii = signalIndices[i];
    names[i] = std::get<DataHeader::SIGNAME>( indexMap[ii] );
    //std::cout << "sig name " << ii << " = " << names[i].toStdString() << std::endl;
  }

  st = csv->writeSignalProperties( fbExport, signalIndices, numSignals );

  st = csv->writeSignalNames( fbExport, names, numSignals );

  // seek to start of binary data for input files
  for ( int i=0; i<numSignals; i++ ) {
    this->bd->inputSeekToStartOfData( fbInput[i], minByte );
  }

  //std::cout << "numFullOps = " << numFullOps << std::endl;
  //std::cout << "numRemaining = " << numRemaining << std::endl;

  zero( nr, numSignals );
  nw = 0;

  double *slope = new double[numSignals];
  double *intercept = new double[numSignals];
  for ( int i=0; i<numSignals; i++ ) {
    int ii = signalIndices[i];
    slope[i] = std::get<DataHeader::SLOPE>( indexMap[ii] );
    intercept[i] = std::get<DataHeader::INTERCEPT>( indexMap[ii] );
    //std::cout << i << ": slope = " << slope[i] << ", intercept = " <<  intercept[i] << std::endl;
  }

  unsigned long rec = 0;
  double time = minT;
  //std::cout << "numFullOps = " << numFullOps << ", numRemaining = " << numRemaining << std::endl;
  //read and write binary files in chunks
  for ( i=0; i<numFullOps; i++ ) {

    for ( int ii=0; ii<numSignals; ii++ ) {
      nr[ii] += this->bd->readTraceData( fbInput[ii], intBuf[ii], 100 );
    }

    for ( int iii=0; iii<100/sizeof(int); iii++ ) {
      for ( int ii=0; ii<numSignals; ii++ ) {
        outBuf[ii] = (double) intBuf[ii][iii] * slope[ii] + intercept[ii];
      }
      nw += csv->writeData( fbExport, rec, time, outBuf, numSignals );
      rec++;
      time += timeInc;
    }
    
  }

  // perform final op
  if ( numRemaining ) {

    for ( int ii=0; ii<numSignals; ii++ ) {
      nr[ii] += this->bd->readTraceData( fbInput[ii], intBuf[ii], numRemaining );
    }

    for ( int iii=0; iii<numRemaining/sizeof(int); iii++ ) {
      for ( int ii=0; ii<numSignals; ii++ ) {
        outBuf[ii] = (double) intBuf[ii][iii] * slope[ii] + intercept[ii];
      }
      //std::cout << "intBuf[ii][iii] = " << intBuf[ii][iii] << std::endl;
      //std::cout << "iii = " << iii << ", ii = " << ii << std::endl;
      nw += csv->writeData( fbExport, rec, time, outBuf, numSignals );
      rec++;
      time += timeInc;
    }

  }

  //std::cout << "nr = " << nr << ", nw = " << nw  << std::endl;
    
  //close input file
  closeAll( fbInput, numSignals );

  // close export file
  fbExport.close();
  
  std::cout << "CSV export complete." << std::endl;
  
  return 0;

}

int ViewerCtlr::uff58bExport ( void ) {
  qWarning()<<__func__<<"enter";
  if ( !haveHeader ) {
    std::cout << "No header file is open\n";
    return -1;
  }

  int st, i, ii;
  QString str;
  std::filebuf fbInput, fbExport;

  // add extension to get header file name
  QString hdrFile = this->fileName + "." + Cnst::HdrExtension.c_str();

  // create uff58b object
  std::unique_ptr<Uff58bExport> uff58b{new Uff58bExport( hdrFile )};

  // get map of channel/signal properties by signal index
  DataHeader::DataHeaderIndexMapType indexMap = dh->getIndexMap();

  // get list of channels/signals to export
  std::unique_ptr<ChanSelector> chans{new ChanSelector()};
  chans->setText( mainWindow->exportDialog->chanSelect );
  const auto& sigNumList = chans->getList();

  // make sure we have at least one valid signal number in the list

  bool atLeastOneSignal = false;
  int numGoodSignalsFound = 0;
  
  for ( int sigIndex : sigNumList ) {

    QString sigNameFromMap = std::get<DataHeader::SIGNAME>( indexMap[sigIndex] );

    if ( !sigNameFromMap.isEmpty() ) {

      QString bfname = FileUtil::makeBinFileName( dh.get(), this->fileName, sigIndex );
      std::filebuf fb;
      auto result = fb.open( bfname.toStdString(), std::ios::in | std::ios::binary );
      if ( result ) {
        atLeastOneSignal = true;
        fb.close();
        numGoodSignalsFound++;
      }
      
    }

  }

  if ( !atLeastOneSignal ) {
    return -1;
  }

  std::cout << "found " << numGoodSignalsFound << " signals" << std::endl;

  // open export bin file
  QString exportFileName = mainWindow->exportDialog->exportFileName;
  auto result = fbExport.open( exportFileName.toStdString(), std::ios::out | std::ios::binary );
  if ( !result ) {
    qWarning()<<__func__<<"Unable to open for writing"<<exportFileName;
    return -1;
  }

  //std::cout << "export file " << exportFileName.toStdString() << " is open\n";

  // write header and data to the uff58b file for each signal in list
  for ( int sigIndex : sigNumList ) {

    // get binary file name from fileName and signal + extension
    QString binFile = FileUtil::makeBinFileName( dh.get(), fileName, sigIndex );

    // build a string containing just the binary file name without the directory
    QString simpleName = FileUtil::extractFileName( binFile );
    simpleName = simpleName + "." + Cnst::BinExtension.c_str();

    double minT=0, maxT=0;
    unsigned long minByte=0, maxByte=0;

    if ( mainWindow->exportDialog->exportRange == "From Start/End" ) {
    
      // get min and max time
      minT = mainWindow->exportDialog->startTimeValInSec;
      maxT = mainWindow->exportDialog->endTimeValInSec;

      // get min and max rec number corresponding to min and max time
      st = bd->getRecordRangeForTime( binFile, sampleRate, minT, maxT,
                                      minByte, maxByte );

    }
    else {

      unsigned long maxEle;
      bd->getMaxElements ( binFile, 0, maxEle );
      minByte = 0;
      maxByte = maxEle * sizeof(int) - 1;

    }

    double timeInc = 1.0 / sampleRate;
    //unsigned long recRange = maxByte - minByte + 1;
    unsigned long recRange = maxByte - minByte;
    minT = minByte / sizeof(int) / sampleRate;

    //std::cout << "minT = " << minT << std::endl;
    //std::cout << "maxT = " <<  maxT << std::endl;
    //std::cout << "minByte = " <<  minByte << std::endl;
    //std::cout << "maxByte = " <<  maxByte << std::endl;
    //std::cout << "recRange (ele) = " << recRange/sizeof(int) << std::endl;

    // build the uff58b header lines
    st = uff58b->set58bHeader( recRange );

    st = uff58b->set80CharRec( 1, std::get<DataHeader::SIGNAME>( indexMap[sigIndex] ) );
    st = uff58b->set80CharRec( 2, std::get<DataHeader::DESC>( indexMap[sigIndex] ) );
    st = dh->getString( "AcquisitionStartDate", str );
    st = uff58b->set80CharRec( 3, str );
    st = uff58b->set80CharRec( 4, "NONE" );
    st = uff58b->set80CharRec( 5, std::get<DataHeader::IDLINE5>( indexMap[sigIndex] ) );

    int funcType = 1; // general or unknown - (0 to 27 possible values)
    int dataCategory = std::get<DataHeader::TYPE>( indexMap[sigIndex] ); // (21 possible values)
    int funcIdNum = 0; // signal number
    int versionOrSeq = 0;
    int loadCaseIdNum = 0;
    QString rspEntityName = std::get<DataHeader::EGU>( indexMap[sigIndex] );
    int rspNode = std::get<DataHeader::RESPONSENODE>( indexMap[sigIndex] );
    int rspDir = std::get<DataHeader::RESPONSEDIRECTION>( indexMap[sigIndex] );
    QString refEntityName = "NONE";
    int refNode = std::get<DataHeader::REFERENCENODE>( indexMap[sigIndex] );
    int refDir = std::get<DataHeader::REFERENCEDIRECTION>( indexMap[sigIndex] );
    st = uff58b->setDofIdentification( funcType, funcIdNum, versionOrSeq, loadCaseIdNum,
                                       rspEntityName, rspNode, rspDir, refEntityName, refNode, refDir );

    int ordinateDataType = 2; int eleSize = sizeof(float); // single prec float
    //int ordinateDataType = 4; int eleSize = sizeof(double); // double prec float
    //int ordinateDataType = 5; int eleSize = sizeof(float) * 2; // single prec complex
    //int ordinateDataType = 6; int eleSize = sizeof(double) * 2; // double prec complex
    //int abscissaSpacing = 0; // uneven
    int abscissaSpacing = 1; // even
    st = uff58b->setDataForm( ordinateDataType, recRange/eleSize, abscissaSpacing, minT, timeInc, 0.0 );

    int lenUnitsExponent, forceUnitsExponent, tempUnitsExponent;

    int ordinateDataCategory = 17; // time (21 possible values)
    uff58b->getExponents( ordinateDataCategory, rspDir, lenUnitsExponent, forceUnitsExponent, tempUnitsExponent );
    st = uff58b->setDataCharacteristics( 8, ordinateDataCategory, lenUnitsExponent, forceUnitsExponent,
                                         tempUnitsExponent, "Time", "s" );
    
    // exponents depend on dataCategory
    uff58b->getExponents( dataCategory, refDir, lenUnitsExponent, forceUnitsExponent, tempUnitsExponent );
    //st = uff58b->setDataCharacteristics( 9,  dataCategory, lenUnitsExponent, forceUnitsExponent,
    //                                     tempUnitsExponent, "Acceleration", "NONE" );
    st = uff58b->setDataCharacteristics( 9,  dataCategory, lenUnitsExponent, forceUnitsExponent,
                                         tempUnitsExponent,
                                         std::get<DataHeader::SIGNAME>( indexMap[sigIndex] ),
                                         std::get<DataHeader::EGU>( indexMap[sigIndex] ) );

    // ordinate denominator data characteristics
    st = uff58b->setDataCharacteristics( 10, 0, 0, 0, 0, "NONE", "NONE" ); // unused
    
    // Z-axis data characteristics
    st = uff58b->setDataCharacteristics( 11, 0, 0, 0, 0, "NONE", "NONE" ); // unused

    //std::fbin = open input bin file
    result = fbInput.open( binFile.toStdString(), std::ios::in | std::ios::binary );
    if ( !result ) {
      qWarning()<<__func__<<"Unable to open for reading"<<binFile;
      fbExport.close();
      return -1;
    }

    qWarning() << __func__ << "processing input file: " << binFile << " ...";

    st = uff58b->writeHeader( fbExport );
    if ( st ) {
      fbInput.close();
      fbExport.close();
      qWarning()<<__func__<<"Unable to write header";
      return -1;
    }

    //std::cout << "export header is written\n";
    
    // get slope and intercept for conversion
    double slope = std::get<DataHeader::SLOPE>( indexMap[sigIndex] );
    double intercept = std::get<DataHeader::INTERCEPT>( indexMap[sigIndex] );

    int intBuf[1000];
    float outBuf[1000];
    int numFullOps = recRange / 1000;
    int numRemaining = recRange % 1000;
    int nr, nw;

    // seek to start of binary data for input file
    this->bd->inputSeekToStartOfData( fbInput, minByte );

    //std::cout << "numFullOps = " << numFullOps << std::endl;
    //std::cout << "numRemaining = " << numRemaining << std::endl;

    nr = nw = 0;
    
    //read and write binary files in chunks
    for ( i=0; i<numFullOps; i++ ) {

      nr += this->bd->readTraceData( fbInput, intBuf, 1000 );

      for ( ii=0; ii<1000; ii++ ) {
        outBuf[ii] = (float) intBuf[ii] * slope + intercept;
      }

      nw += uff58b->writeBinary( fbExport, outBuf, 1000 );

    }

    // perform final op
    if ( numRemaining ) {

      nr += this->bd->readTraceData( fbInput, intBuf, numRemaining );

      for ( ii=0; ii<numRemaining; ii++ ) {
        outBuf[ii] = (float) intBuf[ii] * slope + intercept;
      }

      nw += uff58b->writeBinary( fbExport, outBuf, numRemaining );
      
    }

    uff58b->writeSpacer( fbExport );

    //std::cout << "nr = " << nr << ", nw = " << nw  << std::endl;
    
    //close input file
    std::cout << " complete." << std::endl;
    fbInput.close();

  }

  // close export file
  fbExport.close();

  std::cout << "UFF58b export complete." << std::endl;
  
  return 0;
  
}

void ViewerCtlr::doUff58bExport( void ) {

  int request = ViewerCtlr::HaveUff58bExportRequest;
  dataRequestList.push_back( std::make_tuple( request, nullptr, 0, "" ) );

}

void ViewerCtlr::doCsvExport( void ) {

  int request = ViewerCtlr::HaveCsvExportRequest;
  dataRequestList.push_back( std::make_tuple( request, nullptr, 0, "" ) );

}

void ViewerCtlr::haveHorizontalPan (int id, int curSigIndex, QString& curFileName,
                                    double x0, double x1, double y0, double y1 ) {

  //std::cout << "have H pan - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;
  //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());
  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    //haveHorizontalPanRequest = true;
    int request = ViewerCtlr::HaveHorizontalPanRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtlr::haveFullScale (int id, int curSigIndex, QString& curFileName ) {

  //std::cout << "have Full Scale - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;

  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());
  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtlr::HaveFullScaleRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtlr::haveScale (int id, int curSigIndex, QString& curFileName,
                            double x0, double x1, double y0, double y1 ) {

  //std::cout << "have Scale - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;
  //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());
  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtlr::HaveScaleRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtlr::nonSlotHaveScale ( ViewerGraphAreaBase *vga, int curSigIndex, QString& curFileName,
                            double x0, double x1, double y0, double y1 ) {

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {

    nonSlotHavScaleX0 = x0;
    nonSlotHavScaleX1 = x1;

    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtlr::HaveNonSlotScaleRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
    
  }

}

void ViewerCtlr::haveVgaRoiSelectScale ( QWidget *_vga, int curSigIndex, QString& curFileName,
  double leftX, double rightX ) {

  roiX0 = leftX;
  roiX1 = rightX;

  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( _vga );
  ViewerGraphBase *vg = vga->graph.data();

  //std::cout << "vga roi select scale - id = " << vga->id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;
  //std::cout << "leftX = " << leftX << ", rightX = " << rightX << std::endl;

  double  x0, x1, y0, y1;
  
  vg->getAxesLimits( x0, y0, x1, y1 );
  vg->views.pushView( x0, y0, x1, y1 );
  vg->prevViewAction->setEnabled( true );

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtlr::HaveRoiSelectScaleRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtlr::haveRubberBandScale (int id, int curSigIndex, QString& curFileName ) {

  //std::cout << "have rubber band Scale - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;

  double  x0, x1, y0, y1;
  
  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());
  vg->getAxesLimits( x0, y0, x1, y1 );
  vg->views.pushView( x0, y0, x1, y1 );
  vg->prevViewAction->setEnabled( true );

  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtlr::HaveRubberBandScaleRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtlr::havePrevViewRequest (int id, int curSigIndex, QString& curFileName ) {

  //std::cout << "have prev view request - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;

  double  x0, x1, y0, y1;
  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());

  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtlr::HavePrevViewRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtlr::haveReset (int id, int curSigIndex, QString& curFileName ) {
  
  //std::cout << "have Reset - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;

  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());
  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtlr::HaveResetRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }
  
}

void ViewerCtlr::haveDoCalcFft (QWidget *w ) {

  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( w );
  
  //std::cout << "have do calc fft - id = " << vga->id << 
  //  ", cur sig index = " << vga->curSigIndex <<
  //  ", cur file = " << vga->curFileName.toStdString() << std::endl;

  if ( ( vga->curSigIndex >= 0 ) && ( vga->curSigIndex <= Cnst::MaxSigIndex ) ) {
    int request = ViewerCtlr::HaveFftRequest;
    //std::cout << "push fft request" << std::endl;
    dataRequestList.push_back( std::make_tuple( request, vga, vga->curSigIndex, vga->curFileName ) );
  }
  
}

void ViewerCtlr::selectionRange ( int vgaId, double xMin, double xMax ) {

  std::stringstream strm1, strm2;
  
  //std::cout << "selectionRange - id = " << vgaId << ", xMin = " << xMin << ", xMax = " << xMax << std::endl;
  
  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());
  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );

  if( isFFT( vga ) ) return;

  if ( xMin < 0.0 ) xMin = 0.0;

  if ( haveDataForFft ) {
    int curSigIndex;
    QString curFileName;
    vga->getCurInfo( curFileName, curSigIndex );
    double minT=0, maxT=0;
    QString binFile = FileUtil::makeBinFileName( dh.get(), curFileName, curSigIndex );
    int st = bd->getDataFullTimeRange( binFile, sampleRate, minT, maxT );
    if ( !st ) {
      if ( xMax > maxT ) xMax = maxT;
      if ( xMin < minT ) xMin = minT;
    }
  }
  
  vga->updateSelectionRange( xMin, xMax );

  mainWindow->exportDialog->endTimeValInSec = xMax;
  strm1.clear();
  strm1 << std::left << std::setprecision(8) << xMax;
  QString text1 = strm1.str().c_str();
  text1 = text1.simplified();
  mainWindow->exportDialog->endTimeLineEdit->setText( text1 );
  //std::cout << "text1 = " << text1.toStdString() << std::endl;
  
  mainWindow->exportDialog->startTimeValInSec = xMin;
  strm2.clear();
  strm2 << std::left << std::setprecision(8) << xMin;
  QString text2 = strm2.str().c_str();
  text2 = text2.simplified();
  mainWindow->exportDialog->startTimeLineEdit->setText( text2 );
  //std::cout << "text2 = " << text2.toStdString() << std::endl;
  
}

void ViewerCtlr::mousePosition ( int vgaId, double x, double y ) {

  //std::cout << "mouse pos - id = " << vgaId << ", x = " << x << ", y = " << y << std::endl;
  ViewerGraphBase *vg = qobject_cast<ViewerGraphBase *>(sender());
  ViewerGraphAreaBase *vga = qobject_cast<ViewerGraphAreaBase *>( vg->parent1 );

  if ( isFFT( vga ) ) {

    double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0,
      distX=0, distY=0, newXMin=0, newXMax=0, newYMin=0, newYMax=0,
      plotW=0, plotH=0, chartXRange=1, chartYRange=1, xFact, yFact,
      x0, x1, y0, y1;

    //vg->getPlotSize( plotW, plotH );
    //vg->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
    //if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
    //if ( ( chartYMax - chartYMin ) != 0 ) chartYRange = chartYMax - chartYMin;

    //std::cout << "numFft = " << numFft << std::endl;

    double sampleRate = 0.0;
    int stat = this->dh->getDouble( "SampleRate", sampleRate );
    if ( stat ) {
      //std::cout << "getDouble - stat = " << stat << std::endl;
    }
    double sr = sampleRate;
    int index = 0;
    //std::cout << "sr = " << sr << std::endl;
    if ( sr != 0.0 ) {
      index = x * numFft / sr;
      if ( index < 0 ) index = 0.0;
      if ( index > numFft ) index = numFft;
      //std::cout << "index = " << index << std::endl;
    }

    vg->getPlotSize( plotW, plotH );
    //std::cout << " plotW = " << plotW << " plotH = " << plotH << std::endl;
    
    vg->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );
    //std::cout << " chartXMin = " << chartXMin << " chartXMax = " << chartXMax
    //          << " chartYMin = " << chartYMin << " chartYMax = " << chartYMax << std::endl << std::endl;
      
    if ( ( chartXMax - chartXMin ) != 0 ) chartXRange = chartXMax - chartXMin;
    if ( ( chartYMax - chartYMin ) != 0 ) chartYRange = chartYMax - chartYMin;
    //std::cout << " chartXRange = " << chartXRange << " chartYRange = " << chartYRange   << std::endl << std::endl;
    
    double onePixel = chartXRange / plotW;
    //std::cout << "onePixel = " << onePixel << std::endl  << std::endl;

    //double freq = index * sr / numFft;
    //double loFreq = freq - onePixel * Cnst::peakPixelRange;
    double freq = x;
    double loFreq = freq - onePixel * Cnst::peakPixelRange;
    if ( loFreq < 0.0 ) loFreq = 0.0;
    double hiFreq = freq + onePixel * Cnst::peakPixelRange;
    //if ( hiFreq > sr / 2.0 ) hiFreq = sr / 2.0;
    if ( hiFreq > sr ) hiFreq = sr;

    //std::cout << "loFreq = " << loFreq << std::endl;
    //std::cout << "hiFreq = " << hiFreq << std::endl;

    int loIndex = loFreq * numFft / sr;
    if ( loIndex < 0 ) loIndex = 0;
    int hiIndex = hiFreq * numFft / sr;
    if ( hiIndex > numFft ) hiIndex = numFft;

    if ( ( hiIndex - loIndex ) < 7 ) {
      loIndex = index - 3;
      if ( loIndex < 0 ) loIndex = 0;
      hiIndex = index + 3;
      if ( hiIndex > numFft ) hiIndex = numFft;
    }

    loFreq = loIndex * sr / numFft;

    //std::cout << "loIndex = " << loIndex << std::endl;
    //std::cout << "hiIndex = " << hiIndex << std::endl;

    //find max y and freq at max y
    double frac = 2.0 / (double) numFft;
    double max = sqrt( fftOut[loIndex][0]*fftOut[loIndex][0] +
                       fftOut[loIndex][1]*fftOut[loIndex][1] ) * frac;
    double maxFreq = loFreq;

    double mag;
    for ( int i=loIndex; i<hiIndex; i++ ) {
      
      mag = sqrt( fftOut[i][0]*fftOut[i][0] +
                       fftOut[i][1]*fftOut[i][1] ) * frac;

      freq = i * sr / numFft;
      //std::cout << "i = " << i << ", freq = " << freq << ". mag = " << mag
      //          << ", max = " << max << ", maxFreq = " << maxFreq << std::endl;
      
      if ( mag > max ) {
        max = mag;
        maxFreq = i * sr / numFft;
      }

    }

    vga->updatePeakInfo( maxFreq, max );

  }
  
  vga->updateMousePosition( x, y );
  
}

void ViewerCtlr::sliderValue( QWidget *w, int value ) {

  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0;
  ViewerGraphArea *vga = dynamic_cast<ViewerGraphArea *>( w );

  if ( !vga ) return;
  if ( !(vga->graph) ) return;
  if ( vga->graph->isEmpty ) return;

  if ( value == vga->curSliderValue ) return;
  vga->curSliderValue = value;

  vga->graph->getAxesLimits(  chartXMin, chartYMin, chartXMax, chartYMax );

  vga->graph->views.pushView( chartXMin, chartYMin, chartXMax, chartYMax );
  vga->graph->prevViewAction->setEnabled( true );

  double range = chartXMax - chartXMin;
  double dataTimeIncrementInSec = 1.0 / sampleRate;
  double maxT = (double) curMaxElements * dataTimeIncrementInSec;
  double minT = (double) value / 100.0 * maxT;
  if ( minT < 0.0 ) {
    minT -= 0.0;
  }
  else if ( minT > maxT ) {
    minT = maxT;
  }

  double newChartXMin = minT;
  double newChartXMax = minT + range;

  vga->graph->setAxesLimits(  newChartXMin, chartYMin, newChartXMax, chartYMax );

  if ( ( vga->curSigIndex >= 0 ) && ( vga->curSigIndex <= Cnst::MaxSigIndex ) ) {
    int request = ViewerCtlr::HaveHorizontalPanRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, vga->curSigIndex, vga->curFileName ) );
  }

}

void ViewerCtlr::setSlider( QWidget *w ) {

  double chartXMin=0, chartXMax=0, chartYMin=0, chartYMax=0;
  ViewerGraphArea *vga = dynamic_cast<ViewerGraphArea *>( w );

  if ( !vga ) return;
  if ( !(vga->graph) ) return;
  if ( vga->graph->isEmpty ) return;

  double dataTimeIncrementInSec = 1.0 / sampleRate;
  double maxT = (double) curMaxElements * dataTimeIncrementInSec;
  vga->graph->getAxesLimits( chartXMin, chartYMin, chartXMax, chartYMax );

  vga->curSliderValue = (int) ( chartXMin / maxT * 100.0 );

  vga->slider->setValue( vga->curSliderValue );

}

void ViewerCtlr::lockTimeScaleEvent( bool flag ) {

  lockTimeScale = flag;
  
}
