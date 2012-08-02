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


#include "QuasiRandomSequence.hpp"
#include <malloc.h>


/*
* \brief Generate direction numbers
*        v[j][32] : j dimensions each having 32 direction numbers
*/
void 
QuasiRandomSequence::generateDirectionNumbers(cl_uint nDimensions, 
                                              cl_uint* directionNumbers)
{
    cl_uint *v = directionNumbers;

    for (int dim = 0 ; dim < (int)(nDimensions); dim++)
    {
        // First dimension is a special case
        if (dim == 0)
        {
            for (int i = 0 ; i < N_DIRECTIONS ; i++)
            {
                // All m's are 1
                v[i] = 1 << (31 - i);
            }
        }
        else
        {
            int d = sobolPrimitives[dim].degree;
            /* The first direction numbers (up to the degree of the polynomial)
             are simply v[i] = m[i] / 2^i (stored in Q0.32 format)*/
            for (int i = 0 ; i < d ; i++)
            {
                v[i] = sobolPrimitives[dim].m[i] << (31 - i);
            }
            /* The remaining direction numbers are computed as described in
             the Bratley and Fox paper.
             ^ = Xor operator
             v[i] = a[1]v[i-1] ^ a[2]v[i-2] ^ ... ^ a[v-1]v[i-d+1] ^ v[i-d] ^ v[i-d]/2^d*/
            for (int i = d ; i < N_DIRECTIONS ; i++)
            {
                // First do the v[i-d] ^ v[i-d]/2^d part
                v[i] = v[i - d] ^ (v[i - d] >> d);
                /* Now do the a[1]v[i-1] ^ a[2]v[i-2] ^ ... part
                 Note that the coefficients a[] are zero or one and for compactness in
                 the input tables they are stored as bits of a single integer. To extract
                 the relevant bit we use right shift and mask with 1.
                 For example, for a 10 degree polynomial there are ten useful bits in a,
                 so to get a[2] we need to right shift 7 times (to get the 8th bit into
                 the LSB) and then mask with 1.*/
                for (int j = 1 ; j < d ; j++)
                {
                    v[i] ^= (((sobolPrimitives[dim].a >> (d - 1 - j)) & 1) * v[i - j]);
                }
            }
        }
        v += N_DIRECTIONS;
    }
}


/*
* \brief Host Initialization 
*        Allocate and initialize memory 
*        on the host. Print input array. 
*/

int 
QuasiRandomSequence::setupQuasiRandomSequence()
{
    // allocate input array 
#ifdef _WIN32
    input = (cl_uint*)_aligned_malloc(nDimensions * N_DIRECTIONS * 
                                      sizeof(cl_uint),
                                      16);
#else
    input = (cl_uint*)memalign(16, 
                               nDimensions * N_DIRECTIONS * 
                               sizeof(cl_uint));
#endif
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    // Check for dimensions 
    if(nDimensions > MAX_DIMENSIONS)
    {
        std::cout << "Max allowed dimension is 12240!\n";
        return SDK_FAILURE;
    }

    // initializa sobol direction numbers
    generateDirectionNumbers(nDimensions, input);

    if(!quiet) 
    {
        sampleCommon->printArray<cl_uint>(
            "Input", 
            input, 
            nDimensions * N_DIRECTIONS, 
            1);
    }

    // allocate output array 
#ifdef _WIN32
    output = (cl_float*)_aligned_malloc(nVectors * nDimensions * 
                                        sizeof(cl_float), 
                                        16);
#else
    output = (cl_float*)memalign(16, 
                                 nVectors * nDimensions * 
                                 sizeof(cl_float));
#endif
    CHECK_ALLOCATION(output, "Failed to allocate host memory(output)");

    // if verification is enabled
    if(verify)
    {
        // allocate memory for verification output array 
        verificationOutput = (cl_float*)malloc(nVectors * nDimensions * 
                                               sizeof(cl_float));
        CHECK_ALLOCATION(verificationOutput,"Failed to allocate host memory. (verify)");

        memset(verificationOutput, 
               0, 
               nVectors * nDimensions * sizeof(cl_float));
    }
    return SDK_SUCCESS;
}

int 
QuasiRandomSequence::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("QuasiRandomSequence_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
QuasiRandomSequence::setupCL(void)
{
    cl_int status = 0;
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


    /*
     * If we could find our platform, use it. Otherwise use just available platform.
     */
    cl_context_properties cps[3] = 
    {
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)platform, 
        0
    };

    context = clCreateContextFromType(
                  cps,
                  dType,
                  NULL,
                  NULL,
                  &status);
    CHECK_OPENCL_ERROR( status, "clCreateContextFromType failed.");

    // getting device on which to run the sample
    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");

    {
        //The block is to move the declaration of prop closer to its use
        cl_command_queue_properties prop = 0;
        commandQueue = clCreateCommandQueue(
                context, 
                devices[deviceId], 
                prop, 
                &status);
        CHECK_OPENCL_ERROR( status, "clCreateCommandQueue failed.");
    }

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, 0, "SDKDeviceInfo::setDeviceInfo() failed");

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    inputBuffer = clCreateBuffer(
        context, 
        inMemFlags,
        sizeof(cl_uint) * nDimensions * N_DIRECTIONS,
        0, 
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer)");

    outputBuffer = clCreateBuffer(
        context, 
        CL_MEM_WRITE_ONLY,
        sizeof(cl_float) * nVectors * nDimensions,
        0, 
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("QuasiRandomSequence_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string("");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, 0, "sampleCommon::buildOpenCLProgram() failed");

    // get a kernel object handle for a kernel with the given name 
    kernel = clCreateKernel(program, "QuasiRandomSequence", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "KernelInfo.setKernelWorkGroupInfo() failed");

    if(nVectors / 4 > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified (nVectors/4) : "
                      << nVectors / 4 << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelInfo.kernelWorkGroupSize<<std::endl;
            std::cout << "Falling back to " 
                      << kernelInfo.kernelWorkGroupSize << std::endl;
        }
        nVectors = (cl_uint)kernelInfo.kernelWorkGroupSize * 4;
    }
    return SDK_SUCCESS;
}



int 
QuasiRandomSequence::runCLKernels(void)
{
    cl_int   status;

    // set total threads and block size 
    size_t globalThreads[1]= {nDimensions * nVectors / 4};
    size_t localThreads[1] = {nVectors / 4};

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] || 
       localThreads[0] > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support"
                  << "requested number of work items." << std::endl;
        return SDK_FAILURE;
    }

    // Enqueue write to inputBuffer 
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                    commandQueue,
                    inputBuffer,
                    CL_FALSE,
                    0,
                    sizeof(cl_uint) * nDimensions * N_DIRECTIONS,
                    input,
                    0, 
                    NULL,
                    &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    // Set appropriate arguments to the kernel 

    // 1st argument to the kernel - outputBuffer 
    status = clSetKernelArg(
        kernel, 
        0, 
        sizeof(cl_mem), 
        (void *)&outputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (outputBuffer)");

    // 2nd argument to the kernel - inputBuffer
    status = clSetKernelArg(
        kernel, 
        1, 
        sizeof(cl_mem), 
        (void *)&inputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputBuffer)");

    // 3rd argument to the kernel - localBuffer(shared memory)
    status = clSetKernelArg(
        kernel, 
        2, 
        32 * sizeof(cl_uint), 
        NULL);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (localBuffer)");

    availableLocalMemory = deviceInfo.localMemSize - kernelInfo.localMemoryUsed;

    neededLocalMemory = 32 * sizeof(cl_uint); 

    if(neededLocalMemory > availableLocalMemory)
    {
        std::cout << "Unsupported: Insufficient"
            "local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    // Enqueue a kernel run call
    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        1,
        NULL,
        globalThreads,
        localThreads,
        0,
        NULL,
        &ndrEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

    // Enqueue readBuffer
    cl_event readEvt;
    status = clEnqueueReadBuffer(
        commandQueue,
        outputBuffer,
        CL_TRUE,
        0,
        nDimensions * nVectors * sizeof(cl_float),
        output,
        0,
        NULL,
        &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt) Failed");

    return SDK_SUCCESS;
}


int 
QuasiRandomSequence::initialize()
{
    // Call base class Initialize to get default configuration
    if (this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    streamsdk::Option* array_length = new streamsdk::Option;
    CHECK_ALLOCATION(array_length, "Memory allocation error.\n");

    array_length->_sVersion = "y";
    array_length->_lVersion = "dimension";
    array_length->_description = "Number of dimensions";
    array_length->_type = streamsdk::CA_ARG_INT;
    array_length->_value = &nDimensions;
    sampleArgs->AddOption(array_length);


    array_length->_sVersion = "x";
    array_length->_lVersion = "vector";
    array_length->_description = "Number of vectors";
    array_length->_type = streamsdk::CA_ARG_INT;
    array_length->_value = &nVectors;
    sampleArgs->AddOption(array_length);

    delete array_length;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory Allocation error.\n");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);
    delete iteration_option;

    return SDK_SUCCESS;
}

int 
QuasiRandomSequence::setup()
{
    if (setupQuasiRandomSequence() != SDK_SUCCESS)
        return SDK_FAILURE;

    // create and initialize timers 
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    if (setupCL() != SDK_SUCCESS)
        return SDK_FAILURE;

    sampleCommon->stopTimer(timer);
    // Compute setup time 
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}


int QuasiRandomSequence::run()
{
    // Warm up
    int status = 0;
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if (runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << 
        iterations<<" iterations" << std::endl;
    std::cout<<"-------------------------------------------"<<std::endl;

    // create and initialize timers 
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if (runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    // Compute kernel time 
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet) 
    {
        sampleCommon->printArray<cl_float>("Output", 
                                           output, 
                                           nDimensions * nVectors, 1);
    }

    return SDK_SUCCESS;
}


void 
QuasiRandomSequence::quasiRandomSequenceCPUReference()
{

    for(int j=0; j < (int)nDimensions; j++)
    {
        for(int i=0; i < (int)nVectors; i++)
        {
            unsigned int temp = 0;
            for(int k=0; k < 32; k++)
            {
                int mask = (int)(pow(2, (double)k));

                temp ^= ((i & mask) >> k) * input[j * 32 + k];
            }

            if(i==0 && j==0)
            {
                verificationOutput[j * nVectors + i] = 0;
            }
            else
            {
                verificationOutput[j * nVectors + i] = (cl_float)(temp / 
                    pow(2, (double)32));
            }
        }
    }
}


int 
QuasiRandomSequence::verifyResults()
{

    if(verify)
    {
        // reference implementation 
        quasiRandomSequenceCPUReference();

        // compare the results and see if they match 
        if(sampleCommon->compare(output, 
                                 verificationOutput, nDimensions * nVectors))
        {
            std::cout<<"Passed!\n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout<<"Failed\n" << std::endl;
            return SDK_FAILURE;
        }
    }

    return SDK_SUCCESS;
}

void 
QuasiRandomSequence::printStats()
{
    std::string strArray[3] = {"Elements", "Time(sec)", "[Transfer+Kernel]Time(sec)"};
    std::string stats[3];

    int length = nDimensions * nVectors;

    totalTime = setupTime + kernelTime;
    stats[0]  = sampleCommon->toString(length, std::dec);
    stats[1]  = sampleCommon->toString(totalTime, std::dec);
    stats[2]  = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 3);

}

int 
QuasiRandomSequence::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseMemObject(inputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBuffer)");

    status = clReleaseMemObject(outputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(outputBuffer)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.(context)");

#ifdef _WIN32
        ALIGNED_FREE(input);

        ALIGNED_FREE(output);
#else
    if(input)
        FREE(input);

    if(output)
        FREE(output);
#endif


    FREE(verificationOutput);
    FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    cl_int status = 0;
    QuasiRandomSequence clQuasiRandomSequence("OpenCL Reduction");

    if (clQuasiRandomSequence.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clQuasiRandomSequence.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clQuasiRandomSequence.isDumpBinaryEnabled())
    {
        return clQuasiRandomSequence.genBinaryImage();
    }

    if (clQuasiRandomSequence.setup() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clQuasiRandomSequence.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clQuasiRandomSequence.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clQuasiRandomSequence.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    clQuasiRandomSequence.printStats();
    return SDK_SUCCESS;
}
