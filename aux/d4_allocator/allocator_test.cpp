#include "allocator.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>

class ClassyClass
{
public:

    inline ClassyClass() : m_id(m_count++) {}

    CLASS_NEW_DELETE();

private:

    static int m_count;
    int        m_id;
};

int ClassyClass::m_count = 0;

class ClassyClass2 : public ClassyClass
{
public:

    CLASS_NEW_DELETE();

private:

    int data[23];
};

#include "FixedString.h"
#include "FastString.h"
#include "FastBuffer.h"

typedef FixedString<256> fixed_string;

void TestFixedString()
{
    fixed_string a("hello there sir");

    fixed_string b = a;

    printf("FixedString: %s\n", b.c_str());

    fixed_string c;

    c.assign("hello\0you!", 10);

    printf("FixedString: %s\n", b.c_str());
    printf("FixedString: %s\n", c.c_str());

    fixed_string d = c;

    printf("FixedString: %s\n", d.c_str());
}

typedef FastString<5> fast_string;

void TestFastString()
{
    fast_string a("hello there sir");

    fast_string b = a;

    fast_string ba("Hi!");

    printf("FastString: %s\n", ba.c_str());

    printf("FastString: %s\n", b.c_str());

    fast_string c;

    c.assign("hello\0you!", 10);

    printf("FastString: %s\n", b.c_str());
    printf("FastString: %s\n", c.c_str());

    fast_string d = c;

    printf("FastString: %s\n", d.c_str());
}

typedef FastBuffer<5> fast_buffer;

void TestFastBuffer()
{
    fast_string a("hello there sir");

    fast_string b = a;

    fast_string ba("Hi!");

    printf("FastString: %s\n", ba.c_str());

    printf("FastString: %s\n", b.c_str());

    fast_string c;

    c.assign("hello\0you!", 10);

    printf("FastString: %s\n", b.c_str());
    printf("FastString: %s\n", c.c_str());

    fast_string d = c;

    printf("FastString: %s\n", d.c_str());
}

int main()
{
    enum { ITERATIONS = 10, COUNT = 128 * 1024 };

    ClassyClass *instances[COUNT];

    unsigned time = ::timeGetTime();

    for (int j = 0; j < ITERATIONS; j++)
    {
        for (int i = 0; i < COUNT; i++)
            instances[i] = (i % 2) == 0 ? new ClassyClass : new ClassyClass2;
    //  printf("instance %p=\n", instances[0]);
        for (int i = 0; i < COUNT; i++)
            delete instances[i];
    }

    printf("\n\nTime %d ms\n", ::timeGetTime() - time);

    _gAllocator.PrintStatistics();

    TestFixedString();
    TestFastString();

    getch();
}