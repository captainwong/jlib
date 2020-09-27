#pragma once

#ifdef _WIN32
	namespace jlib { namespace net {																			
	struct SimpleLibeventOneTimeInitHelper {																	
		SimpleLibeventOneTimeInitHelper() {																		
			WSADATA wsa_data;																					
			WSAStartup(0x0202, &wsa_data);																		
			if (0 != evthread_use_windows_threads()) {															
				JLOG_CRTC("failed to init libevent with thread by calling evthread_use_windows_threads");		
				abort();																						
			}																									
		}																										
	};																											
	}}
#else		
	namespace jlib { namespace net {																			
	struct SimpleLibeventOneTimeInitHelper {																	
		SimpleLibeventOneTimeInitHelper() {																		
			if (0 != evthread_use_pthreads()) {																	
				JLOG_CRTC("failed to init libevent with thread by calling evthread_use_pthreads");				
				abort();																						
			}																									
		}																										
	};																											
	}}
#endif		
															
namespace jlib {
namespace net {
struct SimpleLibeventOneTimeInitter {	
	SimpleLibeventOneTimeInitter() { static SimpleLibeventOneTimeInitHelper __simple_libevent_one_time_init_helper; }																								
};																									
}
}


#define SIMPLE_LIBEVENT_ONE_TIME_INITTER static jlib::net::SimpleLibeventOneTimeInitter __simple_libevent_one_time_initter;

namespace jlib {
namespace net {
inline std::string eventToString(short evs) {
	std::string s;
#define check_event_append_to_s(e) if (evs & e) { s += #e " "; }
	check_event_append_to_s(BEV_EVENT_READING);
	check_event_append_to_s(BEV_EVENT_WRITING);
	check_event_append_to_s(BEV_EVENT_EOF);
	check_event_append_to_s(BEV_EVENT_ERROR);
	check_event_append_to_s(BEV_EVENT_TIMEOUT);
	check_event_append_to_s(BEV_EVENT_CONNECTED);
	return s;
}
}
}
	
