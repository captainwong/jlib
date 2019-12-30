#pragma once
#include <string>

namespace jlib
{

struct JsoncppHelper {

	template <typename JsonValue>
	static bool safelyGetIntValueStrict(const JsonValue& value, const char* name, int& out) {
		if (value[name].isInt()) { out = value[name].asInt(); return true; }
		return false;
	}

	template <typename JsonValue>
	static bool safelyGetIntValue(const JsonValue& value, const char* name, int& out) {
		if (safelyGetIntValueStrict(value, name, out)) {
			return true;
		} else if (value[name].isString()) {
			out = std::stoi(value[name].asString());
			return true;
		}
		return false;
	}

	template <typename JsonValue>
	static bool safelyGetStringValue(const JsonValue& value, const char* name, std::string& out) {
		if (value[name].isString()) { out = value[name].asString(); return true; }
		return false;
	}

	template <typename JsonValue, typename QString>
	static bool safelyGetStringValue(const JsonValue& value, const char* name, QString& out) {
		if (value[name].isString()) { out = value[name].asCString(); return true; }
		return false;
	}
};

}
