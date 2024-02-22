//
//  main.cpp
//  collider64
//
//  Created by Paul Senzee on 12/22/19.
//  Copyright © 2019 Paul Senzee. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "strs_c.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct Node64
{
    enum Type { POINTER = 0, OUT = -1, IN = -2, PARTITION = -3, _ = 0x7fffffff };

    Type    type;
    float   plane[4];
    int64_t neg,
            pos;

    inline static bool IsType(const Node64 *n)
    {
        intptr_t ni = (intptr_t)n;
        return ni < 0 && ni >= PARTITION;
    }
    inline static Type GetType(const Node64 *n)
    {
        intptr_t ni = (intptr_t)n;
        return (ni < 0 && ni >= PARTITION) ? (Type)ni : POINTER;
    }
    inline static bool IsInOut(Type type)
    {
        return type == IN || type == OUT;
    }
};

struct Node32
{
    enum Type { POINTER = 0, OUT = -1, IN = -2, PARTITION = -3, _ = 0x7fffffff };

    int32_t type;
    float   plane[4];
    int32_t neg,
            pos;

    inline static bool IsType(const int32_t n)
    {
        int32_t ni = n;
        return ni < 0 && ni >= PARTITION;
    }
    inline static Type GetType(const int32_t n)
    {
        int32_t ni = n;
        return (ni < 0 && ni >= PARTITION) ? (Type)ni : POINTER;
    }
    inline static bool IsInOut(int32_t type)
    {
        return type == IN || type == OUT;
    }
    
    inline char *type_str(int32_t type)
    {
        char pointer[64];
        memset(pointer, 0, sizeof(pointer));
        const char *str = 0;
        switch (type) {
            case POINTER:
                str = "POINTER";
                break;
            case OUT:
                str = "OUT";
                break;
            case IN:
                str = "IN";
                break;
            case PARTITION:
                str = "PARTITION";
                break;
            case _:
                str = "_";
                break;
            default:
                sprintf(pointer, "%d", type);
                str = pointer;
                break;
        }
        return strs_strdup(str);
    }
    
    inline void print()
    {
        char *str = 0;
        str = type_str(this->type);
        printf("T: %s\n", str);
        free(str);
        printf("p: %.2f %.2f %.2f %.2f\n",
               this->plane[0], this->plane[1], this->plane[2], this->plane[3]
               );
        str = type_str(this->neg);
        printf("-: %s\n", str);
        free(str);
        str = type_str(this->pos);
        printf("+: %s\n", str);
        free(str);
        printf("\n");
    }
};

struct Tree32
{
    Node32 *nodes;
    int     count;
  
    inline void print()
    {
        for (int i = 0; i < this->count; i++)
        {
            (this->nodes)[i].print();
        }
    }
};

struct Tree64
{
    Node64 *nodes;
    int     count;
};

Tree32 ReadCollider(const char *filename)
{
    file::buffer_t buffer = file::read_file(filename);
    Tree32 tree;
    tree.nodes = 0;
    tree.count = 0;
    if (!buffer.data || !buffer.size)
    {
        printf("Unable to read collider '%s'!\n", filename);
        return tree;
    }
    tree.count = (int)buffer.size / sizeof(Node32);
    tree.nodes = (Node32 *)buffer.data;
    char *raw   = (char *)buffer.data;
    /*
    for (int i = 0; i < tree.count; i++)
    {
        Node32 *n = &(tree.nodes[i]);
        if (n->neg != Node32::IN && n->neg != Node32::OUT && n->neg != Node32::PARTITION && n->neg != Node32::_) {
            n->neg = n->neg / sizeof(Node32);
        }
        if (n->pos != Node32::IN && n->pos != Node32::OUT && n->pos != Node32::PARTITION && n->pos != Node32::_) {
            n->pos = n->pos / sizeof(Node32);
        }
    }
     */
    // fixup
    /*
    for (int i = 0; i < nodeCount; i++)
    {
        Node &n = nodes[i];
        if (!Node::IsType(n.neg)) n.neg = (Node *)(raw + (intptr_t)n.neg);
        if (!Node::IsType(n.pos)) n.pos = (Node *)(raw + (intptr_t)n.pos);
    }
    */
    if (tree.nodes)
    {
        printf("Read collider '%s'\n", filename);
    }
    return tree;
}

Tree64 convertTo64(Tree32 tree32)
{
    Tree64 tree64;
    tree64.count = tree32.count;
    tree64.nodes = new Node64 [tree32.count];
    memset((void *)tree64.nodes, 0, tree64.count * sizeof(Node64));
    for (int i = 0; i < tree32.count; i++)
    {
        Node32 *n32 = &(tree32.nodes[i]);
        Node64 *n64 = &(tree64.nodes[i]);
        if (n32->neg != Node32::IN && n32->neg != Node32::OUT &&
            n32->neg != Node32::PARTITION && n32->neg != Node32::_) {
            n64->neg = (n32->neg / sizeof(Node32)) * sizeof(Node64);
        } else {
            n64->neg = n32->neg;
        }
        if (n32->pos != Node32::IN && n32->pos != Node32::OUT &&
            n32->pos != Node32::PARTITION && n32->pos != Node32::_) {
            n64->pos = (n32->pos / sizeof(Node32)) * sizeof(Node64);
        } else {
            n64->pos = n32->pos;
        }
    }
    
    return tree64;
}

int main(int argc, const char * argv[]) {
    //char path[1024];
    //printf("dir: %s\n", getcwd(path, sizeof(path) - 1));
    if (argc < 2) {
        printf("Please supply a 32-bit collider file path on the command line\n");
        exit(1);
    }
    const char *collider_path = argv[1];
    Tree32 collider = ReadCollider(collider_path);
    if (collider.nodes == 0) {
        exit(1);
    }
    Tree64 collider64 = convertTo64(collider);
    char new_path[1024];
    sprintf(new_path, "%s64", collider_path);
    int handle = ::open(new_path, O_WRONLY | O_CREAT);
    if (handle == -1) {
        printf("Error: %s\n", file::fopen_error());
        exit(1);
    }
    ::write(handle, collider64.nodes, collider64.count * sizeof(Node64));
    ::close(handle);
    collider.print();
    return 0;
}
