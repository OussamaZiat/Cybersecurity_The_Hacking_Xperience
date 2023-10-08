#ifndef HELLO_WORLD_
#define HELLO_WORLD_H
void print_hello_string();
#endif //HELLO_WORLD_H

#ifdef WIN32
#include <win32functions.h>
#endif
#ifdef MACOS
#include <macosfunctions.h>
#ifdef LINUX
#include <linuxfunctions.h>