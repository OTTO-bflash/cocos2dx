#ifndef _CARD_DEFINES_H_
#define _CARD_DEFINES_H_

#include "cocos2d.h"

/*卡牌花色类型
*/
enum class CardSuitType
{
	NONE = -1,
	CLUBS, //梅花
	DIAMONDS, //方块
	HEARTS, //红桃
	SPADES, //黑桃
	NUM_CARD_SUIT_TYPES

};
/*卡牌点数
*/
enum class CardFaceType
{
	NONE = -1,
	ACE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN,
	JACK,
	QUEEN,
	KING,
	NUM_CARD_FACE_TYPES
};

/*
* 游戏区域类型
*/
enum class GameAreaType
{
	PLAY_FIELD, //游戏区域
	BOTTOM, //底部区域
	STACK, //堆栈区域
};

/*操作类型
*/
enum class OperationType
{
	CARD_MATCH, //卡牌匹配
	DRAW_CARD, //抽牌
	NEW_CARD_ADDED, //新卡牌添加到游戏区域
	SPECIAL_MOVE //特殊移动
};

#endif

