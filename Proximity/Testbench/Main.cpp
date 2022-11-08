#include "pch.h"
#include"../PrxEngine/Include/PrxEngine.h"
#include "../PrxEngine/Include/PrxMath.h"
#include "../PrxEngine/Include/PrxUtils.h"
using namespace Proximity::Math;
using namespace Proximity::Utils;


class TestService : public IService
{
public:
	TestService()
	{

	}
	~TestService()
	{

	}

	static void TestFunction(int num)
	{
		int x = num;
	}
};


	
int main()
{
	return 0;
}