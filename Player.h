#pragma once
#include "PlayerBullet.h"
#include "Button.h"

#define PLAYER_BULLET_MAX 1

typedef enum {
	BOTTOM,
	MID,
	TOP,
}Lane;

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
	WorldTransform worldTransform_;

	//体力
	int32_t hp = 100;

	//デスフラグ
	bool isDead_ = false;

	//プレイヤーの座標レーン
	int lane_ = MID;

	//移動フラグ
	bool isMove_ = false;
	int moveCount_ = 0;

	//攻撃フラグ
	bool isAttack_ = false;
	int attackChargeCount_ = 0;
	int attackCoolDown_ = 0;

	int playerBulletNum_ = 0;

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

	//攻撃
	void Attack();
	//弾発射
	void Shot();

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
