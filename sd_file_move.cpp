#include <iostream>
#include <vector>
#include <windows.h>
#include <fstream>

void copyFile(const std::wstring& source, const std::wstring& destination) {
    std::wifstream src(source.c_str(), std::ios::binary);
    std::wofstream dest(destination.c_str(), std::ios::binary);

    dest << src.rdbuf();
}

void copyFilesFromSDCard(const std::wstring& destinationDirectory) {
    WCHAR drives[256];
    DWORD drivesSize = GetLogicalDriveStringsW(256, drives);
    if (drivesSize == 0) {
        std::cerr << "Failed to get logical drives.\n";
        return;
    }

    WCHAR* drive = drives;
    while (*drive != L'\0') {
        if (GetDriveTypeW(drive) == DRIVE_REMOVABLE) {
            std::wstring sdCard(drive);
            std::wstring destination = destinationDirectory + L"\\";

            if (!CreateDirectoryW(destination.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
                std::cerr << "Failed to create destination directory.\n";
                return;
            }

            WIN32_FIND_DATAW findFileData;
            HANDLE hFind = FindFirstFileW((sdCard + L"\\*").c_str(), &findFileData);
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        std::wstring sourceFile = sdCard + L"\\" + findFileData.cFileName;
                        std::wstring destinationFile = destination + findFileData.cFileName;
                        copyFile(sourceFile, destinationFile);
                        std::wcout << L"Copied " << findFileData.cFileName << L" to " << destinationFile << std::endl;
                    }
                } while (FindNextFileW(hFind, &findFileData) != 0);
                FindClose(hFind);
            } else {
                std::cerr << "Failed to enumerate files in SD card directory.\n";
                return;
            }
        }
        drive += wcslen(drive) + 1;
    }
}

int main() {
    // Example usage:
    std::wstring destinationDirectory = L"C:\\MyDestinationDirectory";

    copyFilesFromSDCard(destinationDirectory);

    return 0;
}
