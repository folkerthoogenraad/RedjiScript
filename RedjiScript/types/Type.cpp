#include "Type.h"

namespace redji {
	namespace compiled {
		std::shared_ptr<Type> Namespace::findTypeByName(std::string name)
		{
			if (name == "float")
				return BuiltinType::getFloat();
			if (name == "int")
				return BuiltinType::getInteger();
			if (name == "void")
				return BuiltinType::getVoid();
			if (name == "string")
				return BuiltinType::getString();

			for (int i = 0; i < m_Types.size(); i++) {
				if (m_Types[i]->m_Name == name)
					return m_Types[i];
			}


			return nullptr;
		}
		std::shared_ptr<BuiltinType> BuiltinType::getFloat()
		{
			static auto type = std::make_shared<BuiltinType>(Float);

			return type;
		}
		std::shared_ptr<BuiltinType> BuiltinType::getString()
		{
			static auto type = std::make_shared<BuiltinType>(String);

			return type;
		}
		std::shared_ptr<BuiltinType> BuiltinType::getVoid()
		{
			static auto type = std::make_shared<BuiltinType>(Void);

			return type;
		}
		std::shared_ptr<BuiltinType> BuiltinType::getInteger()
		{
			static auto type = std::make_shared<BuiltinType>(Integer);

			return type;
		}
	}
}
