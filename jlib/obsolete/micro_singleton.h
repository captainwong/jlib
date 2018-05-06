#pragma once


#define DECLARE_SINGLETON(class_name) \
private: \
	class_name(); \
	static class_name* instance_for_singleton_; \
	static std::mutex mutex_for_singleton_; \
public: \
	static class_name* get_instance() { \
		std::lock_guard<std::mutex> lock(mutex_for_singleton_); \
		if (instance_for_singleton_ == nullptr) { \
			instance_for_singleton_ = new class_name(); \
		} \
		return instance_for_singleton_; \
	} \
	static void release_singleton_object()() { \
		if (instance_for_singleton_) { delete instance_for_singleton_; instance_for_singleton_ = nullptr; } \
	}



#define IMPLEMENT_SINGLETON(class_name) \
	class_name* class_name::instance_for_singleton_ = nullptr; \
	std::mutex class_name::mutex_for_singleton_;

