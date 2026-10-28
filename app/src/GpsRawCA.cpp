#include "GpsRawCA.h"
#include "Utils.h"

GpsRawCA::GpsRawCA()
    : m_svid(0), m_crcPassed(0), m_viterbi(0), m_source(0), m_freqNr(0),
      m_rxChannel(0), m_wnk(0), m_tow(0) {
  for (int nBitsIter = 0; nBitsIter < 10; ++nBitsIter)
  {
    m_navBits[nBitsIter] = 0;
  }
}

bool GpsRawCA::decode(const RawBlock &block) 
{
  QByteArray payload = block.payload;
  QDataStream stream(payload);

  stream.setByteOrder(QDataStream::LittleEndian);

  stream >> m_tow >> m_wnk >> m_svid >> m_crcPassed >> m_viterbi >> m_source >>
      m_freqNr >> m_rxChannel;

  for (int nBitsIter = 0; nBitsIter < 10; ++nBitsIter) 
  {
    stream >> m_navBits[nBitsIter];
  }

  if (validateBody() == 0) 
  {
    return false;
  }

  print();

  return true;
}

bool GpsRawCA::validateBody()   
{
  if (m_tow == Sbf::Constants::INVALID_TOW ||
      m_wnk == Sbf::Constants::INVALID_WNC) {
    return false;
  }

  return true;
}

void GpsRawCA::print() const 
{
  qInfo() << "Found block with ID 4017 (GPSRawCA)!";
  qDebug() << "tow: " << m_tow;
  qDebug() << "wnk: " << m_wnk;
  qDebug() << "svid: " << m_svid;
  qDebug() << "crcPassed: " << m_crcPassed;
  qDebug() << "viterbi: " << m_viterbi;
  qDebug() << "source: " << m_source;
  qDebug() << "freqNr: " << m_freqNr;
  qDebug() << "rxChannel: " << m_rxChannel;
  qDebug() << "navBits: ";
  for (int nBitsIter = 0; nBitsIter < 10; ++nBitsIter) 
  {
    qDebug() << "  [" << nBitsIter + 1 << "]: " << Qt::hex
             << m_navBits[nBitsIter] << Qt::dec;
  }
  qDebug() << " ";
}