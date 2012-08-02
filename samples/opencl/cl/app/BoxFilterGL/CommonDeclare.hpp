#ifndef COMMON_DECLARE_HPP__
#define COMMON_DECLARE_HPP__
/**
* This file contains the commonn declarations for BoxFilterGLSAT and BoxFilterGLSeperable
*/
#ifdef _WIN32
#include <windows.h>
#endif

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include <SDKFile.hpp>
#include <SDKBitMap.hpp>

// GLEW and GLUT includes
#include <GL/glew.h>
#include <CL/cl_gl.h>

#ifdef _WIN32
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma warning( disable : 4996)
#endif

// Define DISPLAY_DEVICE_ACTIVE as it is not defined in MinGW
#ifdef _WIN32
#ifndef DISPLAY_DEVICE_ACTIVE
#define DISPLAY_DEVICE_ACTIVE    0x00000001
#endif
#endif

#ifndef _WIN32
#include <GL/glut.h>
#endif

typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetGLContextInfoKHR_fn)(
    const cl_context_properties *properties,
    cl_gl_context_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret);

 /**
  * Rename references to this dynamically linked function to avoid
  * collision with static link version
  */
#define clGetGLContextInfoKHR clGetGLContextInfoKHR_proc
static clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;

#endif

