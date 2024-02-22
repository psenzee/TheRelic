#include "fileiterator.h"

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include <string>

#define MAX_PATH 1024

static inline bool ends(const char *s, const char *ends)
{
    size_t ssz = strlen(s), esz = strlen(ends);
    return esz <= ssz && stricmp(s + (ssz - esz), ends) == 0;
}

void change_extension(const char *filename, const char *ext, const char *newext, char *buffer)
{
    size_t fnsz = strlen(filename);
    strcpy(buffer, filename);
    if (ends(filename, ext))
        fnsz -= strlen(ext);
    strcpy(buffer + fnsz, newext);
}

size_t filesize(const char *filename)
{
    int id = open(filename, O_RDONLY);
    if (!id) return 0;
    size_t sz = filelength(id);
    close(id);
    return sz;
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

static void split_path(const char *filepath, char *path, char *file)
{
    const char *p = filepath + strlen(filepath), *end = p;
    while (p > filepath && *p != '/' && *p != '\\')
        p--;
    *path = 0; *file = 0;
    if (*p == '/' || *p == '\\')
        p++;
    if (p != filepath)
    {
        // has a path
        memcpy(path, filepath, p - filepath);
        path[p - filepath] = 0;
    }
    if (p != end)
    {
        // has a filename
        memcpy(file, p, end - p);
        file[end - p] = 0;
    }
}

// there must be a *much* better and faster way of doing this..
bool isdirectory(const char *directory)
{
    char path[1024];
    char *cwd = _getcwd(path, 1023);
    if (!cwd) throw std::exception("unable to _getcwd");
    if (_chdir(directory) == 0)
    {
        if (_chdir(cwd) < 0)
            throw std::exception("unable to _chdir");
        return true;
    }
    return false;
}

FileIterator::FileIterator(const char *filespec) : m_path(""), m_spec(""), m_handle(0), m_end(false)
{
    char path[MAX_PATH], file[MAX_PATH];
    split_path(filespec, path, file);
    m_path = path;
    m_spec = file;
    if (!strlen(file))
        m_spec = "*.*";
    memset(&m_current, 0, sizeof(m_current));
}

FileIterator::~FileIterator()
{
    close();
}

const char *FileIterator::name() const
{
    return m_current.name;
}

const char *FileIterator::path() const
{
    make_path(m_path.c_str(), name(), m_completepath);
    return m_completepath.c_str();
}

size_t FileIterator::size() const
{
    return (size_t)m_current.size;
}

void FileIterator::close()
{
    if (m_handle)
    {
        memset(&m_current, 0, sizeof(m_current));
        _findclose(m_handle);
        m_handle = 0;
        m_end = true;
    }
}

bool FileIterator::next()
{
    if (m_end)
        return false;
    intptr_t result = 0;
    if (m_handle)
        result = _findnext(m_handle, &m_current);
    else
    {
        make_path(m_path.c_str(), m_spec.c_str(), m_completepath);
        result = m_handle = _findfirst(m_completepath.c_str(), &m_current);
    }
    if (result < 0)
    {
        close();
        return false;
    }
    return true;
}