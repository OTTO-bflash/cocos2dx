#include "GameScene.h"
#include "../controllers/GameController.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace ui;

GameScene* GameScene::create()
{
	GameScene* pRet = new GameScene();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool GameScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	//添加背景
	auto background = LayerColor::create(Color4B(0, 100, 0, 255));
	addChild(background, -1);
	//创建控制器并开始游戏（使用关卡1）
	_gameController = new GameController();
	_gameController->startGame(1);

	//将游戏视图放入场景
	if (_gameController->getGameView())
	{
		addChild(_gameController->getGameView());
	}
	//创建UI
	createUI();

	return true;
}

void GameScene::createUI()
{
	//创建撤销按钮
	auto undoButton = Button::create();
	undoButton->setTitleText("Undo");
	undoButton->setTitleFontSize(38);
	undoButton->setTitleColor(Color3B::WHITE);
	undoButton->setColor(Color3B(255,200,0));
	undoButton->setContentSize(Size(200, 80));
	undoButton->setOpacity(200);
	undoButton->setPosition(Vec2(180, 300));

	undoButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type) {
		auto button = static_cast<ui::Button*>(sender);
		if (type == ui::Widget::TouchEventType::BEGAN) {
			button->setScale(0.95f); // 按下时稍微缩小
			button->setColor(Color3B(255, 180, 0));
		}
		else if (type == ui::Widget::TouchEventType::ENDED) {
			button->setScale(1.0f);
			button->setColor(Color3B(255, 200, 0));
			this->onUndoButtonClicked();
		}
		else if (type == ui::Widget::TouchEventType::CANCELED) {
			button->setScale(1.0f);
			button->setColor(Color3B(255, 200, 0));
		}
		});
	addChild(undoButton);

	// 创建游戏状态标签
	auto statusLabel = Label::createWithSystemFont("Card Game", "Arial", 32);
	statusLabel->setPosition(Vec2(540, 1900));
	statusLabel->setTextColor(Color4B::WHITE);
	addChild(statusLabel);

	// 创建操作提示标签
	auto hintLabel = Label::createWithSystemFont("Tap card to match | Tap stack to draw", "Arial", 20);
	hintLabel->setPosition(Vec2(540, 1850));
	hintLabel->setTextColor(Color4B::YELLOW);
	addChild(hintLabel);
}

void GameScene::onUndoButtonClicked() {
	if (_gameController) {
		_gameController->handleUndo();
	}
}
