#include "GameScene.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <memory>
#include <list>

#include "MyMath.h"

void GameScene::CheckAllCollisions() {
	//判定対象AとBの座標
	Vector3 posA, posB;
#pragma region 自弾と敵キャラの当たり判定
	//自弾の座標
	for (PlayerBullet* playerBullet : playerBullets_) {
		posA = playerBullet->GetWorldPosition();

		//自弾と敵キャラ全ての当たり判定
		//敵キャラの座標
		for (std::unique_ptr<Enemy>& enemy : enemies_) {
			posB = enemy->GetWorldPosition();

			//座標AとBの距離を求める
			//球と球の交差判定
			if (posA.z + 0.1f >= posB.z && posA.z <= posB.z + 0.2f) {
				if (posA.y + 0.1f >= posB.y && posA.y <= posB.y + 0.2f) {
					if (posA.x + 0.1f >= posB.x && posA.x <= posB.x + 0.2f) {
						//自弾の衝突時コールバックを呼び出す
						playerBullet->OnCollision();
						//敵キャラの衝突時コールバックを呼び出す
						enemy->OnCollision();
					}
				}
			}
		}
	}
#pragma endregion

#pragma region 自キャラと敵キャラの当たり判定
	//自キャラの座標
	posA = player_->GetWorldPosition();

	//敵キャラの座標
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		posB = enemy->GetWorldPosition();

		//座標AとBの距離を求める
		//球と球の交差判定
		if (posA.z + 0.2f >= posB.z && posA.z <= posB.z + 0.2f) {
			if (posA.y + 0.2f >= posB.y && posA.y <= posB.y + 0.2f) {
				if (posA.x + 0.2f >= posB.x && posA.x <= posB.x + 0.2f) {
					//自弾の衝突時コールバックを呼び出す
					player_->OnCollision();
					//敵キャラの衝突時コールバックを呼び出す
					enemy->OnCollision();
				}
			}
		}
	}
#pragma endregion

#pragma region 自キャラとアイテムの当たり判定
	//自キャラの座標
	posA = player_->GetWorldPosition();

	//敵キャラの座標
	for (std::unique_ptr<DropItem>& item : items_) {
		posB = item->GetWorldPosition();

		//座標AとBの距離を求める
		//球と球の交差判定
		if (posA.z + 0.2f >= posB.z && posA.z <= posB.z + 0.2f) {
			if (posA.y + 0.2f >= posB.y && posA.y <= posB.y + 0.2f) {
				if (posA.x + 0.2f >= posB.x && posA.x <= posB.x + 0.2f) {
					//アイテムの衝突時コールバックを呼び出す
					item->OnCollision();
				}
			}
		}
	}
#pragma endregion
}


void GameScene::AllReset()
{
	player_->Reset();

	for (PlayerBullet* playerBullet : playerBullets_) {
		playerBullet->Reset();
	}
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	//bullet_の開放
	for (PlayerBullet* playerBullet : playerBullets_) {
		delete playerBullet;
	}
}

//自弾を追加する
void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	//リストに登録する
	playerBullets_.push_back(playerBullet);
}

//void GameScene::TitleSceneInitialize() {
//	pressButtonWaitCount = 0;
//}
//void GameScene::TitleSceneUpdate() {
//	nowScene = TITLE;
//	if (++pressButtonWaitCount >= 20) {
//		if (button->isTrigger() || input_->TriggerKey(DIK_SPACE)) {
//			if (!transition_->GetIsChangeScene()) {
//				transition_->SetIsChangeScene(true);
//				transition_->Reset();
//				nextScene_ = GAME;
//				pressButtonWaitCount = 0;
//			}
//		}
//	}
//}
//void GameScene::TitleSceneDraw() {}
//void GameScene::TitleSceneDrawUI() {
//	backgroundSprites_[0]->Draw();
//	setumeiSprites_[0]->Draw();
//}

void GameScene::GameSceneInitialize() {
	//カメラの初期化
	camera_ = std::make_unique<RailCamera>();
	camera_->Initialize(worldTransform_, { 0.0f,0.0f,0.0f });

	//スカイドームの初期化
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelSkydome_.get());

	//グラウンドの初期化
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(modelGround_.get());

	//プレイヤーの初期化
	std::vector<uint32_t> playerTextures{playerTex_, whiteTex_, enemyTex_, playerBulletTex_};
	std::vector<Model*> playerModels{playerModel_.get(), playerModel_.get()};
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels, playerTextures);
	player_->SetGameScene(this);
	player_->SetViewProjection(&viewProjection_);

	mapPlayerRightTopX = ((int)player_->GetWorldPosition().x + (int)player_->GetRadius()) / BLOCK_SIZE;
	mapPlayerRightTopY = ((int)player_->GetWorldPosition().y) / BLOCK_SIZE;
	mapPlayerRightBottomX = ((int)player_->GetWorldPosition().x + (int)player_->GetRadius()) / BLOCK_SIZE;
	mapPlayerRightBottomY = ((int)player_->GetWorldPosition().y) / BLOCK_SIZE;
	mapPlayerLeftTopX = ((int)player_->GetWorldPosition().x) / BLOCK_SIZE;
	mapPlayerLeftTopY = ((int)player_->GetWorldPosition().y - (int)player_->GetRadius()) / BLOCK_SIZE;
	mapPlayerLeftBottomX = ((int)player_->GetWorldPosition().x) / BLOCK_SIZE;
	mapPlayerLeftBottomY = ((int)player_->GetWorldPosition().y - (int)player_->GetRadius()) / BLOCK_SIZE;

	for (auto& sprite1 : mapPos1_) {
		sprite1 = std::make_unique<Sprite>();
		sprite1->Initialize();
		sprite1->SetAnchorPoint({ 0.0f, 0.0f });
	}
	for (auto& sprite2 : mapPos2_) {
		sprite2 = std::make_unique<Sprite>();
		sprite2->Initialize();
		sprite2->SetAnchorPoint({ 0.0f, 0.0f });
	}
	for (auto& sprite3 : mapPos3_) {
		sprite3 = std::make_unique<Sprite>();
		sprite3->Initialize();
		sprite3->SetAnchorPoint({ 0.0f, 0.0f });
	}

	//エネミーの初期化
	LoadEnemyPopData();
	UpdateEnemyPopCommands();

	//アイテムの初期化
	LoadItemPopData();
	UpdateItemPopCommands();

	//ロックオン
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//テンポの初期化
	tempo_ = std::make_unique<tempo>();
	tempo_->Initialize();
}
void GameScene::GameSceneUpdate() {
	//スカイドームの更新
	skydome_->Update();

	//グラウンドの更新
	ground_->Update();

	//テンポの更新
	tempo_->Update();

	//プレイヤーの更新
	if (tempo_->GetTempo()) {
		player_->Input();
	}
	player_->Update(viewProjection_);

	//自弾更新
	for (PlayerBullet* playerBullet : playerBullets_) {
		playerBullet->Update();
	}
	//デスフラグの立った弾を削除
	playerBullets_.remove_if([](PlayerBullet* playerBullet) {
		if (playerBullet->IsDead()) {
			delete playerBullet;
			return true;
		}
		return false;
		}
	);

	//敵の発生と更新
	UpdateEnemyPopCommands();
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Update();
		if (tempo_->GetTempo()) {
			enemy->BehaviorSetJump();
		}
	}
	//デスフラグの立った敵を削除
	enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		if (enemy->IsDead()) {
			delete enemy.release();
			return true;
		}
		return false;
		}
	);

	//アイテム
	UpdateItemPopCommands();
	for (std::unique_ptr<DropItem>& item : items_) {
		item->Update();
	}

	//ロックオン
	lockOn_->Update(enemies_, viewProjection_);
	player_->SetSpritePos(lockOn_->GetSpritePosition());

	//ビュープロジェクション行列の更新と転送
	camera_->Update();
	viewProjection_.matView = camera_->GetViewProjection().matView;
	viewProjection_.matProjection = camera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	CheckAllCollisions();
}
void GameScene::GameSceneDraw() {
	//スカイドームの描画
	//skydome_->Draw(viewProjection_);

	//グラウンドの更新
	//ground_->Draw(viewProjection_);

	//プレイヤーの描画
	player_->Draw(viewProjection_);

	//エネミーの描画
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}

	//アイテムの描画
	for (std::unique_ptr<DropItem>& item : items_) {
		item->Draw(viewProjection_);
	}

	//自弾描画
	for (PlayerBullet* playerBullet : playerBullets_) {
		playerBullet->Draw(viewProjection_);
	}

}
void GameScene::GameSceneDrawUI() {
	player_->DrawUI();

	//マップ
	/*for (int y = 0; y < 25; y++) {
		for (int x = 0; x < 80; x++) {
			if (map[y][x] == BLOCK) {
				assert(m < BLOCK_MAX);
				auto& sprite1 = mapPos1_[m];
				sprite1->SetTextureHandle(mapTextures_[0]);
				sprite1->SetPosition({ (float)x * 32, (float)y * 32 });
				sprite1->SetRotation(0.0f);
				sprite1->SetSize({ 32.0f, 32.0f });
				sprite1->SetTextureRect({ (float)0, (float)0 }, { (float)48, (float)48 });
				sprite1->Draw();
				m++;
			}
			else if (map[y][x] == UPONLY) {
				assert(l < BLOCK_MAX);
				auto& sprite2 = mapPos2_[l];
				sprite2->SetTextureHandle(mapTextures_[1]);
				sprite2->SetPosition({ (float)x * 32, (float)y * 32 });
				sprite2->SetRotation(0.0f);
				sprite2->SetSize({ 32.0f, 32.0f });
				sprite2->SetTextureRect({ (float)0, (float)0 }, { (float)48, (float)48 });
				sprite2->Draw();
				l++;
			}
			else if (map[y][x] == GOAL) {
				assert(n < BLOCK_MAX);
				auto& sprite3 = mapPos3_[n];
				sprite3->SetTextureHandle(mapTextures_[2]);
				sprite3->SetPosition({ (float)x * 32, (float)y * 32 });
				sprite3->SetRotation(0.0f);
				sprite3->SetSize({ 32.0f, 32.0f });
				sprite3->SetTextureRect({ (float)0, (float)0 }, { (float)48, (float)48 });
				sprite3->Draw();
				n++;
			}
		}
	}*/
}

//void GameScene::ClearSceneInitialize() {
//	pressButtonWaitCount = 0;
//}
//void GameScene::ClearSceneUpdate() {
//	nowScene = CLEAR;
//
//	if (++pressButtonWaitCount >= 20) {
//		if (button->isTrigger() || input_->TriggerKey(DIK_SPACE)) {
//			if (!transition_->GetIsChangeScene()) {
//				transition_->SetIsChangeScene(true);
//				transition_->Reset();
//				nextScene_ = TITLE;
//				pressButtonWaitCount = 0;
//			}
//		}
//	}
//}
//void GameScene::ClearSceneDraw() {}
//void GameScene::ClearSceneDrawUI() {
//	backgroundSprites_[1]->Draw();
//	setumeiSprites_[0]->Draw();
//}

//void GameScene::GameoverSceneInitialize() {
//	pressButtonWaitCount = 0;
//}
//void GameScene::GameoverSceneUpdate() {
//	nowScene = GAMEOVER;
//
//	if (++pressButtonWaitCount >= 20) {
//		if (button->isTrigger() || input_->TriggerKey(DIK_SPACE)) {
//			if (!transition_->GetIsChangeScene()) {
//				transition_->SetIsChangeScene(true);
//				transition_->Reset();
//				nextScene_ = TITLE;
//				pressButtonWaitCount = 0;
//			}
//		}
//	}
//}
//void GameScene::GameoverSceneDraw() {}
//void GameScene::GameoverSceneDrawUI() {
//	backgroundSprites_[2]->Draw();
//	setumeiSprites_[0]->Draw();
//}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// キー入力 初期化
	button->Initialize();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化
	viewProjection_.farZ = 10000.0f;
	viewProjection_.Initialize();

	//テクスチャ
	playerTex_ = TextureManager::Load("blue.png");
	playerBulletTex_ = TextureManager::Load("lockOn.png");
	enemyTex_ = TextureManager::Load("red.png");
	itemTex_ = TextureManager::Load("white1x1.png");
	whiteTex_ = TextureManager::Load("white1x1.png");
	mapTextures_[0] = TextureManager::Load("1.png");
	mapTextures_[1] = TextureManager::Load("2.png");
	mapTextures_[2] = TextureManager::Load("3.png");

	//スプライト
	/*for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 1; j++) {
			mapPos_[i][j].reset(Sprite::Create(mapTextures_[i], { 0.0f, 0.0f }));
			mapPos_[i][j]->SetTextureHandle({ mapTextures_[i] });
			mapPos_[i][j]->SetPosition({ 0.0f, 0.0f });
			mapPos_[i][j]->SetSize({ 32.0f, 32.0f });
			mapPos_[i][j]->SetTextureRect({ 0.0f,0.0f, }, { 48.0f, 48.0f });
		}
	}*/
	m = 0;
	l = 0;
	n = 0;

	//3Dモデル
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	modelGround_.reset(Model::CreateFromOBJ("ground", true));
	playerModel_.reset(Model::Create());
	enemyModel_.reset(Model::Create());
	itemModel_.reset(Model::Create());

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向が参照するビュープロジェクションを指定する(アドレスなし)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	GameSceneInitialize();
}

void GameScene::Update() {

	//キー入力の更新
	button->Update();
	GameSceneUpdate();

#ifdef _DEBUG

	ImGui::Begin("window");
	//ImGui::Text("scene %d", sceneCount_);
	ImGui::End();

#endif // _DEBUG
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	GameSceneDraw();

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	GameSceneDrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

//敵発生データの読み込み
void GameScene::LoadEnemyPopData() {
	//ファイルを開く
	std::ifstream file;
	file.open("./Resources/enemyPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

//敵発生コマンドの更新
void GameScene::UpdateEnemyPopCommands() {
	//待機処理
	if (isWait) {
		waitTimer--;
		if (waitTimer <= 0) {
			//待機完了
			isWait = false;
		}
		return;
	}

	//1行文の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		//1行文分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');
		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}
		//POPコマンド
		if (word.find("POP") == 0) {
			//X座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//Y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//Z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//敵を発生させる
			EnemyPop(Vector3(x, y, z));
		}
		//WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			isWait = true;
			waitTimer = waitTime;

			//コマンドループを抜ける
			break;//重要
		}
	}
}

//敵発生関数
void GameScene::EnemyPop(Vector3 pos) {
	//敵の生成
	std::vector<uint32_t> enemyTextures{enemyTex_, whiteTex_};
	std::vector<Model*> enemyModels{enemyModel_.get()};
	std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();

	//初期化
	newEnemy->Initialize(pos, enemyModels, enemyTextures);
	//リストに敵を登録する, std::moveでユニークポインタの所有権移動
	enemies_.push_back(std::move(newEnemy));

	//イテレータ
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		//各セッターに値を代入
		enemy->SetViewProjection(&viewProjection_);
		enemy->SetGameScene(this);
		//更新
		enemy->Update();
	}
}

void GameScene::LoadItemPopData() {
	//ファイルを開く
	std::ifstream file;
	file.open("./Resources/itemPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	itemPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GameScene::UpdateItemPopCommands() {
	//待機処理
	if (isItemWait) {
		waitItemTimer--;
		if (waitItemTimer <= 0) {
			//待機完了
			isItemWait = false;
		}
		return;
	}

	//1行文の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(itemPopCommands, line)) {
		//1行文分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');
		//"//"から始まる行はコメント
		if (word.find("//") == 0) {
			//コメント行を飛ばす
			continue;
		}
		//POPコマンド
		if (word.find("POP") == 0) {
			//X座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			//Y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//Z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//敵を発生させる
			ItemPop(Vector3(x, y, z));
		}
		//WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機開始
			isItemWait = true;
			waitItemTimer = waitTime;

			//コマンドループを抜ける
			break;//重要
		}
	}
}

void GameScene::ItemPop(Vector3 pos) {
	//敵の生成
	std::vector<uint32_t> itemTextures{itemTex_};
	std::vector<Model*> itemModels{itemModel_.get()};
	std::unique_ptr<DropItem> newItem = std::make_unique<DropItem>();

	//初期化
	newItem->Initialize(pos, itemModels, itemTextures);
	//リストに敵を登録する, std::moveでユニークポインタの所有権移動
	items_.push_back(std::move(newItem));

	//イテレータ
	for (std::unique_ptr<DropItem>& item : items_) {
		//各セッターに値を代入
		item->SetViewProjection(&viewProjection_);
		//更新
		item->Update();
	}
}