#pragma once
#include "BaseCharactor.h"

//自機クラスの前方宣言
class Player;

//GameSceneクラスの前方宣言
class GameScene;

class Enemy : public BaseCharacter
{
private:
	WorldTransform worldTransformBase_;
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

	//自キャラ
	Player* player_ = nullptr;

	bool isAppear_ = false;

	bool isDead_ = false;

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
	void SetPlayer(Player* player) { player_ = player; }

	//中心座標を取得
	//<returns">中心座標</returns>
	Vector3 GetCenterPosition();
	const Vector3 constGetCenterPosition();

	bool IsDead() const { return isDead_; }

	//衝突を検出したら呼び出されるコールバック関数	
	void OnCollision();

	//const Enemy GetEnemy() { return *this; }
	Vector3 GetWorldPosition();

};