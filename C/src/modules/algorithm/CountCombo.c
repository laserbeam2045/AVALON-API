#include "count_combo.h"

// クラス変数
int CountCombo_canNotClearDrops;   // 消せないドロップ（ビットフラグ)
int CountCombo_activeDrops;        // 落ちうるドロップ（ビットフラグ）

// 初期化関数
void CountCombo_init(BoardSettings *bsp)
{
  CountCombo_canNotClearDrops = BoardSettings_getCanNotClearDrops(bsp);
  CountCombo_activeDrops = BoardSettings_getActiveDrops(bsp);
}