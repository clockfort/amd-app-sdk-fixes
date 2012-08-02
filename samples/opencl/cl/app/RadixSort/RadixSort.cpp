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


#include "RadixSort.hpp"

#include <math.h>

int 
RadixSort::hostRadixSort()
{
    cl_uint *histogram = (cl_uint*)malloc(RADICES * sizeof(cl_uint));
    CHECK_ALLOCATION(histogram, "Failed to allocate host memory. (histogram)");

    cl_uint *tempData = (cl_uint*)malloc(elementCount * sizeof(cl_uint));
    CHECK_ALLOCATION(tempData, "Failed to allocate host memory. (tempData)");

    if(histogram != NULL && tempData != NULL)
    {
        memcpy(tempData, unsortedData, elementCount * sizeof(cl_uint));
        for(int bits = 0; bits < sizeof(cl_uint) * RADIX ; bits += RADIX)
        {
            // Initialize histogram bucket to zeros
            memset(histogram, 0, RADICES * sizeof(cl_uint));

            // Calculate 256 histogram for all element
            for(int i = 0; i < elementCount; ++i)
            {
                cl_uint element = tempData[i];
                cl_uint value = (element >> bits) & RADIX_MASK;
                histogram[value]++;
            }

            // Prescan the histogram bucket
            cl_uint sum = 0;
            for(int i = 0; i < RADICES; ++i)
            {
                cl_uint val = histogram[i];
                histogram[i] = sum;
                sum += val;
            }

            // Rearrange  the elements based on prescaned histogram
            for(int i = 0; i < elementCount; ++i)
            {
                cl_uint element = tempData[i];
                cl_uint value = (element >> bits) & RADIX_MASK;
                cl_uint index = histogram[value];
                hSortedData[index] = tempData[i];
                histogram[value] = index + 1;
            }

            // Copy to tempData for further use
            if(bits != RADIX * 3)
                memcpy(tempData, hSortedData, elementCount * sizeof(cl_uint));
        }
    }

    FREE(tempData);
    FREE(histogram);
    return SDK_SUCCESS;
}

int
RadixSort::setupRadixSort()
{
    int i = 0;

    groupSize = GROUP_SIZE;
    elementCount = sampleCommon->roundToPowerOf2<cl_uint>(elementCount);

    //element count must be multiple of GROUP_SIZE * RADICES 
    int mulFactor = groupSize * RADICES;

    if(elementCount < mulFactor)
        elementCount = mulFactor;
    else
        elementCount = (elementCount / mulFactor) * mulFactor;

    numGroups = elementCount / mulFactor;

    // Allocate and init memory used by host
    unsortedData = (cl_uint*)malloc(elementCount * sizeof(cl_uint));
    CHECK_ALLOCATION(unsortedData, "Failed to allocate host memory. (unsortedData)");

    for(i = 0; i < elementCount; i++)
    {
        unsortedData[i] = (cl_uint)rand();
    }

    dSortedData = (cl_uint*)malloc(elementCount * sizeof(cl_uint));
    CHECK_ALLOCATION(dSortedData, "Failed to allocate host memory. (dSortedData)");

    memset(dSortedData, 0, elementCount * sizeof(cl_uint));

    hSortedData = (cl_uint*)malloc(elementCount * sizeof(cl_uint));
    CHECK_ALLOCATION(hSortedData, "Failed to allocate host memory. (hSortedData)");

    memset(hSortedData, 0, elementCount * sizeof(cl_uint));

    size_t tempSize = numGroups * groupSize * RADICES * sizeof(cl_uint);
    histogramBins = (cl_uint*)malloc(tempSize);
    CHECK_ALLOCATION(histogramBins, "Failed to allocate host memory. (histogramBins)");
    memset(histogramBins, 0, tempSize);

    return SDK_SUCCESS;
}

int 
RadixSort::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("RadixSort_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;

}


int
RadixSort::setupCL(void)
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
        // The block is to move the declaration of prop closer to its use
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
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    // Check particular extension
    if(!strstr(deviceInfo.extensions, "cl_khr_byte_addressable_store"))
    {
        byteRWSupport = false;
        OPENCL_EXPECTED_ERROR("Device does not support cl_khr_byte_addressable_store extension!");
    }

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Input buffer
    unsortedDataBuf = clCreateBuffer(
        context,
        inMemFlags,
        sizeof(cl_uint) * elementCount,
        NULL,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (unsortedDataBuf)");

    // Output for histogram kernel
    histogramBinsBuf = clCreateBuffer(
        context, 
        CL_MEM_WRITE_ONLY,
        numGroups * groupSize * RADICES * sizeof(cl_uint),
        NULL, 
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (histogramBinsBuf)");

    // Input for permute kernel
    scanedHistogramBinsBuf = clCreateBuffer(
        context, 
        inMemFlags,
        numGroups * groupSize * RADICES * sizeof(cl_uint),
        NULL, 
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (scanedHistogramBinsBuf)");

    // Final output
    sortedDataBuf = clCreateBuffer(
        context, 
        CL_MEM_WRITE_ONLY,
        elementCount * sizeof(cl_uint),
        NULL,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (sortedDataBuf)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("RadixSort_Kernels.cl");
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
    histogramKernel = clCreateKernel(program, "histogram", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    status = kernelInfoHistogram.setKernelWorkGroupInfo(histogramKernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "kernelInfoHistogram.setKernelWorkGroupInfo() failed");

    permuteKernel = clCreateKernel(program, "permute", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    status = kernelInfoPermute.setKernelWorkGroupInfo(permuteKernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "kernelInfoPermute.setKernelWorkGroupInfo() failed");

    // Find minimum of all kernel's work-group sizes
    size_t temp = min(kernelInfoHistogram.kernelWorkGroupSize, kernelInfoPermute.kernelWorkGroupSize);

    // If groupSize exceeds the minimum
    if((size_t)groupSize > temp)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << groupSize << std::endl;
            std::cout << "Max Group Size supported on the kernel : "
                      << temp << std::endl;
            std::cout << "Falling back to " << temp << std::endl;
        }
        groupSize = (cl_int)temp;
    }

    return SDK_SUCCESS;
}

int 
RadixSort::runHistogramKernel(int bits)
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;
    cl_event ndrEvt;

    size_t globalThreads = elementCount / RADICES;
    size_t localThreads = groupSize;

    if(localThreads > deviceInfo.maxWorkItemSizes[0] || 
       localThreads > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not"
            "support requested number of work items.";
        return SDK_FAILURE;
    }

    // Enqueue write from unSortedData to unSortedDataBuf
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(commandQueue, 
                                  unsortedDataBuf, 
                                  CL_FALSE,
                                  0, 
                                  sizeof(cl_uint) * elementCount,
                                  unsortedData, 
                                  0, 
                                  NULL, 
                                  &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (unsortedDataBuf)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    // Setup kernel arguments
    status = clSetKernelArg(histogramKernel, 
                            0, 
                            sizeof(cl_mem), 
                            (void *)&unsortedDataBuf); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (unsortedDataBuf)");

    status = clSetKernelArg(histogramKernel, 
                            1, 
                            sizeof(cl_mem), 
                            (void *)&histogramBinsBuf); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (histogramBinsBuf)");

    status = clSetKernelArg(histogramKernel, 
                            2, 
                            sizeof(cl_int), 
                            (void *)&bits); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (bits)");

    status = clSetKernelArg(histogramKernel, 
                            3, 
                            (groupSize * RADICES * sizeof(cl_ushort)), 
                            NULL); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (local memory)");

    if(kernelInfoHistogram.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient"
            "local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    /* 
    * Enqueue a kernel run call.
    */
    status = clEnqueueNDRangeKernel(
        commandQueue,
        histogramKernel,
        1,
        NULL,
        &globalThreads,
        &localThreads,
        0,
        NULL,
        &ndrEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

    // Enqueue the results to application pointer
    cl_event readEvt;
    status = clEnqueueReadBuffer(
        commandQueue, 
        histogramBinsBuf, 
        CL_FALSE,
        0,
        numGroups * groupSize * RADICES * sizeof(cl_uint),
        histogramBins,
        0,
        NULL,
        &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt) Failed");

    return SDK_SUCCESS;
}


int
RadixSort::runPermuteKernel(int bits)
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;
    cl_event ndrEvt;

    size_t bufferSize = numGroups * groupSize * RADICES * sizeof(cl_uint);

    // Write the host updated data to histogramBinsBuf
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(commandQueue,
                                  scanedHistogramBinsBuf,
                                  CL_FALSE,
                                  0,
                                  bufferSize,
                                  histogramBins,
                                  0, 
                                  NULL, 
                                  &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (scanedHistogramBinsBuf)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    size_t globalThreads = elementCount / RADICES;
    size_t localThreads = groupSize;

    if(localThreads > deviceInfo.maxWorkItemSizes[0] || 
       localThreads > deviceInfo.maxWorkGroupSize)
    {
        std::cout<<"Unsupported: Device does not"
            "support requested number of work items.";
        return SDK_FAILURE;
    }

    // Whether sort is to be in increasing order. CL_TRUE implies increasing
    status = clSetKernelArg(permuteKernel, 
                            0, 
                            sizeof(cl_mem), 
                            (void *)&unsortedDataBuf); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (unsortedDataBuf)");

    status = clSetKernelArg(permuteKernel, 
                            1, 
                            sizeof(cl_mem), 
                            (void *)&scanedHistogramBinsBuf); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (scanedHistogramBinsBuf)");

    status = clSetKernelArg(permuteKernel, 
                            2, 
                            sizeof(cl_int), 
                            (void *)&bits); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (bits)");

    status = clSetKernelArg(permuteKernel, 
                            3, 
                            (groupSize * RADICES * sizeof(cl_ushort)),
                            NULL); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (local memory)");

    status = clSetKernelArg(permuteKernel, 
                            4, 
                            sizeof(cl_mem), 
                            (void *)&sortedDataBuf); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (sortedDataBuf)");

    if(kernelInfoPermute.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient"
            "local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    /* 
    * Enqueue a kernel run call.
    */
    status = clEnqueueNDRangeKernel(
        commandQueue,
        permuteKernel,
        1,
        NULL,
        &globalThreads,
        &localThreads,
        0,
        NULL,
        &ndrEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

    // Enqueue the results to application pointe
    cl_event readEvt;
    status = clEnqueueReadBuffer(
        commandQueue, 
        sortedDataBuf, 
        CL_FALSE,
        0,
        elementCount * sizeof(cl_uint),
        dSortedData,
        0,
        NULL,
        &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt) Failed");

    return SDK_SUCCESS;
}



int 
RadixSort::runCLKernels(void)
{
    for(int bits = 0; bits < sizeof(cl_uint) * RADIX; bits += RADIX)
    {
        // Calculate thread-histograms
        runHistogramKernel(bits);

        // Scan the histogram
        int sum = 0;
        for(int i = 0; i < RADICES; ++i)
        {
            for(int j = 0; j < numGroups; ++j)
            {
                for(int k = 0; k < groupSize; ++k)
                {
                    int index = j * groupSize * RADICES + k * RADICES + i;
                    int value = histogramBins[index];
                    histogramBins[index] = sum;
                    sum += value;
                }
            }
        }

        // Permute the element to appropriate place
        runPermuteKernel(bits);

        // Current output now becomes the next input
        memcpy(unsortedData, dSortedData, elementCount * sizeof(cl_uint));
    }
    return SDK_SUCCESS;
}

int 
RadixSort::initialize()
{
    // Call base class Initialize to get default configuration
    if (this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Now add customized options
    streamsdk::Option* array_length = new streamsdk::Option;
    if(!array_length)
    {
        std::cout << "Memory allocation error.\n";
        return SDK_FAILURE;
    }

    array_length->_sVersion = "x";
    array_length->_lVersion = "count";
    array_length->_description = "Element count";
    array_length->_type = streamsdk::CA_ARG_INT;
    array_length->_value = &elementCount;
    sampleArgs->AddOption(array_length);
    delete array_length;


    streamsdk::Option* iteration_option = new streamsdk::Option;
    if(!iteration_option)
    {
        std::cout << "Memory Allocation error.\n";
        return SDK_FAILURE;
    }
    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);
    delete iteration_option;

    return SDK_SUCCESS;
}

int RadixSort::setup()
{
    int status = setupRadixSort();
    CHECK_ERROR(status, SDK_SUCCESS, "Sample SetUp Resources Failed");

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    status = setupCL();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE) // Here
            return SDK_EXPECTED_FAILURE;

        return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    // Compute setup time
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}


int RadixSort::run()
{
    int status = 0;
    if(!byteRWSupport)
        return SDK_SUCCESS;

    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
        if (runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    std::cout << "Executing kernel for " <<
        iterations<<" iterations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

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
        sampleCommon->printArray<cl_uint>("dSortedData", dSortedData, 512, 1);

    return SDK_SUCCESS;
}

int 
RadixSort::verifyResults()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    if(verify)
    {
        /* Rreference implementation on host device
        * Sorted by radix sort
        */
        int status = hostRadixSort();
        CHECK_ERROR(status, SDK_SUCCESS, "Host Implementation Failed");

        // compare the results and see if they match
        bool result = true;
        int failedCount = 0;
        for(int i = 0; i < elementCount; ++i)
        {
            if(dSortedData[i] != hSortedData[i])
            {
                result = false;
                failedCount++;
                printf("Element(%d) -  %u = %u \n", 
                       i,
                       dSortedData[i], hSortedData[i]);
            }
        }

        if(result)
        {
            std::cout << "Passed!\n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout <<" Failed\n" << std::endl;
            return SDK_FAILURE;
        }
    }
    return SDK_SUCCESS;
}

void
RadixSort::printStats()
{
    if(!byteRWSupport)
        return;

    std::string strArray[3] = {"Elements", "Time(sec)", "[Transfer+Kernel]Time(sec)"};
    std::string stats[3];

    totalTime = setupTime + kernelTime;
    stats[0]  = sampleCommon->toString(elementCount, std::dec);
    stats[1]  = sampleCommon->toString(totalTime, std::dec);
    stats[2]  = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 3);
}


int
RadixSort::cleanup()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseMemObject(unsortedDataBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(unsortedDataBuf)");

    status = clReleaseMemObject(sortedDataBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(sortedDataBuf)");

    status = clReleaseMemObject(histogramBinsBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(histogramBinsBuf)");

    status = clReleaseMemObject(scanedHistogramBinsBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(scanedHistogramBinsBuf)");

    status = clReleaseKernel(histogramKernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(histogramKernel)");

    status = clReleaseKernel(permuteKernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(permuteKernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // Release program resources (input memory etc.) 
    FREE(unsortedData);
    FREE(dSortedData);
    FREE(hSortedData);
    FREE(histogramBins);
    FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    cl_int status = 0;
    RadixSort clRadixSort("RadixSort Sample");

    if (clRadixSort.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clRadixSort.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clRadixSort.isDumpBinaryEnabled())
    {
        return clRadixSort.genBinaryImage();
    }

    status = clRadixSort.setup();
    if(status != SDK_SUCCESS)
        return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

    if (clRadixSort.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clRadixSort.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clRadixSort.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    clRadixSort.printStats();
    return SDK_SUCCESS;
}
