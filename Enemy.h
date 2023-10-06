#pragma once
#include "BaseCharactor.h"

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

	void SetViewPRojection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

};

