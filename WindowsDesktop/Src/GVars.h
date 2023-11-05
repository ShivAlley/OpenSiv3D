#pragma once

namespace CharaParam
{
	inline const Vec3 initialPos = { 0,5,-16 };
	inline std::vector<double> enemyWeights = { 100,80,50,0,50 };
	inline std::vector<double> itemWeights = { 100,33 };
	inline const double enemyBias = 1;
}

namespace Global
{
	inline double deltaTime = 0;
	inline const Size windowSize = { 1920,1080 };
	inline std::deque<OrientedBox> walls;
	//inline Stopwatch recordDPSTimer{ StartImmediately::Yes };
	inline double accumlatedDamage = 0;
	inline double escalation = 1;
	inline double maxDPS = 0;
	inline bool isGameOver = false;

	inline Plane gameFloor{ 100,250 };


	inline int BiasedRandom(std::vector<double>& weights, double bias = 1)
	{
		// 各重みをバイアス値で調整する
		/*
		for (auto& weight : weights)
		{
			// 簡素化のため今回は常に１
			weight = pow(weight, bias);
		}
		 */

		 // 重みの合計を計算する
		double totalWeight = std::accumulate(weights.begin(), weights.end(), 0.0);

		double randomValue = Random(0.0, totalWeight);
		// ルーレット選択法を使用して選択肢を選ぶ
		double runningTotal = 0.0;
		for (size_t i = 0; i < weights.size(); ++i)
		{
			runningTotal += weights[i];
			if (randomValue <= runningTotal)
			{
				return i;
			}
		}

		assert(false, "fail random pick");
		return 0;  // 何かが間違っている場合とりあえず０を返す
	}
	inline double CalcDropRate(double x)
	{
		constexpr double balance = 0.5;
		double result = 1.0 / (1.0 + balance * x);
		assert(result > 0);
		return std::abs(result);
	}
	inline int CalcSpawnCount()
	{
		return 1 + static_cast<int>(maxDPS) / 1000;
	}
	inline void CalcEscalate()
	{
		escalation = 1 + maxDPS * 0.001;
	}

	inline Vec2 WorldToScreen(const DirectX::XMFLOAT3& worldPosition,
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
}
