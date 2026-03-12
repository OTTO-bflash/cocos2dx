#include "CardView.h"
#include "cocos2d.h"

USING_NS_CC;

CardView* CardView::create()
{
    CardView* pRet = new CardView();
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

bool CardView::init()
{
    if (!Node::init()) {
        return false;
    }
    _cardSprite = nullptr;
    _faceSprite = nullptr;
    _suitSprite = nullptr;
    _cardId = 0;
    _flipped = false;

    return true;
}

void CardView::updateView(const CardModel* cardModel)
{
    if (!cardModel) {
        return;
    }

    CCLOG("CardView: Updating card %d with face %d and suit %d",
        cardModel->getCardId(),
        (int)cardModel->getFace(),
        (int)cardModel->getSuit());

    _cardId = cardModel->getCardId();
    _flipped = cardModel->isFlipped();

    // 设置位置
    setPosition(cardModel->getPosition());

    // 移除旧的显示内容
    removeAllChildren();

    if (_flipped) {
        // 显示卡牌正面 - 使用 card_general.png 作为背景
        _cardSprite = Sprite::create("res/card_general.png");
        if (!_cardSprite) {
            // 如果卡牌背景图片不存在，使用备用方案
            _cardSprite = Sprite::create();
            auto drawNode = DrawNode::create();
            Vec2 rectangle[4];
            rectangle[0] = Vec2(0, 0);
            rectangle[1] = Vec2(100, 0);
            rectangle[2] = Vec2(100, 150);
            rectangle[3] = Vec2(0, 150);
            drawNode->drawPolygon(rectangle, 4, Color4F(1, 1, 1, 1), 1, Color4F(0, 0, 0, 1));
            _cardSprite->addChild(drawNode);
            _cardSprite->setContentSize(Size(100, 150));
        }
        addChild(_cardSprite);

        // 显示数字和花色
        int faceValue = cardModel->getFaceValue();
        CardSuitType suit = cardModel->getSuit();

        // 根据点数和花色构建颜色
        std::string faceColor = (suit == CardSuitType::HEARTS || suit == CardSuitType::DIAMONDS) ? "red" : "black";

        // 1. 创建中间大数字精灵
        std::string bigFaceFilename;
        switch (faceValue) {
        case 1: bigFaceFilename = "res/number/big_" + faceColor + "_A.png"; break;
        case 11: bigFaceFilename = "res/number/big_" + faceColor + "_J.png"; break;
        case 12: bigFaceFilename = "res/number/big_" + faceColor + "_Q.png"; break;
        case 13: bigFaceFilename = "res/number/big_" + faceColor + "_K.png"; break;
        default: bigFaceFilename = "res/number/big_" + faceColor + "_" + std::to_string(faceValue) + ".png"; break;
        }

        _faceSprite = Sprite::create(bigFaceFilename);
        if (_faceSprite) {
            // 设置大数字在卡牌正中间
            _faceSprite->setPosition(Vec2(_cardSprite->getContentSize().width / 2, _cardSprite->getContentSize().height / 2));
            _cardSprite->addChild(_faceSprite);
        }
        else {
            // 如果数字图片不存在，使用文本标签
            auto label = Label::createWithSystemFont(
                StringUtils::format("%d", faceValue),
                "Arial", 36
            );
            label->setPosition(Vec2(_cardSprite->getContentSize().width / 2, _cardSprite->getContentSize().height / 2));
            label->setTextColor((faceColor == "red") ? Color4B::RED : Color4B::BLACK);
            _cardSprite->addChild(label);
        }

        // 2. 创建左上角小数字
        std::string smallFaceFilename;
        switch (faceValue) {
        case 1: smallFaceFilename = "res/number/small_" + faceColor + "_A.png"; break;
        case 11: smallFaceFilename = "res/number/small_" + faceColor + "_J.png"; break;
        case 12: smallFaceFilename = "res/number/small_" + faceColor + "_Q.png"; break;
        case 13: smallFaceFilename = "res/number/small_" + faceColor + "_K.png"; break;
        default: smallFaceFilename = "res/number/small_" + faceColor + "_" + std::to_string(faceValue) + ".png"; break;
        }

        auto smallFaceSprite = Sprite::create(smallFaceFilename);
        if (smallFaceSprite) {
            // 设置小数字在左上角
            smallFaceSprite->setPosition(Vec2(25, _cardSprite->getContentSize().height - 25));
            _cardSprite->addChild(smallFaceSprite);
        }
        else {
            // 如果数字图片不存在，使用文本标签
            auto smallLabel = Label::createWithSystemFont(
                StringUtils::format("%d", faceValue),
                "Arial", 18
            );
            smallLabel->setPosition(Vec2(25, _cardSprite->getContentSize().height - 25));
            smallLabel->setTextColor((faceColor == "red") ? Color4B::RED : Color4B::BLACK);
            _cardSprite->addChild(smallLabel);
        }

        // 3. 创建右上角花色 - 修正路径为 suits/
        std::string suitFilename;
        switch (suit) {
        case CardSuitType::CLUBS:
            suitFilename = "res/suits/club.png";
            break;
        case CardSuitType::DIAMONDS:
            suitFilename = "res/suits/diamond.png";
            break;
        case CardSuitType::HEARTS:
            suitFilename = "res/suits/heart.png";
            break;
        case CardSuitType::SPADES:
            suitFilename = "res/suits/spade.png";
            break;
        default:
            suitFilename = "";
            break;
        }

        if (!suitFilename.empty()) {
            _suitSprite = Sprite::create(suitFilename);
            if (_suitSprite) {
                // 设置花色在右上角
                _suitSprite->setPosition(Vec2(_cardSprite->getContentSize().width - 25, _cardSprite->getContentSize().height - 25));
                _cardSprite->addChild(_suitSprite);
            }
        }
        else {
            // 如果花色图片不存在，使用文本表示
            std::string suitText;
            switch (suit) {
            case CardSuitType::CLUBS: suitText = "C"; break;
            case CardSuitType::DIAMONDS: suitText = "D"; break;
            case CardSuitType::HEARTS: suitText = "H"; break;
            case CardSuitType::SPADES: suitText = "S"; break;
            default: suitText = "?"; break;
            }
            auto suitLabel = Label::createWithSystemFont(suitText, "Arial", 18);
            suitLabel->setPosition(Vec2(_cardSprite->getContentSize().width - 25, _cardSprite->getContentSize().height - 25));
            suitLabel->setTextColor((faceColor == "red") ? Color4B::RED : Color4B::BLACK);
            _cardSprite->addChild(suitLabel);
        }
    }
    else {
        // 显示卡牌背面 - 使用 card_general.png 作为背面
        _cardSprite = Sprite::create("res/card_general.png");
        if (!_cardSprite) {
            // 如果卡牌背景图片不存在，使用备用方案
            _cardSprite = Sprite::create();
            auto drawNode = DrawNode::create();
            Vec2 rectangle[4];
            rectangle[0] = Vec2(0, 0);
            rectangle[1] = Vec2(100, 0);
            rectangle[2] = Vec2(100, 150);
            rectangle[3] = Vec2(0, 150);
            drawNode->drawPolygon(rectangle, 4, Color4F(0.3f, 0.3f, 0.5f, 1.0f), 1, Color4F(0.0f, 0.0f, 0.0f, 1.0f));
            _cardSprite->addChild(drawNode);
            _cardSprite->setContentSize(Size(100, 150));
        }
        addChild(_cardSprite);
    }

    setupTouchHandling();
}

void CardView::setupTouchHandling()
{
    // Remove old touch listeners
    _eventDispatcher->removeEventListenersForTarget(this);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        if (!_cardSprite || !isVisible()) return false;

        Vec2 locationInNode = convertToNodeSpace(touch->getLocation());
        Size s = _cardSprite->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);

        if (rect.containsPoint(locationInNode)) {
            // Add visual feedback
            _cardSprite->setColor(Color3B(200, 200, 255));
            return true;
        }
        return false;
        };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        // Reset color
        if (_cardSprite) {
            _cardSprite->setColor(Color3B::WHITE);
        }

        if (_clickCallback) {
            CCLOG("CardView: Touch ended, calling click callback for card %d", _cardId);
            _clickCallback(_cardId);
        }
        };

    listener->onTouchCancelled = [this](Touch* touch, Event* event) {
        // Reset color if touch cancelled
        if (_cardSprite) {
            _cardSprite->setColor(Color3B::WHITE);
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void CardView::onTouched()
{
    if (_clickCallback) {
        _clickCallback(_cardId);
    }
}

void CardView::playMoveAnimation(const cocos2d::Vec2& targetPosition, float duration, const std::function<void()>& callback)
{
    auto move = MoveTo::create(duration, targetPosition);
    if (callback) {
        auto sequence = Sequence::create(move, CallFunc::create(callback), nullptr);
        runAction(sequence);
    }
    else {
        runAction(move);
    }
}

void CardView::playMatchAnimation(const std::function<void()>& callback)
{
    // 简单的缩放动画表示匹配
    auto scaleUp = ScaleTo::create(0.1f, 1.2f);
    auto scaleDown = ScaleTo::create(0.1f, 1.0f);
    if (callback) {
        auto sequence = Sequence::create(scaleUp, scaleDown, CallFunc::create(callback), nullptr);
        runAction(sequence);
    }
    else {
        auto sequence = Sequence::create(scaleUp, scaleDown, nullptr);
        runAction(sequence);
    }
}

void CardView::setTouchEnabled(bool enabled)
{
    // 通过设置触摸监听器的启用状态来实现
    _eventDispatcher->pauseEventListenersForTarget(this, !enabled);
    CCLOG("CardView: Touch %s for card %d", enabled ? "enabled" : "disabled", _cardId);
}