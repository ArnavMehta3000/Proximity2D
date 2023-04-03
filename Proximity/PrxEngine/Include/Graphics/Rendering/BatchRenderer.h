#pragma once


namespace Proximity::Graphics
{
	class BatchRenderer
	{
	public:
		struct Stats
		{
			Math::U32 DrawCount = 0;
			Math::U32 QuadCount = 0;
		};

	public:
		static void Init();
		static void Shutdown();

		static void BeginBatch();
		static void EndBatch();
		static void Flush();

		static void DrawQaud(const Math::Matrix& worldMatrix, const DirectX::XMFLOAT4& color);

		// TODO: Create a draw quad with texture data

		static const Stats& GetStats();
		static void ResetStats()
	};
}