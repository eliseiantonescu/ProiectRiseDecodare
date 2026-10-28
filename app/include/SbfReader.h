#pragma once
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include "SbfBlock.h"


class SbfReader
{
public:
    SbfReader(const QString &fileName);
    ~SbfReader();

    bool readNextBlock(RawBlock &block);
    
    

private:
    QFile m_File;
    QDataStream m_Stream;
};
