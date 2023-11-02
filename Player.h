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
#include "Button.h"
#include "scene.h"

//GameSceneクラスの前方宣言
class GameScene;

class Player {
private:
	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//キーボード入力
	Input* input_ = nullptr;
	Button* button = Button::GetInstance();

	//モデル
	std::vector<Model*> models_;

	//テクスチャハンドル
	std::vector<uint32_t> textures_;
	uint32_t tex_ = 0u;

	//ワールド変換データ
	//Base
	WorldTransform worldTransform_;

	//移動
	Vector3 velocity_ = {};

	//デスフラグ
	bool isDead_ = false;

	//Behavior
	enum class Behavior {
		kRoot,//通常状態
		kAttack,//攻撃中
		kJump,//ジャンプ中
	};
	Behavior behavior_ = Behavior::kRoot;

	//次のふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	bool isJump = false;

	//ジャンプ初速
	const float kJumpFirstSpeed = 0.4f;

	//重力加速度
	const float kGravityAcceleration = 0.05f;

	//加速度ベクトル
	Vector3 accelerationVector = {};

public:
	Player();
	~Player();
	//初期化
	void Initialize(const std::vector<Model*>& models, const std::vector<uint32_t>& textures);
	//更新
	void Update(const ViewProjection viewProjection);
	//描画
	void Draw(const ViewProjection& viewProjection);
	void DrawUI();

	//移動
	void Move();

	//Behavior
	//通常行動初期化
	void BehaviorRootInitialize();
	//通常行動更新
	void BehaviorRootUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//攻撃行動更新
	void BehaviorAttackUpdate();

	//ジャンプ行動初期化
	void BehaviorJumpInitialize();
	//ジャンプ行動更新
	void BehaviorJumpUpdate();

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

};