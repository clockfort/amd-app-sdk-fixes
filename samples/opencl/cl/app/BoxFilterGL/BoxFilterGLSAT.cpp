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


#include "BoxFilterGLSAT.hpp"
#include <cmath>

#ifndef _WIN32
#include <GL/glx.h>
#endif //!_WIN32

#ifdef _WIN32
static HWND               gHwndSAT;
HDC                       gHdcSAT;
HGLRC                     gGlCtxSAT;
BOOL quitSAT = FALSE;
MSG msgSAT;
#else 
GLXContext gGlCtxSAT;
#define GLX_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB           0x2092
typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
Window          winSAT;
Display         *displayNameSAT;
XEvent          xevSAT;
#endif

int filterWidthSAT = FILTER;                /**< Width of filter */
int filterWidthSATTmp = FILTER;
#ifdef _WIN32
LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			filterWidthSATTmp += 2;
			break;
		case 0x53://'S'
			filterWidthSATTmp -= 2;
			break;
		}
		if(filterWidthSATTmp<0)
		{
			filterWidthSAT = -filterWidthSATTmp;
		}
		else
		{
			filterWidthSAT = filterWidthSATTmp;
		}
		return 0;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );

	}
}
#endif

int
BoxFilterGLSAT::readInputImage(std::string inputImageName)
{
    // load input bitmap image 
    std::string filePath = sampleCommon->getPath() + std::string(INPUT_IMAGE);
    inputBitmap.load(filePath.c_str());

    // error if image did not load 
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

    // allocate memory for verification output 
    verificationOutput = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(verificationOutput, "verificationOutput heap allocation failed!");

    // initialize the data to NULL 
    memset(verificationOutput, 0, width * height * pixelSize);

    // Calculate passes required for SAT computation 
    cl_float tempN = (cl_float)((cl_float)log((float)width) / 
        log((float)rHorizontal));
    cl_float tempM = (cl_float)((cl_float)log((float)height) / 
        log((float)rVertical));

    // Calculate horizontal passes 
    while(fabs(tempN - (cl_int)tempN) > 1e-7)
    {
        if(rHorizontal < 2)
        {
            std::cout << 
                "Width should be a multiple of power of 2 atleast!" << 
                std::endl;
            return SDK_FAILURE;
        }
        rHorizontal /= 2;
        tempN = (cl_float)((cl_float)log((float)width) /
            log((float)rHorizontal));
    }
    n = (cl_uint)tempN;

    // Calculate vertical passes 
    while(fabs(tempM - (cl_int)tempM) > 1e-7)
    {
        if(rVertical < 2)
        {
            std::cout <<
                "Height should be a multiple of power of 2 atleast!" << 
                std::endl;
            return SDK_FAILURE;
        }
        rVertical /= 2;
        tempM = (cl_float)((cl_float)log((float)width) 
            / log((float)rVertical));
    }
    m = (cl_uint)tempM;

    return SDK_SUCCESS;

}


int
BoxFilterGLSAT::writeOutputImage(std::string outputImageName)
{
    // copy output image data back to original pixel data 
    memcpy(pixelData, outputImageData, width * height * pixelSize);

    // write the output bmp file 
    if(!inputBitmap.write(outputImageName.c_str()))
    {
        std::cout << "Failed to write output image!";
        return SDK_FAILURE;
    }
    return SDK_SUCCESS;
}

int 
BoxFilterGLSAT::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("BoxFilterGL_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int 
BoxFilterGLSAT::setupCL()
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
    int success = enableGLAndGetGLContext(gHwndSAT, gHdcSAT, gGlCtxSAT, platform, context, interopDeviceId);
    if (SDK_SUCCESS != success)
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
    // Create command queue 
    cl_command_queue_properties prop = 0;
    commandQueue = clCreateCommandQueue(
        context,
        interopDeviceId,
        CL_QUEUE_PROFILING_ENABLE,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");

    /*
    * Create and initialize memory objects
    */

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create memory object for input Image 
    inputImageBuffer = clCreateBuffer(context,
                                      inMemFlags,
                                      width * height * pixelSize,
                                      0,
                                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputImageBuffer)");

    // Write Data to inputImageBuffer 
    cl_event wrtevt;
    status = clEnqueueWriteBuffer(commandQueue,
                                  inputImageBuffer,
                                  CL_FALSE,
                                  0,
                                  width * height * sizeof(cl_uchar4),
                                  inputImageData,
                                  0,
                                  0,
                                  &wrtevt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputImageBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush Command failed. (inputImageBuffer)");


    // Create memory object for temp0 
    tempImageBuffer0 = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        width * height * sizeof(cl_uint4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (tempImageBuffer0)");

    // Create memory object for temp1 
    tempImageBuffer1 = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        width * height * sizeof(cl_uint4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (tempImageBuffer1)");

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
     * Create pixel-buffer object
     */
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_ARRAY_BUFFER, pbo);

    // initialize buffer object
    unsigned int size = width * height * sizeof(cl_uchar4);

    // buffer data
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create OpenCL buffer from GL PBO 
    outputImageBuffer = clCreateFromGLBuffer(context, 
                                             CL_MEM_WRITE_ONLY, 
                                             pbo, 
                                             &status);
    CHECK_OPENCL_ERROR(status, "clCreateFromGLBuffer failed. (outputImageBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("BoxFilterGL_Kernels.cl");
    buildData.devices = &interopDeviceId;
    buildData.deviceId = 0;
    buildData.flagsStr = std::string("");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, 0, "sampleCommon::buildOpenCLProgram() failed");


    // get a kernel object handle for a kernel with the given name 
    kernel = clCreateKernel(program, "box_filter",&status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(box)");

    // Create a kernel object for first horizontal pass of SAT computation 
    horizontalSAT0 = clCreateKernel(program, "horizontalSAT0", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(horizontalSAT0)");

    // Create a kernel object for rest horizontal pass of SAT computation 
    horizontalSAT = clCreateKernel(program, "horizontalSAT", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(horizontalSAT)");

    // Create a kernel object for vertical pass of SAT computation 
    verticalSAT = clCreateKernel(program, "verticalSAT", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(verticalSAT)");

    status =  kernelInfo.setKernelWorkGroupInfo(kernel, interopDeviceId);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    status =  kernelInfoHSAT0.setKernelWorkGroupInfo(horizontalSAT0, interopDeviceId);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    status =  kernelInfoHSAT.setKernelWorkGroupInfo(horizontalSAT, interopDeviceId);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    status =  kernelInfoVSAT.setKernelWorkGroupInfo(verticalSAT, interopDeviceId);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    size_t minA = min(kernelInfoVSAT.kernelWorkGroupSize, kernelInfoHSAT.kernelWorkGroupSize);
    size_t minB = min(kernelInfo.kernelWorkGroupSize, kernelInfoHSAT0.kernelWorkGroupSize);
    kernelWorkGroupSize = min(minA, minB);

    if((blockSizeX * blockSizeY) > kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : "
                << blockSizeX * blockSizeY << std::endl;
            std::cout << "Max Group Size supported on the kernel : "
                << kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
        }

        // Three possible cases 
        if(blockSizeX > kernelWorkGroupSize)
        {
            blockSizeX = kernelWorkGroupSize;
            blockSizeY = 1;
        }
    }

    status = sampleCommon->waitForEventAndRelease(&wrtevt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(wrtevt) Failed");

    return SDK_SUCCESS;
}

#ifdef _WIN32
int
BoxFilterGLSAT::enableGLAndGetGLContext(HWND hWnd, HDC &hDC, HGLRC &hRC, cl_platform_id platform, cl_context &context, cl_device_id &interopDevice)
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

    DWORD displayDevices = 0;
    DWORD connectedDisplays = 0;
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
            std::cout << "Display device " << deviceNum << " is not connected!!"<<std::endl;
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
        windowclass.lpfnWndProc = WndProc1;
        windowclass.cbClsExtra = 0;
        windowclass.cbWndExtra = 0;
        windowclass.hInstance = NULL;
        windowclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        windowclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        windowclass.lpszMenuName = NULL;
        windowclass.lpszClassName = reinterpret_cast<LPCSTR>("BoxFilterGLSAT");
        RegisterClass(&windowclass);

        gHwndSAT = CreateWindow(reinterpret_cast<LPCSTR>("BoxFilterGLSAT"), 
                              reinterpret_cast<LPCSTR>("BoxFilterGLSAT"), 
                              WS_CAPTION | WS_POPUPWINDOW, 
                              isDeviceIdEnabled() ? xCoordinate1 : xCoordinate, 
                              yCoordinate, 
                              width, 
                              height, 
                              NULL, 
                              NULL, 
                              windowclass.hInstance, 
                              NULL);
        hDC = GetDC(gHwndSAT);

        pfmt = ChoosePixelFormat(hDC, 
                    &pfd);
        if(pfmt == 0) 
        {
            std::cout<<"Failed choosing the requested PixelFormat.\n";
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
        if(status != CL_SUCCESS)
        {
            std::cout << "clGetGLContextInfoKHR failed!!"<<std::endl;
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
            DestroyWindow(gHwndSAT);
            // try the next display
            continue;
        }
        else 
        {
            if (deviceId == 0)
            {
                ShowWindow(gHwndSAT, SW_SHOW);
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
                DestroyWindow(gHwndSAT);
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
                ShowWindow(gHwndSAT, SW_SHOW);
                //Found a winner 
                break;
            }
        }
    }

     if (!hRC || !hDC) 
     {
        std::cout << "OpenGL interoperability is not feasible." << std::endl;
        return SDK_EXPECTED_FAILURE;
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
BoxFilterGLSAT::disableGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );
}

#endif

int
BoxFilterGLSAT::runSatKernel(cl_kernel kernel,
                        cl_mem *input,
                        cl_mem *output,
                        cl_uint pass,
                        cl_uint r)
{
    int status;

    // Setup arguments to the kernel 

    // input buffer 
    status = clSetKernelArg(kernel, 
                            0, 
                            sizeof(cl_mem), 
                            input);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (input)");

    // output buffer 
    status = clSetKernelArg(kernel, 
                            1, 
                            sizeof(cl_mem), 
                            output);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (satHorizontalPass[0])");

    // current pass 
    status = clSetKernelArg(kernel, 
                            2, 
                            sizeof(cl_uint), 
                            &pass);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (pass)");

    // Memory Fetches for SAT computation 
    status = clSetKernelArg(kernel, 
                            3, 
                            sizeof(cl_uint), 
                            &r);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (r)");

    // width of input image 
    status = clSetKernelArg(kernel, 
                            4, 
                            sizeof(cl_uint), 
                            &width);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width)");

    size_t globalThreads[] = {width, height};
    size_t localThreads[] = {blockSizeX, blockSizeY};

    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(commandQueue,
                                    kernel,
                                    2,
                                    NULL,
                                    globalThreads,
                                    localThreads,
                                    0,
                                    NULL,
                                    &ndrEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    cl_int eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        ndrEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
    }

    cl_ulong startTime=0;
    cl_ulong endTime=0;
    // Display proiling info 
    status = clGetEventProfilingInfo(ndrEvt,
                                     CL_PROFILING_COMMAND_START,
                                     sizeof(cl_ulong),
                                     &startTime,
                                     0);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo(CL_PROFILING_COMMAND_START) failed.");

    status = clGetEventProfilingInfo(ndrEvt,
                                     CL_PROFILING_COMMAND_END,
                                     sizeof(cl_ulong),
                                     &endTime,
                                     0);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo(CL_PROFILING_COMMAND_END) failed.");
    
    status = clReleaseEvent(ndrEvt);
    CHECK_OPENCL_ERROR(status, "clReleaseEvent failed. (*event)");

    cl_double elapsedTime = (endTime - startTime) * 1e-6;

    if(!quiet)
        std::cout <<" \t" << elapsedTime <<" ms"<<std::endl;

    return SDK_SUCCESS;
}

int 
BoxFilterGLSAT::runBoxFilterKernel()
{
    int status;
    cl_mem *input;

    if((n - 1 + m) % 2)
        input = &tempImageBuffer1;
    else
        input = &tempImageBuffer0;

    // Setup arguments to the kernel 

    // input buffer 
    status = clSetKernelArg(kernel, 
                            0, 
                            sizeof(cl_mem), 
                            input);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (input)");

    // Acquire outputImageBuffer from GL 
    cl_event acquireEvt;
    status = clEnqueueAcquireGLObjects(commandQueue, 
                                       1, 
                                       &outputImageBuffer, 
                                       0, 
                                       0,
                                       &acquireEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueAcquireGLObjects failed.");
    
    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&acquireEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    // output buffer 
    status = clSetKernelArg(kernel, 
                            1, 
                            sizeof(cl_mem), 
                            &outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(output)");

    // current pass 
    status = clSetKernelArg(kernel, 
                            2, 
                            sizeof(cl_uint), 
                            &filterWidthSAT);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (pass)");

    size_t globalThreads[] = 
    {
        width,
        height
    };
    size_t localThreads[] = {blockSizeX, blockSizeY};

    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(commandQueue,
                                    kernel,
                                    2,
                                    NULL,
                                    globalThreads,
                                    localThreads,
                                    0,
                                    NULL,
                                    &ndrEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");
    
    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

        // Read back the value 
       status = clEnqueueReadBuffer(commandQueue,
            outputImageBuffer,
            CL_TRUE,
            0,
            width * height * sizeof(cl_uchar4),
            outputImageData,
            0,
            0,
            0);
        CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    // Now OpenGL gets control of outputImageBuffer 
    cl_event releaseGLEvt;
    status = clEnqueueReleaseGLObjects(commandQueue, 
                                       1, 
                                       &outputImageBuffer, 
                                       0, 
                                       0, 
                                       &releaseGLEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReleaseGLObjects failed.");
    
    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&releaseGLEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(releaseGLEvt) Failed");

    return SDK_SUCCESS;
}


int 
BoxFilterGLSAT::runCLKernels()
{
    cl_uint pass = 0;

    if(!quiet)
    {
        std::cout << "Computing SAT \t\t Time" << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        std::cout << "HorizontalPass[" << pass << "]";
    }

    /* SAT : Summed Area table computation 
     * This is done in log(N) time
     * This is done in 2 steps : HorizontalPasses and VerticalPasses 
     * This will compute a data structure(i.e SAT) to do fast blur on the image
     * as it allows to apply filter of any size with only 4 fetches from SAT
     * Hence this algorithm is ideal for interactive applications */

    //Compute Horizontal pass = 0
    if(!runSatKernel(horizontalSAT0, 
                     &inputImageBuffer, 
                     &tempImageBuffer0,
                     pass,
                     rHorizontal))
    {
        cl_mem *inputHorizontal = &tempImageBuffer0;
        cl_mem *outputHorizontal = &tempImageBuffer1;

        //Do rest of the horizontal passes
        for(pass = 1; pass < n; pass++)
        {
            if(!quiet)
                std::cout << "HorizontalPass[" << pass << "]";

            if(!runSatKernel(horizontalSAT, 
                             inputHorizontal, 
                             outputHorizontal,
                             pass,
                             rHorizontal))
            {
            }

            // Swap buffers : tempImageBuffer0 and tempImageBuffer1 
            cl_mem *temp = inputHorizontal;
            inputHorizontal = outputHorizontal;
            outputHorizontal = temp;
        }
    }

    cl_mem *inputVertical;
    cl_mem *outputVertical;

    if(n % 2)
    {
        inputVertical = &tempImageBuffer0;
        outputVertical = &tempImageBuffer1;
    }
    else
    {
        inputVertical = &tempImageBuffer1;
        outputVertical = &tempImageBuffer0;
    }

    //Do vertical passes
    for(pass = 0; pass < m; pass++)
    {
        if(!quiet)
            std::cout << "VerticalPass[" << pass << "]";

            if(!runSatKernel(verticalSAT,
                             inputVertical,
                             outputVertical,
                             pass,
                             rVertical))
            {
            }

            // Swap Buffers here 
            cl_mem *temp = inputVertical;
            inputVertical = outputVertical;
            outputVertical = temp;
    }

    return SDK_SUCCESS;
}



int 
BoxFilterGLSAT::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize() != SDK_SUCCESS)
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

    streamsdk::Option* filter_width = new streamsdk::Option;
    CHECK_ALLOCATION(filter_width, "Memory Allocation error.\n");

    filter_width->_sVersion = "x";
    filter_width->_lVersion = "width";
    filter_width->_description = "Filter width";
    filter_width->_type = streamsdk::CA_ARG_INT;
    filter_width->_value = &filterWidthSAT;

    sampleArgs->AddOption(filter_width);
    delete filter_width;

    // Dummy command line option for sep 
    int dummy_sep_variable = false;
    streamsdk::Option* dummySep = new streamsdk::Option;
    CHECK_ALLOCATION(dummySep, "Memory Allocation error.\n");

    dummySep->_sVersion = "sep";
    dummySep->_lVersion = "separable";
    dummySep->_description = "Flag for separable version";
    dummySep->_type = streamsdk::CA_NO_ARGUMENT;
    dummySep->_value = &dummy_sep_variable;

    sampleArgs->AddOption(dummySep);
    delete dummySep;

    // Dummy command line option for SAT 
    int dummy_sat_variable = false;
    streamsdk::Option* dummySAT = new streamsdk::Option;
    CHECK_ALLOCATION(dummySAT, "Memory Allocation error.\n");

    dummySAT->_sVersion = "sat";
    dummySAT->_lVersion = "sat";
    dummySAT->_description = "Flag for SAT version";
    dummySAT->_type = streamsdk::CA_NO_ARGUMENT;
    dummySAT->_value = &dummy_sat_variable;

    sampleArgs->AddOption(dummySAT);
    delete dummySAT;
    return SDK_SUCCESS;
}

int 
BoxFilterGLSAT::setup()
{   
     // Allocate host memory and read input image 
    if(readInputImage(INPUT_IMAGE) != SDK_SUCCESS)
        return SDK_FAILURE;

    // create and initialize timers 
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);
    
    int status = setupCL();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_EXPECTED_FAILURE;
        return SDK_FAILURE;
    }    

    sampleCommon->stopTimer(timer);
    // Compute setup time 
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;

}

int 
BoxFilterGLSAT::run()
{
	// create and initialize timers 
	int timer = sampleCommon->createTimer();
	sampleCommon->resetTimer(timer);

	if(!quiet)
	{
		std::cout << "Executing kernel for " << iterations << 
			" iterations" <<std::endl;
		std::cout << "-------------------------------------------" << std::endl;
	}

	sampleCommon->startTimer(timer);

	for(int i = 0; i < iterations; i++)
	{
		// Set kernel arguments and run kernel 
		if(runCLKernels() != SDK_SUCCESS)
			return SDK_FAILURE;

		if(runBoxFilterKernel() != SDK_SUCCESS)
			return SDK_FAILURE;
	}

	sampleCommon->stopTimer(timer); 
	// Compute kernel time 
	kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

	if(!verify && !quiet)
	{
		std::cout << "\nPress key w to increase the filter size \n";
		std::cout << "Press key s to decrease the filter size \n";
		std::cout << "Press ESC key to quit \n";
		// Start the main glut loop  
#ifndef _WIN32
		XSelectInput(displayNameSAT,
			winSAT,
			ExposureMask | KeyPressMask | ButtonPressMask);
		while(1)
		{

			t1 = clock() * CLOCKS_PER_SEC;
			frameCount++;

			// Execute the kernel which applies the boxfilter
			if(runBoxFilterKernel() != SDK_SUCCESS)
				return SDK_FAILURE;

			// Bind PBO and texture 
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
			glBindTexture(GL_TEXTURE_2D, tex);

			// Copy pixels from pbo to texture 
			glTexSubImage2D(GL_TEXTURE_2D, 
				0,
				0,
				0,
				width,
				height,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				NULL);
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
			glXSwapBuffers (displayNameSAT, winSAT);

			t2 = clock() * CLOCKS_PER_SEC;
			totalElapsedTime += (double)(t2 - t1);
			if(frameCount && frameCount > frameRefCount)
			{
				// set GLUT Window Title
				char title[256];
				double fMs = (double)((totalElapsedTime / (double)CLOCKS_PER_SEC) / (double) frameCount);
				int framesPerSec = (int)(1.0 / (fMs / CLOCKS_PER_SEC));
#if defined (_WIN32) && !defined(__MINGW32__)
				sprintf_s(title, 256, "BoxFilterGL SAT | %d fps ", framesPerSec);
#else
				sprintf(title, "BoxFilterGL SAT | %d fps ", framesPerSec);
#endif
				//glutSetWindowTitle(title);
				frameCount = 0;
				totalElapsedTime = 0.0;
				XStoreName(displayNameSAT, winSAT, title);
			}
			/* handle the events in the queue */
			bool goOn = true;
			while (goOn)
			{
				if(XPending(displayNameSAT) <= 0)
				{
					break;
				}
				XNextEvent(displayNameSAT, &xevSAT);
				switch(xevSAT.type)
				{
					/* exit in case of a mouse button press */
				case ButtonPress:
					if(xevSAT.xbutton.button == Button2)
					{
						goOn = false;
					}
					break;
				case KeyPress:
					char buf[2];
					int len;
					KeySym keysym_return;
					len = XLookupString(&xevSAT.xkey,
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
							filterWidthSATTmp += 2;
						}
						else if ((buf[0] == 's') || (buf[0] == 'S'))
						{
							filterWidthSATTmp -= 2;
						}
						if(filterWidthSATTmp<0)
						{
							filterWidthSAT = -filterWidthSATTmp;
						}
						else
						{
							filterWidthSAT = filterWidthSATTmp;
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
		while(!quitSAT)
		{
			if (PeekMessage(&msgSAT, NULL, 0, 0, PM_REMOVE))
			{
				// handle or dispatch messages
				if (msgSAT.message == WM_QUIT) 
				{
					quitSAT = TRUE;
				} 
				else 
				{
					TranslateMessage(&msgSAT);
					DispatchMessage(&msgSAT);
				}

			} 
			else 
			{
				t1 = clock() * CLOCKS_PER_SEC;
				frameCount++;

				// Execute the kernel which applies the boxfilter
				if(runBoxFilterKernel() != SDK_SUCCESS)
					return SDK_FAILURE;

				// Bind PBO and texture 
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
				glBindTexture(GL_TEXTURE_2D, tex);

				// Copy pixels from pbo to texture 
				glTexSubImage2D(GL_TEXTURE_2D, 
					0, 
					0, 
					0, 
					width, 
					height, 
					GL_RGBA, 
					GL_UNSIGNED_BYTE, 
					NULL);

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

				//glutSwapBuffers();
				//glutPostRedisplay();
				SwapBuffers(gHdcSAT);

				t2 = clock() * CLOCKS_PER_SEC;
				totalElapsedTime += (double)(t2 - t1);
				if(frameCount && frameCount > frameRefCount)
				{
					// set GLUT Window Title
					char title[256];
					double fMs = (double)((totalElapsedTime / (double)CLOCKS_PER_SEC) / (double) frameCount);
					int framesPerSec = (int)(1.0 / (fMs / CLOCKS_PER_SEC));
#if defined (_WIN32) && !defined(__MINGW32__)
					sprintf_s(title, 256, "BoxFilterGL SAT | %d fps ", framesPerSec);
#else 
					sprintf(title, "BoxFilterGL SAT | %d fps ", framesPerSec);
#endif
					//glutSetWindowTitle(title);
					frameCount = 0;
					totalElapsedTime = 0.0;
					SetWindowText(gHwndSAT, title);
				}
			}
		}
#endif
	}

	// write the output image to bitmap file 
	if(writeOutputImage(OUTPUT_SAT_IMAGE) != SDK_SUCCESS)
	{
		return SDK_FAILURE;
	}

	return SDK_SUCCESS;
}

int 
BoxFilterGLSAT::cleanup()
{
#ifdef _WIN32
    wglDeleteContext(gGlCtxSAT);
    DeleteDC(gHdcSAT);
    gHdcSAT = NULL;
    gGlCtxSAT = NULL;
    //DestroyWindow(gHwndSAT);
#endif
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseKernel(horizontalSAT0);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseKernel(horizontalSAT);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseKernel(verticalSAT);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseMemObject(inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");
#ifndef _WIN32
	glXMakeCurrent(displayNameSAT, None, NULL);
	glXDestroyContext(displayNameSAT, gGlCtxSAT);
	XDestroyWindow(displayNameSAT, winSAT);
	XCloseDisplay(displayNameSAT);
#endif
	status = clReleaseMemObject(tempImageBuffer0);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(tempImageBuffer1);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");
    
    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // release program resources (input memory etc.) 

    FREE(inputImageData);
    FREE(outputImageData);
    FREE(verificationOutput);
    FREE(devices);

    std::cout<<"Cleanup of SAT version successful"<<std::endl;

    return SDK_SUCCESS;
}


void 
BoxFilterGLSAT::boxFilterCPUReference()
{
    std::cout << "Verifying results...";
    int t = (filterWidthSAT - 1) / 2;
    int filterSize = filterWidthSAT * filterWidthSAT;

    for(int y = 0; y < (int)height; y++)
    {
        for(int x = 0; x < (int)width; x++)
        {
            // Only threads inside apron will calculate their pixel value 
            if(x >= t && x < (int)(width - t) && y >= t && y < (int)(height - t))
            {
                cl_int4 sum = {0, 0, 0, 0};
                // For all pixels inside box 
                for(int y1 = -t; y1 <= t; y1++)
                {
                    for(int x1 = -t; x1 <= t; x1++)
                    {
                        sum.s[0] += inputImageData[x + x1 + (y + y1) * width].s[0];
                        sum.s[1] += inputImageData[x + x1 + (y + y1) * width].s[1];
                        sum.s[2] += inputImageData[x + x1 + (y + y1) * width].s[2];
                        sum.s[3] += inputImageData[x + x1 + (y + y1) * width].s[3];
                    }
                }
                verificationOutput[x + y * width].s[0] = sum.s[0] / filterSize;
                verificationOutput[x + y * width].s[1] = sum.s[1] / filterSize;
                verificationOutput[x + y * width].s[2] = sum.s[2] / filterSize;
                verificationOutput[x + y * width].s[3] = sum.s[3] / filterSize;
            }
        }
    }
    std::cout<<"done!" <<std::endl;
}


int 
BoxFilterGLSAT::verifyResults()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    if(verify)
    {
        // reference implementation 
        boxFilterCPUReference();

        // Compare between outputImageData and verificationOutput 
        if(!memcmp(outputImageData, 
                   verificationOutput, 
                   width * height * sizeof(cl_uchar4)))
        {
            std::cout << "Passed!\n" <<std::endl;
        }
        else
        {
            std::cout << "Failed!\n" <<std::endl;
        }
    }
    return SDK_SUCCESS;
}

void 
BoxFilterGLSAT::printStats()
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
BoxFilterGLSAT *BoxFilterGLSAT::boxFilterGLSAT = NULL;


int BoxFilterGLSAT::initializeGLAndGetCLContext(cl_platform_id platform,
                        cl_context &context,
                        cl_device_id &interopDevice)
{
#ifndef _WIN32
    cl_int status = SDK_SUCCESS;
    displayNameSAT = XOpenDisplay(NULL);
    int screenNumber = ScreenCount(displayNameSAT);
    std::cout<<"Number of displays "<<screenNumber<<std::endl;
    XCloseDisplay(displayNameSAT);
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
        displayNameSAT = XOpenDisplay(0);
        int nelements;

        GLXFBConfig *fbc = glXChooseFBConfig(displayNameSAT,
                                DefaultScreen(displayNameSAT),
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
        XVisualInfo *vi = glXChooseVisual(displayNameSAT,
                            DefaultScreen(displayNameSAT),
                            attributeList);
        XSetWindowAttributes swa;
        swa.colormap = XCreateColormap(displayNameSAT,
                            RootWindow(displayNameSAT, vi->screen),
                            vi->visual,
                            AllocNone);
        swa.border_pixel = 0;
        swa.event_mask = StructureNotifyMask;
        winSAT = XCreateWindow(displayNameSAT,
                    RootWindow(displayNameSAT, vi->screen),
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

        XMapWindow (displayNameSAT, winSAT);	
        std::cout << "glXCreateContextAttribsARB "
              << (void*) glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB")
              << std::endl;
        GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

        int attribs[] = {
                        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                        0
                        };

        GLXContext ctx = glXCreateContextAttribsARB(displayNameSAT,
                            *fbc,
                            0,
                            true,
                            attribs);
        glXMakeCurrent (displayNameSAT,
            winSAT,
            ctx);
        gGlCtxSAT = glXGetCurrentContext();
        cl_context_properties cpsGL[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
                                          CL_GLX_DISPLAY_KHR, (intptr_t) glXGetCurrentDisplay(),
                                          CL_GL_CONTEXT_KHR, (intptr_t) gGlCtxSAT, 0
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
        if(numDevices == 0)
        {
            glXDestroyContext(glXGetCurrentDisplay(), gGlCtxSAT);
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
                                      CL_GL_CONTEXT_KHR, (intptr_t) gGlCtxSAT, 0
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






