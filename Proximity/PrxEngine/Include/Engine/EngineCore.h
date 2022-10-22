#pragma once

namespace Proximity
{
	namespace Core
	{
		class EngineCore
		{
		public:
			EngineCore();
			~EngineCore();

			bool Init(HINSTANCE hInst);
			void Run();
			void Shutdown();

		private:

		};

	}
}