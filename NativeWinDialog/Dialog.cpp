#include "Dialog.hpp"

// https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog
// Text Files (*.txt)|*.txt|All Files (*.*)|*.*

namespace Dialog {
	UINT lastFilterSize = 0;
	LPWSTR initDir = { 0 };

	LPWSTR GetFormattedFileExtension(LPCWSTR str) {
		LPWSTR temp = (LPWSTR)malloc(sizeof(LPWSTR) * (lstrlenW(str) - 1));
		ZeroMemory(temp, sizeof(LPWSTR) * (lstrlenW(str) - 1));

		int index = 0;

		for (LPCWSTR ite = str; *ite != L'\0'; ite++) {

			if (*ite != L'*')
				temp[index++] = *ite;
		}

		if (temp == L".") {
			free(temp);
			temp = 0;
		}

		return temp;
	}

	void GetPathFromDialog(IFileDialog* pFileDialog, IShellItem* pItem, const COMDLG_FILTERSPEC* types, LPWSTR* result, bool addExtension) {
		LPWSTR temp;
		UINT index;

		pItem->GetDisplayName(SIGDN_FILESYSPATH, &temp);
		pFileDialog->GetFileTypeIndex(&index);

		LPWSTR extension = GetFormattedFileExtension((types[index - 1]).pszSpec);

		*result = (LPWSTR)malloc((lstrlenW(temp) + lstrlenW(extension)) * sizeof(LPWSTR));

		if (addExtension)
			wsprintfW(*result, L"%s%s", temp, extension);
		else
			wsprintfW(*result, L"%s", temp);

		free(extension);
		CoTaskMemFree(temp);
	}

	LPWSTR GetOpenFilePath(WCHAR* title, const COMDLG_FILTERSPEC* arrayTypes, UINT arraySize, UINT fileTypeIndex) {
		LPWSTR result = { 0 };
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); // ???
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			// Create the FileOpenDialog object. (Only one instance per machine)
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			pFileOpen->SetTitle(title);
			if (SUCCEEDED(hr)) {

				DWORD options;
				pFileOpen->GetOptions(&options);
				if (SUCCEEDED(hr))
					pFileOpen->SetOptions(options | FOS_NOCHANGEDIR); // just to make sure

				IShellItem* pCurFolder = NULL;
				HRESULT opH = SHCreateItemFromParsingName(initDir, NULL, IID_PPV_ARGS(&pCurFolder));
				if (SUCCEEDED(opH)) {
					pFileOpen->SetFolder(pCurFolder);
					pCurFolder->Release();
				}

				pFileOpen->SetFileTypes(arraySize, arrayTypes);
				if (SUCCEEDED(hr)) {

					pFileOpen->SetFileTypeIndex(fileTypeIndex);
					if (SUCCEEDED(hr))
					{
						// Show the Open dialog box.
						hr = pFileOpen->Show(GetActiveWindow());

						// Get the file name from the dialog box.
						if (SUCCEEDED(hr))
						{
							IShellItem* pItem;
							hr = pFileOpen->GetResult(&pItem);
							if (SUCCEEDED(hr))
							{
								GetPathFromDialog(pFileOpen, pItem, arrayTypes, &result, false);
								pItem->Release();
							}
						}
						pFileOpen->Release();
					}
				}
			}
			CoUninitialize();
		}

		return result;
	}

	LPWSTR GetSaveFilePath(WCHAR* title, const COMDLG_FILTERSPEC* arrayTypes, UINT arraySize, UINT fileTypeIndex) {
		LPWSTR result = { 0 };
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); // ???
		if (SUCCEEDED(hr))
		{
			IFileSaveDialog* pFileSave;

			// Create the FileOpenDialog object. (Only one instance per machine)
			hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

			pFileSave->SetTitle(title);
			if (SUCCEEDED(hr)) {

				DWORD options;
				pFileSave->GetOptions(&options);
				if (SUCCEEDED(hr))
					pFileSave->SetOptions(options | FOS_NOCHANGEDIR); // just to make sure

				IShellItem* pCurFolder = NULL;
				HRESULT opH = SHCreateItemFromParsingName(initDir, NULL, IID_PPV_ARGS(&pCurFolder));
				if (SUCCEEDED(opH)) {
					pFileSave->SetFolder(pCurFolder);
					pCurFolder->Release();
				}

				pFileSave->SetFileTypes(arraySize, arrayTypes);
				if (SUCCEEDED(hr)) {

					pFileSave->SetFileTypeIndex(fileTypeIndex);
					if (SUCCEEDED(hr))
					{
						// Show the Open dialog box.
						hr = pFileSave->Show(GetActiveWindow());

						// Get the file name from the dialog box.
						if (SUCCEEDED(hr))
						{
							IShellItem* pItem;
							hr = pFileSave->GetResult(&pItem);
							if (SUCCEEDED(hr))
							{
								GetPathFromDialog(pFileSave, pItem, arrayTypes, &result, true);
								pItem->Release();
							}
						}
						pFileSave->Release();
					}
				}
			}
			CoUninitialize();
		}
		return result;
	}

	COMDLG_FILTERSPEC* parseFilters(WCHAR* filters) {
		UINT maxArraySize = sizeof(WCHAR) * 256; // 256 caracteres maximum

		wchar_t* description = (wchar_t*)malloc(maxArraySize);
		wchar_t* extensions = (wchar_t*)malloc(maxArraySize);
		ZeroMemory(description, maxArraySize);
		ZeroMemory(extensions, maxArraySize);

		bool idk = false;
		int arrIndex = 0;
		int i = 0;
		COMDLG_FILTERSPEC* ok = reinterpret_cast<COMDLG_FILTERSPEC*>(malloc(4096)); // just to make sure :/

		for (WCHAR* view = filters; *view != L'\0'; view++) {

			if (*view == L'|') {

				if (!idk)
				{
					idk = true;
				}
				else
				{
					addNewFilter(ok, arrIndex++, description, extensions);
					idk = false;
					ZeroMemory(description, maxArraySize);
					ZeroMemory(extensions, maxArraySize);
				}

				i = 0;
				continue;
			}

			if (!idk) {
				description[i] = *view;
			}
			else
			{
				extensions[i] = *view;
			}

			i++;
		}

		if (description != NULL) {
			addNewFilter(ok, arrIndex++, description, extensions);
			ZeroMemory(description, maxArraySize);
			ZeroMemory(extensions, maxArraySize);
		}

		free(description);
		free(extensions);

		lastFilterSize = arrIndex;

		return ok;
	}

	UINT getLastFilterSize() {
		return lastFilterSize;
	}

	int getWCharArrayLength(WCHAR* arr) {
		int length = 0;
		for (WCHAR* test = arr; *test != L'\0'; test++) {
			length++;
		}
		return length + 1;
	}

	void addNewFilter(COMDLG_FILTERSPEC* ref1, UINT index, WCHAR* ref2_des, WCHAR* ref2_ext) {
		UINT length1 = getWCharArrayLength(ref2_des) * sizeof(WCHAR);
		UINT length2 = getWCharArrayLength(ref2_ext) * sizeof(WCHAR);

		WCHAR* temp1 = (WCHAR*)malloc(length1);
		WCHAR* temp2 = (WCHAR*)malloc(length2);

		memcpy(temp1, ref2_des, length1);
		memcpy(temp2, ref2_ext, length2);

		ref1[index] = { temp1, temp2 };
	}

	void setInitPath(LPWSTR init) {
		initDir = init;
	}
}