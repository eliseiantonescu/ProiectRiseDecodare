#pragma once
#include <QCoreApplication>
#include <QDataStream>


struct SbfBlock//headeru
{
    quint8 sync1;
    quint8 sync2;
    quint16 crc;
    quint16 id;
    quint16 length;

    SbfBlock() :sync1(0),sync2(0), crc(0), id(0), length(0) {}

    quint16 getRealId() const
    {
        return id & 0x1FFF;
    }
};

struct RawBlock
{
    SbfBlock header;
    QByteArray payload;

};
