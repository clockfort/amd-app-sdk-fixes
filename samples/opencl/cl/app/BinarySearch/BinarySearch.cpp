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


#include "BinarySearch.hpp"
#include <malloc.h>

/* 
 * \brief set up program input data 
 */
int BinarySearch::setupBinarySearch()
{
    // allocate and init memory used by host
    cl_uint inputSizeBytes = length *  sizeof(cl_uint);
    input = (cl_uint *) malloc(inputSizeBytes);
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    cl_uint max = length * 20;
    
    // random initialisation of input
    input[0] = 0;
    for(cl_uint i = 1; i < length; i++)
        input[i] = input[i - 1] + (cl_uint) (max * rand() / (float)RAND_MAX);
   
#if defined (_WIN32)
    output = (cl_uint *)_aligned_malloc(sizeof(cl_uint4), 16);
#else
    output = (cl_uint *)memalign(16, sizeof(cl_uint4));
#endif 
    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");

    /* 
     * Unless quiet mode has been enabled, print the INPUT array.
     */
    if(!quiet) 
    {
        sampleCommon->printArray<cl_uint>(
            "Sorted Input", 
            input, 
            length, 
            1);
    }

    return SDK_SUCCESS;
}


int 
BinarySearch::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("BinarySearch_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());
    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


/*
 * \brief OpenCL related initialisations are done here.
 *        Context, Device list, Command Queue are set up.
 *        Calls are made to set up OpenCL memory buffers that this program uses
 *        and to load the programs into memory and get kernel handles.
 *          Load and build OpenCL program and get kernel handles.
 *        Set up OpenCL memory buffers used by this program.
 */

int
BinarySearch::setupCL(void)
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


    // If we could find our platform, use it. Otherwise use just available platform.

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
        // The block is to move the declaration of prop closer to its use 
        cl_command_queue_properties prop = 0;
        commandQueue = clCreateCommandQueue(
                context, 
                devices[deviceId], 
                prop, 
                &status);
        CHECK_OPENCL_ERROR( status, "clCreateCommandQueue failed.");
    }

    inputBuffer = clCreateBuffer(
                      context, 
                      CL_MEM_READ_ONLY,
                      sizeof(cl_uint) * length,
                      NULL, 
                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer)");

    outputBuffer = clCreateBuffer(
                      context, 
                      CL_MEM_WRITE_ONLY,
                      sizeof(cl_uint4),
                      NULL, 
                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("BinarySearch_Kernels.cl");
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
    kernel = clCreateKernel(program, "binarySearch", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    return SDK_SUCCESS;
}

int 
BinarySearch::runCLKernels(void)
{
    cl_int   status;
    size_t globalThreads[1];
    size_t localThreads[1];

    localThreads[0] = 256;

    numSubdivisions = length / (cl_uint)localThreads[0];
    if(numSubdivisions < localThreads[0])
    {
        numSubdivisions = (cl_uint)localThreads[0];
    }
    globalThreads[0] = numSubdivisions;

    // Check group size against kernelWorkGroupSize
    status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "clGetKernelWorkGroupInfo failed.");

    if((cl_uint)(localThreads[0]) > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << localThreads[0] << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                << kernelInfo.kernelWorkGroupSize << std::endl;
            std::cout << "Changing the group size to " 
                << kernelInfo.kernelWorkGroupSize << std::endl;
        }

        localThreads[0] = kernelInfo.kernelWorkGroupSize;
        numSubdivisions = length / (cl_uint)localThreads[0];
        if(numSubdivisions < localThreads[0])
        {
            numSubdivisions = (cl_uint)localThreads[0];
        }
        globalThreads[0] = numSubdivisions;
    }

    
    /**
     * Since a plain binary search on the GPU would not achieve much benefit over the GPU
     * we are doing an N'ary search. We split the array into N segments every pass and therefore
     * get log (base N) passes instead of log (base 2) passes.
     *
     * In every pass, only the thread that can potentially have the element we are looking for
     * writes to the output array. For ex: if we are looking to find 4567 in the array and every
     * thread is searching over a segment of 1000 values and the input array is 1, 2, 3, 4,...
     * then the first thread is searching in 1 to 1000, the second one from 1001 to 2000, etc.
     * The first one does not write to the output. The second one doesn't either. The fifth one however is from 
     * 4001 to 5000. So it can potentially have the element 4567 which lies between them.
     *
     * This particular thread writes to the output the lower bound, upper bound and whether the element equals the lower bound element.
     * So, it would be 4001, 5000, 0
     *
     * The next pass would subdivide 4001 to 5000 into smaller segments and continue the same process from there.
     *
     * When a pass returns 1 in the third element, it means the element has been found and we can stop executing the kernel.
     * If the element is not found, then the execution stops after looking at segment of size 1.
     */

    // Set input data
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                inputBuffer,
                CL_FALSE,
                0,
                sizeof(cl_uint) * length,
                input,
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    cl_uint globalLowerBound = 0;
    cl_uint globalUpperBound = length - 1;
    cl_uint subdivSize = (globalUpperBound - globalLowerBound + 1) / numSubdivisions;
    cl_uint isElementFound = 0;


    if((input[0] > findMe) || (input[length - 1] < findMe))
    {
        output[0] = 0;
        output[1] = length - 1;
        output[2] = 0;

        return SDK_SUCCESS;
    }
    output[3] = 1;

    // Set appropruiate arguments to the kernel

    /*
     * First argument of the kernel is the output buffer
     */
    status = clSetKernelArg(
             kernel, 
             0, 
             sizeof(cl_mem), 
            (void *)&outputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg 0(OutputBuffer) failed.");

    /*
     * Second argument is input buffer
     */
    status = clSetKernelArg(
                    kernel, 
                    1, 
                    sizeof(cl_mem), 
                    (void *)&inputBuffer);
     CHECK_OPENCL_ERROR(status, "clSetKernelArg 1(inputBuffer) failed.");

    /*
     * Third is the element we are looking for
     */
    status = clSetKernelArg(
                    kernel, 
                    2, 
                    sizeof(cl_uint), 
                    (void *)&findMe);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg 2(findMe) failed.");

    while(subdivSize > 1 && output[3] != 0)
    {
        output[3] = 0;

        // Enqueue readBuffer
        cl_event writeEvt2;
        status = clEnqueueWriteBuffer(
                    commandQueue,
                    outputBuffer,
                    CL_FALSE,
                    0,
                    sizeof(cl_uint4),
                    output,
                    0,
                    NULL,
                    &writeEvt2);
        CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed.");

        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush failed.");

        status = sampleCommon->waitForEventAndRelease(&writeEvt2);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt2) Failed");


        /*
         * Fourth argument is the lower bound for the full segment for this pass.
         * Each thread derives its own lower and upper bound from this.
         */
        status = clSetKernelArg(
                            kernel, 
                            3, 
                            sizeof(cl_uint), 
                            (void *)&globalLowerBound);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg 3(globalLowerBound) failed.");

        /*
         * Similar to the above, but it is the upper bound
         */
        status = clSetKernelArg(
                            kernel, 
                            4, 
                            sizeof(cl_uint), 
                            (void *)&globalUpperBound);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg 4(globalUpperBound) failed.");

        /*
         * The size of the subdivision for each thread
         */
        status = clSetKernelArg(
                            kernel, 
                            5, 
                            sizeof(cl_uint), 
                            (void *)&subdivSize);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg 5(sumdivSize) failed.");

        /*
         * Enqueue a kernel run call
         */
        cl_event ndrEvt;
        status = clEnqueueNDRangeKernel(commandQueue,
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
        CHECK_OPENCL_ERROR(status, "clFlush failed.");

        status = sampleCommon->waitForEventAndRelease(&ndrEvt);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

        // Enqueue readBuffer
        cl_event readEvt;
        status = clEnqueueReadBuffer(
                    commandQueue,
                    outputBuffer,
                    CL_FALSE,
                    0,
                    sizeof(cl_uint4),
                    output,
                    0,
                    NULL,
                    &readEvt);
        CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush failed.");

        status = sampleCommon->waitForEventAndRelease(&readEvt);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt) Failed");

        globalLowerBound = output[0];
        globalUpperBound = output[1];
        subdivSize = (globalUpperBound - globalLowerBound + 1)/numSubdivisions;
    }

   
    for(cl_uint i=globalLowerBound; i<= globalUpperBound; i++)
    {
        if(input[i] == findMe)
        {
            output[0] = i;
            output[1] = i+1;
            output[2] = 1;
            return SDK_SUCCESS;
        }
    }

    // The findMe element is not found from globalLowerBound to globalUpperBound
    output[2] = 0;
    return SDK_SUCCESS;
}


/**
 * CPU verification for the BinarySearch algorithm
 */
int 
BinarySearch::binarySearchCPUReference() 
{
    cl_uint globalLowerBound = output[0];
    cl_uint globalUpperBound = output[1];
    cl_uint isElementFound = output[2];

    if(isElementFound)
    {
        if(input[globalLowerBound] == findMe)
            return SDK_SUCCESS;
        else
            return SDK_FAILURE;
    }
    else
    {
        for(cl_uint i = 0; i < length; i++)
        {
            if(input[i] == findMe)
                return SDK_FAILURE;
        }
        return SDK_SUCCESS;
    }
}

int BinarySearch::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize())
        return SDK_FAILURE;

    // Now add customized options
    streamsdk::Option* array_length = new streamsdk::Option;
    CHECK_ALLOCATION(array_length, "Memory allocation error.\n");
    
    array_length->_sVersion = "x";
    array_length->_lVersion = "length";
    array_length->_description = "Lenght of the input array";
    array_length->_type = streamsdk::CA_ARG_INT;
    array_length->_value = &length;

    sampleArgs->AddOption(array_length);

    delete array_length;

    streamsdk::Option* find_me = new streamsdk::Option;
    CHECK_ALLOCATION(find_me, "Memory allocation error.\n");

    find_me->_sVersion = "f";
    find_me->_lVersion = "find";
    find_me->_description = "element to be found";
    find_me->_type = streamsdk::CA_ARG_INT;
    find_me->_value = &findMe;
    sampleArgs->AddOption(find_me);

    delete find_me;

    streamsdk::Option* sub_div = new streamsdk::Option;
    CHECK_ALLOCATION(sub_div, "Memory allocation error.\n");
    
    sub_div->_sVersion = "s";
    sub_div->_lVersion = "subdivisions";
    sub_div->_description = "number of subdivisions";
    sub_div->_type = streamsdk::CA_ARG_INT;
    sub_div->_value = &numSubdivisions;
    sampleArgs->AddOption(sub_div);

    delete sub_div;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "Memory allocation error.\n");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);

    delete num_iterations;

    return SDK_SUCCESS;
}

int BinarySearch::setup()
{
    if(!sampleCommon->isPowerOf2(length))
        length = sampleCommon->roundToPowerOf2(length);

    if(setupBinarySearch() != SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer); 

    if(setupCL() != SDK_SUCCESS)
        return SDK_FAILURE;

    sampleCommon->stopTimer(timer);
    setupTime = (cl_double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}


int BinarySearch::run()
{
    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << iterations << 
        " iterations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);   

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    totalKernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;


    if(!quiet) {
        cl_uint globalLowerBound = output[0];
        cl_uint globalUpperBound = output[1];
        cl_uint isElementFound = output[2];

        std::cout << "l = " << globalLowerBound << ", u = " << globalUpperBound << 
                     ", isfound = " << isElementFound << ", fm = " << findMe << std::endl;
    }

    return SDK_SUCCESS;
}

int BinarySearch::verifyResults()
{
    if(verify)
    {
        verificationInput = (cl_uint *) malloc(length * sizeof(cl_int));
        CHECK_ALLOCATION(verificationInput, "Failed to allocate host memory. (verificationInput)");
        memcpy(verificationInput, input, length*sizeof(cl_int));

        /* reference implementation
         * it overwrites the input array with the output
         */
        int refTimer = sampleCommon->createTimer();
        sampleCommon->resetTimer(refTimer);
        sampleCommon->startTimer(refTimer);
        cl_int verified = binarySearchCPUReference();
        sampleCommon->stopTimer(refTimer);
        referenceKernelTime = sampleCommon->readTimer(refTimer);

        // compare the results and see if they match
        if(verified == SDK_SUCCESS)
        {
            std::cout << "Passed!\n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout << "Failed\n" << std::endl;
            return SDK_FAILURE;
        }
    }
    
    return SDK_SUCCESS;
}

void BinarySearch::printStats()
{
    std::string strArray[3] = {"Length", "Time(sec)", "[Transfer+Kernel]Time(sec)"};
    std::string stats[3];

    totalTime = setupTime + totalKernelTime;

    stats[0] = sampleCommon->toString(length   , std::dec);
    stats[1] = sampleCommon->toString(totalTime, std::dec);
    stats[2] = sampleCommon->toString(totalKernelTime, std::dec);
    
    this->SDKSample::printStats(strArray, stats, 3);
}

int BinarySearch::cleanup()
{
    /* Releases OpenCL resources (Context, Memory etc.) */
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");
   
    status = clReleaseMemObject(inputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseCommandQueue(commandQueue);
     CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // release program resources (input memory etc.)
    FREE(input);

#if defined (_WIN32)
        ALIGNED_FREE(output);
#else
        FREE(output);
#endif 

    FREE(devices);

    FREE(verificationInput);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    BinarySearch clBinarySearch("OpenCL Binary Search");

    if(clBinarySearch.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clBinarySearch.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clBinarySearch.isDumpBinaryEnabled())
    {
        return clBinarySearch.genBinaryImage();
    }
    else
    {
        if(clBinarySearch.setup() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clBinarySearch.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clBinarySearch.verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clBinarySearch.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        clBinarySearch.printStats();
    }

    return SDK_SUCCESS;
}