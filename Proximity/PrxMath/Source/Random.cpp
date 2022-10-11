#include "Random.h"

namespace PRX
{
	namespace Math
	{
		Random::RandomGenerator Random::s_randomGenerator = Random::RandomGenerator(0.0f, 1.0f);

		F32 Random::Float(F32 start, F32 end)
		{
			return RandomGenerator(start, end).Value();
		}

		F32 Random::Float(F32 start, F32 end, I32 seed)
		{
			return RandomGenerator(start, end, seed).Value();
		}

		F32 Random::Value() { return s_randomGenerator.Value(); }

	}
}