//
// Created by jws3 on 5/10/24.
//

#ifndef VIEWER_UFF58BEXPORT_H
#define VIEWER_UFF58BEXPORT_H

#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#include <map>
#include <tuple>

#include <QString>

#include "ExportDialog.h"
#include "DataHeaderFac.h"

class Uff58bExport {

public:

  static const int LITTLE = 1;
  static const int BIG = 2;
  static const int DEC_VMS = 1;
  static const int IEEE_754 = 2;
  static const int IBM_5_370 = 3;
  static const int NUM_ASCII_LINES = 11;
  static const int SINGLE_PREC_REAL = 2;
  static const int DOUBLE_PREC_REAL = 4;
  static const int SINGLE_PREC_COMPLEX = 5;
  static const int DOUBLE_PREC_COMPLEX = 6;
  static const int UNEVEN = 0;
  static const int EVEN = 1;
  static const int ABCISSA = 8;
  static const int ORDINATE = 9;
  static const int ORDINATE_DENOMINATOR = 10;
  static const int ZAXIS = 11;

  enum class DirectionType { Scalar, PlusXTrans, MinusXTrans,
     PlusYTrans, MinusYTrans, PlusZTrans, MinusZTrans, PlusXRot, MinusXRot,
     PlusYRot, MinusYRot, PlusZRot, MinusZRot };

  enum class DataCharacteristics { Unknown, General, Stress, Strain, Temperature,
     HeatFlux, Displacement, ReactionForce, Velocity, Acceleration, ExcitationForce,
     Pressure, Mass, Time, Frequency, Rpm, Order };

  enum class FuncType { GeneralOrUnknown, TimeResponse, AutoSectrum, CrossSpectrum,
     FrequencyResponseFunction, Transmissibility, Coherence, AutoCorrelation,
     CrossCorrelation, PowerSpectralDensity, EnergySpectralDensity,
     ProbabilityDensityfunction, Spectrum, CumulativeFrequencyDistribution,
     PeaksValley, StressCycles, StrainCycles, Orbit, ModeIndicatorFunction,
     ForcePattern, ParialPower, PartialCoherence, Eigenvalue, Eigenvector,
     ShockResponseSpectrum, FiniteImpulseResponseFilter, MultipleCoherence,
     OrderFunction};

  static const int LENGTH_EXP = 0;
  static const int FORCE_EXP = 1;
  static const int TEMP_EXP = 2;

  typedef std::tuple<int, int, int> ExponentTplType;
  typedef std::map<int, ExponentTplType> ExponentMapType;
  ExponentMapType translationExponentMap {
    {  0, std::make_tuple( 0, 0, 0 ) },
    {  1, std::make_tuple( 0, 0, 0 ) },
    {  2, std::make_tuple(-2, 1, 0 ) },
    {  3, std::make_tuple( 0, 0, 0 ) },
    {  4, std::make_tuple( 0, 0, 0 ) },
    {  5, std::make_tuple( 0, 0, 1 ) },
    {  6, std::make_tuple( 1, 1, 0 ) },
    {  7, std::make_tuple( 0, 0, 0 ) },
    {  8, std::make_tuple( 1, 0, 0 ) },
    {  9, std::make_tuple( 0, 1, 0 ) },
    { 10, std::make_tuple( 0, 0, 0 ) },
    { 11, std::make_tuple( 1, 0, 0 ) },
    { 12, std::make_tuple( 1, 0, 0 ) },
    { 13, std::make_tuple( 0, 1, 0 ) },
    { 14, std::make_tuple( 0, 0, 0 ) },
    { 15, std::make_tuple(-2, 1, 0 ) },
    { 16, std::make_tuple(-1, 1, 0 ) },
    { 17, std::make_tuple( 0, 0, 0 ) },
    { 18, std::make_tuple( 0, 0, 0 ) },
    { 19, std::make_tuple( 0, 0, 0 ) },
    { 20, std::make_tuple( 0, 0, 0 ) } };

  ExponentMapType rotationExponentMap {
    {  0, std::make_tuple( 0, 0, 0 ) },
    {  1, std::make_tuple( 0, 0, 0 ) },
    {  2, std::make_tuple(-1, 1, 0 ) },
    {  3, std::make_tuple( 0, 0, 0 ) },
    {  4, std::make_tuple( 0, 0, 0 ) },
    {  5, std::make_tuple( 0, 0, 1 ) },
    {  6, std::make_tuple( 1, 1, 0 ) },
    {  7, std::make_tuple( 0, 0, 0 ) },
    {  8, std::make_tuple( 0, 0, 0 ) },
    {  9, std::make_tuple( 1, 1, 0 ) },
    { 10, std::make_tuple( 0, 0, 0 ) },
    { 11, std::make_tuple( 0, 0, 0 ) },
    { 12, std::make_tuple( 0, 0, 0 ) },
    { 13, std::make_tuple( 1, 1, 0 ) },
    { 14, std::make_tuple( 0, 0, 0 ) },
    { 15, std::make_tuple(-1, 1, 0 ) },
    { 16, std::make_tuple( 1, 1, 0 ) },
    { 17, std::make_tuple( 0, 0, 0 ) },
    { 18, std::make_tuple( 0, 0, 0 ) },
    { 19, std::make_tuple( 0, 0, 0 ) },
    { 20, std::make_tuple( 0, 0, 0 ) } };

  explicit Uff58bExport ( const QString& );

  virtual ~Uff58bExport ();

  int set58bHeader( int64_t numBytes );

  int set80CharRec ( int rec, QString line );

  int setDofIdentification( int funcType, int funcIdNum, int versNum, int ldCaseIdNum,
                            QString rspEntityName, int rspNode, int rspDir,
                            QString refEntityName, int refNode, int refDir );

  int setDataForm( int dataType, int numEle, int xSpacing, double xMin, double xInc, double zAxisVal );


  int setDataCharacteristics( int recNum, int dataType, int lengthUnitsExponent, int forceUnitsExponent,
                              int tempUnitsExponent, QString axisLabel, QString axisUnitsLabel );

  int writeSpacer( std::filebuf &fb );

  int writeHeader( std::filebuf &fb );

  int64_t writeBinary( std::filebuf &fb, double *buf, int64_t numBytes  );
  
  int64_t writeBinary( std::filebuf &fb, float *buf, int64_t numBytes  );

  int64_t writeBinary( std::filebuf &fb, char *buf, int64_t numBytes  );

  int64_t writeBinary( std::filebuf &fb, const char *buf, int64_t numBytes  );

  void getExponents( int dataType, int direction, int &lenExp, int &forceExp, int &tempExp );

  bool isTranslation( int val );

  bool isRotation( int val );

  DataHeaderFac dhf;
  std::shared_ptr<DataHeader> dh;
  QString hdrFile;
  QString outFile;
  QString uff58bIdLine;
  QString headerLines1thru11[11];

};


#endif //VIEWER_UFF58BEXPORT_H
