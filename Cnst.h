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
// Created by jws3 on 4/10/24.
//

#ifndef VIEWER_CNST_H
#define VIEWER_CNST_H


class Cnst {

  public:
  
  static const int NumGraphs {2};
  // as of 4/23/2024, NumRows must be 2
  static const int NumRows {2};
  static const int NumCols {1};

  //inline static const std::string defaultDir {"/home/osprey/viewerData/"};
  inline static const std::string BinRoot {"/home/osprey/dataBin/"};
  inline static const std::string HdrRoot {"/home/osprey/dataHdr/"};
  inline static const std::string DefaultHdrExtension {"hdr"};
  inline static const std::string DefaultFilter {"*.hdr"};

  static const int MaxSigIndex {1023};

  static const unsigned long MaxFftSize {45000000};
  static const bool UseHanning {true};
  static constexpr double peakPixelRange {5.0}; // num of pixels (+/-) for peak search

  static const unsigned int InitialGridRgb {0xa0a0a0};

  static constexpr double EventProcessPeriod {0.05};
  static constexpr double LoopsPerSec {1.0/EventProcessPeriod};

  static constexpr double InitialMinTime {0.0};
  static constexpr double InitialMaxTime {60.0};
  static constexpr double InitialMinFreq {0.0};
  static constexpr double InitialMaxFreq {1000.0};
  static constexpr double InitialMinSig {-1023.0};
  static constexpr double InitialMaxSig {1023.0};
  static constexpr double InitialMinFft {0.0};
  static constexpr double InitialMaxFft {10000.0};

  static constexpr double InitialMinSigLog {0.0};
  static constexpr double InitialMaxSigLog {1000.0};
  static constexpr double InitialMinFftLog {0.0};
  static constexpr double InitialMaxFftLog {10000.0};

};


#endif //VIEWER_CNST_H
