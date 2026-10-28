#include "GpsIon.h"
#include "Utils.h"

GpsIon::GpsIon()
    : m_tow(0), m_wnk(0), m_prn(0), m_reserved(0), fAlpha0(0.0f), fAlpha1(0.0f),
      fAlpha2(0.0f), fAlpha3(0.0f), fBeta0(0.0f), fBeta1(0.0f), fBeta2(0.0f),
      fBeta3(0.0f) {}

bool GpsIon::decode(const RawBlock &block) 
{
  QByteArray payload = block.payload;
  QDataStream stream(payload);

  stream.setByteOrder(QDataStream::LittleEndian);

  stream >> m_tow >> m_wnk >> m_prn >> m_reserved >> fAlpha0 >> fAlpha1 >>
      fAlpha2 >> fAlpha3 >> fBeta0 >> fBeta1 >> fBeta2 >> fBeta3;

  if (validateBody() == 0) {
    return false;
  }

  print();

  return true;
}

bool GpsIon::validateBody() 
{
  if (m_tow == Sbf::Constants::INVALID_TOW ||
      m_wnk == Sbf::Constants::INVALID_WNC) {
    return false;
  }

  return true;
}

void GpsIon::print() const 
{
  qInfo() << "Found block with ID 5893 (GPSIon)!";
  qDebug() << "tow: " << m_tow;
  qDebug() << "wnk: " << m_wnk;
  qDebug() << "prn: " << m_prn;
  qDebug() << "reserved: " << m_reserved;
  qDebug() << "alpha0: " << fAlpha0;
  qDebug() << "alpha1: " << fAlpha1;
  qDebug() << "alpha2: " << fAlpha2;
  qDebug() << "alpha3: " << fAlpha3;
  qDebug() << "beta0: " << fBeta0;
  qDebug() << "beta1: " << fBeta1;
  qDebug() << "beta2: " << fBeta2;
  qDebug() << "beta3: " << fBeta3 << Qt::endl;
  qDebug() << " ";
}
