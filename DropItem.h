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
class DropItem
{
public:
	DropItem();
	~DropItem();
	void Initialize(Vector3 pos, const std::vector<Model*>& models, const std::vector<uint32_t>& textures);
	void Update();
	void Draw(const ViewProjection& viewProjection);

	bool IsDead() const { return isDead_; }

	//衝突を検出したら呼び出されるコールバック関数	
	void OnCollision();

	/// <summary>
	/// モデル配列のセット
	/// </summary>
	/// <param name="models">モデル配列</param>
	void SetModels(const std::vector<Model*>& models) { models_ = models; }

	/// <summary>
	/// 画像配列のセット
	/// </summary>
	/// <param name="textures">画像配列</param>
	void SetTextures(const std::vector<uint32_t>& textures) { textures_ = textures; }

	Vector3 GetWorldPosition();
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

	//リセット
	void Reset(Vector3 pos);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	std::vector<Model*> models_;

	//カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	// テクスチャハンドル
	std::vector<uint32_t> textures_;
	uint32_t tex_ = 0u;

	//デスフラグ
	bool isDead_ = false;
};

