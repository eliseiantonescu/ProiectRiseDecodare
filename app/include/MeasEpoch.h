#pragma once
#include "Block.h"
#include <QDataStream>
#include <QDebug>
#include <QVector>
// al 2 lea subbloc
struct MeasEpochChannelType2 
{
  quint8 type;
  quint8 lockTime;
  quint8 cn0;
  quint8 offSetsMSB;
  qint8 carrierMSB;
  quint8 obsInfo;
  quint16 codeOffsetLSB;
  quint16 carrierLSB;
  quint16 dopplerOffSetLSB;
};

// primul subbloc
struct MeasEpochChannelType1 
{
  quint8 rxChannel;
  quint8 type;
  quint8 svid;
  quint8 misc;
  quint32 codeLSB;
  qint32 doppler;
  quint16 carrierLSB;
  qint8 carrierMSB;
  quint8 cn0;
  quint16 lockTime;
  quint8 obsInfo;
  quint8 n2;

  QVector<MeasEpochChannelType2> type2Blocks;
};

class MeasEpoch : public Block 
{
  public:
    MeasEpoch();
    quint16 blockId() const override { return 4027; }
    bool decode(const RawBlock &block) override;
    bool validateBody() override;
    void print() const override;

  private:
    quint32 m_tow;
    quint16 m_wnk;
    quint8 m_n1;
    quint8 m_sb1Length;
    quint8 m_sb2Length;
    quint8 m_commonFlags;
    quint8 m_cumClkJumps;
    quint8 m_reserved;

    QVector<MeasEpochChannelType1> m_channels;
};
