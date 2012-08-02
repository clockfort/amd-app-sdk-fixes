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


#ifndef REDUCTION_H_
#define REDUCTION_H_

//Headers Files
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include <SDKFile.hpp>

#define GROUP_SIZE 256
#define DEFAULT_INPUT_SIZE 1024
#define VALUES_PRINTED 20

// Init extension function pointers
#define INIT_CL_EXT_FCN_PTR(name) \
    if(!pfn_##name) { \
        pfn_##name = (name##_fn) clGetExtensionFunctionAddress(#name); \
        if(!pfn_##name) { \
            std::cout << "Cannot get pointer to ext. fcn. " #name << std::endl; \
            return SDK_FAILURE; \
        } \
    }

/**
 * DeviceFission 
 * Class implements OpenCL  DeviceFission sample
 * Derived from SDKSample base class
 */

class DeviceFission : public SDKSample
{
    cl_uint length;                 /**< Length of the input array */
	cl_uint half_length;            /**< Half length of the input array */
    cl_int *input;                  /**< Input array */

    cl_int *subOutput;              /**< Output for sub-devices */
	cl_int *gpuOutput;              /**< Output for gpu-device */

	cl_context rContext;		    /**< CL context for all devices */
    cl_device_id *Devices;          /**< CL device list */
	cl_device_id cpuDevice;         /**< CPU CL device */
	cl_device_id gpuDevice;         /**< GPU CL device */

    cl_uint numSubDevices;          /**< Number of sub-devices */
    cl_device_id *subDevices;       /**< CL Sub device list */

    cl_mem InBuf;                   /**< CL memory buffers for sub-devices */
    cl_mem *subOutBuf;              /**< CL memory buffers for sub-devices */
	cl_mem *gpuOutBuf;              /**< CL memory buffers for gpu-device */

    cl_command_queue *subCmdQueue;  /**< CL command queue for sub-devices */
	cl_command_queue gpuCmdQueue;   /**< CL command queue for gpu-device */

    cl_program subProgram;          /**< CL program for sub-devices */
	cl_program gpuProgram;          /**< CL program for gpu device */

	cl_kernel *subKernel;           /**< CL kernel for sub-devices */
	cl_kernel *gpuKernel;           /**< CL kernel for gpu device */

    size_t kernelWorkGroupSize;     /**< Group size returned by kernel */
    size_t groupSize;               /**< Work-group size */
    streamsdk::SDKDeviceInfo deviceInfo;/**< Structure to store device information */

	size_t deviceListSize;	        /**< Size of all device-list */

    bool gpuAvailable;				/**< Bool for gpu device available */
	bool cpu2gpu;					/**< Bool for migrate memory from Sub cpu to GPU */
	bool cpu2cpu;					/**< Bool for migrate memory from Sub cpu to sub cpu */

	cl_uint cpu2gpuValue;			/**< Input Value for cpu2gpu option*/
	cl_uint cpu2cpuValue;			/**< Input Value for cpu2cpu option*/

	std::string loadBinaryGPU;		/**< Cmd Line Option- Load GPU Binary with name */
public:
    /** 
     * Constructor 
     * Initialize member variables
     * @param name name of sample (string)
     */
    explicit DeviceFission(std::string name)
        : SDKSample(name), 
          input(NULL), 
          subOutput(NULL),
		  gpuOutput(NULL),
          Devices(NULL),
		  subDevices(NULL),
		  gpuDevice(NULL),
		  cpuDevice(NULL),
		  subOutBuf(NULL),
		  gpuOutBuf(NULL),
		  subCmdQueue(NULL),
		  subKernel(NULL),
		  gpuKernel(NULL),
          numSubDevices(2),
          length(DEFAULT_INPUT_SIZE),
          groupSize(GROUP_SIZE),
		  deviceListSize(0),
		  gpuAvailable(CL_FALSE),
		  cpu2gpu(CL_FALSE),
		  cpu2cpu(CL_FALSE),
		  cpu2gpuValue(1),
		  cpu2cpuValue(0)
    {
    }

    /** 
     * Constructor 
     * Initialize member variables
     * @param name name of sample (const char*)
     */
    explicit DeviceFission(const char* name)
        : SDKSample(name), 
          input(NULL), 
          subOutput(NULL),
		  gpuOutput(NULL),
          Devices(NULL),
		  subDevices(NULL),
		  subOutBuf(NULL),
  		  gpuOutBuf(NULL),
		  gpuDevice(NULL),
		  cpuDevice(NULL),
		  subCmdQueue(NULL),
		  subKernel(NULL),
  		  gpuKernel(NULL),
          numSubDevices(2),
          length(DEFAULT_INPUT_SIZE),
          groupSize(GROUP_SIZE),
		  deviceListSize(0),
		  gpuAvailable(CL_FALSE),
		  cpu2gpu(CL_FALSE),
		  cpu2cpu(CL_FALSE),
		  cpu2gpuValue(1),
		  cpu2cpuValue(0)
    {
    }

    ~DeviceFission();

    /**
     * Allocate and initialize host memory array with random values
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int setupDeviceFission();

    /**
     * Override from SDKSample, Generate binary image of given kernel 
     * and exit application
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int genBinaryImage();

    /**
     * OpenCL related initialisations. 
     * Set up Context, Device list and Device properties.
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int setupCLPlatform();

    /**
     * OpenCL related initialisations. 
     * Create of command queue, buffers, Building program and kernel
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int setupCLRuntime();

    /**
     * Set values for kernels' arguments, enqueue calls to the kernels
     * on to the command queue, wait till end of kernel execution.
     * Get kernel start and end time if timing is enabled
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int runCLALLKerenls();


	/**
	 * Creat and initialize timer for runCLALLKerenls
	 */
    int runCLKernels();

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
     * Run OpenCL DeviceFission
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

#endif // REDUCTION_H_
