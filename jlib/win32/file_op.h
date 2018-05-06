#pragma once
#include <Windows.h>
#include <string>

namespace jlib {
inline bool get_file_open_dialog_result(std::wstring& path, 
										HWND hWnd = nullptr,
										const std::wstring& default_folder = L"",
										const std::wstring& ext = L"",
										UINT cFileTypes = 0,
										const COMDLG_FILTERSPEC *rgFilterSpec = nullptr) {
	bool ok = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
								COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {

		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
							  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {

			if (!default_folder.empty()) {
				IShellItem *psiFolder;
				//LPCWSTR szFilePath = SysAllocStringLen(default_folder.data(), default_folder.size());
				//hr = SHCreateItemFromParsingName(szFilePath, NULL, IID_PPV_ARGS(&psiFolder));
				//if (SUCCEEDED(hr))
				//	hr = pFileOpen->SetDefaultFolder(psiFolder);

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

			if (cFileTypes != 0 && rgFilterSpec) {
				pFileOpen->SetFileTypes(cFileTypes, rgFilterSpec);
			}

			if (!hWnd) {
				hWnd = ::GetDesktopWindow();
			}

			// Show the Open dialog box.
			hr = pFileOpen->Show(hWnd);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr)) {
				IShellItem *pItem;
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

inline bool get_save_as_dialog_path(std::wstring& path,
									HWND hWnd = nullptr,
									const std::wstring& default_folder = L"",
									const std::wstring& default_name = L"",
									const std::wstring& ext = L"",
									UINT cFileTypes = 0,
									const COMDLG_FILTERSPEC *rgFilterSpec = nullptr) {
	bool ok = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
								COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {

		IFileSaveDialog *pFileSave;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
							  IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr)) {

			if (!default_folder.empty()) {
				IShellItem *psiFolder;
				//LPCWSTR szFilePath = SysAllocStringLen(default_folder.data(), default_folder.size());
				//hr = SHCreateItemFromParsingName(szFilePath, NULL, IID_PPV_ARGS(&psiFolder));
				//if (SUCCEEDED(hr))
				//	hr = pFileSave->SetDefaultFolder(psiFolder);

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
			
			if (cFileTypes != 0 && rgFilterSpec) {
				pFileSave->SetFileTypes(cFileTypes, rgFilterSpec);
			}

			if (!hWnd) {
				hWnd = ::GetDesktopWindow();
			}

			// Show the Open dialog box.
			hr = pFileSave->Show(hWnd);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr)) {
				IShellItem *pItem;
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
