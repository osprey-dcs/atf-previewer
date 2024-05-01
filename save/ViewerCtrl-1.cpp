//
// Created by jws3 on 4/3/24.
//

#include <iostream>
#include <iomanip>

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

#include "ViewerCtlr.h"

static void setAll( int *ary, int n, int v ) {
  for ( int i=0; i<n; i++ ) {
    ary[i] = v;
  }
}

ViewerCtrl::ViewerCtrl( ViewerMainWin *mainw ) {

  this->up = upfac.createUserPrefs();

  this->mainWindow = mainw;
  this->haveFile = false;
  this->fileName = "";
  this->haveHeader = false;
  this->readyForData = false;
  setAll( this->prevSigIndex, Cnst::NumGraphs, -1 );
  this->sampleRate = 0.0;

  QRectF r { 0, 0, 0, 0 };
  for ( int i=0; i<Cnst::NumGraphs; i++ ) {
    this->plotAreaDimensions[i] = r;
  }

  dhf = new DataHeaderFac();
  dh = dhf->createDataHeader();

  bdf = new BinDataFac();
  bd = bdf->createBinData();

  fftw_complex *fftIn = nullptr;
  numPts = 0ul;
  numFft = 0ul;
  maxFft = 0ul;
        
  // Main window dimemsion change requires replotting all signals
  // I probably don't need this
  this->connect( mainw, SIGNAL( dimensionChange( double, double, double, double ) ),
                 this, SLOT( dimension( double, double, double, double ) ) );

  this->connect( mainw->fileSelect, SIGNAL( fileSelected( const QString& ) ),
                 this, SLOT( fileSelected1( const QString& ) ) );

  // Signal name change or plot area change requires replotting that signal
  for ( ViewerGraphArea *w : mainw->getVgaList() ) {

    //Sig name change is the result of user choosing new sig name
    this->connect( w->sigName, SIGNAL( activated1( int, int, QWidget * ) ),
                   this, SLOT( sigNameChange1( int, int, QWidget * ) ) );

    this->connect( w->graph, SIGNAL( horizontalPan( int, int, QString&, double, double, double, double ) ),
                   this, SLOT( haveHorizontalPan( int, int, QString&, double, double, double, double ) ) );

    this->connect( w->graph, SIGNAL( scale( int, int, QString&, double, double, double, double ) ),
                   this, SLOT( haveScale( int, int, QString&, double, double, double, double ) ) );
    
    this->connect( w->graph, SIGNAL( reset( int, int, QString& ) ),
                   this, SLOT( haveReset( int, int, QString& ) ) );
    
    this->connect( w->graph, SIGNAL( rubberBandScale( int, int, QString& ) ),
                   this, SLOT( haveRubberBandScale( int, int, QString& ) ) );
    
    // Plot area change is the result of window resize
    // I probably don't need this
    //this->connect( w->graph->chart, SIGNAL( plotAreaChanged( const QRectF & ) ),
    //               this, SLOT( sigAreaChanged( const QRectF & ) ) );

  }

}

static int count = 0;
static int delayCount = 0;
static int secCount = 0;
static int count250ms = 0;
static double value = 0;
static bool plotAreasAllNonZero = false;
static bool doNothing = false;
static bool once = true;

int ViewerCtrl::processHeaderFile ( void ) {

  QString hdrFile = this->fileName + ".hdr";

  int stat = this->dh->readContents( hdrFile );
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
    this->mainWindow->setSampleRate( sampleRate );
  }
  else {
    std::cout << "Failed to get sample rate" << std::endl;
  }

  // populate combo boxes for all graphs
  auto nameList = this->dh->getNameList();
  auto nameMap = this->dh->getNameMap();
  std::list<ViewerGraphArea *> vgaList = this->mainWindow->getVgaList();
  for ( ViewerGraphArea *vga : vgaList ) {
    vga->sigName->clear();
    vga->sigName->addItem( "", -1 );
    setAll( this->prevSigIndex, Cnst::NumGraphs, -1 );
    for ( QString name : nameList ) {
      int sigIndex = std::get<7>( nameMap[name] );
      vga->sigName->addItem( name, sigIndex );
    }
  }

  return 0;

}

void ViewerCtrl::process( void ) {

  int stat;
  
  // process all handled events here

  if ( doNothing ) return;

  if ( once ) {
    once = false;
    //std::cout << "process events " << std::endl;
  }

  // wait until plot area dimension are all non-zero (or max 10 seconds)
  if ( !plotAreasAllNonZero ) {
    std::list<ViewerGraphArea *> vgalist = this->mainWindow->getVgaList();
    int maxCount = Cnst::LoopsPerSec * 10;
    while ( !plotAreasAllNonZero && (delayCount < maxCount) ) {

      // check about once per sec
      secCount += 1;
      if ( secCount > Cnst::LoopsPerSec ) {
        secCount = 0;
        //std::cout << "check plot areas" << std::endl;
        for ( ViewerGraphArea *vga: vgalist ) {
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
    
    while ( !(this->dataRequestList.empty()) ) {

      this->mainWindow->setWorking( QString("Working...") );

      auto dataReq = this->dataRequestList.back();
      this->dataRequestList.pop_back();

      int request = std::get<0>( dataReq );
      ViewerGraphArea *grArea = std::get<1>( dataReq );
      int grAreaId = grArea->id;
      int sigIndex = std::get<2>( dataReq );
      QString reqFileName = std::get<3>( dataReq );

      //std::cout << "req = " <<  std::get<0>( dataReq ) <<
      //  ", vga id = " << std::get<1>( dataReq )->id <<
      //  ", sig index = " << std::get<2>( dataReq ) <<
      //  ", file name = " << std::get<3>( dataReq ).toStdString() << std::endl;

      if ( ( request == ViewerCtrl::HaveDataRequest ) && this->readyForData ) {
          
        //std::cout << "haveDataRequest" << std::endl;

        // get min/max time values
        double x0, x1, y0, y1;
        //grArea->graph->getAxesLimits( x0, x1, y0, y1 );
        //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

        x0 = 0.0;
        x1 = Cnst::InitialMaxTime;
        y0 = Cnst::InitialMinSig;
        y1 = Cnst::InitialMaxSig;
        
        // get num of pixels in x
        QSizeF size = grArea->graph->chart->size();
        //std::cout << "x pixels = " << size.width() << std::endl;

        // save reqFileName before appending extension
        grArea->setCurInfo( reqFileName, sigIndex );
        
        QString binFile = reqFileName + ".bin";
        
        // Viewer graph object manages qls
        QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
        double miny, maxy;
        double dataTimeIncrementInSec = 1.0 / sampleRate/ 1000.0;

        double x0WithLimits = x0, x1WithLimits = x1;
        if ( x0WithLimits < 0.0 ) x0WithLimits = 0.0;
        if ( x1WithLimits < 0.0 ) x1WithLimits = 0.0;
        if ( x1WithLimits < x0WithLimits ) std::swap( x1WithLimits, x0WithLimits );

        std::cout << "sizeof(fftw_complex) = " << sizeof(fftw_complex) << std::endl;
        fftIn = new fftw_complex[Cnst::MaxFftSize];
        maxFft = Cnst::MaxFftSize;
        numFft = 0ul;
        numPts = 0ul;

        mainWindow->setWhat( "reading file..." );
    
        //  file name, sig index, x scale width in pixels, start time in sec,
        //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
        stat = this->bd->genLineSeries( binFile, sigIndex, size.width(), x0WithLimits, x1WithLimits,
                                        dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
        if ( !stat ) {
          
          mainWindow->setNumPoints( numPts );

          // Viewer graph object manages qls
          grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, miny, maxy );

          if ( numFft ) {
            mainWindow->setWhat( "fft..." );
            fftw_complex *fftOut = new fftw_complex[numFft];
            fftw_plan p = fftw_plan_dft_1d( numFft, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE );
            fftw_execute( p );
            std::cout << "fft done" << std::endl;
            fftw_destroy_plan( p );
            fftw_free( fftOut );
          }

          fftw_free( fftIn );
        
        }
        else {
          
          mainWindow->setNumPoints( 0ul );

          std::cout << "genLineSeries failure" << std::endl;

        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtrl::HaveResetRequest ) && this->readyForData ) {

        //std::cout << "haveResetRequest" << std::endl;

        // get min/max time values
        double x0, x1, y0, y1;
        //grArea->graph->getAxesLimits( x0, x1, y0, y1 );
        //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

        x0 = 0.0;
        x1 = Cnst::InitialMaxTime;
        y0 = Cnst::InitialMinSig;
        y1 = Cnst::InitialMaxSig;
        
        // get num of pixels in x
        QSizeF size = grArea->graph->chart->size();
        //std::cout << "x pixels = " << size.width() << std::endl;

        // save reqFileName before appending extension
        grArea->setCurInfo( reqFileName, sigIndex );
        
        QString binFile = reqFileName + ".bin";
        
        // Viewer graph object manages qls
        QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
        double miny, maxy;
        double dataTimeIncrementInSec = 1.0 / sampleRate/ 1000.0;

        double x0WithLimits = x0, x1WithLimits = x1;
        if ( x0WithLimits < 0.0 ) x0WithLimits = 0.0;
        if ( x1WithLimits < 0.0 ) x1WithLimits = 0.0;
        if ( x1WithLimits < x0WithLimits ) std::swap( x1WithLimits, x0WithLimits );

        fftIn = new fftw_complex[Cnst::MaxFftSize];
        maxFft = Cnst::MaxFftSize;
        numFft = 0ul;
        numPts = 0ul;
       
        //  file name, sig index, x scale width in pixels, start time in sec,
        //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
        stat = this->bd->genLineSeries( binFile, sigIndex, size.width(), x0WithLimits, x1WithLimits,
                                        dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
        if ( !stat ) {

          mainWindow->setNumPoints( numPts );

          // Viewer graph object manages qls
          grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, miny, maxy );

          if ( numFft ) {
            mainWindow->setWhat( "fft..." );
            fftw_complex *fftOut = new fftw_complex[numFft];
            fftw_plan p = fftw_plan_dft_1d( numFft, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE );
            fftw_execute( p );
            std::cout << "fft done" << std::endl;
            fftw_destroy_plan( p );
            fftw_free( fftOut );
          }

          fftw_free( fftIn );
        
        }
        else {
          
          mainWindow->setNumPoints( 0ul );
          
          std::cout << "genLineSeries failure" << std::endl;
          
        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtrl::HaveHorizontalPanRequest ) && this->readyForData ) {

        //std::cout << "haveHorizontalPanRequest" << std::endl;

        // get min/max time values
        double x0, x1, y0, y1;
        grArea->graph->getAxesLimits( x0, x1, y0, y1 );
        
        // get num of pixels in x
        QSizeF size = grArea->graph->chart->size();
        //std::cout << "x pixels = " << size.width() << std::endl;

        // save reqFileName before appending extension
        grArea->setCurInfo( reqFileName, sigIndex );
        
        QString binFile = reqFileName + ".bin";
        
        // Viewer graph object manages qls
        QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
        double miny, maxy;
        double dataTimeIncrementInSec = 1.0 / sampleRate/ 1000.0;
          
        double x0WithLimits = x0, x1WithLimits = x1;
        if ( x0WithLimits < 0.0 ) x0WithLimits = 0.0;
        if ( x1WithLimits < 0.0 ) x1WithLimits = 0.0;
        if ( x1WithLimits < x0WithLimits ) std::swap( x1WithLimits, x0WithLimits );

        fftIn = new fftw_complex[Cnst::MaxFftSize];
        maxFft = Cnst::MaxFftSize;
        numFft = 0ul;
        numPts = 0ul;
        
        //  file name, sig index, x scale width in pixels, start time in sec,
        //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
        stat = this->bd->genLineSeries( binFile, sigIndex, size.width(), x0WithLimits, x1WithLimits,
                                        dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
        if ( !stat ) {
          
          mainWindow->setNumPoints( numPts );
          
          // Viewer graph object manages qls
          grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1 );

          if ( numFft ) {
            mainWindow->setWhat( "fft..." );
            fftw_complex *fftOut = new fftw_complex[numFft];
            fftw_plan p = fftw_plan_dft_1d( numFft, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE );
            fftw_execute( p );
            std::cout << "fft done" << std::endl;
            fftw_destroy_plan( p );
            fftw_free( fftOut );
          }

          fftw_free( fftIn );
        
        }
        else {
          
          mainWindow->setNumPoints( 0ul );
          
          std::cout << "genLineSeries failure" << std::endl;
          
        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtrl::HaveScaleRequest ) && this->readyForData ) {

        //std::cout << "haveScaleRequest" << std::endl;

        // get min/max time values
        double x0, x1, y0, y1;
        grArea->graph->getAxesLimits( x0, x1, y0, y1 );
        
        // get num of pixels in x
        QSizeF size = grArea->graph->chart->size();
        //std::cout << "x pixels = " << size.width() << std::endl;

        // save reqFileName before appending extension
        grArea->setCurInfo( reqFileName, sigIndex );
        
        QString binFile = reqFileName + ".bin";
        
        // Viewer graph object manages qls
        QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
        double miny, maxy;
        double dataTimeIncrementInSec = 1.0 / sampleRate/ 1000.0;
          
        double x0WithLimits = x0, x1WithLimits = x1;
        if ( x0WithLimits < 0.0 ) x0WithLimits = 0.0;
        if ( x1WithLimits < 0.0 ) x1WithLimits = 0.0;
        if ( x1WithLimits < x0WithLimits ) std::swap( x1WithLimits, x0WithLimits );
        
        fftIn = new fftw_complex[Cnst::MaxFftSize];
        maxFft = Cnst::MaxFftSize;
        numFft = 0ul;
        numPts = 0ul;
        
        //  file name, sig index, x scale width in pixels, start time in sec,
        //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
        stat = this->bd->genLineSeries( binFile, sigIndex, size.width(), x0WithLimits, x1WithLimits,
                                          dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
        if ( !stat ) {
          
          mainWindow->setNumPoints( numPts );

          // Viewer graph object manages qls
          grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

          if ( numFft ) {
            mainWindow->setWhat( "fft..." );
            fftw_complex *fftOut = new fftw_complex[numFft];
            fftw_plan p = fftw_plan_dft_1d( numFft, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE );
            fftw_execute( p );
            std::cout << "fft done" << std::endl;
            fftw_destroy_plan( p );
            fftw_free( fftOut );
          }

          fftw_free( fftIn );
        
        }
        else {
          
          mainWindow->setNumPoints( 0ul );
          
          std::cout << "genLineSeries failure" << std::endl;
          
        }

        mainWindow->setWhat( "Idle" );
        
      }
      else if ( ( request == ViewerCtrl::HaveRubberBandScaleRequest ) ) {

        // rubber band does not cause rescale with signal emission hence the following kludge
        int maxMs = 100;
        for ( int i=0; i<25; i++ ) {
          this->mainWindow->app->processEvents( QEventLoop::AllEvents, maxMs );
          QThread::usleep( 10000 ); // 1/100 sec
        }

        double  x0, x1, y0, y1;
        grArea->graph->getAxesLimits( x0, x1, y0, y1 );
        //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

        // get num of pixels in x
        QSizeF size = grArea->graph->chart->size();
        //std::cout << "x pixels = " << size.width() << std::endl;

        // save reqFileName before appending extension
        grArea->setCurInfo( reqFileName, sigIndex );

        QString binFile = reqFileName + ".bin";

        // Viewer graph object manages qls
        QtCharts::QLineSeries *qls = new QtCharts::QLineSeries();
        double miny, maxy;
        double dataTimeIncrementInSec = 1.0 / sampleRate/ 1000.0;

        double x0WithLimits = x0, x1WithLimits = x1;
        if ( x0WithLimits < 0.0 ) x0WithLimits = 0.0;
        if ( x1WithLimits < 0.0 ) x1WithLimits = 0.0;
        if ( x1WithLimits < x0WithLimits ) std::swap( x1WithLimits, x0WithLimits );

        fftIn = new fftw_complex[Cnst::MaxFftSize];
        maxFft = Cnst::MaxFftSize;
        numFft = 0ul;
        numPts = 0ul;
        
        //  file name, sig index, x scale width in pixels, start time in sec,
        //  end time in sec, data time increment in sec, qls pointer, miny (returned), maxy (returned)
        stat = this->bd->genLineSeries( binFile, sigIndex, size.width(), x0WithLimits, x1WithLimits,
                                        dataTimeIncrementInSec, numPts, *qls, miny, maxy, maxFft, numFft, fftIn );
        if ( !stat ) {
          
          mainWindow->setNumPoints( numPts );
          
          // Viewer graph object manages qls
          grArea->graph->setSeries( qls, sigIndex, reqFileName, x0, x1, y0, y1 );

          if ( numFft ) {
            mainWindow->setWhat( "fft..." );
            fftw_complex *fftOut = new fftw_complex[numFft];
            fftw_plan p = fftw_plan_dft_1d( numFft, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE );
            fftw_execute( p );
            std::cout << "fft done" << std::endl;
            fftw_destroy_plan( p );
            fftw_free( fftOut );
          }

          fftw_free( fftIn );
        
        }
        else {
          
          mainWindow->setNumPoints( 0ul );
          
          std::cout << "genLineSeries failure" << std::endl;
          
        }

        mainWindow->setWhat( "Idle" );
        
      }
        
    }
    
    if ( this->haveFile ) {
      
      this->readyForData = false;
      stat = processHeaderFile();
      if ( !stat ) {
        this->haveHeader = true;
        this->dataRequestList.clear();
        setAll( this->prevSigIndex, Cnst::NumGraphs, -1 );
        this->readyForData = true;
      }
      else {
        this->haveHeader = false;
        this->readyForData = false;
      }
      this->haveFile = false;
    }

  }

}

bool ViewerCtrl::allZero( const QRectF& r ) {
  if ( ( r.right() - r.left()) != 0.0 ) return false;
  if ( ( r.bottom() - r.top()) != 0.0 ) return false;
  return true;
}

bool ViewerCtrl::allNonZero( const QRectF& r ) {
  if ( ( r.right() - r.left()) == 0.0 ) return false;
  if ( ( r.bottom() - r.top()) == 0.0 ) return false;
  return true;
}

bool ViewerCtrl::arrayAllNonZero( const QRectF r[], int n ) {

  for ( int i=0; i<n; i++ ) {
    //qDebug() << n << "   " << i << " : " << r[i];
    if ( ( r[i].right() - r[i].left()) == 0.0 ) return false;
    if ( ( r[i].bottom() - r[i].top()) == 0.0 ) return false;

  }

  return true;

}


void ViewerCtrl::dimension( double _x, double _y, double _w, double _h ) {

  BinData::TwoDIntPtr arry;

  up->setDouble( "x", _x );
  up->setDouble( "y", _y );
  up->setDouble( "w", _w );
  up->setDouble( "h", _h );

  up->update();

}

void ViewerCtrl::sigNameChange( int index ) {
  //std::cout << "ViewerCtrl::sigNameChange - index = " << index << std::endl;
}

void ViewerCtrl::sigNameChange1( int index, int sigIndex, QWidget *w ) {
  ViewerGraphArea *vga = dynamic_cast<ViewerGraphArea *>( w );
  if ( ( sigIndex >= 0 ) && ( sigIndex <= Cnst::MaxSigIndex ) &&
   ( sigIndex != prevSigIndex[vga->id] ) ) {
    prevSigIndex[vga->id] = sigIndex;
    //haveDataRequest = true;
    int request = ViewerCtrl::HaveDataRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, sigIndex, this->fileName ) );
  }

}

void ViewerCtrl::sigAreaChanged( const QRectF& plotArea ) {
  //std::cout << "sigAreaChange" << std::endl;
  QtCharts::QChart *chart = qobject_cast<QtCharts::QChart *>(sender());
  ViewerGraph *vg = qobject_cast<ViewerGraph *>(chart->parent());
  ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>(vg->parent());

  /*std::cout << "sigAreaChange, chart = " << chart << std::endl;
  //std::cout << "sigAreaChange, vg = " << vg << std::endl;
  //std::cout << "sigAreaChange, vga = " << vga << ", id = " << vga->id << std::endl;*/

  if ( vga->id == 0 ) {
    //std::cout << "L = " << plotArea.left() << std::endl;
    //std::cout << "R = " << plotArea.right() << std::endl;
    //std::cout << "T = " << plotArea.top() << std::endl;
    //std::cout << "B = " << plotArea.bottom() << std::endl;
  }

  if ( ( vga->id < 0 ) || ( vga->id > Cnst::NumGraphs) ) {
    std::cout << "Graph id out of bounds";
  }
  else {
    plotAreaDimensions[vga->id] = plotArea;
  }

}

void ViewerCtrl::fileSelected1( const QString& file ) {

  //qDebug() << "file is " << file;

  int l, count;

  l = count = file.length();
  
  QString::const_reverse_iterator it = file.rbegin();
  while ( it != file.rend() ) {
    //qDebug() << *it;
    if ( *it == '.' ) {
      count--;
      break;
    }
    if ( *it != '.' ) count--;
    (it)++;
  }

  if ( count > 1 ) {
    this->fileName = file;
    this->fileName.replace( count, l-count+1, "" );
    this->haveFile = true;
    for ( ViewerGraphArea *w : this->mainWindow->getVgaList() ) {
      w->clear();
    }
  }
  
}

void ViewerCtrl::haveHorizontalPan ( int id, int curSigIndex, QString& curFileName,
                           double x0, double x1, double y0, double y1 ) {

  //std::cout << "have H pan - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;
  //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

  ViewerGraph *vg = qobject_cast<ViewerGraph *>(sender());
  ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    //haveHorizontalPanRequest = true;
    int request = ViewerCtrl::HaveHorizontalPanRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtrl::haveScale ( int id, int curSigIndex, QString& curFileName,
                           double x0, double x1, double y0, double y1 ) {

  //std::cout << "have Scale - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;
  //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;

  ViewerGraph *vg = qobject_cast<ViewerGraph *>(sender());
  ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    //haveScaleRequest = true;
    int request = ViewerCtrl::HaveScaleRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtrl::haveRubberBandScale ( int id, int curSigIndex, QString& curFileName ) {

  //std::cout << "have rubber band Scale - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;

  double  x0, x1, y0, y1;
  ViewerGraph *vg = qobject_cast<ViewerGraph *>(sender());
  vg->getAxesLimits( x0, x1, y0, y1 );
  //std::cout << "x0 = " << x0 << ", x1 = " << x1 << ", y0 = " << y0 << ", y1 = " << y1 << std::endl;
  
  ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    int request = ViewerCtrl::HaveRubberBandScaleRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }

}

void ViewerCtrl::haveReset ( int id, int curSigIndex, QString& curFileName ) {
  
  //std::cout << "have Reset - id = " << id << ", cur sig index = " << curSigIndex <<
  //  ", cur file = " << curFileName.toStdString() << std::endl;

  ViewerGraph *vg = qobject_cast<ViewerGraph *>(sender());
  ViewerGraphArea *vga = qobject_cast<ViewerGraphArea *>( vg->parent1 );
  //std::cout << "vga id = " << vga->id << std::endl;

  if ( ( curSigIndex >= 0 ) && ( curSigIndex <= Cnst::MaxSigIndex ) ) {
    prevSigIndex[vga->id] = curSigIndex;
    //haveDataRequest = true;
    int request = ViewerCtrl::HaveDataRequest;
    dataRequestList.push_back( std::make_tuple( request, vga, curSigIndex, curFileName ) );
  }
  
}

