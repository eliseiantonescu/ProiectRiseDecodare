#include "MeasEpoch.h"
#include "Utils.h"
#include <QIODevice>

MeasEpoch::MeasEpoch()
    : m_tow(0), m_wnk(0), m_n1(0), m_sb1Length(0), m_sb2Length(0),
      m_commonFlags(0), m_cumClkJumps(0), m_reserved(0) {}

bool MeasEpoch::decode(const RawBlock &block) 
{
  QByteArray payload = block.payload;
  QDataStream stream(payload);

  stream.setByteOrder(QDataStream::LittleEndian);

  stream >> m_tow >> m_wnk >> m_n1 >> m_sb1Length >> m_sb2Length >>
      m_commonFlags >> m_cumClkJumps >> m_reserved;

  m_channels.clear();

  for (int type1Iter = 0; type1Iter < m_n1; ++type1Iter) 
  {
    qint64 startType1 = stream.device()->pos();
    MeasEpochChannelType1 ch1;

    stream >> ch1.rxChannel >> ch1.type >> ch1.svid >> ch1.misc >>
        ch1.codeLSB >> ch1.doppler >> ch1.carrierLSB >> ch1.carrierMSB >>
        ch1.cn0 >> ch1.lockTime >> ch1.obsInfo >> ch1.n2;

    stream.device()->seek(startType1 + m_sb1Length);

    if (ch1.n2 > 0) {
      for (int type2Iter = 0; type2Iter < ch1.n2; ++type2Iter) 
      {
        qint64 startType2 = stream.device()->pos();
        MeasEpochChannelType2 ch2;
        stream >> ch2.type >> ch2.lockTime >> ch2.cn0 >> ch2.offSetsMSB >>
            ch2.carrierMSB >> ch2.obsInfo >> ch2.codeOffsetLSB >>
            ch2.carrierLSB >> ch2.dopplerOffSetLSB;

        ch1.type2Blocks.append(ch2);

        stream.device()->seek(startType2 + m_sb2Length);
      }
    }
    m_channels.append(ch1);
    qint64 nextType1Start = startType1 + m_sb1Length + (ch1.n2 * m_sb2Length);
    stream.device()->seek(nextType1Start);
  }

  if (validateBody() == 0) 
  {
    return false;
  }

  print();

  return true;
}

void MeasEpoch::print() const 
{
  qDebug() << "==================================================";
  qDebug() << "BLOC MEAS EPOCH 4027 | TOW:" << m_tow << "| WNc:" << m_wnk;
  qDebug() << "Number of Type 1 sub-blocks (n1):" << m_n1;
  qDebug() << "Sb1length :" << m_sb1Length;
  qDebug() << "Sb2length :" << m_sb2Length;
  qDebug() << "CommonFlags :" << m_commonFlags;
  qDebug() << "cumclKJumps :" << m_cumClkJumps;
  qDebug() << "Reserved :" << m_reserved;
  qDebug() << "==================================================";

  for (int type1Iter = 0; type1Iter < m_channels.size(); ++type1Iter) 
  {
    const auto &ch1 = m_channels[type1Iter];

    qDebug() << "[ Type 1 sub-block #" << type1Iter + 1 << " ] ";
    qDebug() << " SVID  =" << ch1.svid << "| RxChannel  = " << ch1.rxChannel
             << "| N2  =" << ch1.n2 << "| type   = " << ch1.type
             << "| misc    = " << ch1.misc;
    qDebug() << " CodeLSB  =" << ch1.codeLSB << "| Doppler  =" << ch1.doppler
             << "| CarrierLSB  =" << ch1.carrierLSB
             << "| CarrierMSB  =" << ch1.carrierMSB << "| CN0    =" << ch1.cn0;
    qDebug() << " Lock time  =" << ch1.lockTime
             << "| Obs info  =" << ch1.obsInfo << Qt::endl;
    qDebug() << "-------------------------------------------";

    for (int type2Iter = 0; type2Iter < ch1.type2Blocks.size(); ++type2Iter) {
      const auto &ch2 = ch1.type2Blocks[type2Iter];

      qDebug() << "[ Type 2 sub-block #" << type2Iter + 1
               << "] Type:" << ch2.type;
      qDebug() << "Lock Time :" << ch2.lockTime;
      qDebug() << "CN0 :" << ch2.cn0;
      qDebug() << "OffsetsMSB :" << ch2.offSetsMSB;
      qDebug() << "Carrier MSB :" << ch2.carrierMSB;
      qDebug() << "Obs info : " << ch2.obsInfo;
      qDebug() << "Code offset LSB :" << ch2.codeOffsetLSB;
      qDebug() << "Carrier LSB " << ch2.carrierLSB;
      qDebug() << "Dopler Offset LSB :" << ch2.dopplerOffSetLSB << Qt::endl
               << Qt::endl;
    }
  }
  qDebug() << "--------------------------------------------------" << Qt::endl;
}

bool MeasEpoch::validateBody() {
  if (m_tow == Sbf::Constants::INVALID_TOW ||
      m_wnk == Sbf::Constants::INVALID_WNC) {
    qWarning() << "MeasEpoch rejected: TOW or WNc is invalid (Do-Not-Use).";
    return false;
  }

  for (const auto &ch1 : m_channels) 
  {
    quint8 codeMSB = ch1.misc & 0x0F;

    if (codeMSB == 0 && ch1.codeLSB == 0) 
    {
      qWarning() << "MeasEpoch Type 1 rejected (SVID:" << ch1.svid
                 << "): Pseudorange is invalid.";
      return false;
    }

    if (ch1.doppler == -2147483648) 
    {
      qWarning() << "MeasEpoch Type 1 rejected (SVID:" << ch1.svid
                 << "): Doppler is invalid.";
      return false;
    }

    if (ch1.carrierMSB == -128 && ch1.carrierLSB == 0) 
    {
      qWarning() << "MeasEpoch Type 1 rejected (SVID:" << ch1.svid
                 << "): Carrier Phase is invalid.";
      return false;
    }

    if (ch1.cn0 == 255) 
    {
      qWarning() << "MeasEpoch Type 1 rejected (SVID:" << ch1.svid
                 << "): C/N0 is invalid.";
      return false;
    }

    if (ch1.lockTime == 65535) 
    {
      qWarning() << "MeasEpoch Type 1 rejected (SVID:" << ch1.svid
                 << "): Lock Time is invalid.";
      return false;
    }

    for (const auto &ch2 : ch1.type2Blocks) 
    {
      quint8 codeOffsetMSB = ch2.offSetsMSB & 0x07;
      quint8 dopplerOffsetMSB = (ch2.offSetsMSB >> 3) & 0x1F;

      if (ch2.lockTime == 255)
    {
        qWarning() << "MeasEpoch Type 2 rejected (SVID:" << ch1.svid
                   << ", Signal:" << ch2.type << "): Lock Time invalid.";
        return false;
      }

      if (ch2.cn0 == 255) 
      {
        qWarning() << "MeasEpoch Type 2 rejected (SVID:" << ch1.svid
                   << ", Signal:" << ch2.type << "): C/N0 invalid.";
        return false;
      }

      if (ch2.carrierMSB == -128 && ch2.carrierLSB == 0) 
      {
        qWarning() << "MeasEpoch Type 2 rejected (SVID:" << ch1.svid
                   << ", Signal:" << ch2.type << "): Carrier Phase invalid.";
        return false;
      }

      if (codeOffsetMSB == 0x04 && ch2.codeOffsetLSB == 0) 
      {
        qWarning() << "MeasEpoch Type 2 rejected (SVID:" << ch1.svid
                   << ", Signal:" << ch2.type << "): Code Offset invalid (-4).";
        return false;
      }

      if (dopplerOffsetMSB == 0x10 && ch2.dopplerOffSetLSB == 0) 
      {
        qWarning() << "MeasEpoch Type 2 rejected (SVID:" << ch1.svid
                   << ", Signal:" << ch2.type
                   << "): Doppler Offset invalid (-16).";
        return false;
      }
    }
  }
  return true;
}