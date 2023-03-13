#pragma once
#include <stdexcept>

namespace Proximity::Execptions
{
	class MethodNotImplemented : public std::logic_error
	{
	public:
		MethodNotImplemented(const char* message)
			:
			MethodNotImplemented(message, __FUNCTION__)
		{}
		

		virtual char const* what() const { return m_message.c_str(); }

	private:
		MethodNotImplemented(const char* message, const char* function)
			:
			std::logic_error("Not Implemented")
		{
			m_message = std::string(message).append(": ").append(function);
		}		

	private:
		std::string m_message;
	};
}