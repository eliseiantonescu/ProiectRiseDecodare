#pragma once

#include <QtGlobal>
#include <QByteArray>

namespace Sbf {

    // 1. ID-urile Blocurilor SBF
    enum BlockId : quint16 {
        GEO_RAW_L1 = 4020,
        GAL_RAW_FNAV = 4022,
        MEAS_EPOCH = 4027,
        GPS_RAW_CA = 4017,
        GPS_ION = 5893
    };

    // 2. Constante Globale (Do-Not-Use)
    namespace Constants {
        constexpr quint32 INVALID_TOW = 4294967295;
        constexpr quint16 INVALID_WNC = 65535;
    }

    
    namespace Utils {
        inline quint16 calculateCrc(const QByteArray &data) {
            quint16 crcFinal = 0x0000;
            quint32 length = data.size();
            const quint8* p_u8Data = reinterpret_cast<const quint8*>(data.constData());
            for (quint32 iterator = 0; iterator < length; ++iterator) {
                crcFinal ^= (static_cast<quint16>(p_u8Data[iterator]) << 8);
                for (int bit = 0; bit < 8; ++bit) {
                    if (crcFinal & 0x8000) {
                        crcFinal = (crcFinal << 1) ^ 0x1021;
                    } else {
                        crcFinal <<= 1;
                    }
                }
            }
            return crcFinal;
        }
    }
}