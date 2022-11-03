#include "enginepch.h"
#include "Math/MathUtils.h"
#include <cmath>

namespace Proximity
{
	namespace Math
	{
		const F32 Utils::PI      = static_cast<float>(3.141592653589793238);
		const F32 Utils::PI_HALF = 0.5f * static_cast<float>(3.141592653589793238);
		const F32 Utils::EPSILON = FLT_EPSILON;
		const F32 Utils::DEG2RAD = static_cast<float>(3.141592653589793238) / 180.f;
		const F32 Utils::RAD2DEG = 180.f * static_cast<float>(3.141592653589793238);


		F32 Utils::Abs(F32 number)                                 { return number < 0 ? -number : number; }

		F32 Utils::Min(F32 a, F32 b)                               { return min(a, b); }

		F32 Utils::Max(F32 a, F32 b)                               { return max(a, b); }

		F32 Utils::Pow(F32 number, F32 power)                      { return powf(number, power); }

		F32 Utils::Sqrt(F32 number)                                { return sqrtf(number); }

		F32 Utils::Square(F32 number)                              { return number * number; }

		F32 Utils::Sin(F32 number)                                 { return asinf(number); }

		F32 Utils::Cos(F32 number)                                 { return acosf(number); }

		F32 Utils::Tan(F32 number)                                 { return atanf(number); }

		F32 Utils::Atan2(F32 y, F32 x)                             { return atan2f(y, x); }

		F32 Utils::Ceil(F32 number)                                { return ceilf(number); }

		I32 Utils::CeilInt(F32 number)                             { return static_cast<I32>(ceilf(number)); }

		F32 Utils::Round(F32 number)                               { return roundf(number); }

		F32 Utils::Floor(F32 number)                               { return floorf(number); }

		I32 Utils::FloorInt(F32 number)                            { return static_cast<I32>(floorf(number)); }

		F32 Utils::InverseLerp(F32 start, F32 end, F32 number)     { return (number - start) / (end - start); }

		F32 Utils::Lerp(F32 start, F32 end, F32 t)                 { return start + (end - start) * t; }

		F32 Utils::MoveTowards(F32 current, F32 target, F32 delta) { return min(current + delta, target); }

		F32 Utils::SmoothStep(F32 start, F32 end, F32 step)
		{
			step = Utils::Clamp01((step - start) / (end - start));
			return Square(step) * (3 - 2 * step);
		}

		F32 Utils::Ln(F32 number)                                  { return logf(number); }

		F32 Utils::Log(F32 number, F32 base)                       { return logf(number) / logf(base); }

		F32 Utils::Log10(F32 number)                               { return log10(number); }

		F32 Utils::Clamp(F32 start, F32 end, F32 number)           { return number < start ? start : (number > end ? end : number); }

		F32 Utils::Clamp01(F32 number)                             { return Proximity::Math::Utils::Clamp(0.0f, 1.0f, number); }

		bool Utils::AlmostEquals(F32 a, F32 b)                      { return Utils::Abs(a - b) < Utils::EPSILON; }

	}
}