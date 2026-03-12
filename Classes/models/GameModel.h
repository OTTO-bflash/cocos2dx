#pragma once
#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "cocos2d.h"
#include "CardModel.h"
#include <vector>
#include <unordered_map>

// 前向声明
class CardModel;

/**
 * 游戏数据模型
 * 负责存储整体游戏状态和数据
 */
class GameModel
{
public:
    GameModel();
    ~GameModel();

    // 获取和设置游戏区域卡牌
    const std::vector<CardModel*>& getPlayFieldCards() const { return _playFieldCards; }
    void setPlayFieldCards(const std::vector<CardModel*>& cards);

    // 获取和设置底部卡牌（当前显示的底部卡牌）
    CardModel* getBottomCard() const { return _bottomCard; }
    void setBottomCard(CardModel* card);

    // 获取和设置堆栈卡牌
    const std::vector<CardModel*>& getStackCards() const { return _stackCards; }
    void setStackCards(const std::vector<CardModel*>& cards);

    // 根据ID获取卡牌
    CardModel* getCardById(int cardId) const;

    // 从游戏区域移除卡牌
    bool removeCardFromPlayField(int cardId);

    // 从堆栈抽取顶部卡牌
    CardModel* drawCardFromStack();

    // 添加卡牌到底部
    void addCardToBottom(CardModel* card);

private:
    std::vector<CardModel*> _playFieldCards; // 游戏区域卡牌
    CardModel* _bottomCard;                  // 当前底部卡牌
    std::vector<CardModel*> _stackCards;     // 堆栈卡牌

    // 用于快速查找卡牌的映射表
    std::unordered_map<int, CardModel*> _cardMap;

    void updateCardMap();
};

#endif // GAME_MODEL_H