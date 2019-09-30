#include <jlib/win32/DeviceUniqueIdentifier.h>
#include <stdio.h>
#include <algorithm>
#include <locale.h>

int main()
{
	std::locale::global(std::locale(""));
	using namespace jlib::win32::DeviceUniqueIdentifier;
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

	printf("\nJoined results:\n%ls\n", join_result(results, L",").data());

	system("pause");
}