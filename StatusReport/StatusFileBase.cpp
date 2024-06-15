//
// Created by jws3 on 6/15/24.
//

#include "StatusFileBase.h"

StatusFileBase::StatusFileBase() : ErrHndlr( NumErrs, errMsgs ) {

}

StatusFileBase::~StatusFileBase() {

}

int StatusFileBase::openRead(const std::string &name) {
  return 0;
}

int StatusFileBase::openWrite(const std::string &name) {
  return 0;
}

int StatusFileBase::closeRead(void) {
  return 0;
}

int StatusFileBase::closeWrite(void) {
  return 0;
}

int StatusFileBase::readHeader(void) {
  return 0;
}

void StatusFileBase::getVersion(int64_t &major, int64_t &minor, int64_t &release) {

}

int64_t StatusFileBase::getRecSize(void) {
  return 0;
}

int64_t StatusFileBase::getNumBytes(void) {
  return 0;
}

int64_t StatusFileBase::getMaxElements(void) {
  return 0;
}

std::string StatusFileBase::getFileType(void) {
  return std::string();
}

std::string StatusFileBase::getCccr(void) {
  return std::string();
}

void StatusFileBase::getSummaryRecord( int *rec ) {
}

int64_t StatusFileBase::getHeaderSize(void) {
  return 0;
}

int StatusFileBase::getDataFullTimeRange(double &minTime, double &maxTime) {
  return 0;
}

int StatusFileBase::getRecordRangeForTime(double minTime, double maxTime, int64_t &min, int64_t &max) {
  return 0;
}

void StatusFileBase::inputSeekToStartOfData(int64_t firstDataByte) {

}

void StatusFileBase::outputSeekToStartOfData(int64_t firstDataByte) {

}

void StatusFileBase::seekToReadOffset(int64_t offset) {

}

int64_t StatusFileBase::readData(int *buf, int64_t readSizeInbytes) {
  return 0;
}
