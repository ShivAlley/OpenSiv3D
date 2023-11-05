#include "Game.h"

//#include "Enemy.h"
//#include "Item.h"
#include "Enemy.h"
#include "GVars.h"
#include "Item.h"
#include "Player.h"
#include <Siv3D/Windows/Windows.hpp>

inline void TickWalls() {
	auto playerPos = Player::GetInstance().GetPos();

	double wallLength = 10;
	auto lastwall = Global::walls.rbegin()->z;
	auto lastwall2 = (Global::walls.rbegin() + 1)->z;
	Vec3 leftWallPos = {
		CharaParam::initialPos.x - 50,
		CharaParam::initialPos.y,
		playerPos.z + 100
	};

	Vec3 rightWallPos = {
		CharaParam::initialPos.x + 50,
		CharaParam::initialPos.y,
		playerPos.z + 100 };
	bool isCreateForward = false;

	if (lastwall < playerPos.z + 100 && lastwall2 < playerPos.z + 100
		and rightWallPos.z - lastwall > wallLength
		and leftWallPos.z - lastwall2 > wallLength)
	{
		isCreateForward = true;
	}

	if (isCreateForward)
	{
		OrientedBox leftWall = OrientedBox(Arg::bottomCenter(leftWallPos), Vec3{ 1, 10, 10 });
		OrientedBox rightWall = OrientedBox(Arg::bottomCenter(rightWallPos), Vec3{ 1, 10, 10 });
		Global::walls.push_back(leftWall);
		Global::walls.push_back(rightWall);
	}
	std::erase_if(Global::walls, [&](OrientedBox& wall)
	{
		double distance = playerPos.z - wall.z;
		return distance > 100;
	});
}

inline void DrawWalls()
{
	for (const auto& wall : Global::walls)
	{
		wall.draw(Linear::Palette::Maroon);
	}
}

void Game::update()
{
	ClearPrint();
	if (KeyEscape.down())
	{
		isPaused = not isPaused;
		if (isPaused)
		{
			Cursor::RequestStyle(CursorStyle::Default);

			while (ShowCursor(TRUE) < 0);
		}
		else
		{
			Cursor::RequestStyle(CursorStyle::Hidden);

			while (ShowCursor(FALSE) >= 0);
		}
	}
	m_itemsDraw();
	m_printDebugInfo();
	if (isPaused)
	{
		m_pause();
		return;
	}

	Global::CalcEscalate();
	const bool isOpening = Player::GetInstance().GetItemCount() < 5;
	if (isOpening)
	{
		m_initialSpawn();
	}
	if (m_spawnTimer.s() > 1 and not isOpening)
	{
		for (int i = 0; i < Global::CalcSpawnCount(); ++i)
		{
			int whatSpawn = Global::BiasedRandom(CharaParam::enemyWeights);
			switch (whatSpawn)
			{
			case 0:
				Global::enemies.push_back(std::make_unique<StraightMoveEnemy>());
				break;
			case 1:
				Global::enemies.push_back(std::make_unique<SlitheringEnemy>());
				break;
			case 2:
				Global::enemies.push_back(std::make_unique<DistanceAttackEnemy>());
				break;
			case 3:
				assert(false, "dont reach");
				Global::enemies.push_back(std::make_unique<FlyingObject>());
				break;
			case 4:
				Global::enemies.push_back(std::make_unique<ChargingEnemy>());
				break;
			default:
				break;
			}
		}
		m_spawnTimer.restart();
	}
	Global::deltaTime = Scene::DeltaTime();

	if (not Global::isGameOver)
	{
		Player::GetInstance().Input();
		Player::GetInstance().Update();
		for (auto& enemy : Global::enemies)
		{
			enemy->Update();
		}
		for (auto& item : Global::items)
		{
			item->Tick();
			item->Update();
		}
	}
	std::_Erase_remove_if(Global::enemies, [&](auto& enemy)
	{
		return enemy->GetState() == State::Dead;
	});
	//TickWalls();
	//Print << Global::walls.size();

	static MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	// 3D 描画
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		Global::gameFloor.draw(Linear::Palette::Dimgray);
		DrawWalls();

		for (const auto& enemy : Global::enemies)
		{
			enemy->Draw();
		}
		for (const auto& item : Global::items)
		{
			item->Draw();
		}
	}

	// 3D シーンを 2D シーンに描画
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}
	//2D UI描画
	{
		if (Global::isGameOver)
		{
			Rect{ Scene::Size() }.draw(ColorF{ 0,0,0,0.5 });
			FontAsset{ U"TitleFont" }(U"GameOver...").drawAt(Scene::Center(), Palette::Gray);
			while (ShowCursor(TRUE) < 0) {}
			if (SimpleGUI::ButtonAt(U"Back Title", Scene::Center() + Vec2{0, 100}))
			{
				changeScene(U"Title");
			}
			return;
		}

		const Mat4x4 viewMat = Player::GetInstance().GetCamera().getView();
		const Mat4x4 projMat = Player::GetInstance().GetCamera().getProj();
		for (auto& enemy : Global::enemies)
		{
			DirectX::XMFLOAT3 enemypos = DirectX::XMFLOAT3{};
			enemypos.x = enemy->GetPos().x;
			enemypos.y = enemy->GetPos().y + 2;
			enemypos.z = enemy->GetPos().z;
			Vec2 enemyHead =
				WorldToScreen(enemypos, viewMat.value, projMat.value, Scene::Size().x, Scene::Size().y);
			//	Circle{enemyHead,20}.draw();
		}
		Player::GetInstance().DrawDamageQueue();
		Circle{ Scene::Center(), 30 }
		.drawFrame(1, 1, Palette::Deepskyblue);

		Circle{ Scene::Center(), 8 }
		.drawFrame(1, 1, Palette::Deepskyblue);
		m_itemsDraw();
		double currentHealth = Player::GetInstance().GetHealth();
		double maxHealth = Player::GetInstance().GetMaxHealth();
		double percentage = (currentHealth) / maxHealth;
		Rect{
			50, Scene::Height() - 100, static_cast<int>(static_cast<double>(Scene::Width()) * 0.4 * percentage), 64
		}.draw(Palette::Red);
		Rect{
			50, Scene::Height() - 100, static_cast<int>(static_cast<double>(Scene::Width()) * 0.4 * percentage), 64
		}.drawShadow(Vec2{ 10,10 }, 0.1);
		FontAsset{ U"UIFont" }(U"{:.1f} / {:.1f}"_fmt(currentHealth, maxHealth)).draw(50, Scene::Height() - 100);
	}

	if (EnemyBase::temp_enemies.size() > 0)
	{
		Global::enemies.insert(Global::enemies.end(),
							   std::make_move_iterator(EnemyBase::temp_enemies.begin()),
							   std::make_move_iterator(EnemyBase::temp_enemies.end()));
		EnemyBase::temp_enemies.clear();
	}
}

void Game::m_initialSpawn()
{
	if (m_spawnTimer.ms() > 8000)
	{
		static int i = 0;
		switch (i % 4)
		{
		case 0:
			Global::enemies.push_back(std::make_shared<StraightMoveEnemy>());
			++i;
			break;
		case 1:
			Global::enemies.push_back(std::make_shared<SlitheringEnemy>());
			++i;
			break;
		case 2:
			Global::enemies.push_back(std::make_shared<DistanceAttackEnemy>());
			++i;
			break;
		case 3:
			Global::enemies.push_back(std::make_shared<ChargingEnemy>());
			++i;
			break;
		}
		m_spawnTimer.restart();
	}
}

void Game::m_pause()
{
	const int labelWidth = 150;
	SimpleGUI::Slider(U"マウス感度{:.2f}"_fmt(Player::GetInstance().m_sensitivity),
		Player::GetInstance().m_sensitivity,
		0.0, 1.0, Vec2{ Scene::Width() * 0.33 - labelWidth, Scene::Height() - 100 }, labelWidth, Scene::Width() * 0.33 + labelWidth);
}

void Game::m_itemsDraw()
{
	const int itemsPerRow = 30;
	const int itemSize = 50;
	const int itemSpacing = 64;
	const int numRows = 3;
	for (int i = 0; i < Player::GetInstance().GetItemCount(); ++i)
	{
		int row = i / itemsPerRow;
		int col = i % itemsPerRow;

		if (row >= numRows and not isPaused)
		{
			break;
		}

		int x = col * itemSpacing + itemSpacing / 2;
		int y = row * itemSpacing + itemSpacing / 2;


		auto item = Player::GetInstance().GetItems()[i];
		item->drawUI(Vec2{ x,y });
	}

	for (int i = 0; i < Player::GetInstance().GetItemCount(); ++i)
	{
		int row = i / itemsPerRow;  		int col = i % itemsPerRow;
		if (row >= numRows)
			break;

		int x = col * itemSpacing + itemSpacing / 2;
		int y = row * itemSpacing + itemSpacing / 2;

		auto item = Player::GetInstance().GetItems()[i];
		if (item->GetTexture().region(Vec2{ x - itemSize * 0.5,y - itemSize * 0.5 }).mouseOver())
		{
			item->DrawDescription(Vec2{ x,y });
		}
		//Rect{ Arg::center(Size{x, y}), Size{itemSize, itemSize} }.mouseOver();
	}
}
