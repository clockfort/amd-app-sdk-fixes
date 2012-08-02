/* ============================================================

Copyright (c) 2009 Advanced Micro Devices, Inc.  All rights reserved.

Redistribution and use of this material is permitted under the following 
conditions:

Redistributions must retain the above copyright notice and all terms of this 
license.

In no event shall anyone redistributing or accessing or using this material 
commence or participate in any arbitration or legal action relating to this 
material against Advanced Micro Devices, Inc. or any copyright holders or 
contributors. The foregoing shall survive any expiration or termination of 
this license or any agreement or access or use related to this material. 

ANY BREACH OF ANY TERM OF THIS LICENSE SHALL RESULT IN THE IMMEDIATE REVOCATION 
OF ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE THIS MATERIAL.

THIS MATERIAL IS PROVIDED BY ADVANCED MICRO DEVICES, INC. AND ANY COPYRIGHT 
HOLDERS AND CONTRIBUTORS "AS IS" IN ITS CURRENT CONDITION AND WITHOUT ANY 
REPRESENTATIONS, GUARANTEE, OR WARRANTY OF ANY KIND OR IN ANY WAY RELATED TO 
SUPPORT, INDEMNITY, ERROR FREE OR UNINTERRUPTED OPERA TION, OR THAT IT IS FREE 
FROM DEFECTS OR VIRUSES.  ALL OBLIGATIONS ARE HEREBY DISCLAIMED - WHETHER 
EXPRESS, IMPLIED, OR STATUTORY - INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED 
WARRANTIES OF TITLE, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
ACCURACY, COMPLETENESS, OPERABILITY, QUALITY OF SERVICE, OR NON-INFRINGEMENT. 
IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, PUNITIVE,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, REVENUE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED OR BASED ON ANY THEORY OF LIABILITY 
ARISING IN ANY WAY RELATED TO THIS MATERIAL, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE. THE ENTIRE AND AGGREGATE LIABILITY OF ADVANCED MICRO DEVICES, 
INC. AND ANY COPYRIGHT HOLDERS AND CONTRIBUTORS SHALL NOT EXCEED TEN DOLLARS 
(US $10.00). ANYONE REDISTRIBUTING OR ACCESSING OR USING THIS MATERIAL ACCEPTS 
THIS ALLOCATION OF RISK AND AGREES TO RELEASE ADVANCED MICRO DEVICES, INC. AND 
ANY COPYRIGHT HOLDERS AND CONTRIBUTORS FROM ANY AND ALL LIABILITIES, 
OBLIGATIONS, CLAIMS, OR DEMANDS IN EXCESS OF TEN DOLLARS (US $10.00). THE 
FOREGOING ARE ESSENTIAL TERMS OF THIS LICENSE AND, IF ANY OF THESE TERMS ARE 
CONSTRUED AS UNENFORCEABLE, FAIL IN ESSENTIAL PURPOSE, OR BECOME VOID OR 
DETRIMENTAL TO ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR 
CONTRIBUTORS FOR ANY REASON, THEN ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE 
THIS MATERIAL SHALL TERMINATE IMMEDIATELY. MOREOVER, THE FOREGOING SHALL 
SURVIVE ANY EXPIRATION OR TERMINATION OF THIS LICENSE OR ANY AGREEMENT OR 
ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE IS HEREBY PROVIDED, AND BY REDISTRIBUTING OR ACCESSING OR USING THIS 
MATERIAL SUCH NOTICE IS ACKNOWLEDGED, THAT THIS MATERIAL MAY BE SUBJECT TO 
RESTRICTIONS UNDER THE LAWS AND REGULATIONS OF THE UNITED STATES OR OTHER 
COUNTRIES, WHICH INCLUDE BUT ARE NOT LIMITED TO, U.S. EXPORT CONTROL LAWS SUCH 
AS THE EXPORT ADMINISTRATION REGULATIONS AND NATIONAL SECURITY CONTROLS AS 
DEFINED THEREUNDER, AS WELL AS STATE DEPARTMENT CONTROLS UNDER THE U.S. 
MUNITIONS LIST. THIS MATERIAL MAY NOT BE USED, RELEASED, TRANSFERRED, IMPORTED,
EXPORTED AND/OR RE-EXPORTED IN ANY MANNER PROHIBITED UNDER ANY APPLICABLE LAWS, 
INCLUDING U.S. EXPORT CONTROL LAWS REGARDING SPECIFICALLY DESIGNATED PERSONS, 
COUNTRIES AND NATIONALS OF COUNTRIES SUBJECT TO NATIONAL SECURITY CONTROLS. 
MOREOVER, THE FOREGOING SHALL SURVIVE ANY EXPIRATION OR TERMINATION OF ANY 
LICENSE OR AGREEMENT OR ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE REGARDING THE U.S. GOVERNMENT AND DOD AGENCIES: This material is 
provided with "RESTRICTED RIGHTS" and/or "LIMITED RIGHTS" as applicable to 
computer software and technical data, respectively. Use, duplication, 
distribution or disclosure by the U.S. Government and/or DOD agencies is 
subject to the full extent of restrictions in all applicable regulations, 
including those found at FAR52.227 and DFARS252.227 et seq. and any successor 
regulations thereof. Use of this material by the U.S. Government and/or DOD 
agencies is acknowledgment of the proprietary rights of any copyright holders 
and contributors, including those of Advanced Micro Devices, Inc., as well as 
the provisions of FAR52.227-14 through 23 regarding privately developed and/or 
commercial computer software.

This license forms the entire agreement regarding the subject matter hereof and 
supersedes all proposals and prior discussions and writings between the parties 
with respect thereto. This license does not affect any ownership, rights, title,
or interest in, or relating to, this material. No terms of this license can be 
modified or waived, and no breach of this license can be excused, unless done 
so in a writing signed by all affected parties. Each term of this license is 
separately enforceable. If any term of this license is determined to be or 
becomes unenforceable or illegal, such term shall be reformed to the minimum 
extent necessary in order for this license to remain in effect in accordance 
with its terms as modified by such reformation. This license shall be governed 
by and construed in accordance with the laws of the State of Texas without 
regard to rules on conflicts of law of any state or jurisdiction or the United 
Nations Convention on the International Sale of Goods. All disputes arising out 
of this license shall be subject to the jurisdiction of the federal and state 
courts in Austin, Texas, and all defenses are hereby waived concerning personal 
jurisdiction and venue of these courts.

============================================================ */



// SimpleGLSample.cpp : Defines the entry point for the console application

#include "SimpleGL.hpp"
#ifndef _WIN32
#include <GL/glx.h>
#endif

#ifdef _WIN32
static HWND   gHwnd;
HDC           gHdc;
HGLRC         gGlCtx;
BOOL quit = FALSE;
MSG msg;
#else 
GLXContext gGlCtx;
#define GLX_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB           0x2092
typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
Window 		win;
Display 	*displayName;
XEvent          xev;
#endif

float theta = 0.0f;

cl_float animate = 0.0f;            /**< Animation rate */
GLuint vertexObj;                   /**< Vertex object */
GLuint texture;                     /**< Texture */
GLuint glProgram;                   /**< GL program object */

int mouseOldX;                      /**< mouse controls */
int mouseOldY;
int mouseButtons = 0;
float rotateX    = 0.0f;
float rotateY    = 0.0f;
float translateZ = -3.0f;

clock_t t1, t2;
int frameCount = 0;
int frameRefCount = 90;
double totalElapsedTime = 0.0;

#define STRINGIFY(A) #A

const char * vertexShader = STRINGIFY(
void main()
{
    gl_TexCoord[0].st = vec2(gl_Vertex.xy) * vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    gl_Position    = ftransform();
}
);

const char * pixelShader = STRINGIFY(
uniform sampler2D tex;
void main()
{
  vec3 color         = vec3(texture2D(tex,gl_TexCoord[0].st));
  gl_FragColor       = vec4(color, 1.0);
}
);

#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        
    case WM_CREATE:
        return 0;
        
    case WM_CLOSE:
        PostQuitMessage( 0 );
        return 0;
        
    case WM_DESTROY:
        return 0;
        
    case WM_KEYDOWN:
        switch ( wParam )
        {
            
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
            
        }
        return 0;
    
    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
            
    }
}
#endif


#ifdef _WIN32
int
SimpleGLSample::enableGLAndGetGLContext(HWND hWnd, HDC &hDC, HGLRC &hRC, cl_platform_id platform, cl_context &context, cl_device_id &interopDevice)
{
    cl_int status;
    BOOL ret = FALSE;
    DISPLAY_DEVICE dispDevice;
    DWORD deviceNum;
    int  pfmt;
    PIXELFORMATDESCRIPTOR  pfd; 
    pfd.nSize           = sizeof(PIXELFORMATDESCRIPTOR); 
    pfd.nVersion        = 1; 
    pfd.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER ;
    pfd.iPixelType      = PFD_TYPE_RGBA; 
    pfd.cColorBits      = 24; 
    pfd.cRedBits        = 8; 
    pfd.cRedShift       = 0; 
    pfd.cGreenBits      = 8; 
    pfd.cGreenShift     = 0; 
    pfd.cBlueBits       = 8; 
    pfd.cBlueShift      = 0; 
    pfd.cAlphaBits      = 8;
    pfd.cAlphaShift     = 0; 
    pfd.cAccumBits      = 0; 
    pfd.cAccumRedBits   = 0; 
    pfd.cAccumGreenBits = 0; 
    pfd.cAccumBlueBits  = 0; 
    pfd.cAccumAlphaBits = 0; 
    pfd.cDepthBits      = 24; 
    pfd.cStencilBits    = 8; 
    pfd.cAuxBuffers     = 0; 
    pfd.iLayerType      = PFD_MAIN_PLANE; 
    pfd.bReserved       = 0; 
    pfd.dwLayerMask     = 0;
    pfd.dwVisibleMask   = 0; 
    pfd.dwDamageMask    = 0;

    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    dispDevice.cb = sizeof(DISPLAY_DEVICE);

    DWORD displayDevices = 0;
    DWORD connectedDisplays = 0;

    int xCoordinate = 0;
    int yCoordinate = 0;
    int xCoordinate1 = 0;

    for (deviceNum = 0; EnumDisplayDevices(NULL, deviceNum, &dispDevice , 0); deviceNum++) 
    {
        if (dispDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) 
        {
                continue;
        }

        if(!(dispDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
        {
            std::cout <<"Display device " << deviceNum << " is not connected!!" << std::endl;
            continue;
        }

        DEVMODE deviceMode;

        // initialize the DEVMODE structure
        ZeroMemory(&deviceMode, sizeof(deviceMode));
        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmDriverExtra = 0;

        EnumDisplaySettings(dispDevice.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode);

        xCoordinate = deviceMode.dmPosition.x;
        yCoordinate = deviceMode.dmPosition.y;

        WNDCLASS windowclass;

        windowclass.style = CS_OWNDC;
        windowclass.lpfnWndProc = WndProc;
        windowclass.cbClsExtra = 0;
        windowclass.cbWndExtra = 0;
        windowclass.hInstance = NULL;
        windowclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        windowclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        windowclass.lpszMenuName = NULL;
        windowclass.lpszClassName = reinterpret_cast<LPCSTR>("SimpleGL");
        RegisterClass(&windowclass);

        gHwnd = CreateWindow(reinterpret_cast<LPCSTR>("SimpleGL"), 
                              reinterpret_cast<LPCSTR>("OpenGL Texture Renderer"), 
                              WS_CAPTION | WS_POPUPWINDOW, 
                              isDeviceIdEnabled() ? xCoordinate1 : xCoordinate, 
                              yCoordinate, 
                              screenWidth, 
                              screenHeight, 
                              NULL, 
                              NULL, 
                              windowclass.hInstance, 
                              NULL);
        hDC = GetDC(gHwnd);

        pfmt = ChoosePixelFormat(hDC, 
                    &pfd);
        if(pfmt == 0) 
        {
            std::cout << "Failed choosing the requested PixelFormat.\n";
            return SDK_FAILURE;
        }

        ret = SetPixelFormat(hDC, pfmt, &pfd);
        if(ret == FALSE) 
        {
            std::cout << "Failed to set the requested PixelFormat.\n";
            return SDK_FAILURE;
        }
        
        hRC = wglCreateContext(hDC);
        if(hRC == NULL) 
        {
            std::cout << "Failed to create a GL context"<<std::endl;
            return SDK_FAILURE;
        }

        ret = wglMakeCurrent(hDC, hRC);
        if(ret == FALSE) 
        {
            std::cout << "Failed to bind GL rendering context";
            return SDK_FAILURE;
        }	
        displayDevices++;

        cl_context_properties properties[] = 
        {
                CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
                CL_GL_CONTEXT_KHR,   (cl_context_properties) hRC,
                CL_WGL_HDC_KHR,      (cl_context_properties) hDC,
                0
        };
        
        if (!clGetGLContextInfoKHR) 
        {
               clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddressForPlatform(platform, "clGetGLContextInfoKHR");
               if (!clGetGLContextInfoKHR) 
               {
                    std::cout << "Failed to query proc address for clGetGLContextInfoKHR";
                    return SDK_FAILURE;
               }
        }
        
        size_t deviceSize = 0;
        status = clGetGLContextInfoKHR(properties, 
                                      CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
                                      0, 
                                      NULL, 
                                      &deviceSize);
        CHECK_OPENCL_ERROR(status, "clGetGLContextInfoKHR failed!!");

        if (deviceSize == 0) 
        {
            // no interopable CL device found, cleanup
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(hRC);
            DeleteDC(hDC);
            hDC = NULL;
            hRC = NULL;
            DestroyWindow(gHwnd);
            // try the next display
            continue;
        }
        else 
        {
            if (deviceId == 0)
            {
                ShowWindow(gHwnd, SW_SHOW);
                //Found a winner 
                break;
            }
            else if (deviceId != connectedDisplays)
            {
                connectedDisplays++;
                wglMakeCurrent(NULL, NULL);
                wglDeleteContext(hRC);
                DeleteDC(hDC);
                hDC = NULL;
                hRC = NULL;
                DestroyWindow(gHwnd);
                if (xCoordinate >= 0)
                {
                    xCoordinate1 += deviceMode.dmPelsWidth;
                    // try the next display
                }
                else 
                {
                    xCoordinate1 -= deviceMode.dmPelsWidth;
                }

                continue;
            } 
            else 
            {
                ShowWindow(gHwnd, SW_SHOW);
                //Found a winner 
                break;
            }
        }

    }

    if (!hRC || !hDC) 
    {
       OPENCL_EXPECTED_ERROR("OpenGL interoperability is not feasible.");
    }

    cl_context_properties properties[] = 
    {
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
        CL_GL_CONTEXT_KHR,   (cl_context_properties) hRC,
        CL_WGL_HDC_KHR,      (cl_context_properties) hDC,
        0
    };


    if (deviceType.compare("gpu") == 0)
    {
        status = clGetGLContextInfoKHR( properties, 
                                        CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
                                        sizeof(cl_device_id), 
                                        &interopDevice, 
                                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetGLContextInfoKHR failed!!");

        // Create OpenCL context from device's id
        context = clCreateContext(properties,
                                         1,
                                         &interopDevice,
                                         0,
                                         0,
                                         &status);
        CHECK_OPENCL_ERROR(status, "clCreateContext failed!!");
        std::cout<<"Interop Device Id "<<interopDevice<<std::endl;
    }
    else 
    {
        context = clCreateContextFromType(
                    properties,
                    CL_DEVICE_TYPE_CPU,
                    NULL,
                    NULL,
                    &status);
        CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed!!");
    }

    // OpenGL animation code goes here

    // GL init
    glewInit();
    if (! glewIsSupported("GL_VERSION_2_0 " "GL_ARB_pixel_buffer_object"))
    {
          std::cerr << "Support for necessary OpenGL extensions missing."
                    << std::endl;
          return SDK_FAILURE;
    }

    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60.0,
        (GLfloat)WINDOW_WIDTH / (GLfloat) WINDOW_HEIGHT,
        0.1,
        10.0);

    return SDK_SUCCESS;
}

void 
SimpleGLSample::disableGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}
#endif

int 
SimpleGLSample::setupSimpleGL()
{
    pos = (cl_float*)malloc(meshWidth * meshHeight * sizeof(cl_float4));
    CHECK_ALLOCATION(pos, "Failed to allocate host memory. (pos)");

    memset(pos, 0, 4 * meshWidth * meshHeight * sizeof(cl_float));
    return SDK_SUCCESS;
}

int SimpleGLSample::initializeGLAndGetCLContext(cl_platform_id platform, 
                        cl_context &context, 
                        cl_device_id &interopDevice)
{
#ifndef _WIN32
    cl_int status = SDK_SUCCESS;
    displayName = XOpenDisplay(NULL);
    int screenNumber = ScreenCount(displayName);
    std::cout<<"Number of displays "<<screenNumber<<std::endl;
    XCloseDisplay(displayName);

    for (int i = 0; i < screenNumber; i++)
    {
        if (isDeviceIdEnabled())
        {
            if (i < deviceId)
            {
            continue;
            }
        }
        char disp[100];
        sprintf(disp, "DISPLAY=:0.%d", i);
        putenv(disp);
        displayName = XOpenDisplay(0);	
        int nelements;
        GLXFBConfig *fbc = glXChooseFBConfig(displayName, DefaultScreen(displayName), 0, &nelements);
        static int attributeList[] = { GLX_RGBA, 
                           GLX_DOUBLEBUFFER, 
                           GLX_RED_SIZE, 
                           1, 
                           GLX_GREEN_SIZE, 
                           1, 
                           GLX_BLUE_SIZE, 
                           1, 
                           None 
                         };
        XVisualInfo *vi = glXChooseVisual(displayName, 
                    DefaultScreen(displayName),
                    attributeList);

        XSetWindowAttributes swa;
        swa.colormap = XCreateColormap(displayName, RootWindow(displayName, vi->screen), vi->visual, AllocNone);
        swa.border_pixel = 0;
        swa.event_mask = StructureNotifyMask;
        win = XCreateWindow(displayName,
                                RootWindow(displayName, vi->screen),
                                10,
                                10,
                                WINDOW_WIDTH,
                                WINDOW_HEIGHT,
                                0,
                                vi->depth,
                                InputOutput,
                                vi->visual,
                                CWBorderPixel|CWColormap|CWEventMask,
                                &swa);

        XMapWindow (displayName, win);

        std::cout << "glXCreateContextAttribsARB " 
          << (void*) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB") 
          << std::endl;

        GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC) 
                                    glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

        int attribs[] = {
                        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                        0
                        };

        GLXContext ctx = glXCreateContextAttribsARB(displayName, *fbc, 0, true, attribs);
        glXMakeCurrent (displayName, win, ctx);	
        gGlCtx = glXGetCurrentContext();
        cl_context_properties cpsGL[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
                                          CL_GLX_DISPLAY_KHR, (intptr_t) glXGetCurrentDisplay(),
                                          CL_GL_CONTEXT_KHR, (intptr_t) gGlCtx, 0
                                        };
        if (!clGetGLContextInfoKHR)
        {
            clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddressForPlatform(platform, "clGetGLContextInfoKHR");
            if (!clGetGLContextInfoKHR)
                {
                    std::cout << "Failed to query proc address for clGetGLContextInfoKHR";
                }
        }

        size_t deviceSize = 0;
        status = clGetGLContextInfoKHR(cpsGL,
                                   CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
                                   0,
                                   NULL,
                                   &deviceSize);
        CHECK_OPENCL_ERROR(status, "clGetGLContextInfoKHR failed!!");

        int numDevices = (deviceSize / sizeof(cl_device_id));
        std::cout<<"Number of interoperable devices "<<numDevices<<std::endl;

        if (numDevices == 0)
        {
            //glXMakeCurrent(glXGetCurrentDisplay(), glXGetCurrentDrawable(), 0 );
            glXDestroyContext(glXGetCurrentDisplay(), gGlCtx);
            continue;
        }
        else 
        {
            //Interoperable device found
            break;
        }
    }
    cl_context_properties cpsGL[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
                                      CL_GLX_DISPLAY_KHR, (intptr_t) glXGetCurrentDisplay(),
                                      CL_GL_CONTEXT_KHR, (intptr_t) gGlCtx, 0
                                    };
    if (deviceType.compare("gpu") == 0)
    {
        status = clGetGLContextInfoKHR( cpsGL,
                                    CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
                                    sizeof(cl_device_id),
                                    &interopDeviceId,
                                    NULL);
        CHECK_OPENCL_ERROR(status, "clGetGLContextInfoKHR failed!!");

        std::cout<<"Interop Device ID is "<<interopDeviceId<<std::endl; 

        // Create OpenCL context from device's id
        context = clCreateContext(cpsGL,
                                 1,
                                 &interopDeviceId,
                                 0,
                                 0,
                                 &status);
        CHECK_OPENCL_ERROR(status, "clCreateContext failed.");
    }
    else 
    {
        context = clCreateContextFromType(cpsGL,
                    CL_DEVICE_TYPE_CPU,
                    NULL,
                    NULL,
                    &status);
        CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed!!");
    }
    // GL init
    glewInit();
    if (! glewIsSupported("GL_VERSION_2_0 " "GL_ARB_pixel_buffer_object"))
    {
          std::cout << "Support for necessary OpenGL extensions missing."
                    << std::endl;
          return SDK_FAILURE;
    }

    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60.0,
        (GLfloat)WINDOW_WIDTH / (GLfloat) WINDOW_HEIGHT,
        0.1,
        10.0);
#endif
    return SDK_SUCCESS;
}


int 
SimpleGLSample::setupCL()
{
    cl_int status = CL_SUCCESS;

    cl_device_type dType;
    if(deviceType.compare("cpu") == 0)
    {
        dType = CL_DEVICE_TYPE_CPU;
    }
    else //deviceType = "gpu" 
    {
        dType = CL_DEVICE_TYPE_GPU;
        if(isThereGPU() == false)
        {
            std::cout << "GPU not found. Falling back to CPU device" << std::endl;
            dType = CL_DEVICE_TYPE_CPU;
        }
    }

    /*
     * Have a look at the available platforms and pick either
     * the AMD one if available or a reasonable default.
     */
    cl_platform_id platform = NULL;
    int retValue = sampleCommon->getPlatform(platform, platformId, isPlatformEnabled());
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::getPlatform() failed");

    // Display available devices.
    retValue = sampleCommon->displayDevices(platform, dType);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::displayDevices() failed");

#ifdef _WIN32
    retValue = enableGLAndGetGLContext(gHwnd, gHdc, gGlCtx, platform, context, interopDeviceId);
    if (retValue != SDK_SUCCESS)
    {
        return retValue;
    }
#else
    retValue = initializeGLAndGetCLContext(platform, 
                   context, 
                   interopDeviceId);
    if (retValue != SDK_SUCCESS)
    {
        return retValue;
    }
#endif 
    if (dType == CL_DEVICE_TYPE_CPU)
    {
        // getting device on which to run the sample
        status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
        CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");
        interopDeviceId = devices[deviceId];
    }
     // Create command queue
    commandQueue = clCreateCommandQueue(
        context,
        interopDeviceId,
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(interopDeviceId);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    // Create Vertex buffer object
    glGenBuffers(1, &vertexObj);
    glBindBuffer(GL_ARRAY_BUFFER, vertexObj);

    // initialize buffer object
    unsigned int size = meshWidth * meshHeight * sizeof(cl_float4);
    cl_float4 *fdata = (cl_float4 *)malloc(size);
    memset(fdata, 0x1, size);
    glBufferData(GL_ARRAY_BUFFER, size, (GLvoid *)fdata, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create OpenCL buffer from GL VBO
    posBuf = clCreateFromGLBuffer(context, CL_MEM_WRITE_ONLY, vertexObj, &status);
    CHECK_OPENCL_ERROR(status, "clCreateFromGLBuffer failed. (posBuf)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("SimpleGL_Kernels.cl");
    buildData.devices = &interopDeviceId;
    buildData.deviceId = 0;
    buildData.flagsStr = std::string("");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

    // get a kernel object handle for a kernel with the given name
    kernel = clCreateKernel(
        program,
        "sineWave",
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    // Load texture
    if(loadTexture(&texture) == SDK_FAILURE)
    {
        std::cout << "ERROR: Failed to load texture " << std::endl;
        return SDK_FAILURE;
    }

    // Compile Vertex and Pixel shaders and create glProgram
    glProgram = compileProgram(vertexShader, pixelShader);
    if(!glProgram)
    {
        std::cout << "ERROR: Failed to create glProgram " << std::endl;
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int 
SimpleGLSample::setup()
{
    if (setupSimpleGL() != SDK_SUCCESS)
        return SDK_FAILURE;

    cl_int retValue = setupCL();
    if (retValue != SDK_SUCCESS)
    return retValue;

    return SDK_SUCCESS;
}

int 
SimpleGLSample::setupCLKernels()
{
     cl_int status;

    // Set appropriate arguments to the kernel

    // Output buffer for position vector
    status = clSetKernelArg(
        kernel,
        0,
        sizeof(cl_mem),
        (void*)&posBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (posBuf)");

    // Mesh Width
    status = clSetKernelArg(
        kernel,
        1,
        sizeof(cl_uint),
        (void *)&meshWidth);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (meshWidth)");

    // Mesh Height 
    status = clSetKernelArg(
        kernel,
        2,
        sizeof(cl_uint),
        (void *)&meshHeight);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (meshHeight)");

    // animation value
    status = clSetKernelArg(
        kernel,
        3,
        sizeof(cl_float),
        (void *)&animate);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (animate)");

    // Check group size against group size returned by kernel
    status = kernelInfo.setKernelWorkGroupInfo(kernel, interopDeviceId);
        /*clGetKernelWorkGroupInfo(kernel,
        interopDeviceId,
        CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t),
        &kernelWorkGroupSize,
        0);*/
    CHECK_OPENCL_ERROR(status, "kernelInfo.setKernelWorkGroupInfo failed.");

    if(groupSize > kernelInfo.kernelWorkGroupSize)
    {
        std::cout << "Out of Resources!" << std::endl;
        std::cout << "Group Size specified : " << groupSize << std::endl;
        std::cout << "Max Group Size supported on the kernel : " 
                  << kernelInfo.kernelWorkGroupSize << std::endl;
        std::cout << "Falling back to " << kernelInfo.kernelWorkGroupSize << std::endl;
        groupSize = kernelInfo.kernelWorkGroupSize;
    }

    return SDK_SUCCESS;
}

int 
SimpleGLSample::executeKernel()
{	
    cl_int status = CL_SUCCESS;
    // Set local and global work group sizes
    size_t localX = (size_t)sqrt((double)groupSize);
    size_t globalWorkSize[2] = {meshWidth, meshHeight};
    size_t localWorkSize[2] = {groupSize, 1};

    // Acquire GL buffer
    status = clEnqueueAcquireGLObjects(commandQueue, 1, &posBuf, 0, 0, NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueAcquireGLObjects failed.");

    // Set kernel argument animate with updated value
    status = clSetKernelArg(kernel, 3, sizeof(float), &animate);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed(animate).");

    // Execute kernel on given device
    cl_event  eventND[1];
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, 0, eventND );
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush() failed");

    status = sampleCommon->waitForEventAndRelease(&eventND[0]);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(eventND[0]) Failed");

    // Release GL buffer
    status = clEnqueueReleaseGLObjects(commandQueue, 1, &posBuf, 0, 0, 0);
    CHECK_OPENCL_ERROR(status, "clEnqueueReleaseGLObjects failed.");

    status = clFinish(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFinish failed.");

    return SDK_SUCCESS;
}

void
SimpleGLSample::SimpleGLCPUReference(void)
{
    for(unsigned int i = 0; i < meshHeight; ++i)
    {
        for(unsigned int j = 0; j < meshWidth; ++j)
        {
            unsigned int x = j;
            unsigned int y = i;

            // calculate uv coordinates
            float u = x / (float)meshWidth;
            float v = y / (float)meshHeight;
            u = u * 2.0f - 1.0f;
            v = v * 2.0f - 1.0f;

            // calculate simple sine wave pattern
            float freq = 4.0f;
            float w = sin(u * freq + animate) * cos(v * freq + animate) * 0.5f;

            // write output vertex
            refPos[i * meshWidth * 4 + j * 4 + 0] = u;
            refPos[i * meshWidth * 4 + j * 4 + 1] = w;
            refPos[i * meshWidth * 4 + j * 4 + 2] = v;
            refPos[i * meshWidth * 4 + j * 4 + 3] = 1.0f;
        }
    }
}

int 
SimpleGLSample::initialize()
{
     // Call base class Initialize to get default configuration
    if (this->SDKSample::initialize() != SDK_SUCCESS)
            return SDK_FAILURE;

    return SDK_SUCCESS;
}

int 
SimpleGLSample::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    glBindBuffer(1, vertexObj);
    glDeleteBuffers(1, &vertexObj);

    status = clReleaseMemObject(posBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(posBuf)");

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed. (context)");
    return SDK_SUCCESS;
}

int 
SimpleGLSample::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("SimpleGL_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int 
SimpleGLSample::verifyResults()
{
     if(verify)
    {
        // it overwrites the input array with the output
        refPos = (cl_float*)malloc(meshWidth * meshHeight * sizeof(cl_float4));
        CHECK_ALLOCATION(refPos, "Failed to allocate host memory. (refPos)");

        memset(refPos, 0, meshWidth * meshHeight * sizeof(cl_float4));
        SimpleGLCPUReference();

        // compare the results and see if they match
        if(compareArray(pos, refPos, meshWidth * meshHeight * 4))
        {
            std::cout << "Failed\n" << std::endl;
            return SDK_FAILURE;
        }
        else
        {
            std::cout << "Passed!\n" << std::endl;
            return SDK_SUCCESS;
        }
    }
    return SDK_SUCCESS;
}

int 
SimpleGLSample::run()
{
    int status = 0;
    // Arguments are set and execution call is enqueued on command buffer
    if (setupCLKernels() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(!quiet && !verify)
    {
#ifdef _WIN32
        // program main loop
        while (!quit)
        {
            // check for messages
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                // handle or dispatch messages
                if (msg.message == WM_QUIT) 
                {
                    quit = TRUE;
                } 
                else 
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            } 
            else 
            {
                // OpenGL animation code goes here		

                t1 = clock() * CLOCKS_PER_SEC;
                frameCount++;

                // run OpenCL kernel to generate vertex positions
                executeKernel();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // set view matrix
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glTranslatef(0.0, 0.0, translateZ);
                glRotatef(rotateX, 1.0, 0.0, 0.0);
                glRotatef(rotateY, 0.0, 1.0, 0.0);

                // render from the vbo
                glBindBuffer(GL_ARRAY_BUFFER, vertexObj);
                glVertexPointer(4, GL_FLOAT, 0, 0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);

                glUseProgram(glProgram);
                glEnableClientState(GL_VERTEX_ARRAY);
                glColor3f(1.0, 0.0, 0.0);
                glDrawArrays(GL_POINTS, 0, WINDOW_WIDTH * WINDOW_HEIGHT);
                glDisableClientState(GL_VERTEX_ARRAY);

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glFinish();

                SwapBuffers(gHdc);

                t2 = clock() * CLOCKS_PER_SEC;
                totalElapsedTime += (double)(t2 - t1);
                if(frameCount && frameCount > frameRefCount)
                {

                    // set  Window Title
                    char title[256];
                    double fMs = (double)((totalElapsedTime / (double)CLOCKS_PER_SEC) / (double) frameCount);
                    int framesPerSec = (int)(1.0 / (fMs / CLOCKS_PER_SEC));
            #if defined (_WIN32) && !defined(__MINGW32__)
                    sprintf_s(title, 256, "OpenCL SimpleGL | %d fps ", framesPerSec);
            #else 
                    sprintf(title, "OpenCL SimpleGL | %d fps ", framesPerSec);
            #endif
                    SetWindowText(gHwnd, title);
                    frameCount = 0;
                    totalElapsedTime = 0.0;
                }

                animate += 0.01f;
            }
            
        }
#else 
    // OpenGL animation code goes here              
    XSelectInput(displayName, 
    win, 
    ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | Button1MotionMask);
    while(1)
    {
        /* handle the events in the queue */
        while (XPending(displayName) > 0)
        {
            XNextEvent(displayName, &xev);
            switch (xev.type)
            {
                /* exit in case of a mouse button press */
                case ButtonPress:
            if (xev.xbutton.button == Button2)//Exit when middle mouse button is pressed
                    {
                        glXMakeCurrent(displayName, None, NULL);
                        glXDestroyContext(displayName, gGlCtx);
                        XDestroyWindow(displayName, win);
                        XCloseDisplay(displayName);
                        exit(0);
                    }
                    else if (xev.xbutton.button == Button1)//When left mouse buttomn is pressed
                    {
            mouseButtons = 1 ;
                        //Get the x, y values of mouse pointer
                        mouseOldX = xev.xbutton.x;
                        mouseOldY = xev.xbutton.y;
                    }
                    break; 	
                
        case ButtonRelease:
                    if (xev.xbutton.button == Button1)//When left mouse button is released
                    {
            mouseButtons = 0;
            mouseOldX = xev.xbutton.x;
                        mouseOldY = xev.xbutton.y;
                    }
                    break;
        case MotionNotify:
             float dx, dy;
                 dx = xev.xbutton.x - mouseOldX;
                 dy = xev.xbutton.y - mouseOldY;

                 if (mouseButtons)
                 {
                rotateX += static_cast<float>(dy * 0.2);
                rotateY += static_cast<float>(dx * 0.2);
                 }

                 mouseOldX = xev.xbutton.x;
                 mouseOldY = xev.xbutton.y;
            break;
        case KeyPress:
                    char buf[2];
                    int len;
                    KeySym keysym_return;
                    len = XLookupString(&xev.xkey, buf, 1, &keysym_return, NULL);
                   
                    if ( len != 0 )
            {
            if(buf[0] == (char)(27))//Escape character
            {
                glXMakeCurrent(displayName, None, NULL);
                            glXDestroyContext(displayName, gGlCtx);
                            XDestroyWindow(displayName, win);
                            XCloseDisplay(displayName);
                            exit(0);			
            }
                    }
                    break;
            }
        }
        t1 = clock() * CLOCKS_PER_SEC;
        frameCount++;

        // run OpenCL kernel to generate vertex positions
        executeKernel();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set view matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0, 0.0, translateZ);
        glRotatef(rotateX, 1.0, 0.0, 0.0);
        glRotatef(rotateY, 0.0, 1.0, 0.0);

        // render from the vbo
        glBindBuffer(GL_ARRAY_BUFFER, vertexObj);
        glVertexPointer(4, GL_FLOAT, 0, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(glProgram);
        glEnableClientState(GL_VERTEX_ARRAY);
        glColor3f(1.0, 0.0, 0.0);
        glDrawArrays(GL_POINTS, 0, WINDOW_WIDTH * WINDOW_HEIGHT);
        glDisableClientState(GL_VERTEX_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glFinish();
        glXSwapBuffers (displayName, win);
        t2 = clock() * CLOCKS_PER_SEC;
        totalElapsedTime += (double)(t2 - t1);
        if(frameCount && frameCount > frameRefCount)
        {
            // set Window Title
            char title[256];
            double fMs = (double)((totalElapsedTime / (double)CLOCKS_PER_SEC) / (double) frameCount);
            int framesPerSec = (int)(1.0 / (fMs / CLOCKS_PER_SEC));
        #if defined (_WIN32) && !defined(__MINGW32__)
            sprintf_s(title, 256, "OpenCL SimpleGL | %d fps ", framesPerSec);
        #else
            sprintf(title, "OpenCL SimpleGL | %d fps ", framesPerSec);
        #endif
            XStoreName(displayName, win, title);
            frameCount = 0;
            totalElapsedTime = 0.0;
         }

         animate += 0.01f;
    }
#endif
    }
    else
    {
        if(verify || timing)
        {
            executeKernel();

            // Copy vertex buffer values to host ptr for correctness verification
            glBindBufferARB(GL_ARRAY_BUFFER, vertexObj); 

            // map the buffer object into client's memory
            float* ptr = (float*)glMapBufferARB(GL_ARRAY_BUFFER, GL_READ_ONLY_ARB);

            if(ptr == NULL)
            {
                std::cout << "glMapBufferARB failed to map.";
                return SDK_FAILURE;
            }
 
            memcpy(pos, ptr, meshWidth * meshHeight * sizeof(cl_float4));
            glUnmapBufferARB(GL_ARRAY_BUFFER); 

        }
        if(!quiet)
            sampleCommon->printArray<cl_float>("Output", pos, 256, 1);
    }

    return SDK_SUCCESS;
}

GLuint SimpleGLSample::compileProgram(const char * vsrc, const char * psrc)
{
    GLint err = 0;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint pixelShader  = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vsrc, 0);
    glShaderSource(pixelShader, 1, &psrc, 0);

    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &err);

    if(!err)
    {
        char temp[256];
        glGetShaderInfoLog(vertexShader, 256, 0, temp);
        std::cout << "Failed to compile shader: " << temp << std::endl;
        return SDK_FAILURE;
    }

    glCompileShader(pixelShader); 

    glGetShaderiv(pixelShader, GL_COMPILE_STATUS, &err);

    if(!err)
    {
        char temp[256];
        glGetShaderInfoLog(pixelShader, 256, 0, temp);
        std::cout << "Failed to compile shader: " << temp << std::endl;
        return SDK_FAILURE;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, pixelShader);

    glLinkProgram(program);

    // check if program linked
    err = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &err);

    if(!err)
    {
        char temp[256];
        glGetProgramInfoLog(program, 256, 0, temp);
        std::cout << "Failed to link program: " << temp << std::endl;
        glDeleteProgram(program);
        program = 0;
    }

    return program;
}

int SimpleGLSample::loadTexture(GLuint * texture)
{
    std::string imagePath = sampleCommon->getPath();
    imagePath.append("SimpleGL.bmp");

    streamsdk::SDKBitMap image(imagePath.c_str());
    if (!image.isLoaded())
    {
        std::cout << "ERROR: could not load bitmap : " << imagePath.c_str() << std::endl;
        return SDK_FAILURE;
    }

    glGenTextures(1, texture );

    glBindTexture(GL_TEXTURE_2D, *texture);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        image.getWidth(),
        image.getHeight(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image.getPixels());

    return SDK_SUCCESS;
}

int
SimpleGLSample::compareArray(const float* mat0,
                    const float* mat1,
                    unsigned int size)
{
    const float epsilon = (float)1e-2;
    for (unsigned int i = 0; i < size; ++i)
    {
        float val0 = mat0[i];
        float val1 = mat1[i];

        float diff = (val1 - val0);
        if (fabs(val1) > epsilon)
        {
            diff /= val0;
        }

        if(fabs(diff) > epsilon)
            return (fabs(diff) > epsilon);
    }

    return SDK_SUCCESS;
}

SimpleGLSample::~SimpleGLSample()
{
    // release program resources
    FREE(pos);
    FREE(refPos);
    FREE(devices);
}

SimpleGLSample *SimpleGLSample::simpleGLSample = NULL;


int 
main(int argc, char* argv[])
{
    int status = 0;

    SimpleGLSample glSampleObj("Simple OpenGL Sample");	

    SimpleGLSample::simpleGLSample = &glSampleObj;

    if (glSampleObj.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (glSampleObj.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(glSampleObj.isDumpBinaryEnabled())
    {
        return glSampleObj.genBinaryImage();
    }

    status = glSampleObj.setup();
    if(status != SDK_SUCCESS)
        return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

    if (glSampleObj.run() != SDK_SUCCESS)
        return SDK_FAILURE;

#ifdef _WIN32
    glSampleObj.disableGL(gHwnd, gHdc, gGlCtx);
#endif

    if (glSampleObj.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (glSampleObj.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    return SDK_SUCCESS;
}

