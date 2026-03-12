#include "GameController.h"
#include "../configs/loaders/LevelConfigLoader.h"
#include "../services/GameModelGenerator.h"
#include "../utils/GameUtils.h"

USING_NS_CC;

GameController::GameController()
    : _gameModel(nullptr)
    , _gameView(nullptr)
    , _undoManager(nullptr)
    , _configLoader(nullptr)
    , _modelGenerator(nullptr) {
    _undoManager = new UndoManager();
    _configLoader = new LevelConfigLoader();
    _modelGenerator = new GameModelGenerator();
}
GameController::~GameController() {
    delete _gameModel;
    delete _gameView;
    delete _undoManager;
    delete _configLoader;
    delete _modelGenerator;
}
void GameController::startGame(int levelId) {
    CCLOG("GameController: Starting game with level %d", levelId);

    // 1. Load level config
    LevelConfig* levelConfig = _configLoader->loadLevelConfig(levelId);
    if (!levelConfig) {
        CCLOG("GameController: Failed to load level config for level %d", levelId);
        return;
    }

    // 2. Generate game model
    _gameModel = _modelGenerator->generateGameModel(levelConfig);
    delete levelConfig; // Clean up config object

    if (!_gameModel) {
        CCLOG("GameController: Failed to generate game model");
        return;
    }

    // 3. Create game view
    _gameView = GameView::create();
    if (!_gameView) {
        CCLOG("GameController: Failed to create game view");
        return;
    }

    // 4. Set callbacks
    _gameView->setCardClickCallback([this](int cardId) {
        this->handleCardClick(cardId);
        });

    _gameView->setStackClickCallback([this]() {
        this->handleDrawCard();
        });

    // 5. Update view display
    _gameView->updateView(_gameModel);

    // 6. Initialize undo manager
    _undoManager->init();

    CCLOG("GameController: Game started successfully");
}
bool GameController::handleCardClick(int cardId)
{
    if (!_gameModel || !_gameView) {
        return false;
    }

    CCLOG("GameController: Card clicked: %d", cardId);

    // 获取点击的卡牌和底牌
    CardModel* clickedCard = _gameModel->getCardById(cardId);
    CardModel* bottomCard = _gameModel->getBottomCard();

    if (!clickedCard || !bottomCard) {
        CCLOG("GameController: Invalid card or bottom card");
        return false;
    }

    // 检查卡牌是否匹配
    if (checkCardsMatch(clickedCard, bottomCard)) {
        CCLOG("GameController: Cards match! %d and %d",
            clickedCard->getFaceValue(), bottomCard->getFaceValue());

        // 记录操作前的位置
        cocos2d::Vec2 clickedCardPosition = clickedCard->getPosition();

        // 创建撤销记录
        UndoModel* undoModel = new UndoModel();
        undoModel->setOperationType(OperationType::CARD_MATCH);
        undoModel->setMatchedCardId(clickedCard->getCardId());
        undoModel->setPreviousBottomCardId(bottomCard->getCardId());
        undoModel->setMatchedCardPosition(clickedCardPosition);
        undoModel->setPreviousBottomPosition(cocos2d::Vec2::ZERO); // 底牌在底牌节点中的位置是(0,0)

        _undoManager->pushUndoRecord(undoModel);

        // 立即从主牌区移除卡牌
        _gameModel->removeCardFromPlayField(clickedCard->getCardId());

        // 播放移动动画到底牌位置
        // 关键修改：使用底牌节点的世界坐标作为目标位置
        auto bottomNodeWorldPos = _gameView->getBottomNode()->convertToWorldSpace(cocos2d::Vec2::ZERO);
        auto playFieldNode = _gameView->getPlayFieldNode();
        auto targetInPlayField = playFieldNode->convertToNodeSpace(bottomNodeWorldPos);

        _gameView->playCardMoveAnimation(
            clickedCard->getCardId(),
            targetInPlayField, // 转换后的正确位置
            0.5f,
            [this, clickedCard, bottomCard]() {
                // 动画完成后更新模型
                // 将当前底牌放回备用牌堆
                if (bottomCard) {
                    std::vector<CardModel*> stackCards = _gameModel->getStackCards();
                    stackCards.insert(stackCards.begin(), bottomCard);
                    _gameModel->setStackCards(stackCards);
                }

                // 设置新的底牌并更新位置
                clickedCard->setPosition(cocos2d::Vec2::ZERO); // 在底牌节点中的位置
                _gameModel->setBottomCard(clickedCard);

                // 更新视图
                this->_gameView->updateView(this->_gameModel);
                CCLOG("GameController: Card moved to bottom successfully");
            }
        );

        return true;
    }
    else {
        CCLOG("GameController: Cards don't match. %d and %d",
            clickedCard->getFaceValue(), bottomCard->getFaceValue());
    }

    return false;
}

void GameController::handleDrawCard()
{
    if (!_gameModel || !_gameView) {
        return;
    }

    // 检查备用牌堆是否有卡牌
    if (_gameModel->getStackCards().empty()) {
        CCLOG("GameController: No cards left in stack");
        return;
    }

    CCLOG("GameController: Drawing card from stack");

    // 记录当前底牌
    CardModel* previousBottomCard = _gameModel->getBottomCard();

    // 从备用牌堆抽牌
    CardModel* drawnCard = _gameModel->drawCardFromStack();
    if (drawnCard) {
        // 创建撤销记录
        UndoModel* undoModel = new UndoModel();
        undoModel->setOperationType(OperationType::DRAW_CARD);
        if (previousBottomCard) {
            undoModel->setPreviousBottomCardId(previousBottomCard->getCardId());
        }
        undoModel->setNewBottomCardId(drawnCard->getCardId());

        _undoManager->pushUndoRecord(undoModel);

        // 计算正确的目标位置
        auto bottomNodeWorldPos = _gameView->getBottomNode()->convertToWorldSpace(cocos2d::Vec2::ZERO);
        auto stackNode = _gameView->getStackNode();
        auto targetInStack = stackNode->convertToNodeSpace(bottomNodeWorldPos);

        // 播放移动动画
        _gameView->playCardMoveAnimation(
            drawnCard->getCardId(),
            targetInStack, // 转换后的正确位置
            0.5f,
            [this, drawnCard, previousBottomCard]() {
                // 动画完成后更新模型
                // 将当前底牌放回备用牌堆
                if (previousBottomCard) {
                    std::vector<CardModel*> stackCards = _gameModel->getStackCards();
                    stackCards.insert(stackCards.begin(), previousBottomCard);
                    _gameModel->setStackCards(stackCards);
                }

                // 设置新的底牌
                drawnCard->setPosition(cocos2d::Vec2::ZERO);
                this->_gameModel->setBottomCard(drawnCard);

                // 更新视图
                this->_gameView->updateView(this->_gameModel);
                CCLOG("GameController: Card drawn successfully");
            }
        );
    }
}

void GameController::handleUndo() {
    if (!_gameModel || !_gameView || !_undoManager->canUndo()) {
        CCLOG("GameController: Cannot undo - gameModel=%p, gameView=%p, canUndo=%d",
            _gameModel, _gameView, _undoManager ? _undoManager->canUndo() : false);
        return;
    }

    UndoModel* undoModel = _undoManager->popUndoRecord();
    if (!undoModel) {
        CCLOG("GameController: No undo record found");
        return;
    }

    CCLOG("GameController: Undoing operation type: %d", (int)undoModel->getOperationType());

    // Execute undo based on operation type
    switch (undoModel->getOperationType()) {
        //case OperationType::CARD_MATCH:
        //    CCLOG("Undo CARD_MATCH: matchedCardId=%d, previousBottomCardId=%d",
        //        undoModel->getMatchedCardId(), undoModel->getPreviousBottomCardId());

        //    // Undo card match: return card to original position
        //    {
        //        CardModel* matchedCard = _gameModel->getCardById(undoModel->getMatchedCardId());
        //        CardModel* previousBottomCard = _gameModel->getCardById(undoModel->getPreviousBottomCardId());

        //        if (matchedCard && previousBottomCard) {
        //            CCLOG("Found cards for undo: matchedCard=%p, previousBottomCard=%p", matchedCard, previousBottomCard);

        //            // Return matched card to play field with original position
        //            matchedCard->setPosition(undoModel->getMatchedCardPosition());
        //            matchedCard->setFlipped(true); // Play field cards are face up
        //            std::vector<CardModel*> playFieldCards = _gameModel->getPlayFieldCards();
        //            playFieldCards.push_back(matchedCard);
        //            _gameModel->setPlayFieldCards(playFieldCards);

        //            // Restore previous bottom card
        //            previousBottomCard->setPosition(undoModel->getPreviousBottomPosition());
        //            previousBottomCard->setFlipped(true); // Bottom card is face up
        //            _gameModel->setBottomCard(previousBottomCard);

        //            // Remove previous bottom card from stack if it's there
        //            std::vector<CardModel*> stackCards = _gameModel->getStackCards();
        //            if (!stackCards.empty()) {
        //                // Find and remove the previous bottom card from stack
        //                for (auto it = stackCards.begin(); it != stackCards.end(); ++it) {
        //                    if ((*it)->getCardId() == previousBottomCard->getCardId()) {
        //                        stackCards.erase(it);
        //                        break;
        //                    }
        //                }
        //                _gameModel->setStackCards(stackCards);
        //            }

        //            // Update view immediately
        //            _gameView->updateView(_gameModel);
        //            CCLOG("Undo CARD_MATCH completed");
        //        }
        //        else {
        //            CCLOG("Cards not found for undo CARD_MATCH");
        //        }
        //    }
        //    break;
    case OperationType::CARD_MATCH:
    {
        CardModel* matchedCard = _gameModel->getCardById(undoModel->getMatchedCardId());
        CardModel* previousBottomCard = _gameModel->getCardById(undoModel->getPreviousBottomCardId());

        if (matchedCard && previousBottomCard) {
            // 恢复匹配牌到主牌区原来的位置
            matchedCard->setPosition(undoModel->getMatchedCardPosition());
            std::vector<CardModel*> playFieldCards = _gameModel->getPlayFieldCards();
            playFieldCards.push_back(matchedCard);
            _gameModel->setPlayFieldCards(playFieldCards);

            // 恢复之前的底牌到底牌区中心位置
            previousBottomCard->setPosition(cocos2d::Vec2::ZERO); // 底牌区中心
            _gameModel->setBottomCard(previousBottomCard);

            // 从备用牌堆移除之前的底牌（如果存在）
            std::vector<CardModel*> stackCards = _gameModel->getStackCards();
            if (!stackCards.empty() && stackCards.front()->getCardId() == previousBottomCard->getCardId()) {
                stackCards.erase(stackCards.begin());
                _gameModel->setStackCards(stackCards);
            }

            // 立即更新视图
            _gameView->updateView(_gameModel);
        }
        break;
    }

    case OperationType::DRAW_CARD:
        CCLOG("Undo DRAW_CARD: newBottomCardId=%d, previousBottomCardId=%d",
            undoModel->getNewBottomCardId(), undoModel->getPreviousBottomCardId());

        // Undo draw card: return drawn card to stack, restore previous bottom card
        {
            CardModel* newBottomCard = _gameModel->getCardById(undoModel->getNewBottomCardId());
            CardModel* previousBottomCard = _gameModel->getCardById(undoModel->getPreviousBottomCardId());

            if (newBottomCard && previousBottomCard) {
                CCLOG("Found cards for undo DRAW_CARD: newBottomCard=%p, previousBottomCard=%p",
                    newBottomCard, previousBottomCard);

                // Return drawn card to stack top (face down)
                newBottomCard->setFlipped(false);
                std::vector<CardModel*> stackCards = _gameModel->getStackCards();
                stackCards.push_back(newBottomCard);
                _gameModel->setStackCards(stackCards);

                // Restore previous bottom card
                previousBottomCard->setPosition(undoModel->getPreviousBottomPosition());
                previousBottomCard->setFlipped(true);
                _gameModel->setBottomCard(previousBottomCard);

                // Update view
                _gameView->updateView(_gameModel);
                CCLOG("Undo DRAW_CARD completed");
            }
            else {
                CCLOG("Cards not found for undo DRAW_CARD");
            }
        }
        break;
    }

    delete undoModel;
}
bool GameController::checkCardsMatch(const CardModel* card1, const CardModel* card2) const {
    if (!card1 || !card2) {
        return false;
    }

    int value1 = card1->getFaceValue();
    int value2 = card2->getFaceValue();

    return GameUtils::isCardsMatch(value1, value2);
}