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

#define ENEMY_BULLET_MAX 1

//GameSceneクラスの前方宣言
class GameScene;

class Enemy {
private:
	//ワールド変換データ
	//Base
	WorldTransform worldTransform_;
	WorldTransform worldTransformTemp_;

	//モデル
	std::vector<Model*> models_;

	//テクスチャハンドル
	std::vector<uint32_t> textures_;
	uint32_t tex_ = 0u;

	//キーボード入力
	Input* input_ = nullptr;
	Button* button = Button::GetInstance();

	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//速度
	Vector3 velocity_ = {};

	//フラグ
	bool isDead_ = false;
	bool isHit = false;
	bool isJump = false;
	bool isAttack = false;

	//タイム
	int AttackWaitTime = 0;

	//Behavior
	enum class Behavior {
		kRoot,//通常
		kHit,//被弾
		kJump,//ジャンプ
		kAttack,//攻撃1
	};
	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//ジャンプ初速
	const float kJumpFirstSpeed = 0.4f;
	//重力加速度
	const float kGravityAcceleration = 0.05f;
	//加速度ベクトル
	Vector3 accelerationVector = {};

public:
	Enemy();
	~Enemy();
	//初期化
	void Initialize(Vector3 pos, const std::vector<Model*>& models, const std::vector<uint32_t>& textures);
	//更新
	void Update();
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
	Vector3 GetCenterPosition();

	//当たり判定
	void OnCollision();

	//Behavior
	//通常行動初期化
	void BehaviorSetRoot();
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();

	//攻撃行動
	void BehaviorSetAttack();
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	//ジャンプ行動
	void BehaviorSetJump();
	void BehaviorJumpInitialize();
	void BehaviorJumpUpdate();

	//被弾時行動
	void BehaviorSetHit();
	void BehaviorHitInitialize();
	void BehaviorHitUpdate();

	//リセット
	void Reset();
	//セーブ
	void Save();

	//移動
	void Move();
};
