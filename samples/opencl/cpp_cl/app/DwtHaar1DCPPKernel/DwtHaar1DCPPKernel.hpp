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


#ifndef DWTHAAR1D_H_
#define DWTHAAR1D_H_




/**
 * Header Files
 */

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include <SDKCommandArgs.hpp>
#include <SDKFile.hpp>

#define SIGNAL_LENGTH (1 << 10)

class ParaClass
{
public:
	ParaClass()
	{
		tLevels=0;
	    signalLength=0;
	    levelsDone=0;
	    mLevels=0;
	}
	~ParaClass(){};
	void setValue(int v1,int v2,int v3,int v4)
	{
		this->tLevels = v1;
		this->signalLength = v2;
		this->levelsDone = v3;
		this->mLevels = v4;
	}
	int getValuetL()
	{
		return this->tLevels;
	}
	int getValuesLen()
	{
		return this->signalLength;
	}
	int getValueslD()
	{
		return this->levelsDone;
	}
	int getValuesmL()
	{
		return this->mLevels;
	}
private:
	int tLevels;
	int signalLength;
	int levelsDone;
	int mLevels;

};

/**
 * DwtHaar1D 
 * Class implements One-dimensional Haar wavelet decomposition
 * Derived from SDKSample base class
 */

class DwtHaar1D : public SDKSample
{
    
    cl_uint signalLength;           /**< Signal length (Must be power of 2)*/
    cl_float *inData;               /**< input data */
    cl_float *dOutData;             /**< output data */
    cl_float *dPartialOutData;      /**< paritial decomposed signal */
    cl_float *hOutData;             /**< output data calculated on host */

    cl_double setupTime;            /**< time taken to setup OpenCL resources and building kernel */
    cl_double kernelTime;           /**< time taken to run kernel and read result back */

    cl_context context;             /**< CL context */
    cl_device_id *devices;          /**< CL device list */
    
    cl_mem inDataBuf;               /**< CL memory buffer for input data */
    cl_mem dOutDataBuf;             /**< CL memory buffer for output data */
    cl_mem dPartialOutDataBuf;      /**< CL memory buffer for paritial decomposed signal */
	cl_mem classObj;

    cl_command_queue commandQueue;  /**< CL command queue */
    cl_program program;             /**< CL program  */
    cl_kernel kernel;               /**< CL kernel for histogram */
    cl_uint maxLevelsOnDevice;      /**< Maximum levels to be computed on device */
    int iterations;                 /**< Number of iterations to be executed on kernel */
    size_t        globalThreads;    /**< global NDRange */
    size_t         localThreads;    /**< Local Work Group Size */
    streamsdk::SDKDeviceInfo deviceInfo;        /**< Structure to store device information*/
    streamsdk::KernelWorkGroupInfo kernelInfo;      /**< Structure to store kernel related info */

public:
    /**
     * Constructor 
     * Initialize member variables
     * @param name name of sample (string)
     */
    DwtHaar1D(std::string name)
        : SDKSample(name),
        signalLength(SIGNAL_LENGTH),
        setupTime(0),
        kernelTime(0),
        inData(NULL),
        dOutData(NULL),
        dPartialOutData(NULL),
        hOutData(NULL),
        devices(NULL),
		classObj(NULL),
        iterations(1)
    {
    }

    /**
     * Constructor
     * Initialize member variables
     * @param name name of sample (const char*)
     */
    DwtHaar1D(const char* name)
        : SDKSample(name),
        signalLength(SIGNAL_LENGTH),
        setupTime(0),
        kernelTime(0),
        inData(NULL),
        dOutData(NULL),
        dPartialOutData(NULL),
        hOutData(NULL),
        devices(NULL),
		classObj(NULL),
        iterations(1)
    {
    }

    ~DwtHaar1D()
    {
    }

    /**
     * Allocate and initialize required host memory with appropriate values
     *  @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int setupDwtHaar1D();

    /**
     * Calculates the value of WorkGroup Size based in global NDRange 
     * and kernel properties
     *  @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int setWorkGroupSize();

    /**
     * Override from SDKSample, Generate binary image of given kernel 
     * and exit application
     *  @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int genBinaryImage();

    /**
     * OpenCL related initialisations. 
     * Set up Context, Device list, Command Queue, Memory buffers
     * Build CL kernel program executable
     *  @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int setupCL();

    /**
     * Set values for kernels' arguments, enqueue calls to the kernels
     * on to the command queue, wait till end of kernel execution.
     * Get kernel start and end time if timing is enabled
     *  @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int runCLKernels();

    /**
     * Override from SDKSample. Print sample stats.
     */
    void printStats();

    /**
     * Override from SDKSample. Initialize 
     * command line parser, add custom options
     *  @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int initialize();

    /**
     * Override from SDKSample, adjust width and height 
     * of execution domain, perform all sample setup
     * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int setup();

    /**
     * Override from SDKSample
     * Run OpenCL DwtHaar1D
     * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int run();

    /**
     * Override from SDKSample
     * Cleanup memory allocations
     *  @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int cleanup();

    /**
     * Override from SDKSample
     * Verify against reference implementation
     * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int verifyResults();

private:

    cl_int groupSize;       /**< Work items in a group */
    cl_int totalLevels;     /**< Total decomposition levels required for given signal length */
    cl_int curSignalLength; /**< Length of signal for given iteration */
    cl_int levelsDone;      /**< levels done */

    /**
     * @brief   Get number of decomposition levels to perform a full decomposition
     *          and also check if the input signal size is suitable
     * @return  returns the number of decomposition levels if they could be detrmined
     *          and the signal length is supported by the implementation,
     *          otherwise it returns SDK_FAILURE
     * @param   length  Length of input signal
     * @param   levels  Number of decoposition levels neessary to perform a full
     *                  decomposition
     *
     */
    int getLevels(unsigned int length, unsigned int* levels);

    /**
     * @brief   Runs the dwtHaar1D kernel to calculate
     *          the approximation coefficients on device
     * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int runDwtHaar1DKernel();

     /**
     * @brief   Reference implementation to calculates
     *          the approximation coefficients on host
     *          by normalized decomposition
     * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
     */
    int calApproxFinalOnHost();

};



#endif 
