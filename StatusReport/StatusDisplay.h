//
// Created by jws3 on 6/16/24.
//

#ifndef STATUSREPORT_STATUSDISPLAY_H
#define STATUSREPORT_STATUSDISPLAY_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <strstream>

/*
Bit 4: Calibration factors are invalid.
Bit 3: Packet transmission overrun
Bit 2: Packet building overrun
Bit 1: Time of day (Seconds/Nanosecods fields) may be invalid
Bit 0: System clock PLL unlocked

 CAL XMIT PKT TIM PLL

Please note that bit 4 is so far always set, and should be ignored.
This firmware feature is not currently used.
*/

class StatusDisplay {

public:

  static const uint Mask = 0xf;
  static const uint NumBits = 5;
  static const uint NumBitsInUse = 4;
  inline static const std::string StatLabel[NumBits] {
     "PLL",
     "TIM",
     "PKT",
     "XMIT",
     "CAL",
  };
  inline static const std::string StatBlank[NumBits] {
     " ok",
     " ok",
     "ok ",
     "ok  ",
     " ok"
  };

  StatusDisplay();
  std::string getLabels( uint status );
  void displayKey( void );

};


#endif //STATUSREPORT_STATUSDISPLAY_H
