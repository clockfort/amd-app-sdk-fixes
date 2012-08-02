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

#ifndef BOX_FILTER_GL_SEPARABLE_H_
#define BOX_FILTER_GL_SEPARABLE_H_
#ifndef INPUT_IMAGE
#define INPUT_IMAGE "GaussianNoiseGL_Input.bmp"
#endif
#define OUTPUT_SEPARABLE_IMAGE "GaussianNoiseGL_Output.bmp"

#define GROUP_SIZE 64
//#define FILTER_WIDTH 9
#define FACTOR 60
/**
* GaussianNoiseGL
* Class implements OpenCL Gaussian Noise
* Derived from SDKSample base class
*/

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



class GaussianNoiseGL : public SDKSample
{
public:
    static GaussianNoiseGL *gaussianNoise;
    cl_double setupTime;                /**< time taken to setup OpenCL resources and building kernel */
    cl_double kernelTime;               /**< time taken to run kernel and read result back */
    cl_uchar4* inputImageData;          /**< Input bitmap data to device */
    cl_uchar4* outputImageData;         /**< Output from device */
    cl_context context;                 /**< CL context */
    cl_device_id *devices;              /**< CL device list */
    cl_mem inputImageBuffer;            /**< CL memory buffer for input Image*/
    cl_mem outputImageBuffer;           /**< CL memory buffer for Output Image*/
    cl_command_queue commandQueue;      /**< CL command queue */
    cl_program program;                 /**< CL program  */
    cl_kernel kernel;
    streamsdk::SDKBitMap inputBitmap;   /**< Bitmap class object */
    streamsdk::uchar4* pixelData;       /**< Pointer to image data */
    cl_uint pixelSize;                  /**< Size of a pixel in BMP format> */
    cl_uint width;                      /**< Width of image */
    cl_uint height;                     /**< Height of image */
    size_t blockSizeX;                  /**< Work-group size in x-direction */
    size_t blockSizeY;                  /**< Work-group size in y-direction */
    int iterations;                     /**< Number of iterations for kernel execution */
    int factor;
    clock_t t1, t2;
    int frameCount;
    int frameRefCount;
    double totalElapsedTime;
    GLuint pbo;                         //pixel-buffer object to hold-image data
    GLuint tex;                         //Texture to display
    cl_device_id interopDeviceId;
    streamsdk::SDKDeviceInfo deviceInfo;                /**< Structure to store device information*/
    streamsdk::KernelWorkGroupInfo kernelInfo;/* kernelInfoH,kernelInfoV*/;     /**< Structure to store kernel related info */

public:

    /**
    * Read bitmap image and allocate host memory
    * @param inputImageName name of the input file
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int readInputImage(std::string inputImageName);

    /**
    * Write to an image file
    * @param outputImageName name of the output file
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int writeOutputImage(std::string outputImageName);

    /**
    * Constructor
    * Initialize member variables
    * @param name name of sample (string)
    */
    GaussianNoiseGL(std::string name)
        : SDKSample(name),
          inputImageData(NULL),
          outputImageData(NULL)
    {
        pixelSize = sizeof(streamsdk::uchar4);
        pixelData = NULL;
        blockSizeX = GROUP_SIZE;
        blockSizeY = 1;
        iterations = 1;
        factor=FACTOR;
        frameCount = 0;
        frameRefCount = 90;
        totalElapsedTime = 0.0;
    }

    /**
    * Constructor
    * Initialize member variables
    * @param name name of sample (const char*)
    */
    GaussianNoiseGL(const char* name)
        : SDKSample(name),
          inputImageData(NULL),
          outputImageData(NULL)
    {
        pixelSize = sizeof(streamsdk::uchar4);
        pixelData = NULL;
        blockSizeX = GROUP_SIZE;
        blockSizeY = 1;
        iterations = 1;
        factor=FACTOR;
        frameCount = 0;
        frameRefCount = 90;
        totalElapsedTime = 0.0;
    }

    ~GaussianNoiseGL()
    {
    }

    /**
    * Allocate image memory and Load bitmap file
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int setupBoxFilter();

    /**
     * Override from SDKSample, Generate binary image of given kernel
     * and exit application
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int genBinaryImage();

    /**
    * OpenCL related initialisations.
    * Set up Context, Device list, Command Queue, Memory buffers
    * Build CL kernel program executable
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int setupCL();

    /**
    * Initializing GL and get interoperable CL context
    * @param argc number of arguments
    * @param argv command line arguments
    * @
    * @return SDK_SUCCESS on success and SDK_FALIURE on failure.
    */
    int initializeGLAndGetCLContext(cl_platform_id platform,
                                    cl_context &context,
                                    cl_device_id &interopDevice);

#ifdef _WIN32
    /**
     * enableGLAndGLContext
     * creates a GL Context on a specified device and get its deviceId
     * @param hWnd Window Handle
     * @param hRC context of window
     * @param platform cl_platform_id selected
     * @param context associated cl_context
     * @param interopDevice cl_device_id of selected device
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int enableGLAndGetGLContext(HWND hWnd,
                                HDC &hDC,
                                HGLRC &hRC,
                                cl_platform_id platform,
                                cl_context &context,
                                cl_device_id &interopDevice);

    void disableGL(HWND hWnd, HDC hDC, HGLRC hRC);
#else

#endif
    /**
    * Set values for kernels' arguments, enqueue calls to the kernels
    * on to the command queue, wait till end of kernel execution.
    * Get kernel start and end time if timing is enabled
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int runCLKernels();

    /**
    * Reference CPU implementation of Binomial Option
    * for performance comparison
    */
    void GaussianNoiseCPUReference();

    /**
    * Override from SDKSample. Print sample stats.
    */
    void printStats();

    /**
    * Override from SDKSample. Initialize
    * command line parser, add custom options
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int initialize();

    /**
    * Override from SDKSample, adjust width and height
    * of execution domain, perform all sample setup
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int setup();

    /**
    * Override from SDKSample
    * Run OpenCL Sobel Filter
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int run();

    /**
    * Override from SDKSample
    * Cleanup memory allocations
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int cleanup();

    /**
    * Override from SDKSample
    * Verify against reference implementation
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int verifyResults();
};
namespace streamsdk
{
/**
* getPath
* @return path of the current directory
*/
std::string getPath();

/**
* buildOpenCLProgram
* builds the opencl program
* @param program program object
* @param context cl_context object
* @param buildData buildProgramData Object
* @return 0 if success else nonzero
*/
int compileOpenCLProgram(cl_program &program, const cl_context& context,  buildProgramData &buildData);


}
#endif // BOX_FILTER_GL_SEPARABLE_H_
