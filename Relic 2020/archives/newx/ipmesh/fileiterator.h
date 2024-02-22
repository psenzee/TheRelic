#ifndef _FILEITERATOR_H
#define _FILEITERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <direct.h>
#include <string>

// $TODO we need to figure out where to put these, how to organize them..
size_t filesize(const char *filename);
void make_path(const char *dir, const char *file, std::string &path);
bool isdirectory(const char *directory);
void change_extension(const char *filename, const char *ext, const char *newext, char *buffer);

class FileIterator
{
    _finddata_t         m_current;
    std::string         m_spec,
		                m_path;
	mutable std::string m_completepath;
    bool                m_end;
    intptr_t            m_handle;

public:

    FileIterator(const char *filespec);
    ~FileIterator();

    const char *name() const;
	const char *path() const;
    size_t      size() const;

    void        close();
    bool        next();
};

#endif  /** _FILEITERATOR_H */
