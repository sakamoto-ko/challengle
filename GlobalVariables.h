#pragma once
#include "Audio.h"
#include "AxisIndicator.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <variant>
#include <map>
#include <json.hpp>
#include <fstream>
#include <iostream>

//using
using json = nlohmann::json;
using Item = std::variant<int32_t, float, Vector3>;
using Group = std::map<std::string, Item>;

class GlobalVariables
{
public:
	static GlobalVariables* GetInstance();

	//項目
	struct Item {
		//項目の値
		std::variant<int32_t, float, Vector3> value;
	};
	//グループ
	struct Group {
		std::map<std::string, Item> items;
	};
	//全データ
	std::map<std::string, Group> dates_;

	//グループの作成
	void CreateGroup(const std::string& groupName);

	//値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	//値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	//値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	//毎フレーム処理
	void Update();

	//ファイルに書き出し
	void SaveFile(const std::string& groupName);

	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";

	//ディレクトリの全ファイル読み込み
	void LoadFiles();

	//ディレクトリのファイル読み込み
	void LoadFile(const std::string& groupName);

	//項目の追加(int)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	//項目の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	//項目の追加(Vector3)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

	//値の取得
	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	//コピーインストラクタ = delete;
	//コピー代入演算子 = delete; 
};

