#pragma once
#include "SbfBlock.h"
#include <QtGlobal>

class Block {
public:
  virtual ~Block() = default;
  virtual quint16 blockId() const = 0;
  virtual bool decode(const RawBlock &block) = 0;
  virtual bool validateBody() = 0;
  virtual void print() const = 0;
};
