#include"../PrxEngine/Include/PrxEngine.h"
#include "../PrxEngine/Include/PrxMath.h"
#include "../PrxEngine/Include/PrxUtils.h"
#include <iostream>
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
};
	
int main()
{
	ServiceLocator* sl = new ServiceLocator();
	sl->RegisterService<TestService>();
	std::cout << sl->ResolveService<TestService>()->ServiceID() << std::endl;
	sl->UnRegisterService<TestService>();
	sl->RegisterService<TestService>();
	std::cout << sl->ResolveService<TestService>()->ServiceID() << std::endl;

	
	return 0;
}