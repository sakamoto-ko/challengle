#pragma once
#include "PlayerBullet.h"
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
	std::vector<Model*> bulletModels_;

	//テクスチャハンドル
	std::vector<uint32_t> textures_;
	uint32_t tex_ = 0u;
	std::vector<uint32_t> bulletTextures_;
	uint32_t bulletTex_ = 0u;

	//ワールド変換データ
	//Base
	WorldTransform worldTransform_;
	WorldTransform worldTransformTemp_;

	//移動
	Vector3 velocity_ = {};

	//デスフラグ
	bool isDead_ = false;

	//Behavior
	enum class Behavior {
		kRoot,//通常状態
		kAttack,//攻撃中
		//kRight,
		//kLeft,
		kJump,//ジャンプ中
		kDive,//急降下中
	};
	Behavior behavior_ = Behavior::kRoot;

	//次のふるまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	bool isAttack = false;
	bool isJump = false;
	bool isJumpFirst = false;
	bool isDive = false;
	bool isMove = false;
	bool isHit = false;

	//ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	//重力加速度
	const float kGravityAcceleration = 0.1f;
	//加速度ベクトル
	Vector3 accelerationVector = {};

	int playerBulletNum_ = 0;
	int playerBulletMax_ = 1;

	int shotFinishTime = 0; 
	int moveFinishTime = 0;

	bool tempo_ = false;

	int hitCount = 0;

	//レティクル用
	std::unique_ptr<Sprite> spritePos_ = {};
	WorldTransform worldTransformReticle_;

	int hp_ = 100;

	int haveBullet = 0;

	float radius = 48.0f;

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

	//入力
	void Input();

	//移動
	void Move();
	//弾発射
	void Shot();
	//着地
	void Landing();
	void PlusGravity();

	//Behavior
	//通常行動
	void BehaviorSetRoot();
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();

	//攻撃行動
	void BehaviorSetAttack();
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	//右移動行動
	void BehaviorSetRight();/*
	void BehaviorRightInitialize();
	void BehaviorRightUpdate();*/

	//左行動行動
	void BehaviorSetLeft();/*
	void BehaviorLeftInitialize();
	void BehaviorLeftUpdate();*/

	//ジャンプ行動
	void BehaviorSetJump();
	void BehaviorJumpInitialize();
	void BehaviorJumpUpdate();

	//ダイブ行動
	void BehaviorSetDive();
	void BehaviorDiveInitialize();
	void BehaviorDiveUpdate();

	//セッター
	void SetParent(const WorldTransform* parent);
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetModels(const std::vector<Model*>& models) { models_ = models; }
	void SetTextures(const std::vector<uint32_t>& textures) { textures_ = textures; }
	void SetSpritePos(Vector2 spritePos) { spritePos_->SetPosition(spritePos); }

	//ゲッター
	Vector3 GetWorldPosition();
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	const ViewProjection* GetViewProjection() { return viewProjection_; }
	bool IsDead() { return isDead_; }
	const float& GetRadius() { return radius; }

	//衝突を検出したら呼び出されるコールバック関数	
	void OnCollision();

	//リセット
	void Reset();
	//セーブ
	void Save();

	//マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	void GetReticlePosition(const ViewProjection viewProjection);
};