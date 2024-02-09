#include "GlobalVariables.h"

GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}
//グループの作成
void GlobalVariables::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトがなければ追加する
	dates_[groupName];
}

//値のセット(int)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int32_t value) {
	//グループの参照値を取得
	Group& group = dates_[groupName];
	//新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}
//値のセット(float)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	//グループの参照値を取得
	Group& group = dates_[groupName];
	//新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}
//値のセット(Vector3)
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3& value) {
	//グループの参照値を取得
	Group& group = dates_[groupName];
	//新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, bool value)
{
	//グループの参照値を取得
	Group& group = dates_[groupName];
	//新しい項目のデータを設定
	Item newItem{};
	newItem.value = value;
	//設定した項目をstd::mapに追加
	group.items[key] = newItem;
}

//毎フレーム処理
void GlobalVariables::Update() {
#ifdef DEBUG

	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) return;

	//各グループについて
	for (std::map<std::string, Group>::iterator itGroup = dates_.begin();
		itGroup != dates_.end(); ++itGroup) {
		//グループ名を取得
		const std::string& groupName = itGroup->first;
		//グループの参照を取得
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		//各項目について
		for (std::map<std::string, Item>::iterator itItem = group.items.begin();
			itItem != group.items.end(); ++itItem) {
			//項目名を取得
			const std::string& itemName = itItem->first;
			//項目の参照を取得
			Item& item = itItem->second;

			//int32_t型の値を保持していれば
			if (std::holds_alternative<int32_t>(item.value)) {
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}

			//float型の値を保持していれば
			else if (std::holds_alternative<float>(item.value)) {
				float* ptr = std::get_if<float>(&item.value);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0, 100);
			}

			//Vector3型の値を保持していれば
			else if (std::holds_alternative<Vector3>(item.value)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::SliderFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), -10.0f, 10.0f);
			}
			
			//bool型の値を保持していれば
			else if (std::holds_alternative<bool>(item.value)) {
				bool* ptr = std::get_if<bool>(&item.value);
				ImGui::Checkbox("isCollider", ptr);
			}
		}

		//改行
		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();

#endif // DEBUG

}

//ファイルに書き出し
void GlobalVariables::SaveFile(const std::string& groupName) {
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = dates_.find(groupName);
	//未登録チェック
	assert(itGroup != dates_.end());

	json root;
	root = json::object();

	//jsonオブジェクト登録
	root[groupName] = json::object();

	//各項目について
	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin();
		itItem != itGroup->second.items.end(); ++itItem) {
		//項目名を取得
		const std::string& itemName = itItem->first;
		//項目の参照を取得
		Item& item = itItem->second;

		//int32_t型の値を保持していれば
		if (std::holds_alternative<int32_t>(item.value)) {
			//int32_t型の値を登録
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}

		//float型の値を保持していれば
		else if (std::holds_alternative<float>(item.value)) {
			//float型の値を登録
			root[groupName][itemName] = std::get<float>(item.value);
		}

		//Vector3型の値を保持していれば
		else if (std::holds_alternative<Vector3>(item.value)) {
			//Vector3型のjson配列登録
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({ value.x,value.y,value.z });
		}

		//bool型の値を保持していれば
		else if (std::holds_alternative<bool>(item.value)) {
			//bool型の値を登録
			root[groupName][itemName] = std::get<bool>(item.value);
		}
	}

	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists("Resources/GlobalVariables/")) {
		std::filesystem::create_directory("Resources/GlobalVariables/");
	}

	//各項目のJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込みように開く
	ofs.open(filePath);
	//ファイルオープン失敗？
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}

//ディレクトリの全ファイル読み込み
void GlobalVariables::LoadFiles() {
	std::string kDirectoryPath_ = "Resources/GlobalVariables/";
	//ディレクトリがなければスキップする
	if (!std::filesystem::exists("Resources/GlobalVariables/")) {
		return;
	}
	std::filesystem::directory_iterator dir_it("Resources/GlobalVariables/");
	for (const std::filesystem::directory_entry& entry : dir_it) {
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();
		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}
		//ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

//ディレクトリのファイル読み込み
void GlobalVariables::LoadFile(const std::string& groupName) {
	//読み込むJSONファイルのフルパすを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";
	//読み込む用ファイルストリーム
	std::ifstream ifs;
	ifs.open(filePath);
	//ファイルオープン失敗？
	if (ifs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	json root;

	//json文字列からjsonのデータ構造に展開
	ifs >> root;
	ifs.close();

	//グループを検索
	json::iterator itGroup = root.find(groupName);

	//未登録チェック
	assert(itGroup != root.end());

	//各アイテムについて
	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		//アイテム名を取得
		const std::string& itemName = itItem.key();

		//int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			//int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}

		//float型の値を保持していれば
		else if (itItem->is_number_float()) {
			//float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}

		//要素数3の配列であれば
		else if (itItem->is_array() && itItem->size() == 3) {
			//float型のjson配列登録
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}

		//bool型の配列であれば
		else if (itItem->is_number_unsigned()) {
			//bool型のjson配列登録
			bool value = itItem->get<bool>();
			SetValue(groupName, itemName, value);
		}
	}
}

//項目の追加(int)
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {

	//グループを検索
	std::map<std::string, Group>::iterator itGroup = dates_.find(groupName);

	//項目が未登録なら
	if (itGroup != dates_.end()) {
		SetValue(groupName, key, value);
	}
}
//項目の追加(float)
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {

	//グループを検索
	std::map<std::string, Group>::iterator itGroup = dates_.find(groupName);

	//項目が未登録なら
	if (itGroup != dates_.end()) {
		SetValue(groupName, key, value);
	}
}
//項目の追加(Vector3)
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const Vector3& value) {

	//グループを検索
	std::map<std::string, Group>::iterator itGroup = dates_.find(groupName);

	//項目が未登録なら
	if (itGroup != dates_.end()) {
		SetValue(groupName, key, value);
	}
}

void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, bool value)
{
	//グループを検索
	std::map<std::string, Group>::iterator itGroup = dates_.find(groupName);

	//項目が未登録なら
	if (itGroup != dates_.end()) {
		SetValue(groupName, key, value);
	}
}

//値の取得
int32_t GlobalVariables::GetIntValue(const std::string& groupName, const std::string& key) const {

	//指定グループが存在している
	assert(&dates_.at(groupName));
	//グループの参照を取得
	const Group& group = dates_.at(groupName);
	//指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	//指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<int32_t>(item.value);

}
float GlobalVariables::GetFloatValue(const std::string& groupName, const std::string& key) const {

	// 指定グループが存在している
	assert(&dates_.at(groupName));
	// グループの参照を取得
	const Group& group = dates_.at(groupName);
	// 指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	// 指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<float>(item.value);
}
Vector3 GlobalVariables::GetVector3Value(const std::string& groupName, const std::string& key) const {

	// 指定グループが存在している
	assert(&dates_.at(groupName));
	// グループの参照を取得
	const Group& group = dates_.at(groupName);
	// 指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	// 指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<Vector3>(item.value);
}

bool GlobalVariables::GetBoolValue(const std::string& groupName, const std::string& key) const
{
	// 指定グループが存在している
	assert(&dates_.at(groupName));
	// グループの参照を取得
	const Group& group = dates_.at(groupName);
	// 指定グループに指定のキーが存在している
	assert(&group.items.at(key));
	// 指定グループから指定のキーの値を取得
	const Item& item = group.items.at(key);
	return std::get<bool>(item.value);
}
