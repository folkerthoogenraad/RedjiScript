#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <iostream>

namespace redji {
	struct LogMessage {
		enum Severity{
			Info,
			Warning,
			Error,
			Unimplemented
		};

		Severity m_Severity;
		std::string m_Message;
	};
	class Logger {
		std::vector<LogMessage> m_Messages;
		std::string m_Prefix = "";
	public:
		void setPrefix(std::string p) { m_Prefix = p; }

		template<typename ... Args>
		void log(LogMessage::Severity severity, const Args &... args) {
			LogMessage message;
			message.m_Severity = severity;
			
			std::stringstream stream;
			write(stream, args...);

			message.m_Message = stream.str();

			std::cout << "[";

			if (m_Prefix.size() > 0) {
				std::cout << m_Prefix << " ";
			}

			switch (severity) {
			case LogMessage::Info:
				std::cout << "Info";
				break;
			case LogMessage::Error:
				std::cout << "Error";
				break;
			case LogMessage::Warning:
				std::cout << "Warning";
				break;
			case LogMessage::Unimplemented:
				std::cout << "Unimplemented";
				break;
			default:
				std::cout << "Unknown";
				break;
			}

			std::cout << "] ";
			std::cout << message.m_Message << std::endl;

			m_Messages.push_back(std::move(message));
		}

		template<typename ... Args>
		void logInfo(const Args & ... args) {
			log(LogMessage::Severity::Info, args...);
		}
		template<typename ... Args>
		void logError(const Args & ... args) {
			log(LogMessage::Severity::Error, args...);
		}
		template<typename ... Args>
		void logWarning(const Args & ... args) {
			log(LogMessage::Severity::Warning, args...);
		}
		template<typename ... Args>
		void logUnimplemented(const Args & ... args) {
			log(LogMessage::Severity::Unimplemented, args...);
		}

	private:
		template<typename T>
		void write(std::stringstream &stream, const T &t) {
			stream << t;
		}

		template<typename T, typename... Args>
		void write(std::stringstream &stream, const T &t, const Args &... args) {
			stream << t;
			write(stream, args...);
		}
	};
}