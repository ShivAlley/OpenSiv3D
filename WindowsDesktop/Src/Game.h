#pragma once
#include "GVars.h"
#include "Player.h"
#include "Item.h"
#include <Siv3D/Windows/Windows.hpp>

inline void spawnWalls()
{
	// initPos = {0,5,16}
	auto initPos = CharaParam::initialPos;
	for (int z = static_cast<int>(initPos.z) - 100; z <= initPos.z + 100; z += 10) {
		// Left wall
		Vec3 leftWallPos = { initPos.x - 50,   initPos.y, z };
		OrientedBox leftWall = OrientedBox(leftWallPos, Vec3{ 1, 20, 10 });
		Global::walls.push_back(leftWall);

		// Right wall
		Vec3 rightWallPos = { initPos.x + 50, initPos.y, z };
		OrientedBox rightWall = OrientedBox(rightWallPos, Vec3{ 1, 20, 10 });
		Global::walls.push_back(rightWall);
	}
}

using App = SceneManager<String>;

const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
class Game : public App::Scene
{
public:

	// コンストラクタ（必ず実装）
	Game(const InitData& init)
		:IScene(init)
	{
		spawnWalls();
		const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

		while (ShowCursor(FALSE) >= 0)
		Global::isGameOver = false;
		Global::enemies.clear();
		Global::accumlatedDamage = 0;
		Global::escalation = 1;
		Global::maxDPS = 0;
		Global::deltaTime = 0;

		Player::GetInstance().Init();
	}

	// 更新関数（オプション）
	void update()override;

private:
	Stopwatch m_spawnTimer{ StartImmediately::Yes };
	void m_initialSpawn();
	MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	Vec2 WorldToScreen(const DirectX::XMFLOAT3& worldPosition,
									const DirectX::XMMATRIX& viewMatrix,
									const DirectX::XMMATRIX& projectionMatrix,
									int screenWidth,
									int screenHeight)
	{
		DirectX::XMVECTOR worldVec = DirectX::XMVectorSet(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f);

		DirectX::XMVECTOR viewVec = DirectX::XMVector4Transform(worldVec, viewMatrix);

		DirectX::XMVECTOR projectedVec = DirectX::XMVector4Transform(viewVec, projectionMatrix);

		DirectX::XMFLOAT4 orthogonal;
		DirectX::XMStoreFloat4(&orthogonal, projectedVec);
		float x = (orthogonal.x / orthogonal.w) * 0.5f + 0.5f;
		float y = -(orthogonal.y / orthogonal.w) * 0.5f + 0.5f;

		DirectX::XMFLOAT2 screenPosition;
		screenPosition.x = x * static_cast<float>(screenWidth);
		screenPosition.y = y * static_cast<float>(screenHeight);

		return Vec2{ screenPosition.x,screenPosition.y };
	}

	void m_pause();
	bool isPaused = false;

	void m_testItemSpawn()
	{
		for (int i = 0; i < 11; i++)
		{
			Global::RouletteDropItem(Vec3{ 0,5,10 });
		}
	}
	void m_itemsDraw();
	void m_printDebugInfo()
	{
#ifdef DEBUG


		Print << U"Escalation" << Global::escalation;
		Print << U"spawnCount" << Global::CalcSpawnCount();
		Print << U"maxDPS" << Global::maxDPS;
		Print << U"accumlatedDamage" << Global::accumlatedDamage;
		Print << U"enemiesCount" << Global::enemies.size();
		Print << U"droprate" << Global::CalcDropRate(Player::GetInstance().GetItemCount());
		Print << U"ItemCount" << Player::GetInstance().GetItemCount();
		Print << U"playerFireRate" << Player::GetInstance().fireRate();
		Print << U"playerRegenRate" << Player::GetInstance().RegenRate();
		Print << U"playerSpeed" << Player::GetInstance().Speed();
		Print << U"playerHealth" << Player::GetInstance().Health();
		Print << U"playerMaxHealth" << Player::GetInstance().MaxHealth();
		Print << U"playerAtk" << Player::GetInstance().Atk();
		Print << U"playerLuckyHitChance" << Player::GetInstance().LuckyHitChance();
		Print << U"playerLuckyMultiplier" << Player::GetInstance().LuckyMultiplier();
		Print << U"playerPrecisionMultiplier" << Player::GetInstance().PrecisionMultiplier();
#else
		return;
#endif // DEBUG
	}
};
