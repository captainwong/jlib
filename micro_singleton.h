#pragma once

#define DECLARE_SINGLETON(class_name) \
private: \
	class_name(); \
	static class_name* m_pInstance; \
	static std::mutex m_lock4Instance; \
public: \
	static class_name* GetInstance() { \
		std::lock_guard<std::mutex> lock(m_lock4Instance); \
		if (m_pInstance == NULL){ \
			m_pInstance = new class_name(); \
		} \
		return m_pInstance; \
	} \
	static void ReleaseObject() { \
		if (m_pInstance) { delete m_pInstance; m_pInstance = NULL; } \
	}


#define IMPLEMENT_SINGLETON(class_name) \
	class_name* class_name::m_pInstance = NULL; \
	std::mutex class_name::m_lock4Instance;
