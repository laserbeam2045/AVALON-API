#include "ClearingSettings.h"

// 初期化関数
void ClearingSettings_init(ClearingSettings* this, char request[])
{
  this->twoWay = Parser_getInt(request, "twoWay");
  this->breakThrough = Parser_getInt(request, "breakThrough");
  this->line = Parser_getInt(request, "line");
  this->cross = Parser_getInt(request, "cross");
  this->L = Parser_getInt(request, "L");
  this->required = Parser_getInt(request, "required");
  this->clearAll = Parser_getInt(request, "clearAll");
  this->notClear = Parser_getInt(request, "notClear");

  if (
    this->twoWay == 0 &&
    this->breakThrough == 0 &&
    this->line == 0 &&
    this->cross == 0 &&
    this->L == 0 &&
    this->required == 0 &&
    this->clearAll == 0 &&
    this->notClear == 0
  ) {
    this->active = false;
  } else {
    this->active = true;
  }
}

// 1つでも設定された項目があるかどうかを返す関数
bool ClearingSettings_isActive(ClearingSettings* this)
{
  return this->active;
}

// type属性のcolor番目のビットについて、フラグが立っている（設定されている）かどうかを返す関数
bool ClearingSettings_isActiveOf(ClearingSettings* this, const char type, const char color)
{
  switch (type) {
  case (CS_TYPE)TWO_WAY       : return (1 << color) & this->twoWay;
  case (CS_TYPE)BREAK_THROUGH : return (1 << color) & this->breakThrough;
  case (CS_TYPE)LINE          : return (1 << color) & this->line;
  case (CS_TYPE)CROSS         : return (1 << color) & this->cross;
  case (CS_TYPE)L             : return (1 << color) & this->L;
  case (CS_TYPE)REQUIRED      : return (1 << color) & this->required;
  case (CS_TYPE)CLEAR_ALL     : return (1 << color) & this->clearAll;
  case (CS_TYPE)NOT_CLEAR     : return (1 << color) & this->notClear;
  }
}