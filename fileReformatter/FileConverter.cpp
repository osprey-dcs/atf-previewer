//
// Created by jws3 on 5/1/24.
//

#include "FileConverter.h"

FileConverter::FileConverter ( ) {

  for ( int i=0; i<Cnst::MaxSignals; i++ ) {
    fileLoc[i] = 0;
  }

  statusFileLoc = 0;


}

int FileConverter::convert ( const DataHeader *dh, const QString &rawDataFile, const QString& binDataFileDir,
                             const QString& simpleName ) {

  // first cut will be single threaded

  // Open rawDataFile

  // Read raw data header.
  // Use signal bit field to determine how many signals are present.
  // Construct a vector of signal numbers.

  // Open all binary signal files (up to 32). Construct a vector
  // of binary data file handles.

  // N will be set to the number of signals.

  // Allocate a raw data file buffer that will accommodate some number, MAXRECS, records of data
  // and N binary signal file buffers of MAXRECS entries for moving data into for writing the binary
  // signal files.

  return FileConverter::E_FAIL;

}

void FileConverter::show( void ) {
  std::cout << "Base Class FileConverter" << std::endl;
}
