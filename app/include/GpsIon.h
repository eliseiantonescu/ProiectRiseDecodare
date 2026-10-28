#pragma once

#include "Block.h"
#include <QDataStream>
#include <QDebug>

class GpsIon : public Block 
{

    public:
        GpsIon();
        quint16 blockId() const override { return 5893; }
        bool decode(const RawBlock &block) override;
        bool validateBody() override;
        void print() const override;
    
    private:
        quint32 m_tow;
        quint16 m_wnk;
        quint8 m_prn;
        quint8 m_reserved;
        float fAlpha0;
        float fAlpha1;
        float fAlpha2;
        float fAlpha3;
        float fBeta0;
        float fBeta1;
        float fBeta2;
        float fBeta3;

};