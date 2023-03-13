#pragma once

namespace Proximity::Buffers
{
	struct WVPMatrices
	{
		Math::Matrix World;
		Math::Matrix View;
		Math::Matrix Projection;
	};
}