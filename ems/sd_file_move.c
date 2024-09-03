#include <stdio.h>
#include <windows.h>

void copyFile(const wchar_t* source, const wchar_t* destination) {
    FILE* src = _wfopen(source, L"rb");
    FILE* dest = _wfopen(destination, L"wb");

    if (src == NULL || dest == NULL) {
        printf("Failed to open files.\n");
        return;
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes_read, dest);
    }

    fclose(src);
    fclose(dest);
}

void copyFilesFromSDCard(const wchar_t* destinationDirectory) {
    WCHAR drives[256];
    DWORD drivesSize = GetLogicalDriveStringsW(256, drives);
    if (drivesSize == 0) {
        printf("Failed to get logical drives.\n");
        return;
    }

    WCHAR* drive = drives;
    while (*drive != L'\0') {
        if (GetDriveTypeW(drive) == DRIVE_REMOVABLE) {
            WCHAR sdCard[MAX_PATH];
            wsprintfW(sdCard, L"%s", drive);
            WCHAR destination[MAX_PATH];
            wsprintfW(destination, L"%s\\", destinationDirectory);

            if (!CreateDirectoryW(destination, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
                printf("Failed to create destination directory.\n");
                return;
            }

            WIN32_FIND_DATAW findFileData;
            WCHAR searchPath[MAX_PATH];
            wsprintfW(searchPath, L"%s\\*", sdCard);
            HANDLE hFind = FindFirstFileW(searchPath, &findFileData);
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        WCHAR sourceFile[MAX_PATH];
                        wsprintfW(sourceFile, L"%s\\%s", sdCard, findFileData.cFileName);
                        WCHAR destinationFile[MAX_PATH];
                        wsprintfW(destinationFile, L"%s%s", destination, findFileData.cFileName);
                        copyFile(sourceFile, destinationFile);
                        wprintf(L"Copied %s to %s\n", findFileData.cFileName, destinationFile);
                    }
                } while (FindNextFileW(hFind, &findFileData) != 0);
                FindClose(hFind);
            } else {
                printf("Failed to enumerate files in SD card directory.\n");
                return;
            }
        }
        drive += wcslen(drive) + 1;
    }
}

int main() {
    // Example usage:
    const wchar_t* destinationDirectory = L"C:\\MyDestinationDirectory";

    copyFilesFromSDCard(destinationDirectory);

    return 0;
}
