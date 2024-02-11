#pragma once

#include "ImGuiManager.h"
#include "Input.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <cassert>
#include <list>
#include "PlayerBullet.h"

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection viewProjection);

	// 攻撃
	void Attack();

	Vector3 GetWorldPosition();

	// 衝突を検証したら呼び出される関数
	void OnCollision();
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	void SetParent(const WorldTransform* parent);

	WorldTransform& GetWorldMatrix() { return worldTransform_; }

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();


	// デストラクタ
	~Player();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	//頭のワールド変換データ
	WorldTransform worldTransformHead_;
	//手のワールド変換データ
	WorldTransform worldTransformL_;
	WorldTransform worldTransformR_;
	//足のワールド変換データ
	WorldTransform worldTransformLL_;
	WorldTransform worldTransformLR_;

	//付け根左
	WorldTransform worldTransformRootL_;

	//付け根右
	WorldTransform worldTransformRootR_;

	//武器
	WorldTransform worldTransformWeaPon_;





	// モデル
	Model* model_ = nullptr;
	//頭
	Model* modelHead_ = nullptr;
	//手
	Model* modelL_ = nullptr;
	Model* modelR_ = nullptr;

	//足
	Model* modelLL_ = nullptr;
	Model* modelLR_ = nullptr;

	//武器
	Model* modelWeapon_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// キーボード入力
	Input* input_ = nullptr;
	// ImGuiで値を入力する
	float inputFloat3[3] = {0, 0, 0};
	// 弾
	// PlayerBullet* bullet_ = nullptr;
	// 弾
	std::list<PlayerBullet*> bullets_;

	float floatingParameter_ = 0.0f;
	
	int count = 0;
};