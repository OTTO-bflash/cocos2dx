#include "GameView.h"
#include "models/GameModel.h"
#include "models/CardModel.h"

USING_NS_CC;

GameView* GameView::create() {
    GameView* pRet = new GameView();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool GameView::init() {
    if (!Node::init()) {
        return false;
    }

    setupUI();
    return true;
}

void GameView::setupUI() {
    // 创建游戏区域、底部卡牌和堆栈节点
    _playFieldNode = Node::create();
    _bottomNode = Node::create();
    _stackNode = Node::create();

    // 添加区域背景颜色
    auto playFieldBg = LayerColor::create(Color4B(142, 94, 53, 255), 1080, 1500);
    _playFieldNode->addChild(playFieldBg, -1);

    auto handAreaBg = LayerColor::create(Color4B(105, 23, 108, 255), 1080, 580);
    handAreaBg->setPosition(0, 0);
    addChild(handAreaBg, -1);

    addChild(_playFieldNode);
    addChild(_bottomNode);
    addChild(_stackNode);

    // 设置区域位置
    _playFieldNode->setPosition(0, 580);        // 游戏区域在顶部
    _bottomNode->setPosition(540, 290);         // 底部卡牌居中
    _stackNode->setPosition(740, 290);          // 堆栈在底部卡牌右侧

    // 为底部和堆栈区域添加视觉背景
    auto bottomArea = Sprite::create("res/card_general.png");
    if (bottomArea) {
        bottomArea->setColor(Color3B(180, 180, 220));
        bottomArea->setOpacity(120);
        bottomArea->setScale(1.2f);
        _bottomNode->addChild(bottomArea, -1);
    }

    auto stackArea = Sprite::create("res/card_general.png");
    if (stackArea) {
        stackArea->setColor(Color3B(220, 180, 180));
        stackArea->setOpacity(120);
        _stackNode->addChild(stackArea, -1);
    }

    // 添加英文标签
    auto bottomLabel = Label::createWithSystemFont("Bottom Card", "Arial", 18);
    bottomLabel->setPosition(0, -70);
    bottomLabel->setTextColor(Color4B::WHITE);
    _bottomNode->addChild(bottomLabel);

    auto stackLabel = Label::createWithSystemFont("Stack\n(Tap to draw)", "Arial", 16);
    stackLabel->setPosition(0, -70);
    stackLabel->setTextColor(Color4B::YELLOW);
    stackLabel->setAlignment(TextHAlignment::CENTER);
    _stackNode->addChild(stackLabel);

    // 设置堆栈触摸处理
    setupStackTouch();
}

// 在 GameView::setupStackTouch() 中确保触摸监听器正确设置：
void GameView::setupStackTouch()
{
    // 移除旧的监听器
    if (_stackTouchListener) {
        _eventDispatcher->removeEventListener(_stackTouchListener);
        _stackTouchListener = nullptr;
    }

    // 创建触摸监听器
    _stackTouchListener = EventListenerTouchOneByOne::create();
    _stackTouchListener->setSwallowTouches(true);

    _stackTouchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        if (!_stackNode || !_stackNode->isVisible()) {
            return false;
        }

        // 将触摸坐标转换到_stackNode的坐标系
        Vec2 locationInNode = _stackNode->convertToNodeSpace(touch->getLocation());
        CCLOG("GameView: Stack touch at (%.1f, %.1f) in node space", locationInNode.x, locationInNode.y);

        // 使用更大的触摸区域确保检测
        Size areaSize(200, 200); // 扩大触摸区域
        Rect touchArea(-areaSize.width / 2, -areaSize.height / 2, areaSize.width, areaSize.height);

        if (touchArea.containsPoint(locationInNode)) {
            CCLOG("GameView: Stack touch began - area hit");
            return true;
        }
        return false;
        };

    _stackTouchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        CCLOG("GameView: Stack touch ended - calling callback");
        if (_stackClickCallback) {
            _stackClickCallback();
        }
        };

    // 使用场景图优先级确保正确注册
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_stackTouchListener, _stackNode);
}

void GameView::updateView(GameModel* gameModel) {
    if (!gameModel) return;

    CCLOG("GameView: Updating view");

    // 只清除卡牌视图，保留背景和标签
    std::vector<Node*> nodesToRemove;

    // 收集要移除的卡牌视图（避免在迭代时修改）
    for (auto child : _playFieldNode->getChildren()) {
        if (dynamic_cast<CardView*>(child)) {
            nodesToRemove.push_back(child);
        }
    }

    for (auto child : _bottomNode->getChildren()) {
        if (dynamic_cast<CardView*>(child)) {
            nodesToRemove.push_back(child);
        }
    }

    for (auto child : _stackNode->getChildren()) {
        if (dynamic_cast<CardView*>(child)) {
            nodesToRemove.push_back(child);
        }
    }

    // 移除收集的卡牌视图
    for (auto node : nodesToRemove) {
        node->removeFromParent();
    }

    // 清除卡牌视图映射
    _cardViews.clear();

    // 创建游戏区域卡牌
    const auto& playFieldCards = gameModel->getPlayFieldCards();
    for (auto cardModel : playFieldCards) {
        if (cardModel) {
            CardView* cardView = CardView::create();
            if (cardView) {
                cardView->updateView(cardModel);
                cardView->setClickCallback(_cardClickCallback);
                _cardViews[cardModel->getCardId()] = cardView;
                _playFieldNode->addChild(cardView);
            }
        }
    }

    // 创建底部卡牌
    CardModel* bottomCard = gameModel->getBottomCard();
    if (bottomCard) {
        CardView* cardView = CardView::create();
        if (cardView) {
            // 底部卡牌应该总是正面朝上
            bottomCard->setFlipped(true);
            cardView->updateView(bottomCard);
            cardView->setScale(1.05f);
            _cardViews[bottomCard->getCardId()] = cardView;
            _bottomNode->addChild(cardView);
        }
    }

    // 创建堆栈卡牌（只显示顶部卡牌）
    const auto& stackCards = gameModel->getStackCards();
    if (!stackCards.empty()) {
        CardModel* topStackCard = stackCards.back();
        if (topStackCard) {
            CardView* cardView = CardView::create();
            if (cardView) {
                // 堆栈卡牌显示为背面朝上
                topStackCard->setFlipped(false);
                cardView->updateView(topStackCard);
                _cardViews[topStackCard->getCardId()] = cardView;
                _stackNode->addChild(cardView);
            }
        }
    }
}

CardView* GameView::getCardView(int cardId) const {
    auto it = _cardViews.find(cardId);
    return (it != _cardViews.end()) ? it->second : nullptr;
}

void GameView::playMatchAnimation(int cardId, const std::function<void()>& callback) {
    CardView* cardView = getCardView(cardId);
    if (cardView) {
        cardView->playMatchAnimation(callback);
    }
    else if (callback) {
        callback();
    }
}

void GameView::playCardMoveAnimation(int cardId, const cocos2d::Vec2& targetPosition, float duration, const std::function<void()>& callback) {
    CardView* cardView = getCardView(cardId);
    if (cardView) {
        // 如果需要，将目标位置转换为世界坐标
        cardView->playMoveAnimation(targetPosition, duration, callback);
    }
    else if (callback) {
        callback();
    }
}