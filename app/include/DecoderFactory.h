#pragma once
#include "Block.h"
#include "GEORawL1.h"
#include "GalRawFnavDecoder.h"
#include "GpsIon.h"
#include "GpsRawCA.h"
#include "MeasEpoch.h"
#include "Utils.h"
#include <QtGlobal >


class DecoderFactory {
public:
  static Block *createDecoder(quint16 blockId) {

    switch (blockId) {
    case Sbf::BlockId::GAL_RAW_FNAV:
      return new GalRawFnavDecoder();
    case Sbf::BlockId::GEO_RAW_L1:
      return new GEoRawL1();
    case Sbf::BlockId::MEAS_EPOCH:
      return new MeasEpoch();
    case Sbf::BlockId::GPS_RAW_CA:
      return new GpsRawCA();
    case Sbf::BlockId::GPS_ION:
      return new GpsIon();
    default:
      return nullptr;
    }
  }
};
