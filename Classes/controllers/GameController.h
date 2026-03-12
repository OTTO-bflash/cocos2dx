#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../views/GameView.h"
#include "../managers/UndoManager.h"

// 前向声明
class LevelConfigLoader;
class GameModelGenerator;

/**
 * 主游戏控制器
 * 协调整个游戏流程以及模块之间的交互
 */
class GameController
{
public:
    GameController();
    ~GameController();

    /**
     * 开始游戏
     * @param levelId 关卡ID
     */
    void startGame(int levelId);

    /**
     * 处理卡牌点击事件
     * @param cardId 卡牌ID
     * @return 处理是否成功
     */
    bool handleCardClick(int cardId);

    /**
     * 处理抽卡操作
     */
    void handleDrawCard();

    /**
     * 处理撤销操作
     */
    void handleUndo();

    /**
     * 获取游戏视图（用于添加到场景中）
     */
    cocos2d::Node* getGameView() const { return _gameView; }

    /**
     * 获取游戏模型（用于调试）
     */
    GameModel* getGameModel() const { return _gameModel; }

private:
    /**
     * 初始化子控制器和视图
     */
    void initializeControllersAndViews();

    /**
     * 检查两张卡牌是否匹配
     * @param card1 卡牌1
     * @param card2 卡牌2
     * @return 是否匹配
     */
    bool checkCardsMatch(const CardModel* card1, const CardModel* card2) const;

    GameModel* _gameModel; // 游戏数据模型
    GameView* _gameView; // 游戏视图
    UndoManager* _undoManager; // 撤销管理器
    LevelConfigLoader* _configLoader; // 配置加载器
    GameModelGenerator* _modelGenerator; // 模型生成器
};

#endif // GAME_CONTROLLER_H