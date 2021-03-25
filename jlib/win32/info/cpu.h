#pragma once

#include "def.h"
#include <string>
#include <cstdint>

namespace jlib {
namespace win32 {
namespace info {
namespace cpu {

enum class Architecture {
	x86,
	x64,	// AMD or Intel
	arm,		
	arm64,		
	ia64,	// Intel Itanium-based
	unknown,
};

struct Quantity {

};

JINFO_API Architecture architecture() noexcept;
JINFO_API std::string architecture_to_string(Architecture arch);


// like BFEBFBFF000906EA, read from __cpuid
JINFO_API std::string cpu_id();

// GenuineIntel or AuthenticAMD
JINFO_API std::string vendor();

// like Intel(R) Core(TM) i5-2500 CPU @ 3.30GHz
JINFO_API std::string brand();
// get cpu_brand by registry
JINFO_API std::string brand_registry();


//namespace instruction_set_support {
//JINFO_API bool sse3();
//JINFO_API bool pclmulqdq();
//JINFO_API bool monitor();
//JINFO_API bool ssse3();
//JINFO_API bool fma();
//JINFO_API bool cmpxchg16b();
//JINFO_API bool sse41();
//JINFO_API bool sse42();
//JINFO_API bool movbe();
//JINFO_API bool popcnt();
//JINFO_API bool aes();
//JINFO_API bool xsave();
//JINFO_API bool osxsave();
//JINFO_API bool avx();
//JINFO_API bool f16c();
//JINFO_API bool rdrand();
//
//JINFO_API bool msr();
//JINFO_API bool cx8();
//JINFO_API bool sep();
//JINFO_API bool cmov();
//JINFO_API bool clfsh();
//JINFO_API bool mmx();
//JINFO_API bool fxsr();
//JINFO_API bool sse();
//JINFO_API bool sse2();
//
//JINFO_API bool fsgsbase();
//JINFO_API bool bmi1();
//JINFO_API bool hle();
//JINFO_API bool avx2();
//JINFO_API bool bmi2();
//JINFO_API bool erms();
//JINFO_API bool invpcid();
//JINFO_API bool rtm();
//JINFO_API bool avx512f();
//JINFO_API bool rdseed();
//JINFO_API bool adx();
//JINFO_API bool avx512pf();
//JINFO_API bool avx512er();
//JINFO_API bool avx512cd();
//JINFO_API bool sha();
//
//JINFO_API bool prefetchwt1();
//
//JINFO_API bool lahf();
//JINFO_API bool lzcnt();
//JINFO_API bool abm();
//JINFO_API bool sse4a();
//JINFO_API bool xop();
//JINFO_API bool tbm();
//
//JINFO_API bool syscall();
//JINFO_API bool mmxext();
//JINFO_API bool rdtscp();
//JINFO_API bool _3dnowext();
//JINFO_API bool _3dnow();
//}

}
}
}
}
