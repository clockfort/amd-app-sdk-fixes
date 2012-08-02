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


#include "MersenneTwister.hpp"

int 
MersenneTwister::setupMersenneTwister()
{
    // Calculate width and height from numRands
    numRands = numRands / 4;
    numRands = (numRands / GROUP_SIZE)? (numRands / GROUP_SIZE) * GROUP_SIZE: GROUP_SIZE;

    unsigned int tempVar1 = (unsigned int)sqrt((double)numRands);
    tempVar1 = (tempVar1 / GROUP_SIZE)? (tempVar1 / GROUP_SIZE) * GROUP_SIZE: GROUP_SIZE;
    numRands = tempVar1 * tempVar1;

    width = tempVar1;
    height = width;


    // Allocate and init memory used by host 
#if defined (_WIN32)
    seeds = (cl_uint*)_aligned_malloc(width * height * sizeof(cl_uint4), 16);
#else
    seeds = (cl_uint*)memalign(16, width * height * sizeof(cl_uint4));
#endif

    CHECK_ALLOCATION(seeds,"Failed to allocate host memory. (seeds)");

    deviceResult = (cl_float *) malloc(width * height * mulFactor * sizeof(cl_float4));
    CHECK_ALLOCATION(deviceResult, "Failed to allocate host memory. (deviceResult)");

    for(int i = 0; i < width * height * 4; ++i)
    {
        seeds[i] = (unsigned int)rand();
    }

    memset((void*)deviceResult, 0, width * height * mulFactor * sizeof(cl_float4));

    return SDK_SUCCESS;
}

/**
 * genBinary Image function is used to when we want to create the binary 
 * for the kernel and run it at a later time. This is useful where offline  
 * compilation is the preferred mechanism. 
 */
int 
MersenneTwister::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("MersenneTwister_Kernels.cl");
    binaryData.flagsStr = std::string("-x clc++ ");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
MersenneTwister::setupCL(void)
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

    context = clCreateContextFromType(cps,
                                      dType,
                                      NULL,
                                      NULL,
                                      &status);

    if(sampleCommon->checkVal(status,
                               CL_SUCCESS,
                               "clCreateContextFromType failed."))
    {
        return SDK_FAILURE;
    }

    // getting device on which to run the sample
    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, 0, "sampleCommon::getDevices() failed");

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, 0, "SDKDeviceInfo::setDeviceInfo() failed");

    {
        // The block is to move the declaration of prop closer to its use
        cl_command_queue_properties prop = 0;
        commandQueue = clCreateCommandQueue(context,
                                            devices[deviceId],
                                            prop,
                                            &status);
        if(sampleCommon->checkVal(status,
                                   0,
                                   "clCreateCommandQueue failed."))
        {
            return SDK_FAILURE;
        }
    }

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    seedsBuf = clCreateBuffer(context,
                              inMemFlags,
                              width * height * sizeof(cl_float4),
                              0,
                              &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (seedsBuf)");

    resultBuf = clCreateBuffer(context,
                               CL_MEM_WRITE_ONLY,
                               width * height * sizeof(cl_float4) * mulFactor,
                               NULL,
                               &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (resultBuf)");

    cl_event writeEvt;
    // Enqueue write to seedsBuf
    status = clEnqueueWriteBuffer(commandQueue,
                                  seedsBuf,
                                  CL_FALSE,
                                  0,
                                  width * height * sizeof(cl_float4),
                                  seeds,
                                  0,
                                  NULL,
                                  &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (seedsBuf)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt1) Failed");

   // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("MersenneTwister_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string("-x clc++ ");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

    // get a kernel object handle for a kernel with the given name
    kernel = clCreateKernel(program, "gaussianRand", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    return SDK_SUCCESS;
}


int
MersenneTwister::runCLKernels(void)
{
    cl_int   status;

    status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, " setKernelWorkGroupInfo() failed");

    // Calculate 2D block size according to required work-group size by kernel
    kernelInfo.kernelWorkGroupSize = kernelInfo.kernelWorkGroupSize;

    kernelInfo.kernelWorkGroupSize = kernelInfo.kernelWorkGroupSize > GROUP_SIZE ? GROUP_SIZE : kernelInfo.kernelWorkGroupSize;
    while((blockSizeX * blockSizeY) < kernelInfo.kernelWorkGroupSize)
    {
        if(2 * blockSizeX * blockSizeY <= kernelInfo.kernelWorkGroupSize)
        {
            blockSizeX <<= 1;
        }
        if(2 * blockSizeX * blockSizeY <= kernelInfo.kernelWorkGroupSize)
        {
            blockSizeY <<= 1;
        }
    }
        
    size_t globalThreads[2] = {width, height};
    size_t localThreads[2] = {blockSizeX, blockSizeY};

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] ||
       localThreads[1] > deviceInfo.maxWorkItemSizes[1]|| 
       (size_t)blockSizeX * blockSizeY > deviceInfo.maxWorkGroupSize)
    {
        std::cout<<"Unsupported: Device does not support"
            "requested number of work items.";
        return SDK_FAILURE;
    }
    
    // Set appropriate arguments to the kernel

    // Seeds array
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&seedsBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (seedsBuf)");

    // width - i.e width of seeds array
    status = clSetKernelArg(kernel, 1, sizeof(cl_uint), (void*)&width);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width)");

    // mulFactor - i.e each seed generates mulFactor random numbers
    status = clSetKernelArg(kernel, 2, sizeof(cl_uint), (void*)&mulFactor);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (mulFactor)");

    // resultBuf
    status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&resultBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (resultBuf)");

    /*
     * Enqueue a kernel run call.
     * Each thread generates mulFactor random numbers from given seed.
     */
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
    CHECK_OPENCL_ERROR(status, "clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt1) Failed");

    cl_event readEvt;
    // Enqueue the results to application pointer
    status = clEnqueueReadBuffer(commandQueue,
                                 resultBuf,
                                 CL_FALSE,
                                 0,
                                 width * height * mulFactor * sizeof(cl_float4),
                                 deviceResult,
                                 0,
                                 NULL,
                                 &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt1) Failed");

    return SDK_SUCCESS;
}

int MersenneTwister::initialize()
{
    // Call base class Initialize to get default configuration 
    if(this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // add an option for getting blockSize from commandline 
    streamsdk::Option* num_option = new streamsdk::Option;
    CHECK_ALLOCATION(num_option, "Memory Allocation error.\n");

    num_option->_sVersion = "x";
    num_option->_lVersion = "numRands";
    num_option->_description = "Number of random numbers to be generated";
    num_option->_type = streamsdk::CA_ARG_INT;
    num_option->_value = &numRands;

    sampleArgs->AddOption(num_option);
    delete num_option;

    streamsdk::Option* factor_option = new streamsdk::Option;
    CHECK_ALLOCATION(factor_option,"Memory Allocation error.\n");

    factor_option->_sVersion = "y";
    factor_option->_lVersion = "factor";
    factor_option->_description = "Each seed generates 'factor' random numbers";
    factor_option->_type = streamsdk::CA_ARG_INT;
    factor_option->_value = &mulFactor;

    sampleArgs->AddOption(factor_option);
    delete factor_option;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory Allocation error.\n");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);
    delete iteration_option;

    return SDK_SUCCESS;
}

int MersenneTwister::validateCommandLineOptions()
{
    if(mulFactor % 2 == 0 && mulFactor >= 2 && mulFactor <= 8)
        return SDK_SUCCESS;

    std::cout << "Invalid value for -y option: 2 <= factor <= 8 and multiples of 2";
    return SDK_FAILURE;
}

int MersenneTwister::setup()
{
    if(setupMersenneTwister() != SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    if(setupCL() != SDK_SUCCESS)
        return SDK_FAILURE;

    sampleCommon->stopTimer(timer);
    // Compute setup time
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}


int MersenneTwister::run()
{
    //Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    std::cout << "Executing kernel for " << iterations 
        << " iterations" << std::endl;
    std::cout << "-------------------------------------------" <<
        std::endl;

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    // Compute kernel time 
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
    {
        sampleCommon->printArray<cl_float>("Output", 
                                           deviceResult,
                                           width * mulFactor * 4, 
                                           1);
    }

    return SDK_SUCCESS;
}


int 
MersenneTwister::verifyResults()
{
    if(verify)
    {
        // check mean value of generated random numbers
        bool passed = false;
        float meanVal = 0.0f;

        for(int i = 0; i < height * width * (int)mulFactor * 4; ++i)
        {
            meanVal += deviceResult[i];
        }
        
        meanVal = fabs(meanVal) / (height * width * (int)mulFactor * 4);

        if(meanVal <= 0.1f)
            passed = true;

        if(passed == false)
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

void 
MersenneTwister::printStats()
{
    std::string strArray[4] = 
    {
        "Generated Numbers", 
        "Time(sec)", 
        "[Transfer+Kernel]Time(sec)", 
        "Numbers/sec"
    };
    std::string stats[4];

    totalTime = setupTime + kernelTime;
    stats[0] = sampleCommon->toString(height * width * mulFactor * 4, std::dec);
    stats[1] = sampleCommon->toString(totalTime, std::dec);
    stats[2] = sampleCommon->toString(kernelTime, std::dec);
    stats[3] = sampleCommon->toString(height * width * mulFactor * 4 / kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}

int
MersenneTwister::cleanup()
{
    // Releases OpenCL resources
    cl_int status;

    status = clReleaseMemObject(seedsBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(seedsBuf)");

    status = clReleaseMemObject(resultBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(resultBuf)");

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.(context)");

    // Release program resources 
    FREE(deviceResult);

#if defined (_WIN32)
     ALIGNED_FREE(seeds);
#else
     FREE(seeds);
#endif

    FREE(devices);

    return SDK_SUCCESS;
}

int
main(int argc, char * argv[])
{
    // Create MonteCalroAsian object
    MersenneTwister clMersenneTwister("Mersenne Twister sample");

    // Initialization
    if(clMersenneTwister.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Parse command line options
    if(clMersenneTwister.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    // Validate the command line options
    if(clMersenneTwister.validateCommandLineOptions())
        return SDK_FAILURE;

    if(clMersenneTwister.isDumpBinaryEnabled())
    {
        return clMersenneTwister.genBinaryImage();
    }

    // Setup 
    if(clMersenneTwister.setup() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Run
    if(clMersenneTwister.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Verifty
    if(clMersenneTwister.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Cleanup resources created
    if(clMersenneTwister.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Print performance statistics
    clMersenneTwister.printStats();

    return SDK_SUCCESS;
}
