#pragma once
#include "BaseCharactor.h"

//自機クラスの前方宣言
class Player;

//GameSceneクラスの前方宣言
class GameScene;

class Enemy : public BaseCharacter
{
private:
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	uint32_t kModelBody = 0;
	uint32_t kModelL_arm = 1;
	uint32_t kModelR_arm = 2;

	//円運動用
	//中心座標
	Vector3 center{};
	//角度
	Vector3 angle{};
	//半径の長さ
	float length;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	//ゲームシーン
	GameScene* gameScene_ = nullptr;

	bool isDead_ = false;
	bool isCheckDead_ = false;

public:
	Enemy();
	~Enemy();
	void Initialize(const std::vector<Model*>& models) override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	//移動初期化
	void InitializeMoveGimmick();

	//移動更新
	void UpdateMoveGimmick();

	void GetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	bool IsDead() const { return isDead_; }
	bool IsCheckDeaad() const { return isCheckDead_; }
	void SetReturnDead(bool flag) { isCheckDead_ = flag; }

	void Reset(const std::vector<Model*>& models);

	//中心座標を取得
	//<returns">中心座標</returns>
	Vector3 GetCenterPos();
	const Vector3 constGetCenterPosition();

	Vector3 GetCenterPosition() const override;

	//衝突コールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;
	//void OnCollision() override;

	//const Enemy GetEnemy() { return *this; }
};