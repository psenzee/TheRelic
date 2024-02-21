#include "strs.h"

#include <string>
#include <string.h>
#include <ctype.h>

bool read(const char **s, int &u)
{
    while (isspace(**s)) (*s)++;
    const char *ends = *s;
    u = (int)strtoul(*s, const_cast<char **>(&ends), 10);
    if (ends && ends != *s)
    {
        *s = ends;
        return true;
    }
    return false;
}

bool read(const char **s, float &u)
{
    while (isspace(**s)) (*s)++;
	const char *ends = *s;
    u = (float)strtod(*s, const_cast<char **>(&ends));
	if (ends && ends != *s)
	{
		*s = ends;
        const char ind[] = "#IND";
        if (memicmp(*s, ind, sizeof(ind) - 1) == 0)
        {
            u = 0; // QNaN, just set to zero for now.. $todo set properly to QNaN, please..
            *s += sizeof(ind) - 1;
            while (**s == '0') (*s)++;
        }
		return true;
	}
	return false;
}

template <typename T>
int read_type(const char **s, T *v, int expected)
{
    int i = 0;
    T value = T(0);
    for (i = 0; i < expected; i++)
    {
        if (!read(s, value))
            break;
        v[i] = value;
    }
    return i;
}

int read(const char **s, float *v, int expected)
{
    return read_type(s, v, expected);
}

int read(const char **s, int *v, int expected)
{
    return read_type(s, v, expected);
}

std::string read_identifier(const char **s)
{
    std::string str;
    while (isspace(**s) && **s != '\n' && **s != '\r') (*s)++;
    if (!**s || **s == '\n' || **s == '\r')
        return str;
    while (!isspace(**s))
    {
        str += **s;        
        (*s)++;
    }
    return str;
}

void replace_char(char *s, char f, char r)
{
    while (*s) { if (*s == f) *s = r; s++; }
}

bool isspace(const char *s)
{
    while (*s && isspace(*s)) s++;
    return !*s;
}

bool isdigit(const char *s)
{
    while (*s && isdigit(*s)) s++;
    return !*s;
}

bool isupper(const char *s)
{
    while (*s && isupper(*s)) s++;
    return !*s;
}

bool islower(const char *s)
{
    while (*s && isupper(*s)) s++;
    return !*s;
}

bool composed_of(const char *s, const char *of)
{
    while (*s && strchr(of, *s)) s++;
    return !*s;
}

static const char *find_case(const char *s, const char *f)
{
    char c, sc;
    size_t len;

    if ((c = *f++) != 0)
    {
        len = strlen(f);
        do
        {
            do { if ((sc = *s++) == 0) return 0; }
            while (sc != c);
        }
        while (strncmp(s, f, len) != 0);
        s--;
    }
    return s;
}

static const char *find_nocase(const char *s, const char *f)
{
    char c, sc;
    size_t len;

    if ((c = tolower(*f++)) != 0)
    {
        len = strlen(f);
        do
        {
            do { if ((sc = *s++) == 0) return 0; }
            while (tolower(sc) != c);
        }
        while (strnicmp(s, f, len) != 0);
        s--;
    }
    return s;
}

const char *find(const char *s, const char *f, bool ignorecase)
{
    return ignorecase ? find_nocase(s, f) : find_case(s, f);
}

unsigned count(const char *s, const char *f, bool ignorecase)
{
    unsigned count = 0, fsz = (unsigned)strlen(f);
    while (*s && (s = find(s, f, ignorecase))) { s += fsz; count++; }
    return count;
}

bool ends(const char *data, const char *substr, bool ignorecase)
{
    if (!data || !substr) return false;
    size_t dsz = strlen(data), ssz = strlen(substr), sz = std::min(dsz, ssz);
    if (sz < ssz) return false;
    data += dsz - ssz;
    return (ignorecase ? memicmp(data, substr, sz) : memcmp(data, substr, sz)) == 0;
}

bool starts(const char *data, const char *substr, bool ignorecase)
{
    if (!data || !substr) return false;
    size_t dsz = strlen(data), ssz = strlen(substr), sz = std::min(dsz, ssz);
    if (sz < ssz) return false;
    return (ignorecase ? memicmp(data, substr, sz) : memcmp(data, substr, sz)) == 0;
}

void destroy(char **strlist)
{
    for (int i = 0; strlist[i]; i++)
        delete [] strlist[i];
    delete [] strlist;
}

char **split_char_of(const char *in, const char *of)
{
	char **splist = 0, *spstr = 0;
	int lin = (int)strlen(in), nspl = 0,
		j = 0, size = count_char_of(in, of) + 2;
	bool last = false, found = false, dchar = false;

    spstr  = new char [lin + 1];
    splist = new char * [size];
	memset(splist, 0, sizeof(char *) * size);

	for (int i = 0; i < lin; i++)
	{
        if (strchr(of, in[i]))
		{
			if (!last)
			{
				j = 0;
				if (strlen(spstr) != 0)
				{
					splist[nspl] = new char [strlen(spstr) + 1];
					strcpy(splist[nspl], spstr);
					nspl++;
				}
			}
			last = true;
		}
		else
		{
			spstr[j++] = in[i];
			spstr[j] = 0;
			last = false;
		}
	}
	if (j)
	{
		splist[nspl] = new char [strlen(spstr) + 1];
		strcpy(splist[nspl], spstr);
		nspl++;
	}

    delete [] spstr;
	return splist;
}


const char *extension(const char *filename)
{
    if (!filename) return 0;
    const char *p = (const char *)strrchr(filename, '.');
    if (p) p++;
    return p;
}

char *first_not_space(char *p)
{
    while (*p && isspace(*p)) p++;
    return *p ? p : 0;
}

char *first_not_of(char *p, const char *of)
{
    while (*p && strchr(of, *p)) p++;
    return *p ? p : 0;
}

unsigned count_char_of(const char *p, char c)
{
    unsigned count = 0;
    while (*p)
    {
        count += (c == *p) ? 1 : 0;
        p++;
    }
    return count;
}

unsigned count_char_of(const char *p, const char *of)
{
    if (!*(of + 1))
        return count_char_of(p, *of);
    unsigned count = 0;
    while (*p)
    {
        count += strchr(of, *p) != 0 ? 1 : 0;
        p++;
    }
    return count;
}

char *first_of(char *p, const char *of)
{
    if (!*(of + 1)) return strchr(p, *of);
    while (*p && !strchr(of, *p)) p++;
    return *p ? p : 0;
}

char *last_not_of(char *p, const char *of)
{
    while (*p && strchr(of, *p)) p++;
    return *p ? p : 0;
}

char *last_of(char *p, const char *of)
{
    if (!*(of + 1)) return strrchr(p, *of);
    char *q = p + strlen(p) - 1;
    while (q > p && !strchr(of, *q)) q--;
    return *q ? q : 0;
}

void destructive_rtrim(char *p)
{
    char *q = p + strlen(p) - 1;
    while (isspace(*q) && q > p) q--;
    *++q = 0;
}

void destructive_ltrim(char *p)
{
    char *q = p;
    while (isspace(*q)) q++;
    if (q != p) strcpy(p, q);    
}

void destructive_rtrim(char *p, const char *of)
{
    char *q = p + strlen(p) - 1;
    while (strchr(of, *q) && q > p) q--;
    *++q = 0;
}

void destructive_ltrim(char *p, const char *of)
{
    char *q = p;
    while (strchr(of, *q)) q++;
    if (q != p) strcpy(p, q);
}

void destructive_trim(char *p)
{
    destructive_ltrim(p);
    destructive_rtrim(p);
}

void destructive_trim(char *p, const char *of)
{
    destructive_ltrim(p, of);
    destructive_rtrim(p, of);
}

void destructive_trim(char *p, const char *of_l, const char *of_r)
{
    if (of_l) destructive_ltrim(p, of_l);
    if (of_r) destructive_rtrim(p, of_r);
}

char *trim(const char *p)
{
    if (!p) return 0;
    while (isspace(*p)) p++;
    size_t sz = strlen(p);
    char *q = new char [sz + 1];
    strcpy(q, p);
    destructive_rtrim(q);
    return q;
}

char *trim(const char *p, const char *of)
{
    if (!p) return 0;
    while (strchr(of, *p)) p++;
    size_t sz = strlen(p);
    char *q = new char [sz + 1];
    strcpy(q, p);
    destructive_rtrim(q, of);
    return q;
}

void trim(std::string &data)
{
    char *s = trim(data.c_str());
    data = s;
    delete [] s;
}

void trim(std::string &data, const char *of)
{
    char *s = trim(data.c_str(), of);
    data = s;
    delete [] s;
}