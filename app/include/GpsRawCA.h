#pragma once

#include "Block.h"
#include <QDataStream>
#include <QDebug>

class GpsRawCA : public Block 
{
    public:
        GpsRawCA();
        quint16 blockId() const override { return 4017; }
        bool decode(const RawBlock &block) override;
        bool validateBody() override;
        void print() const override;
    
    private:
        quint8 m_svid;
        quint8 m_crcPassed;
        quint8 m_viterbi;
        quint8 m_source;
        quint8 m_freqNr;
        quint8 m_rxChannel;
        quint16 m_wnk;
        quint32 m_tow;
        quint32 m_navBits[10];
    
};
