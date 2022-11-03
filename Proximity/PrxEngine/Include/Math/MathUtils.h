#pragma once
#include "MathDefines.h"

namespace Proximity
{
	namespace Math
	{
		class Utils
		{
		public:
			static const F32 PI;
			static const F32 PI_HALF;
			static const F32 EPSILON;
			static const F32 DEG2RAD;
			static const F32 RAD2DEG;


			// Returns  absolute value of the number
			static F32 Abs(F32 number);
			// Returns the smaller of the two numbers
			static F32 Min(F32 a, F32 b);
			// Returns the larger of the two numbers
			static F32 Max(F32 a, F32 b);
			//Returns the number raised to the power
			static F32 Pow(F32 number, F32 power);
			// Returns the sqare root of a number
			static F32 Sqrt(F32 number);
			// Returns number * number
			static F32 Square(F32 number);

			// Trigonometric sin function
			static F32 Sin(F32 number);
			// Trigonometric cos function
			static F32 Cos(F32 number);
			// Trigonometric tan function
			static F32 Tan(F32 number);
			// Returns the tan angle in radian in y/x
			static F32 Atan2(F32 y, F32 x);

			// Returns the ceiling value of the number
			static F32 Ceil(F32 number);
			// Returns the integer ceiling value of the number
			static I32 CeilInt(F32 number);
			// Returns the number rounded to the nearest integer
			static F32 Round(F32 number);
			// Returns the floor value of the number 
			static F32 Floor(F32 number);
			// Returns the integer floor value of the number
			static I32 FloorInt(F32 number);

			// Calculates the percentage a number is in between two numbers
			static F32 InverseLerp(F32 start, F32 end, F32 number);
			// Linearly interpolates between two numbers based on percentage t
			static F32 Lerp(F32 start, F32 end, F32 t);
			// Moves a value towards a target 
			static F32 MoveTowards(F32 current, F32 target, F32 delta);
			// Interpolates between min and max with smoothing at limits
			static F32 SmoothStep(F32 start, F32 end, F32 step);
			
			// Returns the natural log of a number
			static F32 Ln(F32 number);
			// Returns the log of the number of a given base
			static F32 Log(F32 number, F32 base);
			// Returns the log base 10 of a number
			static F32 Log10(F32 number);

			// Clamps a number in range
			static F32 Clamp(F32 start, F32 end, F32 number);
			// Clamps a number in the [0, 1] range
			static F32 Clamp01(F32 number);

			// Checks if two values are almost equal based on Math::Utils::Epsilon
			static bool AlmostEquals(F32 a, F32 b);
		};
	}
}