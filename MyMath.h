#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <cassert>
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

#define kWindowWidth 1280.0f
#define kWindowHeight 720.0f

#define PAI 3.14f

// 加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
// 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
// スカラー倍
Vector3 Multiply(const float scalar, const Vector3& v);
// 内積
float Dot(const Vector3& v1, const Vector3& v2);
// 長さ
float Length(const Vector3& v);
// 正規化
Vector3 Normalize(const Vector3& v);

// 加算
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
// 減算
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
// 積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// X軸回転
Matrix4x4 MakeRotateXMatrix(float radian);
// Y軸回転
Matrix4x4 MakeRotateYMatrix(float radian);
// Z軸回転
Matrix4x4 MakeRotateZMatrix(float radian);
// 回転行列
Matrix4x4 MakeRotateMatrix(const Vector3& radian);

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

//アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

//ベクトル変換
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);

//ベクトルと行列の積
Vector3 Multiply(Vector3 v, Matrix4x4 m);
// ベクトルと行列の加算
//Vector3 Add(Vector3 v, Matrix4x4 m);
//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

float Clamp(float x, float min, float max);

//ベクトルとベクトルの加算
//Vector3 Add(Vector3 v1, Vector3 v2);

void EaseInLeft(Vector2& pos, float& velocity);
void EaseInRight(Vector2& pos, float& velocity);
void EaseInUp(Vector2& pos, float& velocity);
void EaseInDown(Vector2& pos, float& velocity);

void EaseOutLeft(Vector2& pos, float& velocity);
void EaseOutRight(Vector2& pos, float& velocity);
void EaseOutUp(Vector2& pos, float& velocity);
void EaseOutDown(Vector2& pos, float& velocity);

void SetRandom();
//スクリーン座標変換
Vector3 ConvertScreenPosition(const ViewProjection viewProjection, Vector3 position);