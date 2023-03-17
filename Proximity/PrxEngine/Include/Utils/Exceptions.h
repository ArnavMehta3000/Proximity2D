#pragma once
#include <stdexcept>

namespace Proximity::Execptions
{
	class MethodNotImplemented : public std::logic_error
	{
	public:
		MethodNotImplemented(const char* message)
			:
			std::logic_error("Not Implemented"),
			m_message(message)
		{}
		

		virtual char const* what() const { return m_message.c_str(); }


	private:
		std::string m_message;
	};
}