# File Formats

## UFF58b

[UFF specification](https://www.ceas3.uc.edu/sdrluff/all_files.php) files referenced:

- unv.asc
- 58b.asc
- 58.asc

## .hdr file

Meta-data JSON file.

See [`quartz-header.json`](quartz-header.json) [JSON Schema](https://json-schema.org/) definition.

## .dat file

Input to fileConverter in
[PSC file format](https://github.com/osprey-dcs/atf-acq-ioc/blob/main/documentation/PacketFormat.md#psc-file-format)
.

## .j Channel data

```c
struct jdata { // values always LSBF (aka. little endian)
    // format version
    uint32_t version[3] = {1, 0, 0};
    // size in bytes of data to follow.
    // should be file size - 20 bytes
    uint64_t size;
    int32_t data[(size-20)/4];
};
```

## .j Status data

```c
struct jstatus { // values always LSBF (aka. little endian)
    uint32_t version[3] = {1, 0, 0};
    // size in bytes of data to follow.
    // should be file size - 20 bytes
    uint64_t size;
    struct status { // 40 bytes
        uint32_t status;
        uint32_t chmask;
        uint32_t posixSecDev;
        uint32_t nanoSecDev;
        uint32_t posixSecRecv;
        uint32_t nanoSecRecv;
        uint32_t alarmLo;
        uint32_t alarmLoLo;
        uint32_t alarmHigh;
        uint32_t alarmHighHigh;
    } data[(size-20)/40];
};
```
