#include "renderer2dpch.h"
#include "Renderer2D/Renderer2D.h"

namespace Proximity::Graphics
{
	Renderer2D* Renderer2D::s_instance = nullptr;

	Renderer2D* Renderer2D::Get()
	{
		if (s_instance == nullptr)
			return new Renderer2D();
		else
			return s_instance;
	}
	void Renderer2D::ShutdownAndReset()
	{
		s_instance->Shutdown();
		delete s_instance;
	}

	Renderer2D::Renderer2D()
		:
		m_d3d(nullptr)
	{
	}
	Renderer2D::~Renderer2D()
	{
	}
}