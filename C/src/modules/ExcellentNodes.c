#include "ExcellentNodes.h"


// 初期化関数
void ExcellentNodes_init(ExcellentNodes* this)
{
  ComboData *comboData = SearchNode_getComboData(&this->stack[0]);
  ComboData_setEvaluation(comboData, -9999999999);
  this->nextIndex = 0;
  this->bestIndex = 0;
  this->nodeCount = 0;
}

// 最良ノードをスタックに追加する関数
void ExcellentNodes_setBestNode(ExcellentNodes* this, const SearchNode* searchNode)
{
  int index = this->nextIndex;

  this->stack[index] = *searchNode;
  this->nextIndex = (index + 1) % STACK_NODE_MAX;
  this->bestIndex = index;
  this->nodeCount++;
}

// スタックされたノードの数を返す関数
int ExcellentNodes_getNodeCount(ExcellentNodes* this)
{
  return this->nodeCount;
}

// 指定されたインデックスのノードのポインタアドレスを返す関数
SearchNode* ExcellentNodes_getNodeOf(ExcellentNodes* this, const int index)
{
  return &this->stack[index];
}

// 最良ノードの評価値を返す関数
double ExcellentNodes_getBestEvaluation(ExcellentNodes* this)
{
  SearchNode *bestNode = &this->stack[this->bestIndex];
  ComboData *comboData = SearchNode_getComboData(bestNode);

  return ComboData_getEvaluation(comboData);
}

// 最良ノードを返す関数
SearchNode ExcellentNodes_getBestNode(ExcellentNodes* this)
{
  return this->stack[this->bestIndex];
}