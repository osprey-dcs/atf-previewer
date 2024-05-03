//
// Created by jws3 on 4/10/24.
//

#ifndef CNST_H
#define CNST_H

class Cnst {

  public:
  
  inline static const std::string BinRoot {"/home/osprey/dataBin/"};
  inline static const std::string HdrRoot {"/home/osprey/dataHdr/"};
  inline static const std::string DefaultHdrExtension {"hdr"};
  inline static const std::string DefaultFilter {"*.hdr"};

  static const unsigned int MaxSignals {32};
  static const int MaxSigIndex {1023};
  static const int MaxBufferElements {100};
  static const int MaxRecs {32};
  static const unsigned int Max3PerWord {2000};
  static const unsigned int Max4PerWord {4000};
  static const unsigned int MaxValIndex {100};
  static const unsigned int MaxStatus {1000};
  static const unsigned int MaxStatusIndex {990};

  static constexpr double EventProcessPeriod {0.05};
  static constexpr double LoopsPerSec {1.0/EventProcessPeriod};

};

#endif //CNST_H
