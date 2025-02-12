# Quartz Data Pre-viewer

Viewer for data files acquired by Osprey DCS Quartz ADC system.

See other software components:

- Acquisition sequencing https://github.com/osprey-dcs/atf-engine
- Quartz IOC https://github.com/osprey-dcs/atf-acq-ioc

## Dependencies

- Linux
- CMake
- Qt5
  - Qt5Charts
- FFTW3

## Building

```sh
sudo apt-get install build-essential cmake libqt5charts5-dev libfftw3-dev

git clone https://github.com/osprey-dcs/atf-previewer.git
cd atf-previewer
cmake -S . -B build
cmake --build build
```
