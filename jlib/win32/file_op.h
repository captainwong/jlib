#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <string>
#pragma comment(lib, "Shell32.lib")

namespace jlib
{
namespace win32
{

/**
example of COMDLG_FILTERSPEC:

COMDLG_FILTERSPEC cf[2] = {};
cf[0].pszName = L"Text Files";
cf[0].pszSpec = L"*.txt";
cf[1].pszName = L"All Files";
cf[1].pszSpec = L"*.*";
*/

inline bool getOpenFileDialogResult(std::wstring& path,
									HWND hWnd = nullptr,
									const std::wstring& default_folder = L"",
									const std::wstring& ext = L"",
									UINT cFileTypes = 0,
									const COMDLG_FILTERSPEC* filter = nullptr)
{
	bool ok = false;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
							  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			if (!default_folder.empty()) {
				IShellItem* psiFolder;
				PIDLIST_ABSOLUTE pidl;
				hr = SHParseDisplayName(default_folder.data(), 0, &pidl, SFGAO_FOLDER, 0);
				if (SUCCEEDED(hr)) {
					hr = SHCreateShellItem(NULL, NULL, pidl, &psiFolder);
					if (SUCCEEDED(hr)) {
						pFileOpen->SetDefaultFolder(psiFolder);
					}
					ILFree(pidl);
				}
			}

			if (!ext.empty()) {
				pFileOpen->SetDefaultExtension(ext.data());
			}

			if (cFileTypes != 0 && filter) {
				pFileOpen->SetFileTypes(cFileTypes, filter);
			}

			if (!hWnd) {
				hWnd = ::GetDesktopWindow();
			}

			// Show the Open dialog box.
			hr = pFileOpen->Show(hWnd);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr)) {
						//MessageBox(hWnd, pszFilePath, L"File Path", MB_OK);
						path = pszFilePath;
						ok = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return ok;
}

inline bool getSaveAsDialogPath(std::wstring& path,
								HWND hWnd = nullptr,
								const std::wstring& default_folder = L"",
								const std::wstring& default_name = L"",
								const std::wstring& ext = L"",
								UINT cFileTypes = 0,
								const COMDLG_FILTERSPEC* filter = nullptr)
{
	bool ok = false;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {
		IFileSaveDialog* pFileSave;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
							  IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr)) {
			if (!default_folder.empty()) {
				IShellItem* psiFolder;
				PIDLIST_ABSOLUTE pidl;
				hr = SHParseDisplayName(default_folder.data(), 0, &pidl, SFGAO_FOLDER, 0);
				if (SUCCEEDED(hr)) {
					hr = SHCreateShellItem(NULL, NULL, pidl, &psiFolder);
					if (SUCCEEDED(hr)) {
						pFileSave->SetDefaultFolder(psiFolder);
					}
					ILFree(pidl);
				}
			}

			if (!default_name.empty()) {
				pFileSave->SetFileName(default_name.data());
			}

			if (!ext.empty()) {
				pFileSave->SetDefaultExtension(ext.data());
			}

			if (cFileTypes != 0 && filter) {
				pFileSave->SetFileTypes(cFileTypes, filter);
			}

			if (!hWnd) {
				hWnd = ::GetDesktopWindow();
			}

			// Show the Open dialog box.
			hr = pFileSave->Show(hWnd);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr)) {
						//MessageBox(hWnd, pszFilePath, L"File Path", MB_OK);
						path = pszFilePath;
						ok = true;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileSave->Release();
		}
		CoUninitialize();
	}

	return ok;
}

}
}
