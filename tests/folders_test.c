/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <sddl.h>
#else
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os.h"

constexpr int LEVELS = 22;
constexpr int TRIES = 4;
constexpr char TEXT[] = "deep file";

static char level[201];
static char file[256];

#ifdef _WIN32
static wchar_t chain[16384];
static size_t ends[LEVELS + 1];

static void add_name(const char *name)
{
    size_t at = wcslen(chain);

    chain[at] = L'\\';
    MultiByteToWideChar(CP_UTF8, 0, name, -1, chain + at + 1, 256);
}

static void make_chain(const char *base)
{
    wchar_t wide[4096];
    HANDLE made;
    DWORD put;

    MultiByteToWideChar(CP_UTF8, 0, base, -1, wide, 4096);
    wcscpy(chain, L"\\\\?\\");
    GetFullPathNameW(wide, 4096, chain + 4, nullptr);
    for (int i = 1; i <= LEVELS; i++) {
        add_name(level);
        CreateDirectoryW(chain, nullptr);
        ends[i] = wcslen(chain);
    }
    add_name(file);
    made = CreateFileW(chain, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    WriteFile(made, TEXT, sizeof(TEXT) - 1, &put, nullptr);
    CloseHandle(made);
}

static void remove_chain([[maybe_unused]] const char *base)
{
    DeleteFileW(chain);
    for (int i = LEVELS; i > 0; i--) {
        chain[ends[i]] = L'\0';
        RemoveDirectoryW(chain);
    }
}

static void lock([[maybe_unused]] const char *base, bool last, bool on)
{
    PSECURITY_DESCRIPTOR sd = nullptr;
    wchar_t keep = chain[ends[LEVELS]];
    const wchar_t *list_denied = L"D:P(D;;0x1;;;WD)(A;OICI;FA;;;WD)";

    if (!last || !on)
        return;
    chain[ends[LEVELS]] = L'\0';
    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(list_denied,
        SDDL_REVISION_1, &sd, nullptr))
        SetFileSecurityW(chain, DACL_SECURITY_INFORMATION, sd);
    LocalFree(sd);
    chain[ends[LEVELS]] = keep;
}
#else
static void make_chain(const char *base)
{
    int at = open(base, O_RDONLY | O_DIRECTORY);
    int next;

    for (int i = 0; i < LEVELS; i++) {
        mkdirat(at, level, 0777);
        next = openat(at, level, O_RDONLY | O_DIRECTORY);
        close(at);
        at = next;
    }
    next = openat(at, file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (write(next, TEXT, sizeof(TEXT) - 1) < 0)
        printf("folders: the deep file is not written\n");
    close(next);
    close(at);
}

static void remove_chain(const char *base)
{
    int fds[LEVELS + 1];

    fds[0] = open(base, O_RDONLY | O_DIRECTORY);
    for (int i = 1; i <= LEVELS; i++)
        fds[i] = openat(fds[i - 1], level, O_RDONLY | O_DIRECTORY);
    unlinkat(fds[LEVELS], file, 0);
    for (int i = LEVELS; i > 0; i--) {
        close(fds[i]);
        unlinkat(fds[i - 1], level, AT_REMOVEDIR);
    }
    close(fds[0]);
}

static void lock(const char *base, bool last, bool on)
{
    int at = open(base, O_RDONLY | O_DIRECTORY);
    int next;

    for (int i = 1; i < (last ? LEVELS : LEVELS / 2); i++) {
        next = openat(at, level, O_RDONLY | O_DIRECTORY);
        close(at);
        at = next;
    }
    fchmodat(at, level, on ? 0111 : 0755, 0);
    close(at);
}
#endif

static char *join_chain(const char *base)
{
    size_t size = strlen(base) + LEVELS * sizeof(level) + 1;
    char *path = malloc(size);
    size_t at = 0;

    if (!path)
        return (nullptr);
    at = (size_t)snprintf(path, size, "%s", base);
    for (int i = 0; i < LEVELS; i++)
        at += (size_t)snprintf(path + at, size - at, "%s%s",
            at ? "/" : "", level);
    return (path);
}

static bool lists_file(os_folder_t *f)
{
    bool is_folder = true;
    const char *name = os_folder_read_entry(f, &is_folder);

    return (name && !strcmp(name, file) && !is_folder
        && !os_folder_read_entry(f, &is_folder));
}

static bool read_deep(const char *path, int *refused, char *text)
{
    os_folder_t *f = nullptr;
    const char *bytes = nullptr;
    bool listed = false;
    size_t len = 0;

    for (int i = 0; i < TRIES && !f; i++) {
        f = os_folder_open(path);
        *refused += !f;
    }
    if (!f)
        return (printf("deep: the folder does not open\n"), false);
    listed = lists_file(f);
    for (int i = 0; i < TRIES && listed && !bytes; i++) {
        bytes = os_folder_read_file(f, file, &len);
        *refused += !bytes;
    }
    if (bytes)
        snprintf(text, 32, "%zu bytes: %s", len, bytes);
    else
        printf("deep: the folder does not list and read its one file\n");
    os_folder_close(f);
    return (bytes != nullptr);
}

static void read_joined(const char *whole)
{
    size_t size = strlen(whole) + sizeof(file) + 1;
    char *path = malloc(size);
    FILE *opened = nullptr;

    if (path) {
        snprintf(path, size, "%s/%s", whole, file);
        opened = fopen(path, "rb");
    }
    printf("joined: %s the file\n", opened ? "read" : "cannot read");
    if (opened)
        fclose(opened);
    free(path);
}

static int read_locked(const char *base, const char *whole, bool last)
{
    char text[32] = "";
    int refused = 0;
    bool read;

    lock(base, last, true);
    read = read_deep(whole, &refused, text);
    lock(base, last, false);
    if (read)
        printf("search only: read %s\n", text);
    return (!read);
}

static int run_mode(const char *mode, const char *base, const char *whole,
    const char *relative)
{
    char text[32] = "";
    int refused = 0;

    if (!strcmp(mode, "joined"))
        return (read_joined(whole), 1);
    if (!strcmp(mode, "current") || !strcmp(mode, "base")) {
        if (os_folder_set(!strcmp(mode, "current") ? whole : base))
            return (printf("current: refused\n"), 0);
        return (printf("current: set\n"), 0);
    }
    if (!strcmp(mode, "middle") || !strcmp(mode, "last"))
        return (read_locked(base, whole, !strcmp(mode, "last")));
    if (!read_deep(whole, &refused, text) || os_folder_set(base)
        || !read_deep(relative, &refused, text))
        return (1);
    printf("deep: %d refused, read %s\n", refused, text);
    return (0);
}

int main(int argc, char **argv)
{
    const char *mode = argc == 3 ? argv[2] : "";
    char *whole = nullptr;
    char *relative = nullptr;
    int status;

    if (strcmp(mode, "deep") && strcmp(mode, "joined")
        && strcmp(mode, "current") && strcmp(mode, "base")
        && strcmp(mode, "middle") && strcmp(mode, "last"))
        return (printf("usage: folders <folder> "
            "deep|joined|current|base|middle|last\n"), 2);
    memset(level, 'd', sizeof(level) - 1);
    memset(file, 'f', sizeof(file) - 1);
    whole = join_chain(argv[1]);
    relative = join_chain("");
    if (!whole || !relative)
        return (printf("folders: no memory\n"), 1);
    make_chain(argv[1]);
    status = run_mode(mode, argv[1], whole, relative);
    remove_chain(argv[1]);
    free(whole);
    free(relative);
    return (status);
}
