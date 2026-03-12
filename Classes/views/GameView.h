#pragma once
#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "cocos2d.h"
#include "CardView.h"

// 前向声明
class GameModel;

/**
 * 游戏视图
 * 负责游戏整体的UI显示和用户交互
 */
class GameView : public cocos2d::Node
{
public:
    // 使用标准的cocos2d-x创建模式
    static GameView* create();
    virtual bool init() override;

    /**
     * 更新视图显示
     * @param gameModel 游戏数据模型
     */
    void updateView(GameModel* gameModel);

    /**
     * 设置卡牌点击回调
     * @param callback 回调函数，参数为卡牌ID
     */
    void setCardClickCallback(const std::function<void(int)>& callback) {
        _cardClickCallback = callback;
    }

    /**
     * 设置备用牌堆点击回调
     * @param callback 回调函数
     */
    void setStackClickCallback(const std::function<void()>& callback) {
        _stackClickCallback = callback;
    }

    /**
     * 播放卡牌移动动画
     * @param cardId 卡牌ID
     * @param targetPosition 目标位置
     * @param duration 动画时长
     * @param callback 动画完成后的回调
     */
     /*void playCardMoveAnimation(int cardId, const cocos2d::Vec2& targetPosition,
         float duration, const std::function<void()>& callback = nullptr);*/
    CardView* getCardView(int cardId) const;

    /**
     * 播放匹配消除动画
     * @param cardId 卡牌ID
     * @param callback 动画完成后的回调
     */
    void playMatchAnimation(int cardId, const std::function<void()>& callback = nullptr);

    void playCardMoveAnimation(int cardId, const cocos2d::Vec2& targetPosition, float duration, const std::function<void()>& callback = nullptr);

    /**
     * 获取底牌节点位置（用于撤销功能）
     */
    cocos2d::Vec2 getBottomNodePosition() const { return _bottomNode->getPosition(); }

    cocos2d::Vec2 getStackNodePosition() const { return _stackNode->getPosition(); }

    // 添加获取节点的方法
    cocos2d::Node* getPlayFieldNode() const { return _playFieldNode; }
    cocos2d::Node* getBottomNode() const { return _bottomNode; }
    cocos2d::Node* getStackNode() const { return _stackNode; }

private:
    void setupUI();
    void setupStackTouch();
    void createCardView(CardModel* cardModel);
    void onStackTouched();

    std::unordered_map<int, CardView*> _cardViews; // Card ID to card view mapping
    std::function<void(int)> _cardClickCallback; // Card click callback
    std::function<void()> _stackClickCallback; // Stack click callback

    cocos2d::Node* _playFieldNode; // Play field node
    cocos2d::Node* _bottomNode; // Bottom card node
    cocos2d::Node* _stackNode; // Stack node

    cocos2d::Sprite* _stackBackground; // Stack background
    cocos2d::EventListenerTouchOneByOne* _stackTouchListener; // Stack touch listener};
};

#endif // GAME_VIEW_H