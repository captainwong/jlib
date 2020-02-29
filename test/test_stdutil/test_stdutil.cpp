#include "../../jlib/util/std_util.h"
#include <assert.h>

int main()
{
	std::vector<int> All{ 1, 2, 3,4,5 };
	std::vector<int> Sub{ 1,2,3 };
	auto other = jlib::get_other(All, Sub);
	assert(other == std::vector<int>({4, 5}));
}
