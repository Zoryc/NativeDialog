#pragma once
#include <Shobjidl.h>

namespace Dialog {
	LPWSTR GetFormattedFileExtension(LPCWSTR str);
	void GetPathFromDialog(IFileDialog* pFileDialog, IShellItem* pItem, const COMDLG_FILTERSPEC* types, LPWSTR* result, bool addExtension);
	LPWSTR GetOpenFilePath(WCHAR* title, const COMDLG_FILTERSPEC* arrayTypes, UINT arraySize, UINT fileTypeIndex = 1);
	LPWSTR GetSaveFilePath(WCHAR* title, const COMDLG_FILTERSPEC* arrayTypes, UINT arraySize, UINT fileTypeIndex = 1);
	COMDLG_FILTERSPEC* parseFilters(WCHAR* filters);
	UINT getLastFilterSize();
	void addNewFilter(COMDLG_FILTERSPEC* ref1, UINT index, WCHAR* ref2_des, WCHAR* ref2_ext);
	int getWCharArrayLength(WCHAR* arr);
	void setInitPath(LPWSTR init);
};
