#pragma once

#include <vector>
#include <string>
#include <memory>
#include <string>

#include "ast/ClassSyntax.h"
#include "ast/FunctionSyntax.h"

namespace redji {

	namespace compiled {


		class Type {
		};

		class BuiltinType : public Type{
		public:
			enum Subtype{
				Integer, Float, String, Void
			};

		private:
			Subtype m_Type;

		public:
			BuiltinType(Subtype type) :m_Type(type) {}

			static std::shared_ptr<BuiltinType> getFloat();
			static std::shared_ptr<BuiltinType> getInteger();
			static std::shared_ptr<BuiltinType> getString();
			static std::shared_ptr<BuiltinType> getVoid();
		};

		class NameAndType {
		public:
			std::string m_Name;
			std::shared_ptr<Type> m_Type;
		};

		class CustomType : public Type{
		public:
			std::string m_Name;
			std::vector<std::shared_ptr<NameAndType>> m_Members;

			std::shared_ptr<ClassSyntax> m_Definition;
		};

		class Function {
		public:
			std::string m_Name;
			std::vector<std::shared_ptr<NameAndType>> m_Parameters;
			std::shared_ptr<Type> m_ReturnType;

			std::shared_ptr<FunctionSyntax> m_Definition;
		};

		class Namespace {
			std::vector<std::shared_ptr<Namespace>> m_Namespaces;
			std::vector<std::shared_ptr<CustomType>> m_Types;
			std::vector<std::shared_ptr<Function>> m_Functions;

			std::string m_Name;
		public:
			Namespace(std::string name) : m_Name(name) {};
			
			void addNamespace(std::shared_ptr<Namespace> ns) { m_Namespaces.push_back(ns); }
			void addType(std::shared_ptr<CustomType> type) { m_Types.push_back(type); }
			void addFunction(std::shared_ptr<Function> func) { m_Functions.push_back(func); }

			std::shared_ptr<Type> findTypeByName(std::string name);

			const std::string &getName() { return m_Name; }
		};

	}
}