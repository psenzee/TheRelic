#include "file.h"

#include <fcntl.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace file
{

enum { MAX_PATH = 1024 };
    
size_t file_size(int fd);

buffer_t read_file(const char *filename)
{
    buffer_t data;
    if (filename)
    {
        int handle = 0;
#ifdef WIN32
        handle = ::_open(filename, O_RDONLY | O_BINARY);
#else
        handle = ::open(filename, O_RDONLY);
#endif
        if (handle >= 0)
        {
            data.size = (size_t)file_size(handle);
            data.data = new char [data.size + 1]; // the + 1 we null terminate, convenient if it's a string.
            size_t sz = ::read(handle, data.data, (unsigned)data.size);
            if (sz != data.size)
            {
                delete [] (char *)data.data;
                data.data = 0;
                data.size = 0;
            }
            ((char *)data.data)[sz] = 0; // null terminate
            ::close(handle);
        }
    }
    return data;
}

std::string read_file_string(const char *filename)
{
    buffer_t data = read_file(filename);
    std::string s((const char *)data.data, data.size);
    delete [] (char *)data.data;
    return s;
}

bool status(const char *fn, struct platform_stat *st)
{
    if (!fn || !*fn || !st)
        return false;
    memset(st, 0, sizeof(struct platform_stat));
#ifdef WIN32
    // see MSDN Q168439
    size_t len = strlen(fn);
    char str[MAX_PATH] = { 0 };
    strcpy(str, fn);
    if (str[len - 1] == '\\')
        str[len - 1] = 0;
    return _stat(str, st) == 0;
#else
    return stat(fn, st) == 0;
#endif
}

bool status(int fd, struct stat *st)
{
    if (!st) return false;
    memset(st, 0, sizeof(struct stat));
    return fstat(fd, st) == 0;
}

bool is_directory(const char *path)
{
    if (!path || !*path) return false;
    struct platform_stat st;
    return status(path, &st) && (st.st_mode & S_IFDIR) > 0;
}

bool is_directory(int fd)
{
    if (!fd) return false;
    struct stat st;
    return status(fd, &st) && (st.st_mode & S_IFDIR) > 0;
}

time_t time_modified(const char *fn)
{
    if (!fn || !*fn) return 0;
    struct platform_stat st;
    return !status(fn, &st) ? 0 : st.st_mtime;
}

time_t time_modified(int fd)
{
    if (!fd) return 0;
    struct stat st;
    return !status(fd, &st) ? 0 : st.st_mtime;
}

bool path_exists(const char *path)
{
    return access(path, 0) == 0;
}

bool path_exists(int fd)
{
    struct stat st;
    return fd && status(fd, &st);
}

bool read_only(const char *path)
{
    return access(path, 4) == 0;
}

size_t file_size(const char *fn)
{
    struct platform_stat st;
    return fn && *fn && status(fn, &st) ? st.st_size : 0;
}

size_t file_size(int fd)
{
    struct stat st;
    return fd && status(fd, &st) ? st.st_size : 0;
}

void make_path(const char *dir, const char *file, std::string &path)
{
    path = (dir ? dir : "");
    if (!path.empty())
    {
        char ch = path[path.size() - 1];
        if (ch != '\\' && ch != '/')
            path += "\\";
    }
    path += (file ? file : "");
    for (size_t i = 0, sz = path.size(); i < sz; i++)
        if (path[i] == '/')
            path[i] = '\\';
}

}