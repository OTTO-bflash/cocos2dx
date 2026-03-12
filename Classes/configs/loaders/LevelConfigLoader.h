#ifndef _LEVEL_CONFIG_LOADER_H
#define _LEVEL_CONFIG_LOADER_H

#include "../models/LevelConfig.h"
/*关卡配置加载器
负责从文件（JSON）加载关卡配置
*/
class LevelConfigLoader
{
public:
	LevelConfigLoader();
	~LevelConfigLoader() = default;
	/*
	加载指定关卡ID的配置
	@param levelID 关卡ID
	@return 关卡配置对象，加载失败返回nullptr
	*/
	LevelConfig* loadLevelConfig(int levelID);

private:
	//从JSON文件解析配置
	LevelConfig* parseLevelConfig(const std::string& filename);

};


#endif 

