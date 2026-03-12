#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "cocos2d.h"
#include "../controllers/GameController.h"

/*游戏场景
* 游戏控制器和视图
*/
class GameScene :public cocos2d::Scene
{
public:
	static GameScene* create();
	virtual bool init() override;

private:
	void createUI();
	void onUndoButtonClicked();

	GameController* _gameController;

};

#endif

