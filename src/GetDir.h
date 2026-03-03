
#ifdef _WIN32
#include <windows.h>

// Конвертация из UTF-16 (Windows native) в UTF-8
inline std::string utf16_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
#endif

inline fs::path getExeDirectory() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    // Получаем путь в UTF-16 (всегда корректно)
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring widePath(buffer);

    // Обрезаем имя файла
    size_t pos = widePath.find_last_of(L"\\/");
    std::wstring wideDir = (pos != std::wstring::npos) ? widePath.substr(0, pos) : L"";

    // Переводим в нормальный UTF-8
    return utf16_to_utf8(wideDir);
#else
    return fs::current_path();
#endif
}