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

#ifndef RECURSIVE_GAUSSIAN_H_
#define RECURSIVE_GAUSSIAN_H_

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include <SDKFile.hpp>
#include <SDKBitMap.hpp>

#define INPUT_IMAGE "RecursiveGaussian_Input.bmp"
#define OUTPUT_IMAGE "RecursiveGaussian_Output.bmp"

#define GROUP_SIZE 256

/**
* Custom type for gaussian parameters 
* precomputation
*/
typedef struct _GaussParms
{
    float nsigma; 
    float alpha;
    float ema; 
    float ema2; 
    float b1; 
    float b2; 
    float a0; 
    float a1; 
    float a2; 
    float a3; 
    float coefp; 
    float coefn; 
} GaussParms, *pGaussParms;



/**
* Recursive Gaussian 
* Class implements OpenCL Recursive Gaussian sample
* Derived from SDKSample base class
*/

class RecursiveGaussian : public SDKSample
{
    cl_double setupTime;                /**< time taken to setup OpenCL resources and building kernel */
    cl_double kernelTime;               /**< time taken to run kernel and read result back */

    cl_uchar4* inputImageData;          /**< Input bitmap data to device */
    cl_uchar4* outputImageData;         /**< Output from device */
    cl_context context;                 /**< CL context */
    cl_device_id *devices;              /**< CL device list */
    cl_mem inputImageBuffer;            /**< CL memory buffer for input Image*/
    cl_mem tempImageBuffer;             /**< CL memory buffer for storing the transpose of the image*/ 
    cl_mem outputImageBuffer;           /**< CL memory buffer for Output Image*/
    cl_uchar4* verificationInput;       /**< Input array for reference implementaton */
    cl_uchar4* verificationOutput;      /**< Output array for reference implementation */
    cl_command_queue commandQueue;      /**< CL command queue */
    cl_program program;                 /**< CL program  */
    cl_kernel kernelTranspose;          /**< CL kernel for transpose*/
    cl_kernel kernelRecursiveGaussian;  /**< CL Kernel for gaussian filter */
    streamsdk::SDKBitMap inputBitmap;   /**< Bitmap class object */
    streamsdk::uchar4* pixelData;       /**< Pointer to image data */
    cl_uint pixelSize;                  /**< Size of a pixel in BMP format> */
    GaussParms oclGP;                   /**< intance of struct to hold gaussian parameters */
    cl_uint width;                      /**< Width of image */
    cl_uint height;                     /**< Height of image */
    size_t blockSizeX;                  /**< Work-group size in x-direction */
    size_t blockSizeY;                  /**< Work-group size in y-direction */
    size_t blockSize;                   /**< block size for transpose kernel */
    int iterations;                     /**< Number of iterations for kernel execution */
    streamsdk::SDKDeviceInfo deviceInfo;/**< Structure to store device information*/
    streamsdk::KernelWorkGroupInfo transposeKernelInfo,RGKernelInfo;/**< Structure to store kernel related info */


public:

    /**
    * Read bitmap image and allocate host memory
    * @param inputImageName name of the input file
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int readInputImage(std::string inputImageName);

    /**
    * Write output to an image file
    * @param outputImageName name of the output file
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int writeOutputImage(std::string outputImageName);

    /**
    * Preprocess gaussian parameters
    * @param fSigma sigma value
    * @param iOrder order
    * @param pGp pointer to gaussian parameter object
    */
    void computeGaussParms(float fSigma, int iOrder, GaussParms* pGP);

    /**
    * RecursiveGaussian on CPU (for verification)
    * @param input input image 
    * @param output output image
    * @param width width of image
    * @param height height of image
    * @param a0..a3, b1, b2, coefp, coefn gaussian parameters
    */
    void recursiveGaussianCPU(cl_uchar4* input, cl_uchar4* output,
        const int width, const int height,
        const float a0, const float a1, 
        const float a2, const float a3, 
        const float b1, const float b2, 
        const float coefp, const float coefn);

    /**
    * Transpose on CPU (for verification)
    * @param input input image
    * @param output output image
    * @param width width of input image
    * @param height height of input image
    */
    void transposeCPU(cl_uchar4* input, cl_uchar4* output,
        const int width, const int height);

    /** 
    * Constructor 
    * Initialize member variables
    * @param name name of sample (string)
    */
    RecursiveGaussian(std::string name)
        : SDKSample(name),
        inputImageData(NULL),
        outputImageData(NULL),
        verificationOutput(NULL)
    {
        pixelSize = sizeof(streamsdk::uchar4);
        pixelData = NULL;
        blockSizeX = GROUP_SIZE;
        blockSizeY = 1;
        blockSize = 1;
        iterations = 1;
    }

    /** 
    * Constructor 
    * Initialize member variables
    * @param name name of sample (const char*)
    */
    RecursiveGaussian(const char* name)
        : SDKSample(name),
        inputImageData(NULL),
        outputImageData(NULL),
        verificationOutput(NULL)
    {
        pixelSize = sizeof(streamsdk::uchar4);
        pixelData = NULL;
        blockSizeX = GROUP_SIZE;
        blockSizeY = 1;
        blockSize = 1;
        iterations = 1;
    }

    ~RecursiveGaussian()
    {
    }

    /**
    * Allocate image memory and Load bitmap file
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int setupRecursiveGaussian();

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
    * Set values for kernels' arguments, enqueue calls to the kernels
    * on to the command queue, wait till end of kernel execution.
    * Get kernel start and end time if timing is enabled
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    int runCLKernels();

    /**
    * Reference CPU implementation of Binomial Option
    * for performance comparison
    * @return SDK_SUCCESS on success and SDK_FAILURE on failure
    */
    void recursiveGaussianCPUReference();

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

#endif // RECURSIVE_GAUSSIAN_H_
