#include "pch.h"
#include "CppUnitTest.h"
#include "../PrxMath/Include/PrxMath.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PrxEngineTests
{
	TEST_CLASS(PrxEngineTests)
	{
	public:		
		TEST_METHOD(TestPrxMathUtils)
		{
			Assert::AreEqual(5.0f, PRX::Math::Utils::Abs(-5), 0);
			Assert::AreEqual(1.0f, PRX::Math::Utils::Min(1, 2), 0);
			Assert::AreEqual(2.0f, PRX::Math::Utils::Max(1, 2), 0);
			Assert::AreEqual(4.0f, PRX::Math::Utils::Pow(2, 2), 0);
			Assert::AreEqual(2.0f, PRX::Math::Utils::Sqrt(4), 0);
			Assert::AreEqual(4.0f, PRX::Math::Utils::Square(2), 0);
			Assert::AreEqual(3.0f, PRX::Math::Utils::Ceil(2.6f), 0);
			Assert::AreEqual(3.0,  PRX::Math::Utils::CeilInt(2.6f), 0);
			Assert::AreEqual(3.0f, PRX::Math::Utils::Round(2.6f), 0);
			Assert::AreEqual(2.0f, PRX::Math::Utils::Floor(2.6f), 0);
			Assert::AreEqual(2.0,  PRX::Math::Utils::FloorInt(2.6f), 0);
			Assert::AreEqual(1.0f, PRX::Math::Utils::Lerp(0, 2, 0.5f), 0);
			Assert::AreEqual(0.5f, PRX::Math::Utils::InverseLerp(0, 2, 1.0f), 0);
			Assert::AreEqual(2.0f, PRX::Math::Utils::Clamp(0, 2, 5.0f), 0);
			Assert::AreEqual(true, PRX::Math::Utils::AlmostEquals(0, 0.0000000001), PRX::Math::Utils::EPSILON);
		}

		TEST_METHOD(TestPrxMathVector3)
		{
			
		}
	};
}
