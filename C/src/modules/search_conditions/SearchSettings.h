#ifndef _SEARCH_SETTINGS_H_
#define _SEARCH_SETTINGS_H_

#include "../io/Parser.h"

// 探索に関する設定のクラス
typedef struct {
  int beamWidth;        // 探索の幅
  int beamDepth;        // 探索の深さ
  char diagonalLimit;   // 斜め移動回数の上限
  char comboLimit;      // コンボ数の上限  
} SearchSettings;

// 初期化関数
extern void SearchSettings_init(SearchSettings* this, char request[]);

// beamWidth属性を取得する関数
extern int SearchSettings_getBeamWidth(SearchSettings* this);

// beamDepth属性を取得する関数
extern int SearchSettings_getBeamDepth(SearchSettings* this);

// diagonalLimit属性を取得する関数
extern char SearchSettings_getDiagonalLimit(SearchSettings* this);

// comboLimit属性を取得する関数
extern char SearchSettings_getComboLimit(SearchSettings* this);

#endif  // _SEARCH_SETTINGS_H_