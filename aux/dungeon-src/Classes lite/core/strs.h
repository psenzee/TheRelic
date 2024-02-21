#ifndef _STRS_H
#define _STRS_H

#include <string>

bool          read(const char **s, int &u);
bool          read(const char **s, float &u);
std::string   read_identifier(const char **s); // $todo we should use a version that's consistent with the two read()'s above (ie, returning a bool, and taking a mutable string as an arg)

const char   *find(const char *s, const char *f, bool ignorecase = false);
unsigned      count(const char *s, const char *f, bool ignorecase = false);
bool          ends(const char *data, const char *substr, bool ignorecase = false);
bool          starts(const char *data, const char *substr, bool ignorecase = false);

char        **split_char_of(const char *in, const char *of);
char        **split_char_of_keep_empty(const char *in, const char *of);
void          destroy(char **strlist);

const char   *extension(const char *filename);

void          replace_char(char *s, char f, char r);

bool          isspace(const char *s);
bool          isdigit(const char *s);
bool          isupper(const char *s);
bool          islower(const char *s);

unsigned      count_char_of(const char *p, char c);
unsigned      count_char_of(const char *s, const char *of);

bool          composed_of(const char *s, const char *of);
char         *first_not_of(char *p, const char *of);
char         *first_of(char *p, const char *of);
char         *last_not_of(char *p, const char *of);
char         *last_of(char *p, const char *of);

void          destructive_rtrim(char *p);
void          destructive_ltrim(char *p);
void          destructive_rtrim(char *p, const char *of);
void          destructive_ltrim(char *p, const char *of);

void          destructive_trim(char *p);
void          destructive_trim(char *p, const char *of);
void          destructive_trim(char *p, const char *of_l, const char *of_r);

char         *trim(const char *p);
char         *trim(const char *p, const char *of);
void          trim(std::string &data);
void          trim(std::string &data, const char *of);

#endif // _STRS_H