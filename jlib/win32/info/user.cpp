#include "user.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h> 
#include <lm.h>
#include <assert.h>
#include "../UnicodeTool.h"

#pragma comment(lib, "netapi32.lib")

namespace jlib {
namespace win32 {
namespace info {
namespace user {


static std::string curUserName()
{
	char name[UNLEN + 1];
	DWORD len = sizeof(name);
	if (GetUserNameA(name, &len)) {
		return name;
	}
	return {};
}

std::vector<User> users()
{
	std::vector<User> users;
	std::string cur_user_name = curUserName();

	using User3 = LPUSER_INFO_3;
	User3 pBuf = NULL;
	NET_API_STATUS nStatus;

	// Call the NetUserEnum function, specifying level 0; 
	//   enumerate global user account types only.
	//
	do // begin do
	{
		User3 pTmpBuf;
		DWORD dwLevel = 3;
		DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
		DWORD dwEntriesRead = 0;
		DWORD dwTotalEntries = 0;
		DWORD dwResumeHandle = 0;

		LPTSTR pszServerName = NULL;

		nStatus = NetUserEnum((LPCWSTR)pszServerName,
							  dwLevel,
							  FILTER_NORMAL_ACCOUNT, // global users
							  (LPBYTE*)&pBuf,
							  dwPrefMaxLen,
							  &dwEntriesRead,
							  &dwTotalEntries,
							  &dwResumeHandle);
		//
		// If the call succeeds,
		//
		if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {
			if ((pTmpBuf = pBuf) != NULL) {
				//
				// Loop through the entries.
				//
				for (DWORD i = 0; (i < dwEntriesRead); i++) {
					assert(pTmpBuf != NULL);

					if (pTmpBuf == NULL) {
						fprintf(stderr, "An access violation has occurred\n");
						break;
					}

					if (pTmpBuf->usri3_priv >= USER_PRIV_USER) {
						auto priv = [](int p) {
							if (p == USER_PRIV_USER) { return L"User"; }
							if (p == USER_PRIV_ADMIN) { return L"Admin"; }
							return L"";
						};
						//wprintf(L"\t--%s\t--%s\t--%s\n", pTmpBuf->usri3_name, pTmpBuf->usri3_full_name, priv(pTmpBuf->usri3_priv));
						auto userName = utf16_to_mbcs(pTmpBuf->usri3_name);
						/*QString user = QString("%1 %2 %3")
							.arg(userName)
							.arg(QString::fromWCharArray(pTmpBuf->usri3_full_name))
							.arg(QString::fromWCharArray(priv(pTmpBuf->usri3_priv)));*/

						User u = { userName, utf16_to_mbcs(pTmpBuf->usri3_full_name), utf16_to_mbcs(priv(pTmpBuf->usri3_priv)) };

						if (userName == cur_user_name) {
							//users.push_front(user);
							users.insert(users.begin(), u);
						} else {
							//users.push_back(user);
							users.push_back(u);
						}
					}
					//
					//  Print the name of the user account.
					//
					//wprintf(L"\t-- %s\n", pTmpBuf->usri2_name);

					pTmpBuf++;
				}
			}
		}
		//
		// Otherwise, print the system error.
		//
		else
			fprintf(stderr, "A system error has occurred: %d\n", nStatus);
		//
		// Free the allocated buffer.
		//
		if (pBuf != NULL) {
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		}
	}
	// Continue to call NetUserEnum while 
	//  there are more entries. 
	// 
	while (nStatus == ERROR_MORE_DATA); // end do
										//
										// Check again for allocated memory.
										//
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);

	return users;
}

}
}
}
}

