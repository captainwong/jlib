#pragma once

namespace jlib {
inline bool get_file_open_dialog_result(std::wstring& path, HWND hWnd = nullptr) {
	bool ok = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
								COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {

		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
							  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
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


inline bool get_save_as_dialog_path(std::wstring& path, HWND hWnd = nullptr) {
	bool ok = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
								COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {

		IFileSaveDialog *pFileSave;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
							  IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr)) {
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
