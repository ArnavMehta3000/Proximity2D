#pragma once

#include "MathDefines.h"
#include <random>

#define PRX_RANDOM_SEED std::random_device{}()

namespace Proximity
{
	namespace Math
	{
		class Random
		{
		private:
			class RandomGenerator
			{
				friend class Random;

			private:
				RandomGenerator(F32 start, F32 end)
					:
					m_generator(PRX_RANDOM_SEED),
					m_distribution(std::uniform_real_distribution<float>(start, end))
				{}

				RandomGenerator(F32 start, F32 end, I32 seed)
					:
					m_generator(seed),
					m_distribution(std::uniform_real_distribution<float>(start, end))
				{}

				std::mt19937_64 m_generator;
				std::uniform_real_distribution<F32> m_distribution;

			public:
				F32 Value() { return m_distribution(m_generator); }
			};

		public:
			// Returns a random float in range [start, end)
			static F32 Float(F32 start, F32 end);
			// Returns a random float with a seed in range [start, end)
			static F32 Float(F32 start, F32 end, I32 seed);
			// Returns a random value in range [0, 1)
			static F32 Value();


		private:
			static RandomGenerator s_randomGenerator;
		};
	}
}