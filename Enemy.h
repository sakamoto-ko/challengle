#pragma once
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

//GameSceneクラスの前方宣言
class GameScene;

class Enemy {
private:
	//ワールド変換データ
	WorldTransform worldTransform_;

	//モデル
	std::vector<Model*> models_;

	//テクスチャハンドル
	std::vector<uint32_t> textures_;
	uint32_t tex_ = 0u;

	//キーボード入力
	Input* input_ = nullptr;

	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//デスフラグ
	bool isDead_ = false;

	int32_t hp = 100;

public:
	Enemy();
	~Enemy();
	//初期化
	void Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures);
	//更新
	void Update(const ViewProjection viewProjection);
	//描画
	void Draw(const ViewProjection& viewProjection);
	void DrawUI();

	//セッター
	void SetParent(const WorldTransform* parent);
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetModels(const std::vector<Model*>& models) { models_ = models; }
	void SetTextures(const std::vector<uint32_t>& textures) { textures_ = textures; }

	//ゲッター
	Vector3 GetWorldPosition();
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	const ViewProjection* GetViewProjection() { return viewProjection_; }
	bool IsDead() { return isDead_; }
	int32_t GetPlayerHp() { return hp; }

	//当たり判定
	void OnCollision();
};
