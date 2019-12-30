#include <jlib/win32/DeviceUniqueIdentifier.h>
#include <stdio.h>
#include <algorithm>
#include <locale.h>

using namespace jlib::win32::DeviceUniqueIdentifier;

void test()
{
	const int TIMES = 100;
	printf("\n\nTest %d times on RecommendedQueryTypes...\n", TIMES);
	for (int i = 0; i < TIMES; i++) {
		printf("\r%d/%d", i+1, TIMES);
		QueryResults results; 
		bool ok = query(RecommendedQueryTypes, results);
		if (!ok) {
			printf("Error on #%d\n", i);
			for (auto iter = results.begin(); iter != results.end(); iter++) {
				printf("%ls: %ls\n", queryTypeString(iter->first), iter->second.data());
			}
			return;
		}
	}

	printf("\nDone!\n");
}



int main()
{
	QueryResults results;

	printf("Query RecommendedQueryTypes:\n");
	query(RecommendedQueryTypes, results);
	for (auto iter = results.begin(); iter != results.end(); iter++) {
		printf("%ls: %ls\n", queryTypeString(iter->first), iter->second.data());
	}

	printf("\nQuery AllQueryTypes:\n");
	results.clear();
	query(AllQueryTypes, results);
	for (auto iter = results.begin(); iter != results.end(); iter++) {
		printf("%ls: %ls\n", queryTypeString(iter->first), iter->second.data());
	}
	//IOCTL_NDIS_QUERY_GLOBAL_STATS
	printf("\nJoined results:\n%ls\n", join_result(results, L",").data());

	test();

	system("pause");
}