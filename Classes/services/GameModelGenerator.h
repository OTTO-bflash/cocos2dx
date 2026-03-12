#ifndef _GAME_MODEL_GENERAOTR_H
#define _GAME_MODEL_GENERAOTR_H
#include "../models/GameModel.h"
#include "../configs/models/LevelConfig.h"

/*游戏模型生成服务
将静态配置转换为动态运行时数据
*/
class GameModelGenerator
{ 
public:
	GameModelGenerator();
	~GameModelGenerator() = default;
    /**
     * 根据关卡配置生成游戏模型
     * @param levelConfig 关卡配置
     * @return 生成的游戏模型
     */
    GameModel* generateGameModel(LevelConfig* levelConfig);

private:
    /**
     * 创建卡牌模型
     * @param cardConfig 卡牌配置
     * @param cardId 卡牌ID
     * @return 卡牌模型
     */
    CardModel* createCardModel(const LevelConfig::CardConfig& cardConfig, int cardID);

};

#endif
