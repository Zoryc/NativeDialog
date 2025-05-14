#include "NativeDialog.hpp"

EXPORT_DLL LPWSTR saveFileDialog(WCHAR* title, WCHAR* filters) {
    COMDLG_FILTERSPEC* idk = Dialog::parseFilters(filters);
    WCHAR* result = Dialog::GetSaveFilePath(title, idk, Dialog::getLastFilterSize());
    free(idk);
    return result;
}

EXPORT_DLL LPWSTR openFileDialog(WCHAR* title, WCHAR* filters) {
    COMDLG_FILTERSPEC* idk = Dialog::parseFilters(filters);
    WCHAR* result = Dialog::GetOpenFilePath(title, idk, Dialog::getLastFilterSize());
    free(idk);
    return result;
}

EXPORT_DLL void setInitialDirectory(LPWSTR dir) {
    Dialog::setInitPath(dir);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

