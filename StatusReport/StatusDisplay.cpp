//
// Created by jws3 on 6/16/24.
//

#include "StatusDisplay.h"

StatusDisplay::StatusDisplay() {}

std::string StatusDisplay::getLabels(uint status) {

  status &= Mask;
  std::string str;
  std::stringstream strm;

  for ( int i=NumBitsInUse-1; i>=0; i-- ) {
    if ( status & ( (uint) 1 << i ) ) {
      strm << StatLabel[i];
    }
    else {
      strm << StatBlank[i];
    }
    if ( i != 0 ) strm << " ";
  }

  str = strm.str();
  return str;

}

void StatusDisplay::displayKey(void) {

  std::cout << std::endl << "Status Bit Description [XMIT PKT TIM PLL]:" << std::endl << std::endl <<
  "XMIT: Bit 3 - Packet transmission overrun" << std::endl <<
  "PKT:  Bit 2 - Packet building overrun" << std::endl <<
  "TIM:  Bit 1 - Time of day (Seconds/Nanosecods fields) may be invalid" << std::endl <<
  "PLL:  Bit 0 - System clock PLL unlocked" << std::endl << std::endl;

}
