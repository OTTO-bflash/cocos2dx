#include "GameModelGenerator.h"
#include "../utils/GameUtils.h"
USING_NS_CC;


GameModelGenerator::GameModelGenerator()
{

}

GameModel* GameModelGenerator::generateGameModel(LevelConfig* levelConfig)
{
    if (!levelConfig) {
        return nullptr;
    }

    GameModel* gameModel = new GameModel();

    // 生成主牌区的卡牌
    std::vector<CardModel*> playFieldCards;
    const std::vector<LevelConfig::CardConfig>& playFieldConfigs = levelConfig->getPlayFieldCards();
    for (std::vector<LevelConfig::CardConfig>::const_iterator it = playFieldConfigs.begin(); it != playFieldConfigs.end(); ++it) {
        int cardId = GameUtils::generateCardId();
        CardModel* cardModel = createCardModel(*it, cardId);
        if (cardModel) {
            playFieldCards.push_back(cardModel);
        }
    }
    gameModel->setPlayFieldCards(playFieldCards);

    // 生成备用牌区的卡牌
    std::vector<CardModel*> stackCards;
    const std::vector<LevelConfig::CardConfig>& stackConfigs = levelConfig->getStackCards();
    for (std::vector<LevelConfig::CardConfig>::const_iterator it = stackConfigs.begin(); it != stackConfigs.end(); ++it) {
        int cardId = GameUtils::generateCardId();
        CardModel* cardModel = createCardModel(*it, cardId);
        if (cardModel) {
            stackCards.push_back(cardModel);
        }
    }
    gameModel->setStackCards(stackCards);

    // 设置初始底牌（从备用牌堆抽取第一张）
    if (!stackCards.empty()) {
        // 备份备用牌堆（移除第一张）
        std::vector<CardModel*> newStackCards = stackCards;
        CardModel* initialBottomCard = newStackCards.back();
        newStackCards.pop_back(); // 从备用牌堆移除

        // 设置底牌和更新备用牌堆
        gameModel->setBottomCard(initialBottomCard);
        gameModel->setStackCards(newStackCards);
    }

    return gameModel;

}

CardModel* GameModelGenerator::createCardModel(const LevelConfig::CardConfig& cardConfig, int cardId)
{
    CardModel* cardModel = new CardModel();
    cardModel->setCardId(cardId);
    cardModel->setFace(cardConfig.face);
    cardModel->setSuit(cardConfig.suit);
    cardModel->setPosition(cardConfig.position);
    cardModel->setFlipped(true); // 默认翻开状态

    return cardModel;
}