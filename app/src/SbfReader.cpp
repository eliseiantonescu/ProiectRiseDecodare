#include "SbfReader.h"
#include "Utils.h"

SbfReader::SbfReader(const QString &fileName) : m_File(fileName) 
{
  if (!m_File.open(QIODevice::ReadOnly)) 
  {
    qCritical() << "Failed to open file:" << fileName;
    return;
  } else {
    m_Stream.setDevice(&m_File);
    m_Stream.setByteOrder(QDataStream::LittleEndian);
    qInfo() << "File opened successfully:";
  }
}

SbfReader::~SbfReader() 
{
  if (m_File.isOpen()) 
  {
    m_File.close();
  }
}

bool SbfReader::readNextBlock(RawBlock &block) 
{

  while (!m_Stream.atEnd()) 
  {
    quint8 sync1 = 0;
    quint8 sync2 = 0;

    m_Stream >> sync1;

    if (sync1 != 0x24) 
    {
      continue;
    }

    m_Stream >> sync2;

    if (sync2 != 0x40) 
    {
      m_File.seek(m_File.pos() - 1);
      continue;
    }
    SbfBlock header;
    header.sync1 = sync1;
    header.sync2 = sync2;

    m_Stream >> header.crc >> header.id >> header.length;

    if (header.length % 4 != 0) 
    {
      qWarning() << "Length is not a multiple of 4, skipping" << Qt::endl;
      continue;
    }

    qint64 currentPos = m_File.pos();

    m_File.seek(currentPos - 4);

    QByteArray baData = m_File.read(header.length - 4);

    quint16 calculatedCrc = Sbf::Utils::calculateCrc(baData);
    if (header.crc != calculatedCrc) 
    {
      qWarning() << "CRC mismatch, sarim peste" << Qt::endl;
      m_File.seek(currentPos - 7);
      continue;
    }

    block.header = header;
    block.payload = baData.mid(4);
    return true;
  }
  return false;
}
