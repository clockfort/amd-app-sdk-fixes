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


#include "BinomialOption.hpp"


int
BinomialOption::setupBinomialOption()
{

    // Make numSamples multiple of 4
    numSamples = (numSamples / 4)? (numSamples / 4) * 4: 4;

#if defined (_WIN32)
    randArray = (cl_float*)_aligned_malloc(numSamples * sizeof(cl_float4), 16);
#else
    randArray = (cl_float*)memalign(16, numSamples * sizeof(cl_float4));
#endif
    CHECK_ALLOCATION(randArray, "Failed to allocate host memory. (randArray)");
    
    for(int i = 0; i < numSamples * 4; i++)
    {
        randArray[i] = (float)rand() / (float)RAND_MAX;
    }

#if defined (_WIN32)
    output = (cl_float*)_aligned_malloc(numSamples * sizeof(cl_float4), 16);
#else
    output = (cl_float*)memalign(16, numSamples * sizeof(cl_float4));
#endif

    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");
    memset(output, 0, numSamples * sizeof(cl_float4));

    return SDK_SUCCESS;
}


int 
BinomialOption::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("BinomialOption_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int
BinomialOption::setupCL()
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
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed.");

    // getting device on which to run the sample
    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");

    status = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_OPENCL_ERROR(status, "deviceInfo.setDeviceInfo failed");

    {
        // The block is to move the declaration of prop closer to its use
        cl_command_queue_properties prop = 0;
        commandQueue = clCreateCommandQueue(context, 
                                            devices[deviceId], 
                                            prop, 
                                            &status);
        CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");
    }

    // Create and initialize memory objects

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create memory object for stock price
    randBuffer = clCreateBuffer(context,
                                inMemFlags,
                                numSamples * sizeof(cl_float4),
                                NULL,
                                &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (randBuffer)");

    // Create memory object for output array
    outBuffer = clCreateBuffer(context,
                               CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                               numSamples * sizeof(cl_float4),
                               NULL,
                               &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("BinomialOption_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string("");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

    // get a kernel object handle for a kernel with the given name
    kernel = clCreateKernel(program,
                            "binomial_options",
                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_OPENCL_ERROR(status, "kernelInfo.setKernelWorkGroupInfo failed");

    // If group-size is gerater than maximum supported on kernel
    if((size_t)(numSteps + 1) > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << (numSteps + 1) << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelInfo.kernelWorkGroupSize << std::endl;
            std::cout << "Using appropiate group-size." << std::endl;
            std::cout << "-------------------------------------------" << std::endl;
        }
        numSteps = (cl_int)kernelInfo.kernelWorkGroupSize - 2;
    }

    return SDK_SUCCESS;
}


int 
BinomialOption::runCLKernels()
{
    cl_int status;
    cl_event ndrEvt;
    cl_int eventStatus = CL_QUEUED;

    cl_event inMapEvt;
    void* mapPtr = clEnqueueMapBuffer(commandQueue, 
                                      randBuffer, 
                                      CL_FALSE, 
                                      CL_MAP_WRITE, 
                                      0,
                                      numSamples * sizeof(cl_float4),
                                      0,
                                      NULL,
                                      &inMapEvt,
                                      &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inputBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&inMapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt) Failed");

    memcpy(mapPtr, randArray, numSamples * sizeof(cl_float4));

    cl_event inUnmapEvent;

    status = clEnqueueUnmapMemObject(commandQueue,
                                    randBuffer,
                                    mapPtr,
                                    0,
                                    NULL,
                                    &inUnmapEvent);
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (randBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&inUnmapEvent);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inUnmapEvent) Failed");

    // Set appropriate arguments to the kernel 
    status = clSetKernelArg(kernel,
                            0, 
                            sizeof(int), 
                            (void*)&numSteps);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(numSteps) failed.");

    status = clSetKernelArg(kernel, 
                            1, 
                            sizeof(cl_mem), 
                            (void*)&randBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(randBuffer) failed.");

    status = clSetKernelArg(kernel, 
                            2, 
                            sizeof(cl_mem), 
                            (void*)&outBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(outBuffer) failed.");

    status = clSetKernelArg(kernel,
                            3,
                            (numSteps + 1) * sizeof(cl_float4),
                            NULL);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(callA) failed.");

    status = clSetKernelArg(kernel,
                            4,
                            numSteps * sizeof(cl_float4),
                            NULL);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(callB) failed.");

    // Enqueue a kernel run call.
    size_t globalThreads[] = {numSamples * (numSteps + 1)};
    size_t localThreads[] = {numSteps + 1};

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] || localThreads[0] > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support"
            "requested number of work items.";
        return SDK_FAILURE;
    }
    
    if(kernelInfo.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    /**
     * This algorithm reduces each group of work-items to a single value
     * on OpenCL device
     */
    
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
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel() failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush() failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");
   
    cl_event outMapEvt;
    cl_uint* outMapPtr = (cl_uint*)clEnqueueMapBuffer(commandQueue,
                                                   outBuffer,
                                                   CL_FALSE,
                                                   CL_MAP_READ,
                                                   0,
                                                   numSamples * sizeof(cl_float4),
                                                   0,
                                                   NULL,
                                                   &outMapEvt,
                                                   &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(outputBuffer) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&outMapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapEvt) Failed");
    memcpy(output, outMapPtr, numSamples * sizeof(cl_float4));

    cl_event outUnmapEvt;
    status = clEnqueueUnmapMemObject(commandQueue,
                                     outBuffer,
                                     (void*)outMapPtr,
                                     0,
                                     NULL,
                                     &outUnmapEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(outputBuffer) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&outUnmapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outUnmapEvt) Failed");
    return SDK_SUCCESS;
}

/*
 * Reduces the input array (in place)
 * length specifies the length of the array
 */
int 
BinomialOption::binomialOptionCPUReference()
{
    refOutput = (float*)malloc(numSamples * sizeof(cl_float4));
    CHECK_ALLOCATION(refOutput, "Failed to allocate host memory. (refOutput)");

    float* stepsArray = (float*)malloc((numSteps + 1) * sizeof(cl_float4));
    CHECK_ALLOCATION(stepsArray, "Failed to allocate host memory. (stepsArray)");

    // Iterate for all samples
    for(int bid = 0; bid < numSamples; ++bid)
    {
        float s[4];
        float x[4];
        float vsdt[4];
        float puByr[4];
        float pdByr[4];
        float optionYears[4];

        float inRand[4];

        for(int i = 0; i < 4; ++i)
        {
            inRand[i] = randArray[bid + i];
            s[i] = (1.0f - inRand[i]) * 5.0f + inRand[i] * 30.f;
            x[i] = (1.0f - inRand[i]) * 1.0f + inRand[i] * 100.f;
            optionYears[i] = (1.0f - inRand[i]) * 0.25f + inRand[i] * 10.f; 
            float dt = optionYears[i] * (1.0f / (float)numSteps);
            vsdt[i] = VOLATILITY * sqrtf(dt);
            float rdt = RISKFREE * dt;
            float r = expf(rdt);
            float rInv = 1.0f / r;
            float u = expf(vsdt[i]);
            float d = 1.0f / u;
            float pu = (r - d)/(u - d);
            float pd = 1.0f - pu;
            puByr[i] = pu * rInv;
            pdByr[i] = pd * rInv;
        }
        /**
         * Compute values at expiration date:
         * Call option value at period end is v(t) = s(t) - x
         * If s(t) is greater than x, or zero otherwise...
         * The computation is similar for put options...
         */
        for(int j = 0; j <= numSteps; j++)
        {
            for(int i = 0; i < 4; ++i)
            {
                float profit = s[i] * expf(vsdt[i] * (2.0f * j - numSteps)) - x[i];
                stepsArray[j * 4 + i] = profit > 0.0f ? profit : 0.0f;
            }
        }

        /**
         * walk backwards up on the binomial tree of depth numSteps
         * Reduce the price step by step
         */
        for(int j = numSteps; j > 0; --j)
        {
            for(int k = 0; k <= j - 1; ++k)
            {
                for(int i = 0; i < 4; ++i)
                {
                    stepsArray[k * 4 + i] = pdByr[i] * stepsArray[(k + 1) * 4 + i] + puByr[i] * stepsArray[k * 4 + i];
                }
            }   
        }

        //Copy the root to result
        refOutput[bid] = stepsArray[0];
    }
    
    free(stepsArray);

    return SDK_SUCCESS;
}

int BinomialOption::initialize()
{
    // Call base class Initialize to get default configuration
    CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL Resource Intilization failed");

    streamsdk::Option* num_samples = new streamsdk::Option;
    CHECK_ALLOCATION(num_samples, "Error. Failed to allocate memory (num_samples)\n");

    num_samples->_sVersion = "x";
    num_samples->_lVersion = "samples";
    num_samples->_description = "Number of samples to be calculated";
    num_samples->_type = streamsdk::CA_ARG_INT;
    num_samples->_value = &numSamples;

    sampleArgs->AddOption(num_samples);

    delete num_samples;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "Error. Failed to allocate memory (num_iterations)\n");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);

    delete num_iterations;

    return SDK_SUCCESS;
}

int BinomialOption::setup()
{
    if(setupBinomialOption())
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);
    if(setupCL())
        return SDK_FAILURE;
    sampleCommon->stopTimer(timer);
    // Compute setup time
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}


int BinomialOption::run()
{
    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        if(runCLKernels())
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << iterations 
              << " iterations" << std::endl;
    std::cout << "-------------------------------------------" 
              << std::endl;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);   

    for(int i = 0; i < iterations; i++)
    {
        if(runCLKernels())
            return SDK_FAILURE;
    }
    sampleCommon->stopTimer(timer);

    // Compute average kernel time
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
        sampleCommon->printArray<cl_float>("Output", output, numSamples, 1);
 
    return SDK_SUCCESS;
}

int BinomialOption::verifyResults()
{
    if(verify)
    {
        /**
         * reference implementation
         * it overwrites the input array with the output
         */
        int result = SDK_SUCCESS;
        result = binomialOptionCPUReference();
        CHECK_ERROR(result, SDK_SUCCESS, "OpenCL  verifyResults  failed");
        
        // compare the results and see if they match
        if(sampleCommon->compare(output, refOutput, numSamples, 0.001f))
        {
            std::cout << "Passed!\n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout <<" Failed\n" << std::endl;

            std::cout <<"\n\n\nNo. Output Output(hex) Refoutput Refoutput(hex)\n";
            for(int i = 0; i < numSamples; ++i)
            {
                if(fabs(output[i] - refOutput[i])> 0.0001)
                {

                    printf(" [%d] %f %#x ", i, output[i], *(int*)&output[i]);
                    printf(" %f %#x, \n", refOutput[i], *(int*)&refOutput[i]);
                }
            }

            return SDK_FAILURE;
        }
    }
    return SDK_SUCCESS;
}

void BinomialOption::printStats()
{
    std::string strArray[4] = 
    {
        "Option Samples", 
        "Time(sec)", 
        "Transfer+kernel(sec)" , 
        "Options/sec"
    };

    totalTime = setupTime + kernelTime;

    std::string stats[4];
    stats[0] = sampleCommon->toString(numSamples, std::dec);
    stats[1] = sampleCommon->toString(totalTime, std::dec);
    stats[2] = sampleCommon->toString(kernelTime, std::dec);
    stats[3] = sampleCommon->toString(numSamples / totalTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}

int
BinomialOption::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseMemObject(randBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    return SDK_SUCCESS;
}

BinomialOption::~BinomialOption()
{

#ifdef _WIN32
             ALIGNED_FREE(randArray);
#else
             FREE(randArray);
#endif

#ifdef _WIN32
             ALIGNED_FREE(output);
#else
             FREE(output);
#endif

     FREE(refOutput);

     FREE(devices);
}

int 
main(int argc, char * argv[])
{
    int status = 0;
    BinomialOption clBinomialOption("OpenCL BinomialOption");

    //clBinomialOption.initialize();
    if(clBinomialOption.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clBinomialOption.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;
    
    if(clBinomialOption.isDumpBinaryEnabled())
    {
        return clBinomialOption.genBinaryImage();
    }
    else
    {
        if(clBinomialOption.setup() != SDK_SUCCESS)
        return SDK_FAILURE;

        if(clBinomialOption.run() != SDK_SUCCESS)
        return SDK_FAILURE;

        if(clBinomialOption.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

        if(clBinomialOption.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        clBinomialOption.printStats();
    }
    return SDK_SUCCESS;
}