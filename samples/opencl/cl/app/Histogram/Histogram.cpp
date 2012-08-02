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


#include "Histogram.hpp"

#include <math.h>

void 
Histogram::calculateHostBin()
{
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            hostBin[data[i * width + j]]++;
        }
    }
}

int
Histogram::setupHistogram()
{
    int i = 0;

    /* width must be multiples of binSize and
     * height must be multiples of groupSize
     */
    width = (width / binSize ? width / binSize: 1) * binSize;
    height = (height / groupSize ? height / groupSize: 1) * groupSize;

    subHistgCnt = (width * height) / (groupSize * binSize);

    // Allocate and init memory used by host 
    data = (cl_uint*)malloc(width * height * sizeof(cl_uint));
    CHECK_ALLOCATION(data, "Failed to allocate host memory. (data)");

    for(i = 0; i < width * height; i++)
        data[i] = rand() % (cl_uint)(binSize);
    
    hostBin = (cl_uint*)malloc(binSize * sizeof(cl_uint));
    CHECK_ALLOCATION(hostBin, "Failed to allocate host memory. (hostBin)");

    memset(hostBin, 0, binSize * sizeof(cl_uint));

    midDeviceBin = (cl_uint*)malloc(binSize * subHistgCnt * sizeof(cl_uint));
    CHECK_ALLOCATION(midDeviceBin, "Failed to allocate host memory. (midDeviceBin)");

    memset(midDeviceBin, 0, binSize * subHistgCnt * sizeof(cl_uint));

    deviceBin = (cl_uint*)malloc(binSize * sizeof(cl_uint));
    CHECK_ALLOCATION(deviceBin, "Failed to allocate host memory. (deviceBin)");

    memset(deviceBin, 0, binSize * sizeof(cl_uint));

    return SDK_SUCCESS;
}

int 
Histogram::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("Histogram_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
Histogram::setupCL(void)
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
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed.");

    // getting device on which to run the sample
    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");

    // Create command queue
    commandQueue = clCreateCommandQueue(context,
                                        devices[deviceId],
                                        0,
                                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    // Check if byte-addressable store is supported 
    if(!strstr(deviceInfo.extensions, "cl_khr_byte_addressable_store"))
    {
        byteRWSupport = false;
        OPENCL_EXPECTED_ERROR("Device does not support cl_khr_byte_addressable_store extension!");
    }

    dataBuf = clCreateBuffer(
        context, 
        CL_MEM_READ_ONLY,
        sizeof(cl_uint) * width  * height,
        NULL, 
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (dataBuf)");

    midDeviceBinBuf = clCreateBuffer(
        context, 
        CL_MEM_WRITE_ONLY,
        sizeof(cl_uint) * binSize * subHistgCnt,
        NULL, 
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (midDeviceBinBuf)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("Histogram_Kernels.cl");
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
    kernel = clCreateKernel(program, "histogram256", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    return SDK_SUCCESS;
}

int Histogram::setWorkGroupSize()
{
    cl_int status = 0;
    status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, " setKernelWorkGroupInfo() failed");

    if((size_t)groupSize > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << groupSize << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelInfo.kernelWorkGroupSize << std::endl;
        }
        groupSize = (cl_int)kernelInfo.kernelWorkGroupSize;
    }

    globalThreads = (width * height) / binSize ;
    localThreads = groupSize;

    if(localThreads > deviceInfo.maxWorkItemSizes[0] || 
       localThreads > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not"
                  << "support requested number of work items." << std::endl;
        return SDK_FAILURE;
    }

    if(kernelInfo.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient local "
                  << " memory on device." << std::endl;
        return SDK_FAILURE;
    }
    return SDK_SUCCESS;
}

int 
Histogram::runCLKernels(void)
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;

    status = this->setWorkGroupSize();
    CHECK_ERROR(status, SDK_SUCCESS, "setKernelWorkGroupSize() failed");

    // Set input data
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(commandQueue,
                                  dataBuf,
                                  CL_FALSE,
                                  0,
                                  width * height * sizeof(cl_uint),
                                  data,
                                  0,
                                  NULL,
                                  &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (dataBuf)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    // whether sort is to be in increasing order. CL_TRUE implies increasing 
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&dataBuf); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (dataBuf)");

    status = clSetKernelArg(kernel, 1, groupSize * binSize * sizeof(cl_uchar), NULL); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (local memory)");

    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&midDeviceBinBuf); 
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (deviceBinBuf)");

    // Enqueue a kernel run call.

    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
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
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt1) Failed");

    cl_event readEvt;
    status = clEnqueueReadBuffer(
        commandQueue, 
        midDeviceBinBuf, 
        CL_FALSE,
        0,
        subHistgCnt * binSize * sizeof(cl_uint),
        midDeviceBin,
        0,
        NULL,
        &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt1) Failed");

    // Clear deviceBin array 
    memset(deviceBin, 0, binSize * sizeof(cl_uint));

    // Calculate final histogram bin 
    for(int i = 0; i < subHistgCnt; ++i)
    {
        for(int j = 0; j < binSize; ++j)
        {
            deviceBin[j] += midDeviceBin[i * binSize + j];
        }
    }
    return SDK_SUCCESS;
}

int
Histogram::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize())
        return SDK_FAILURE;

    streamsdk::Option* width_option = new streamsdk::Option;
    CHECK_ALLOCATION(width_option, "Memory allocation error.\n");

    width_option->_sVersion = "x";
    width_option->_lVersion = "width";
    width_option->_description = "Width of the input";
    width_option->_type = streamsdk::CA_ARG_INT;
    width_option->_value = &width;

    sampleArgs->AddOption(width_option);
    delete width_option;

    streamsdk::Option* height_option = new streamsdk::Option;
    CHECK_ALLOCATION(height_option, "Memory allocation error.\n");

    height_option->_sVersion = "y";
    height_option->_lVersion = "height";
    height_option->_description = "Height of the input";
    height_option->_type = streamsdk::CA_ARG_INT;
    height_option->_value = &height;

    sampleArgs->AddOption(height_option);
    delete height_option;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory allocation error.\n");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);
    delete iteration_option;

    return SDK_SUCCESS;
}

int 
Histogram::setup()
{
    int status = 0;
    status = setupHistogram();
    CHECK_ERROR(status, SDK_SUCCESS, "Sample Resource Setup Failed");

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    status = setupCL();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_EXPECTED_FAILURE;
        return SDK_EXPECTED_FAILURE;
    }

    sampleCommon->stopTimer(timer);

    // Compute setup time 
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}


int 
Histogram::run()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;
    
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << 
        iterations << " iterations" << std::endl;
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
    // Compute average kernel time 
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
        sampleCommon->printArray<cl_uint>("deviceBin", deviceBin, binSize, 1);

    return SDK_SUCCESS;
}

int
Histogram::verifyResults()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    if(verify)
    {
        /**
         * Rreference implementation on host device
         * calculates the histogram bin on host
         */
        calculateHostBin();

        // compare the results and see if they match 
        bool result = true;
        for(int i = 0; i < binSize; ++i)
        {
            if(hostBin[i] != deviceBin[i])
            {
                result = false;
                break;
            }
        }

        if(result)
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

void Histogram::printStats()
{
    if(!byteRWSupport)
        return;

    // calculate total time 
    totalTime = setupTime + kernelTime;

    std::string strArray[4] = 
    {
        "Width",
        "Height", 
        "Time(sec)", 
        "[Transfer+Kernel]Time(sec)"
    };
    std::string stats[4];

    stats[0] = sampleCommon->toString(width, std::dec);
    stats[1] = sampleCommon->toString(height, std::dec);
    stats[2] = sampleCommon->toString(totalTime, std::dec);
    stats[3] = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}


int Histogram::cleanup()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseMemObject(dataBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(dataBuf)");

    status = clReleaseMemObject(midDeviceBinBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(midDeviceBinBuf)");

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.(context)");

    // Release program resources (input memory etc.)

    FREE(data);
    FREE(hostBin);
    FREE(midDeviceBin);
    FREE(deviceBin);
    FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    int status = 0;
    // Create MonteCalroAsian object
    Histogram clHistogram("Histogram OpenCL sample");

    // Initialization
    if(clHistogram.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Parse command line options 
    if(clHistogram.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clHistogram.isDumpBinaryEnabled())
    {
        return clHistogram.genBinaryImage();
    }

    // Setup
    status = clHistogram.setup();
    if(status != SDK_SUCCESS)
        return (status == SDK_EXPECTED_FAILURE)? SDK_SUCCESS : SDK_FAILURE;

    // Run
    if(clHistogram.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Verify
    if(clHistogram.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Cleanup resources created
    if(clHistogram.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Print performance statistics
    clHistogram.printStats();

    return SDK_SUCCESS;
}
