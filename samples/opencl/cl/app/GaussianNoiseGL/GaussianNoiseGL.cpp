/* ============================================================

Copyright (c) 2012 Advanced Micro Devices, Inc.  All rights reserved.

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


#include "GaussianNoiseGL.hpp"
#include <cmath>

#ifndef _WIN32
#include <GL/glx.h>
#endif //!_WIN32

#ifdef _WIN32
static HWND               gHwnd;
HDC                       gHdc;
HGLRC                     gGlCtx;
BOOL quit = FALSE;
MSG msg;
#else
GLXContext gGlCtxSep;
#define GLX_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB           0x2092
typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
Window          winSep;
Display         *displayNameSep;
XEvent          xevSep;
#endif


int verfactor = FACTOR;
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
        case 0x57: //'W'
            verfactor += 2;
            break;
        case 0x53://'S'
            verfactor -= 2;
            break;
        }
        return 0;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );

    }
}
#endif

int
GaussianNoiseGL::readInputImage(std::string inputImageName)
{
    // load input bitmap image
    std::string filePath = sampleCommon->getPath() + std::string(INPUT_IMAGE);
    inputBitmap.load(filePath.c_str());
    if(!inputBitmap.isLoaded())
    {
        std::cout << "Failed to load input image!";
        return SDK_FAILURE;
    }

    // get width and height of input image
    height = inputBitmap.getHeight();
    width = inputBitmap.getWidth();

    // allocate memory for input & output image data
    inputImageData  = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(inputImageData, "Failed to allocate memory! (inputImageData)");

    // allocate memory for output image data
    outputImageData = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(outputImageData, "Failed to allocate memory! (outputImageData)");

    // initializa the Image data to NULL
    memset(outputImageData, 0, width * height * pixelSize);

    // get the pointer to pixel data
    pixelData = inputBitmap.getPixels();
    if(pixelData == NULL)
    {
        std::cout << "Failed to read pixel Data!";
        return SDK_FAILURE;
    }

    // Copy pixel data into inputImageData
    memcpy(inputImageData, pixelData, width * height * pixelSize);

    return SDK_SUCCESS;
}


int
GaussianNoiseGL::writeOutputImage(std::string outputImageName)
{
    // copy output image data back to original pixel data
    memcpy(pixelData, outputImageData, width * height * pixelSize);

    // write the output bmp file
    if(!inputBitmap.write(outputImageName.c_str()))
    {
        sampleCommon->error("Failed to write output image!");
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int
GaussianNoiseGL::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("GaussianNoiseGL_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int
GaussianNoiseGL::initializeGLAndGetCLContext(cl_platform_id platform,
        cl_context &context,
        cl_device_id &interopDevice)
{
#ifndef _WIN32
    cl_int status = SDK_SUCCESS;
    displayNameSep = XOpenDisplay(NULL);
    int screenNumber = ScreenCount(displayNameSep);
    std::cout<<"Number of displays "<<screenNumber<<std::endl;
    XCloseDisplay(displayNameSep);
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
        displayNameSep = XOpenDisplay(0);
        int nelements;

        GLXFBConfig *fbc = glXChooseFBConfig(displayNameSep,
                                             DefaultScreen(displayNameSep),
                                             0,
                                             &nelements);

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
        XVisualInfo *vi = glXChooseVisual(displayNameSep,
                                          DefaultScreen(displayNameSep),
                                          attributeList);
        XSetWindowAttributes swa;
        swa.colormap = XCreateColormap(displayNameSep,
                                       RootWindow(displayNameSep, vi->screen),
                                       vi->visual,
                                       AllocNone);
        swa.border_pixel = 0;
        swa.event_mask = StructureNotifyMask;
        winSep = XCreateWindow(displayNameSep,
                               RootWindow(displayNameSep, vi->screen),
                               10,
                               10,
                               width,
                               height,
                               0,
                               vi->depth,
                               InputOutput,
                               vi->visual,
                               CWBorderPixel|CWColormap|CWEventMask,
                               &swa);

        XMapWindow (displayNameSep, winSep);
        std::cout << "glXCreateContextAttribsARB "
                  << (void*) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB")
                  << std::endl;
        GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

        int attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            0
        };

        GLXContext ctx = glXCreateContextAttribsARB(displayNameSep,
                         *fbc,
                         0,
                         true,
                         attribs);
        glXMakeCurrent (displayNameSep,
                        winSep,
                        ctx);
        gGlCtxSep = glXGetCurrentContext();
        cl_context_properties cpsGL[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
                                          CL_GLX_DISPLAY_KHR, (intptr_t) glXGetCurrentDisplay(),
                                          CL_GL_CONTEXT_KHR, (intptr_t) gGlCtxSep, 0
                                        };
        if (!clGetGLContextInfoKHR)
        {
            clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddressForPlatform(platform,"clGetGLContextInfoKHR");
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
        if(numDevices == 0)
        {
            glXDestroyContext(glXGetCurrentDisplay(), gGlCtxSep);
            continue;
        }
        else
        {
            //Interoperable device found
            std::cout<<"Interoperable device found "<<std::endl;
            break;
        }
    }
    cl_context_properties cpsGL[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
                                      CL_GLX_DISPLAY_KHR, (intptr_t) glXGetCurrentDisplay(),
                                      CL_GL_CONTEXT_KHR, (intptr_t) gGlCtxSep, 0
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
    // OpenGL animation code goes here
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

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60.0,
        (GLfloat)width / (GLfloat)height,
        0.1,
        10.0);
#endif
    return SDK_SUCCESS;
}

#ifdef _WIN32
int
GaussianNoiseGL::enableGLAndGetGLContext(HWND hWnd, HDC &hDC, HGLRC &hRC, cl_platform_id platform, cl_context &context, cl_device_id &interopDevice)
{
    cl_int status;
    BOOL ret = FALSE;
    DISPLAY_DEVICE dispDevice;
    DWORD deviceNum;
    int  pfmt;
    PIXELFORMATDESCRIPTOR  pfd;
    pfd.nSize           = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion        = 1;
    pfd.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL  | PFD_DOUBLEBUFFER ;
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

    DWORD connectedDisplays = 0;
    DWORD displayDevices = 0;
    int xCoordinate = 0;
    int yCoordinate = 0;
    int xCoordinate1 = 0;

    for (deviceNum = 0; EnumDisplayDevices(NULL, deviceNum, &dispDevice, 0); deviceNum++)
    {
        if (dispDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)
        {
            continue;
        }

        if(!(dispDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
        {
            std::cout << "Display device " << deviceNum << " is not connected!!" << std::endl;
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
        windowclass.lpszClassName = reinterpret_cast<LPCSTR>("BoxFilterGLSeperable");
        RegisterClass(&windowclass);

        gHwnd = CreateWindow(reinterpret_cast<LPCSTR>("BoxFilterGLSeperable"),
                             reinterpret_cast<LPCSTR>("BoxFilterGLSeperable"),
                             WS_CAPTION | WS_POPUPWINDOW,
                             isDeviceIdEnabled() ? xCoordinate1 : xCoordinate,
                             yCoordinate,
                             width,
                             height,
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
            std::cout<<"Failed to set the requested PixelFormat.\n";
            return SDK_FAILURE;
        }

        hRC = wglCreateContext(hDC);
        if(hRC == NULL)
        {
            std::cout<<"Failed to create a GL context"<<std::endl;
            return SDK_FAILURE;
        }

        ret = wglMakeCurrent(hDC, hRC);
        if(ret == FALSE)
        {
            std::cout<<"Failed to bind GL rendering context";
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
            clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddressForPlatform(platform,"clGetGLContextInfoKHR");
            if (!clGetGLContextInfoKHR)
            {
                std::cout<<"Failed to query proc address for clGetGLContextInfoKHR";
                return SDK_FAILURE;
            }
        }

        size_t deviceSize = 0;
        status = clGetGLContextInfoKHR(properties,
                                       CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
                                       0,
                                       NULL,
                                       &deviceSize);
        if(status != CL_SUCCESS)
        {
            std::cout<<"clGetGLContextInfoKHR failed!!"<<std::endl;
            return SDK_FAILURE;
        }

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
        std::cout
                << "Support for necessary OpenGL extensions missing."
                << std::endl;
        return SDK_FAILURE;
    }

    //glEnable(GL_TEXTURE_2D);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        60.0,
        (GLfloat)width / (GLfloat)height,
        0.1,
        10.0);
    return SDK_SUCCESS;
}

void
GaussianNoiseGL::disableGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}

#endif

int
GaussianNoiseGL::setupCL()
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
    int success = enableGLAndGetGLContext(gHwnd, gHdc, gGlCtx, platform, context, interopDeviceId);
    if(SDK_SUCCESS != success)
    {
        if(success == SDK_EXPECTED_FAILURE)
            return SDK_EXPECTED_FAILURE;
        return SDK_FAILURE;
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

    // getting device on which to run the sample
    // First, get the size of device list data
    size_t deviceListSize = 0;

    status = clGetContextInfo(
                 context,
                 CL_CONTEXT_DEVICES,
                 0,
                 NULL,
                 &deviceListSize);
    CHECK_OPENCL_ERROR(status, "clGetContextInfo failed.");

    int deviceCount = (int)(deviceListSize / sizeof(cl_device_id));

    devices = (cl_device_id *)malloc(deviceListSize);
    CHECK_ALLOCATION((devices), "Failed to allocate memory (devices).");

    // Now, get the device list data
    status = clGetContextInfo(context,
                              CL_CONTEXT_DEVICES,
                              deviceListSize,
                              (devices),
                              NULL);
    CHECK_OPENCL_ERROR(status, "clGetGetContextInfo failed.");

    if (dType == CL_DEVICE_TYPE_CPU)
        interopDeviceId = devices[deviceId];

    // Check for image support 
    status = deviceInfo.setDeviceInfo(interopDeviceId);
    CHECK_ERROR(status, SDK_SUCCESS, "deviceInfo.setDeviceInfo failed");
    
    if(!deviceInfo.imageSupport)
    {
        OPENCL_EXPECTED_ERROR("Error : Images are not supported on this device!\n");
    }

    // Create command queue

    cl_command_queue_properties prop = 0;
    commandQueue = clCreateCommandQueue(
                       context,
                       interopDeviceId,
                       prop,
                       &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");

    /*
     * Create texture object
     */
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Set parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*
     * Create clImage from GLTexture
     */
    outputImageBuffer = clCreateFromGLTexture(context,
                        CL_MEM_WRITE_ONLY,
                        GL_TEXTURE_2D,
                        0,
                        tex,
                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateFromGLTexture failed. (outputImageBuffer)");


    /*
    * Create and initialize memory objects
    */

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create memory object for input Image
    inputImageBuffer = clCreateBuffer(
                           context,
                           inMemFlags,
                           width * height * pixelSize,
                           NULL,
                           &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputImageBuffer)");

    // Set input data
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                 commandQueue,
                 inputImageBuffer,
                 CL_FALSE,
                 0,
                 width * height * pixelSize,
                 inputImageData,
                 0,
                 NULL,
                 &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputImageBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");



    // create a CL program using the kernel source
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("GaussianNoiseGL_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string("");
    

     if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    if(isLoadBinaryEnabled())
    {
        
        buildData.binaryName = std::string(loadBinary.c_str());
    }

   


    streamsdk::buildProgramData buildData2;
    buildData2.kernelName = std::string("GaussianNoiseGL_Kernels2.cl");
    buildData2.devices = devices;
    buildData2.deviceId = deviceId;
    buildData2.flagsStr = std::string("");

    if(isComplierFlagsSpecified())
        buildData2.flagsFileName = std::string(flags.c_str());
    if(isLoadBinaryEnabled())
    {
        
        buildData2.binaryName = std::string(loadBinary.c_str());
    }

   
    cl_program program1;
    cl_program program2;
    retValue = compileOpenCLProgram(program1, context, buildData);
    CHECK_ERROR(retValue, SDK_SUCCESS, "buildOpenCLProgram() failed");
    std::string flagsStr = std::string(buildData.flagsStr.c_str());
    retValue = compileOpenCLProgram(program2, context, buildData2);
    CHECK_ERROR(retValue, SDK_SUCCESS, "buildOpenCLProgram() failed");
    
    retValue=
        clCompileProgram(program1,
                         0,
                         0,
                         flagsStr.c_str(),
                         0,
                         0,
                         0,
                         NULL,NULL);
    CHECK_ERROR(retValue, SDK_SUCCESS, "clCompileProgram() failed");
    retValue=
        clCompileProgram(program2,
                         0,
                         0,
                         flagsStr.c_str(),
                         0,
                         0,
                         0,
                         NULL,NULL);
    CHECK_ERROR(retValue, SDK_SUCCESS, "clCompileProgram() failed");

    cl_program input_program[] = { program1 ,program2 };
    program = clLinkProgram(context,
                            0,
                            0,
                            0,
                            2,
                            input_program,
                            NULL,
                            NULL,
                            &status
                           );

    CHECK_OPENCL_ERROR(status, "clLinkProgram failed.");

    kernel = clCreateKernel(program,
                            "gaussian_transform",
                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");


    status =  kernelInfo.setKernelWorkGroupInfo(kernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    if((blockSizeX * blockSizeY) > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : "
                      << blockSizeX * blockSizeY << std::endl;
            std::cout << "Max Group Size supported on the kernel : "
                      << kernelInfo.kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelInfo.kernelWorkGroupSize << std::endl;
        }

        // Three possible cases
        if(blockSizeX > kernelInfo.kernelWorkGroupSize)
        {
            blockSizeX = kernelInfo.kernelWorkGroupSize;
            blockSizeY = 1;
        }
    }
    return SDK_SUCCESS;
}

int
GaussianNoiseGL::runCLKernels()
{
    cl_int status;



    status = clSetKernelArg(
                 kernel,
                 0,
                 sizeof(cl_mem),
                 &inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputImageBuffer)");

    //Acquire GL buffer
    cl_event acquireEvt;
    status = clEnqueueAcquireGLObjects(commandQueue,
                                       1,
                                       &outputImageBuffer,
                                       0,
                                       NULL,
                                       &acquireEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueAcquireGLObjects failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&acquireEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(acquireEvt) Failed");

    status = clSetKernelArg(
                 kernel,
                 1,
                 sizeof(cl_mem),
                 &outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputImageBuffer)");

    status = clSetKernelArg(
                 kernel,
                 2,
                 sizeof(int),
                 &verfactor);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputImageBuffer)");


    size_t globalThreads[] = {width/2, height};

    size_t localThreads[] = {blockSizeX, blockSizeY};

    cl_event ndrEvt2;
    status = clEnqueueNDRangeKernel(
                 commandQueue,
                 kernel,
                 2,
                 NULL,
                 globalThreads,
                 localThreads,
                 0,
                 NULL,
                 &ndrEvt2);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);

    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt2);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt2) Failed");

    
    //read image to host buffer
    cl_event readEvt;
    size_t origin[3] = {0,0,0};
    size_t region[3] = {width,height,1};
    status = clEnqueueReadImage(
                 commandQueue,
                 outputImageBuffer,
                 CL_FALSE,
                 origin,
                 region,
                 width * pixelSize,
                 0,
                 outputImageData,
                 0,
                 NULL,
                 &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadImage failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt) Failed");
    

    // Now OpenGL gets control of outputImageBuffer
    cl_event releaseGLEvt;
    status = clEnqueueReleaseGLObjects(commandQueue,
                                       1,
                                       &outputImageBuffer,
                                       0,
                                       NULL,
                                       &releaseGLEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReleaseGLObjects failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&releaseGLEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(releaseGLEvt) Failed");

    return SDK_SUCCESS;
}



int
GaussianNoiseGL::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize())
        return SDK_FAILURE;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory Allocation error.\n");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);
    delete iteration_option;



    streamsdk::Option* factor_option = new streamsdk::Option;
    if(!factor_option)
    {
        sampleCommon->error("Memory Allocation error.\n");
        return SDK_FAILURE;
    }
    factor_option->_sVersion = "f";
    factor_option->_lVersion = "factor";
    factor_option->_description = "Noise factor";
    factor_option->_type = streamsdk::CA_ARG_INT;
    factor_option->_value = &verfactor;

    sampleArgs->AddOption(factor_option);
    delete factor_option;



    return SDK_SUCCESS;
}

int
GaussianNoiseGL::setup()
{
    // Allocate host memory and read input image
    if(readInputImage(INPUT_IMAGE) != SDK_SUCCESS)
        return SDK_FAILURE;

    // create and initialize timers
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    cl_int retValue = setupCL();
    if(retValue != SDK_SUCCESS)
    {
        if(retValue == SDK_EXPECTED_FAILURE)
            return SDK_EXPECTED_FAILURE;
        return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    // Compute setup time
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;

}

int
GaussianNoiseGL::run()
{

    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Set kernel arguments and run kernel
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    // create and initialize timers
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);


    std::cout << "Executing kernel for " << iterations <<
              " iterations" <<std::endl;
    std::cout << "-------------------------------------------" << std::endl;


    for(int i = 0; i < iterations; i++)
    {
        // Set kernel arguments and run kernel
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    // Compute kernel time
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!verify && !quiet)
    {
        std::cout << "\nPress key w to increase the factor \n";
        std::cout << "Press key s to decrease the factor \n";
        std::cout << "Press ESC key to quit \n";
#ifndef _WIN32
        //glutMainLoop();
        XSelectInput(displayNameSep,
                     winSep,
                     ExposureMask | KeyPressMask | ButtonPressMask);
        while(1)
        {
            bool goOn = true;
            
            t1 = clock() * CLOCKS_PER_SEC;
            frameCount++;

            // Execute the kernel which applies the boxfilter
            gaussianNoise->runCLKernels();

            // Bind texture

            glBindTexture(GL_TEXTURE_2D, tex);

            // Display image using texture
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

            glMatrixMode( GL_MODELVIEW);
            glLoadIdentity();

            glViewport(0, 0, width, height);

            glBegin(GL_QUADS);

            glTexCoord2f(0.0, 0.0);
            glVertex3f(-1.0, -1.0, 0.5);

            glTexCoord2f(1.0, 0.0);
            glVertex3f(1.0, -1.0, 0.5);

            glTexCoord2f(1.0, 1.0);
            glVertex3f(1.0, 1.0, 0.5);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(-1.0, 1.0, 0.5);

            glEnd();

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            glDisable(GL_TEXTURE_2D);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            glXSwapBuffers (displayNameSep, winSep);

            t2 = clock() * CLOCKS_PER_SEC;
            totalElapsedTime += (double)(t2 - t1);
            if(frameCount && frameCount > frameRefCount)
            {
                // set GLUT Window Title
                char title[256];
                double fMs = (double)((totalElapsedTime / (double)CLOCKS_PER_SEC) / (double) frameCount);
                int framesPerSec = (int)(1.0 / (fMs / CLOCKS_PER_SEC));
#if defined (_WIN32) && !defined(__MINGW32__)
                sprintf_s(title, 256, "GaussianNoiseGL | %d fps ", framesPerSec);
#else
                sprintf(title, "GaussianNoiseGL | %d fps ", framesPerSec);
#endif
                //glutSetWindowTitle(title);
                frameCount = 0;
                totalElapsedTime = 0.0;
                XStoreName(displayNameSep, winSep, title);
            }
            
            /* handle the events in the queue */
            while (goOn)
            {
                if(XPending(displayNameSep)<=0)
                {
                    break;
                }
                XNextEvent(displayNameSep, &xevSep);
                switch(xevSep.type)
                {
                    /* exit in case of a mouse button press */
                case ButtonPress:
                    if (xevSep.xbutton.button == Button2)
                    {
                       goOn = false;
                    }
                    break;
                case KeyPress:
                    char buf[2];
                    int len;
                    KeySym keysym_return;
                    len = XLookupString(&xevSep.xkey,
                                        buf,
                                        1,
                                        &keysym_return,
                                        NULL);
                    if (len != 0)
                    {
                        if(buf[0] == (char)(27))//Escape character
                        {
                            goOn = false;
                        }
                        else if ((buf[0] == 'w') || (buf[0] == 'W'))
                        {

                            verfactor += 2;
                        }
                        else if ((buf[0] == 's') || (buf[0] == 'S'))
                        {

                            verfactor -=2;
                        }
                    }
                    break;
                }
            }
            if(!goOn)
            {
                break;
            }
        }
#else
        while(!quit)
        {
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
                t1 = clock() * CLOCKS_PER_SEC;
                frameCount++;

                // Execute the kernel which applies the boxfilter
                gaussianNoise->runCLKernels();

                // Bind  texture
                glBindTexture(GL_TEXTURE_2D, tex);

                // Display image using texture
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_LIGHTING);
                glEnable(GL_TEXTURE_2D);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

                glMatrixMode( GL_MODELVIEW);
                glLoadIdentity();

                glViewport(0, 0, width, height);

                glBegin(GL_QUADS);

                glTexCoord2f(0.0, 0.0);
                glVertex3f(-1.0, -1.0, 0.5);

                glTexCoord2f(1.0, 0.0);
                glVertex3f(1.0, -1.0, 0.5);

                glTexCoord2f(1.0, 1.0);
                glVertex3f(1.0, 1.0, 0.5);

                glTexCoord2f(0.0, 1.0);
                glVertex3f(-1.0, 1.0, 0.5);

                glEnd();

                glMatrixMode(GL_PROJECTION);
                glPopMatrix();

                glDisable(GL_TEXTURE_2D);
                glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
                glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

                SwapBuffers(gHdc);

                t2 = clock() * CLOCKS_PER_SEC;
                totalElapsedTime += (double)(t2 - t1);
                if(frameCount && frameCount > frameRefCount)
                {
                    // set GLUT Window Title
                    char title[256];
                    double fMs = (double)((totalElapsedTime / (double)CLOCKS_PER_SEC) / (double) frameCount);
                    int framesPerSec = (int)(1.0 / (fMs / CLOCKS_PER_SEC));
#if defined (_WIN32) && !defined(__MINGW32__)
                    sprintf_s(title, 256, "GaussianNoiseGL | %d fps ", framesPerSec);
#else
                    sprintf(title, "GaussianNoiseGL | %d fps ", framesPerSec);
#endif
                    //glutSetWindowTitle(title);
                    frameCount = 0;
                    totalElapsedTime = 0.0;
                    SetWindowText(gHwnd, title);
                }

            }
        }
#endif
    }

    // write the output image to bitmap file
    if(writeOutputImage(OUTPUT_SEPARABLE_IMAGE) != SDK_SUCCESS)
    {
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int
GaussianNoiseGL::cleanup()
{
#ifdef _WIN32
    wglDeleteContext(gGlCtx);
    DeleteDC(gHdc);
    gHdc = NULL;
    gGlCtx = NULL;
    DestroyWindow(gHwnd);
#endif

    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;


    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseMemObject(inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

#ifndef _WIN32 
    glXMakeCurrent(displayNameSep, None, NULL);
    glXDestroyContext(displayNameSep, gGlCtxSep);
    glDeleteTextures(1, &tex);
    XDestroyWindow(displayNameSep, winSep);
    XCloseDisplay(displayNameSep);
#endif

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // release program resources (input memory etc.)
    FREE(inputImageData);

    FREE(outputImageData);

    FREE(devices);

    return SDK_SUCCESS;
}



void
GaussianNoiseGL::GaussianNoiseCPUReference()
{

}


int
GaussianNoiseGL::verifyResults()
{


    if(verify)
    {


        float mean = 0;
        for(int i = 0; i < (int)(width * height); i++)
        {
            mean += outputImageData[i].s[0] - inputImageData[i].s[0];
        }
        mean /= (width * height * factor);

        if(fabs(mean) < 0.1)
        {
            std::cout << "Passed! \n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout << "Failed! \n" << std::endl;
            return SDK_FAILURE;
        }
    }

    return SDK_SUCCESS;
}

void
GaussianNoiseGL::printStats()
{
    std::string strArray[4] =
    {
        "Width",
        "Height",
        "Time(sec)",
        "kernelTime(sec)"
    };
    std::string stats[4];

    totalTime = setupTime + kernelTime;

    stats[0] = sampleCommon->toString(width, std::dec);
    stats[1] = sampleCommon->toString(height, std::dec);
    stats[2] = sampleCommon->toString(totalTime, std::dec);
    stats[3] = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}


// Initialize the value to NULL
GaussianNoiseGL *GaussianNoiseGL::gaussianNoise = NULL;
namespace streamsdk
{
/* Returns the path of executable being generated */
std::string
getPath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
#ifdef UNICODE
    if(!GetModuleFileName(NULL, (LPWCH)buffer, sizeof(buffer)))
        throw std::string("GetModuleFileName() failed!");
#else
    if(!GetModuleFileName(NULL, buffer, sizeof(buffer)))
        throw std::string("GetModuleFileName() failed!");
#endif
    std::string str(buffer);
    /* '\' == 92 */
    int last = (int)str.find_last_of((char)92);
#else
    char buffer[PATH_MAX + 1];
    ssize_t len;
    if((len = readlink("/proc/self/exe",buffer, sizeof(buffer) - 1)) == -1)
        throw std::string("readlink() failed!");
    buffer[len] = '\0';
    std::string str(buffer);
    /* '/' == 47 */
    int last = (int)str.find_last_of((char)47);
#endif
    return str.substr(0, last + 1);
}


int
compileOpenCLProgram(cl_program &program, const cl_context& context,  buildProgramData &buildData)
{
    cl_int status = CL_SUCCESS;
    streamsdk::SDKFile kernelFile;
    std::string kernelPath = getPath();

     std::string flagsStr = std::string(buildData.flagsStr.c_str());

    // Get additional options
    if(buildData.flagsFileName.size() != 0)
    {
        streamsdk::SDKFile flagsFile;
        std::string flagsPath = getPath();
        flagsPath.append(buildData.flagsFileName.c_str());
        if(!flagsFile.open(flagsPath.c_str()))
        {
            std::cout << "Failed to load flags file: " << flagsPath << std::endl;
            return SDK_FAILURE;
        }
        flagsFile.replaceNewlineWithSpaces();
        const char * flags = flagsFile.source().c_str();
        flagsStr.append(flags);
    }

    if(flagsStr.size() != 0)
        std::cout << "Build Options are : " << flagsStr.c_str() << std::endl;

     buildData.flagsStr = std::string(flagsStr.c_str());


    if(buildData.binaryName.size() != 0)
    {
       
         std::cout << "can not support --load ! clCreateProgramWithSource" << std::endl;
    }
    
        kernelPath.append(buildData.kernelName.c_str());
        if(!kernelFile.open(kernelPath.c_str()))//bool
        {
            std::cout << "Failed to load kernel file: " << kernelPath << std::endl;
            return SDK_FAILURE;
        }
        const char * source = kernelFile.source().c_str();
        size_t sourceSize[] = {strlen(source)};
        program = clCreateProgramWithSource(context,
                                            1,
                                            &source,
                                            sourceSize,
                                            &status);
        CHECK_OPENCL_ERROR(status, "clCreateProgramWithSource failed.");
    


    return SDK_SUCCESS;
}


}

int
main(int argc, char *argv[])
{
    GaussianNoiseGL clGaussianNoise("OpenCL GaussianNoiseGL");
    GaussianNoiseGL::gaussianNoise = &clGaussianNoise;

    if(clGaussianNoise.initialize()!= SDK_SUCCESS)
        return SDK_FAILURE;

    if(clGaussianNoise.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clGaussianNoise.isDumpBinaryEnabled())
    {
        //return clGaussianNoise.genBinaryImage();
         std::cout << "can not support --dump !" << std::endl;
         
    }
    else
    {
        int status = clGaussianNoise.setup();
        if(status != SDK_SUCCESS)
            return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

        if(clGaussianNoise.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clGaussianNoise.verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clGaussianNoise.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        clGaussianNoise.printStats();
    }

    return SDK_SUCCESS;
}


