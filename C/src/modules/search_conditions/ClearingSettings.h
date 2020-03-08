#ifndef _CLEARING_SETTINGS_H_
#define _CLEARING_SETTINGS_H_

#include <stdbool.h>
#include "../io/Parser.h"

// 各設定項目を表す定数
typedef enum {
  TWO_WAY,
  BREAK_THROUGH,
  LINE,
  CROSS,
  L,
  REQUIRED,
  CLEAR_ALL,
  NOT_CLEAR,
} CS_TYPE;

// 消し方に関する設定のクラス
// MEMO: activeを除き、それぞれがビットフラグになっている
typedef struct {
  int twoWay;           // ４個消しを含めるかどうか
  int breakThrough;     // 無効貫通を含めるかどうか
  int line;             // 列消しを含めるかどうか
  int cross;            // 十字消しを含めるかどうか
  int L;                // L字消しを含めるかどうか
  int required;         // 必ず消す必要があるかどうか
  int clearAll;         // 全消しをするかどうか
  int notClear;         // 消してはいけないかどうか
  bool active;          // １つでも設定項目があるかどうか
} ClearingSettings;

// 初期化関数
extern void ClearingSettings_init(ClearingSettings* this, char request[]);

// 1つでも設定された項目があるかどうかを返す関数
extern bool ClearingSettings_isActive(ClearingSettings* this);

// type属性のcolor番目のビットについて、フラグが立っている（設定されている）かどうかを返す関数
extern bool ClearingSettings_isActiveOf(ClearingSettings* this, const char type, const char color);

#endif  // _CLEARING_SETTINGS_H_