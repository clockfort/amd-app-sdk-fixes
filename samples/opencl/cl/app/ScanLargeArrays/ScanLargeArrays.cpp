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


#include "ScanLargeArrays.hpp"


int 
ScanLargeArrays::setupScanLargeArrays()
{
    // input buffer size
    cl_uint sizeBytes = length * sizeof(cl_float);

    // allocate memory for input arrray
    input = (cl_float*)malloc(sizeBytes);
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    // random initialisation of input
    sampleCommon->fillRandom<cl_float>(input, length, 1, 0, 255);

    // allocate memory for output buffer
    output = (cl_float*)malloc(sizeBytes);
    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");

    // if verification is enabled
    if(verify)
    {
        // allocate memory for verification output array
        verificationOutput = (cl_float*)malloc(sizeBytes);
        CHECK_ALLOCATION(verificationOutput, "Failed to allocate host memory. (verify)");
        memset(verificationOutput, 0, sizeBytes);
    }
    // Unless quiet mode has been enabled, print the INPUT array
    if(!quiet) 
    {
        sampleCommon->printArray<cl_float>("Input", 
                                           input, 
                                           length, 
                                           1);
    }
    return SDK_SUCCESS;
}

int 
ScanLargeArrays::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("ScanLargeArrays_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    CHECK_ERROR(status, SDK_SUCCESS, "OpenCL Generate Binary Image Failed");
    return SDK_SUCCESS;
}


int
ScanLargeArrays::setupCL(void)
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
    CHECK_OPENCL_ERROR(status,"clCreateContextFromType failed.");

    status = sampleCommon->getDevices(context, &devices, deviceId,  isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");

    {
        // The block is to move the declaration of prop closer to its use
        cl_command_queue_properties prop = 0;
        commandQueue = clCreateCommandQueue(
            context, 
            devices[deviceId], 
            prop, 
            &status);
        if(sampleCommon->checkVal(
            status,
            0,
            "clCreateCommandQueue failed."))
            return SDK_FAILURE;
    }

    // Get Device specific Information 

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("ScanLargeArrays_Kernels.cl");
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
    bScanKernel = clCreateKernel(program, "ScanLargeArrays", &status);
    CHECK_OPENCL_ERROR(status,"clCreateKernel failed.(bScanKernel)");

    bAddKernel = clCreateKernel(program, "blockAddition", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(bAddKernel)");

    // get a kernel object handle for a kernel with the given name
    pScanKernel = clCreateKernel(program, "prefixSum", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(pScanKernel)");

    status = kernelInfoBScan.setKernelWorkGroupInfo(bScanKernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, " setKernelWorkGroupInfo() failed");

    status = kernelInfoBAdd.setKernelWorkGroupInfo(pScanKernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, " setKernelWorkGroupInfo() failed");

    status = kernelInfoPScan.setKernelWorkGroupInfo(bAddKernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, " setKernelWorkGroupInfo() failed");

    // Find munimum of all kernel's group-sizes
    size_t temp = min(kernelInfoBScan.kernelWorkGroupSize, kernelInfoPScan.kernelWorkGroupSize);
    temp = (temp > kernelInfoBAdd.kernelWorkGroupSize) ? kernelInfoBAdd.kernelWorkGroupSize : temp;

    if(blockSize > (cl_uint)temp)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << blockSize << std::endl;
            std::cout << "Max Group Size supported on the kernel : "
                      << temp << std::endl;
            std::cout << "Falling back to " << temp << std::endl;
        }
        blockSize = (cl_uint)temp;
    }
	
	blockSize = min(blockSize,length/2);
    // Calculate number of passes required
    float t = log((float)length) / log((float)blockSize);
    pass = (cl_uint)t;

    // If t is equal to pass
    if(fabs(t - (float)pass) < 1e-7)
    {
        pass--;
    }

    // Create input buffer on device
    inputBuffer = clCreateBuffer(
        context, 
        CL_MEM_READ_ONLY,
        sizeof(cl_float) * length,
        0, 
        &status);
    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(inputBuffer)");

    // Allocate output buffers
    outputBuffer = (cl_mem*)malloc(pass * sizeof(cl_mem));

    for(int i = 0; i < (int)pass; i++)
    {
        int size = (int)(length / pow((float)blockSize,(float)i));
        outputBuffer[i] = clCreateBuffer(
            context, 
            CL_MEM_READ_WRITE,
            sizeof(cl_float) * size,
            0, 
            &status);
        CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(outputBuffer)");
    }

    // Allocate blockSumBuffers
    blockSumBuffer = (cl_mem*)malloc(pass * sizeof(cl_mem));

    for(int i = 0; i < (int)pass; i++)
    {
        int size = (int)(length / pow((float)blockSize,(float)(i + 1)));
        blockSumBuffer[i] = clCreateBuffer(
            context, 
            CL_MEM_READ_WRITE,
            sizeof(cl_float) * size,
            0, 
            &status);

    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(blockSumBuffer)");       	
    }

    // Create a tempBuffer on device
    int tempLength = (int)(length / pow((float)blockSize, (float)pass));

    tempBuffer = clCreateBuffer(context,
        CL_MEM_READ_WRITE,
        sizeof(cl_float) * tempLength,
        0,
        &status);
    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(tempBuffer)");

    return SDK_SUCCESS;
}

int 
ScanLargeArrays::bScan(cl_uint len, 
                       cl_mem *inputBuffer, 
                       cl_mem *outputBuffer,
                       cl_mem *blockSumBuffer)
{
    cl_int status;
    cl_event ndrEvt;

    // set the block size
    size_t globalThreads[1]= {len / 2};
    size_t localThreads[1] = {blockSize / 2}; 

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] ||
       localThreads[0] > deviceInfo.maxWorkGroupSize)
    {
		std::cout<<"Unsupported: Device does not"
            "support requested number of work items.";

        return SDK_FAILURE;

    }

    // Set appropriate arguments to the kernel

    // 1st argument to the kernel - outputBuffer
    status = clSetKernelArg(
        bScanKernel, 
        0, 
        sizeof(cl_mem), 
        (void *)outputBuffer);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(outputBuffer)");

    // 2nd argument to the kernel - inputBuffer
    status = clSetKernelArg(
        bScanKernel, 
        1, 
        sizeof(cl_mem), 
        (void *)inputBuffer);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(inputBuffer)");

    // 3rd argument to the kernel - local memory
    status = clSetKernelArg(
        bScanKernel, 
        2, 
        blockSize * sizeof(cl_float), 
        NULL);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(local memory)");

    // 4th argument to the kernel - block_size
    status = clSetKernelArg(
        bScanKernel, 
        3, 
        sizeof(cl_int),
        &blockSize);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(blockSize)");

    // 5th argument to the kernel - length
    status = clSetKernelArg(
        bScanKernel, 
        4, 
        sizeof(cl_int),
        &len);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(len)");
    
    // 6th argument to the kernel - sum of blocks
    status = clSetKernelArg(
        bScanKernel, 
        5, 
        sizeof(cl_mem),
        blockSumBuffer);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(blockSumBuffer)");

    if(kernelInfoBScan.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient"
            "local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    // Enqueue a kernel run call
    status = clEnqueueNDRangeKernel(
        commandQueue,
        bScanKernel,
        1,
        NULL,
        globalThreads,
        localThreads,
        0,
        NULL,
        &ndrEvt);
    CHECK_OPENCL_ERROR(status,"clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status,"clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

    return SDK_SUCCESS;
}

int 
ScanLargeArrays::pScan(cl_uint len,
                       cl_mem *inputBuffer,
                       cl_mem *outputBuffer)
{
    cl_int status;
    cl_event ndrEvt;

    size_t globalThreads[1]= {len/2};
    size_t localThreads[1] = {len/2};

    if(kernelInfoPScan.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient local memory on device." 
            << std::endl;
        return SDK_FAILURE;
    }

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] ||
        localThreads[0] > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support"
            "requested number of work items." << std::endl;
        return SDK_FAILURE;
    }
    // Set appropriate arguments to the kernel 

    // 1st argument to the kernel - outputBuffer
    status = clSetKernelArg(
        pScanKernel, 
        0, 
        sizeof(cl_mem), 
        (void *)outputBuffer);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(outputBuffer)");

    // 2nd argument to the kernel - inputBuffer
    status = clSetKernelArg(
        pScanKernel, 
        1, 
        sizeof(cl_mem), 
        (void *)inputBuffer);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(inputBuffer)");

    // 3rd argument to the kernel - local memory
    status = clSetKernelArg(
        pScanKernel, 
        2, 
        len * sizeof(cl_float), 
        NULL);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(local)");

    // 4th argument to the kernel - length
    status = clSetKernelArg(
        pScanKernel, 
        3, 
        sizeof(cl_int),
        (void*)&len);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(length)");

    // Enqueue a kernel run call
    status = clEnqueueNDRangeKernel(
        commandQueue,
        pScanKernel,
        1,
        NULL,
        globalThreads,
        localThreads,
        0,
        NULL,
        &ndrEvt);
    CHECK_OPENCL_ERROR(status,"clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status,"clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

    return SDK_SUCCESS;
}



int 
ScanLargeArrays::bAddition(cl_uint len,
                           cl_mem *inputBuffer,
                           cl_mem *outputBuffer)
{
    cl_int   status;
    cl_event ndrEvt;

    // set the block size
    size_t globalThreads[1]= {len};
    size_t localThreads[1] = {blockSize};

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] ||
       localThreads[0] > deviceInfo.maxWorkGroupSize)
    {
        std::cout<<"Unsupported: Device does not support"
            "requested number of work items.";
        return SDK_FAILURE;
    }

    // 1st argument to the kernel - inputBuffer
    status = clSetKernelArg(
        bAddKernel, 
        0, 
        sizeof(cl_mem), 
        (void*)inputBuffer);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(outputBuffer)");

    // 2nd argument to the kernel - outputBuffer
    status = clSetKernelArg(
        bAddKernel, 
        1, 
        sizeof(cl_mem), 
        (void *)outputBuffer);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed.(inputBuffer)");

    if(kernelInfoBAdd.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient local memory on device." 
            << std::endl;
        return SDK_FAILURE;
    }

    // Enqueue a kernel run call
    status = clEnqueueNDRangeKernel(
        commandQueue,
        bAddKernel,
        1,
        NULL,
        globalThreads,
        localThreads,
        0,
        NULL,
        &ndrEvt);
    CHECK_OPENCL_ERROR(status,"clEnqueueNDRangeKernel failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status,"clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");
    return SDK_SUCCESS;
}


int 
ScanLargeArrays::runCLKernels(void)
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;

    cl_event writeEvt;
    // Enqueue write to seedsBuf
    status = clEnqueueWriteBuffer(commandQueue,
                                  inputBuffer,
                                  CL_FALSE,
                                  0,
                                  length * sizeof(cl_float),
                                  input,
                                  0,
                                  NULL,
                                  &writeEvt);
    CHECK_OPENCL_ERROR(status,"clEnqueueWriteBuffer failed.(inputBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status,"clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    // Do block-wise sum
    if(bScan(length, &inputBuffer, &outputBuffer[0], &blockSumBuffer[0]))
        return SDK_FAILURE;

    for(int i = 1; i < (int)pass; i++)
    {
        if(bScan((cl_uint)(length / pow((float)blockSize, (float)i)), 
            &blockSumBuffer[i - 1],
            &outputBuffer[i],
            &blockSumBuffer[i]))
        {
            return SDK_FAILURE;
        }
    }

    int tempLength = (int)(length / pow((float)blockSize, (float)pass));

    // Do scan to tempBuffer
    if(pScan(tempLength, &blockSumBuffer[pass - 1], &tempBuffer))
        return SDK_FAILURE;

    // Do block-addition on outputBuffers
    if(bAddition((cl_uint)(length / pow((float)blockSize, (float)(pass - 1))),
        &tempBuffer, &outputBuffer[pass - 1]))
    {
        return SDK_FAILURE;
    }

    for(int i = pass - 1; i > 0; i--)
    {
        if(bAddition((cl_uint)(length / pow((float)blockSize, (float)(i - 1))),
            &outputBuffer[i], &outputBuffer[i - 1]))
        {
            return SDK_FAILURE;
        }
    }

    cl_event readEvt;
    // Enqueue the results to application pointe
    status = clEnqueueReadBuffer(commandQueue,
                                 outputBuffer[0],
                                 CL_FALSE,
                                 0,
                                 length * sizeof(cl_float),
                                 output,
                                 0,
                                 NULL,
                                 &readEvt);
    CHECK_OPENCL_ERROR(status,"clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status,"clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt) Failed");

    return SDK_SUCCESS;
}

/*
* Naive implementation of Scan 
*/
void 
ScanLargeArrays::scanLargeArraysCPUReference(
    cl_float * output,
    cl_float * input,
    const cl_uint length)
{
    output[0] = 0;

    for(cl_uint i = 1; i < length; ++i)
    {
        output[i] = input[i-1] + output[i-1];
    }
}

int ScanLargeArrays::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    streamsdk::Option* array_length = new streamsdk::Option;
    CHECK_ALLOCATION(array_length,"Memory Allocation error.(array_length)");

    array_length->_sVersion = "x";
    array_length->_lVersion = "length";
    array_length->_description = "Length of the input array";
    array_length->_type = streamsdk::CA_ARG_INT;
    array_length->_value = &length;
    sampleArgs->AddOption(array_length);
    delete array_length;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option,"Memory Allocation error.(iteration_option)");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);
    delete iteration_option;

    return SDK_SUCCESS;
}

int ScanLargeArrays::setup()
{
    if(!sampleCommon->isPowerOf2(length))
    length = sampleCommon->roundToPowerOf2(length);
    
    if(setupScanLargeArrays() != SDK_SUCCESS)
        return SDK_FAILURE;

    // create and initialize timers
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


int ScanLargeArrays::run()
{
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    // create and initialize timers
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    std::cout << "Executing kernel for " << 
        iterations << " iterations" << std::endl;
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

    if(!quiet) {
        sampleCommon->printArray<cl_float>("Output", output, length, 1);
    }
    return SDK_SUCCESS;
}

int ScanLargeArrays::verifyResults()
{
    if(verify)
    {
        // reference implementation
        scanLargeArraysCPUReference(verificationOutput, input, length);

        // compare the results and see if they match
        if(sampleCommon->compare(output, 
                                 verificationOutput, 
                                 length, 
                                 (float)0.001))
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

void ScanLargeArrays::printStats()
{
    std::string strArray[3] = 
    {
        "Elements", 
        "Time(sec)", 
        "[Transfer+Kernel]Time(sec)"
    };
    std::string stats[3];
    totalTime = setupTime + kernelTime;
    stats[0]  = sampleCommon->toString(length, std::dec);
    stats[1]  = sampleCommon->toString(totalTime, std::dec);
    stats[2]  = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 3);
}

int 
ScanLargeArrays::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(pScanKernel);
    CHECK_OPENCL_ERROR(status,"clReleaseProgram failed.(pScanKernel))");

    status = clReleaseKernel(bScanKernel);
    CHECK_OPENCL_ERROR(status,"clReleaseProgram failed.(bScanKernel))");

    status = clReleaseKernel(bAddKernel);
    CHECK_OPENCL_ERROR(status,"clReleaseProgram failed.(bAddKernel))");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status,"clReleaseProgram failed.(program))");

    status = clReleaseMemObject(inputBuffer);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(tempBuffer))");

    status = clReleaseMemObject(tempBuffer);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(tempBuffer))");

    for(int i = 0; i < (int)pass; i++)
    {
        status = clReleaseMemObject(outputBuffer[i]);
        CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(outputBuffer))");
        
        status = clReleaseMemObject(blockSumBuffer[i]);
        CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(blockSumBuffer))");
    }

        FREE(outputBuffer);
        FREE(blockSumBuffer);

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status,"clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status,"clReleaseContext failed.(context)");
    // release program resources (input memory etc.)
        FREE(input);
        FREE(output);
        FREE(verificationOutput);
        FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    ScanLargeArrays clScanLargeArrays("OpenCL ScanLargeArrays");

    // Initialize
    if(clScanLargeArrays.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clScanLargeArrays.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clScanLargeArrays.isDumpBinaryEnabled())
    {
        return clScanLargeArrays.genBinaryImage();
    }
        // Setup
        if(clScanLargeArrays.setup()!=SDK_SUCCESS)
        return SDK_FAILURE;

        //Run
        if(clScanLargeArrays.run()!=SDK_SUCCESS)
        return SDK_FAILURE;

        // VerifyResults
        if(clScanLargeArrays.verifyResults()!=SDK_SUCCESS)
        return SDK_FAILURE;

        // Cleanup
    if(clScanLargeArrays.cleanup()!=SDK_SUCCESS)
        return SDK_FAILURE;


    clScanLargeArrays.printStats();
    return SDK_SUCCESS;
}
