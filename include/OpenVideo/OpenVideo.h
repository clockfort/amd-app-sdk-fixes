// OpenVideo.h : main header file for the OpenVideo DLL
//

#pragma once
#ifndef __OPEN_VIDEO_H__
#define __OPEN_VIDEO_H__


#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

/////////////////////////////////////////////////////////////////
// Macros 
/////////////////////////////////////////////////////////////////


#define FREE(ptr, free_val) \
    if (ptr != free_val)    \
    {                       \
        free(ptr);          \
        ptr = free_val;     \
    }

#ifdef _DEBUG
#define RETURN_DEBUG(errString) \
    {                           \
        std::cout << (errString) << std::endl;  \
        return false;           \
    }
#else // _DEBUG
#define RETURN_DEBUG(errString) \
    return false;
#endif // _DEBUG


#ifdef ASSERT
#undef ASSERT
#endif

#ifdef _DEBUG
#define ASSERT(x, y)            \
{                               \
    if (!(x)) {                  \
        printf(y);              \
        OutputDebugStringA(y);  \
        __debugbreak();         \
    }                           \
}
#else //!_DEBUG
#define ASSERT(x,y) \
{                   \
    if (!(x)) {      \
        printf(y);  \
    }               \
}
#endif //!_DEBUG


VOID InfoPrint(PCTSTR format, ...);

#ifdef _DEBUG
#define INFOMSG(x) InfoPrint x
#else // !_DEBUG
#define INFOMSG(x) 
#endif // !_DEBUG

#endif // #define __OPEN_VIDEO_H__
