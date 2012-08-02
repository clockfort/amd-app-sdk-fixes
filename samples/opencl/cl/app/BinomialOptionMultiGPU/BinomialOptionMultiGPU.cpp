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


#include "BinomialOptionMultiGPU.hpp"


int
BinomialOptionMultiGPU::setupBinomialOptionMultiGPU()
{
    // Make numSamples multiple of 4
    numSamples = (numSamples / 4) ? (numSamples / 4) * 4: 4;

#if defined (_WIN32)
    randArray = (cl_float*)_aligned_malloc(numSamples * sizeof(cl_float4), 16);
#else
    randArray = (cl_float*)memalign(16, numSamples * sizeof(cl_float4));
#endif

    CHECK_ALLOCATION(randArray, "Failed to allocate host memory. (randArray)");

    for(int i = 0; i < numSamples * 4; i += 1)
    {
        randArray[i] = (float)rand() / (float)RAND_MAX;
    }

#if defined (_WIN32)
    output = (cl_float*)_aligned_malloc(numSamples * sizeof(cl_float4), 
                            16);
#else
    output = (cl_float*)memalign(16, 
                            numSamples * sizeof(cl_float4));
#endif

    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");

    memset(output, 0, numSamples * sizeof(cl_float4));
    return SDK_SUCCESS;
}


int 
BinomialOptionMultiGPU::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("BinomialOptionMultiGPU_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());
    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int
BinomialOptionMultiGPU::setupCL()
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

    //If -d option is enabled make noMultiGPUSupport to true
    if(isDeviceIdEnabled())
    {
        noMultiGPUSupport =true;
    }

    if (dType == CL_DEVICE_TYPE_CPU && !noMultiGPUSupport)
    {
        sampleCommon->error("Invalid options!!\nYou can't have MultiGPU support enabled when device type is cpu.\n"
                            "disabling multi gpu support");
        noMultiGPUSupport = true;
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

    if(dType == CL_DEVICE_TYPE_GPU)
    {
        //Get Number of devices available
        status = clGetDeviceIDs(platform,
                    CL_DEVICE_TYPE_GPU, 
                    0, 
                    0, 
                    (cl_uint*)&numGPUDevices);

        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs failed");

        //Get the array having all the GPU device IDs
        gpuDeviceIDs = new cl_device_id[numGPUDevices];
        CHECK_ALLOCATION(gpuDeviceIDs, "Allocation failed(gpuDeviceIDs)");

        status = clGetDeviceIDs(platform, 
                        CL_DEVICE_TYPE_GPU, 
                        numGPUDevices, 
                        gpuDeviceIDs, 
                        0);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs failed.");
    }

    /**
     * When multiGPU support is enabled.
     * Memory allocation
     */
    if(!noMultiGPUSupport)
    {
        /**
         * We have to have one cl_command_queue per GPU
         */ 
        commandQueues = new cl_command_queue[numGPUDevices];
        CHECK_ALLOCATION(commandQueues, "Allocation failed(commandQueues)");

        programs = new cl_program[numGPUDevices];
        CHECK_ALLOCATION(programs, "Allocation failed(programs)");

        kernels = new cl_kernel[numGPUDevices];
        CHECK_ALLOCATION(kernels, "Allocation failed(kernels)");

        randBuffers = new cl_mem[numGPUDevices];
        CHECK_ALLOCATION(randBuffers, "Allocation failed(randBuffers)");

        outputBuffers = new cl_mem[numGPUDevices];
        CHECK_ALLOCATION(randBuffers, "Allocation failed(outputBuffers)!!");

        devicesInfo = new streamsdk::SDKDeviceInfo[numGPUDevices];
        CHECK_ALLOCATION(devicesInfo, "Allocation failed(devicesInfo)!!");
    }

    /**
     * To get the device info. in case of single device
     */
    streamsdk::SDKDeviceInfo currentDeviceInfo;

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
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed");

    // getting device on which to run the sample
    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");
    cl_command_queue_properties prop = 0;

    /**
    * When multiGPU support is enabled.
    */
    if (!noMultiGPUSupport)
    {
        for (int i = 0; i < numGPUDevices; i++)
        {
            commandQueues[i] = clCreateCommandQueue(context, 
                                    gpuDeviceIDs[i], 
                                    prop, 
                                    &status);
            CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");
        }
    }
    else 
    {
        // Create command queue 
        commandQueue = clCreateCommandQueue(context,
                                            devices[deviceId],
                                            0,
                                            &status);
        CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");
    }

    /**
     * Get the device info of all the devices by using SDKDeviceInfo structure and setDeviceInfo function
     **/

    /**
     * When multiGPU support is enabled.
     */
    if (!noMultiGPUSupport)
    {
        for (int i = 0; i < numGPUDevices; i++)
        {
            status = devicesInfo[i].setDeviceInfo(gpuDeviceIDs[i]);
            if(status != SDK_SUCCESS)
            {
                std::cout << "devicesInfo[i].setDeviceInfo failed" << std::endl;
                return SDK_FAILURE;
            }
        }
    }

    /**
     * We get the device info. into currentDeviceInfo struct variable
     */
    status = currentDeviceInfo.setDeviceInfo(devices[deviceId]);
    if(status != SDK_SUCCESS)
    {
        std::cout << "currentDeviceInfo.setDeviceInfo failed " << std::endl;
        return SDK_FAILURE;
    }

    maxWorkGroupSize = currentDeviceInfo.maxWorkGroupSize;

    maxDimensions = currentDeviceInfo.maxWorkItemDims;

    maxWorkItemSizes = (size_t*)malloc(maxDimensions * sizeof(size_t));

    memcpy(maxWorkItemSizes, currentDeviceInfo.maxWorkItemSizes, maxDimensions * sizeof(size_t));

    totalLocalMemory = currentDeviceInfo.localMemSize;

    /**
     * Call load balancing for work divison
     **/

    /**
     * When multiGPU support is enabled.
     */
    if (!noMultiGPUSupport)
    {
        /**
         * This function would divide the work amongst the devices 
         * according to the capability of the device.
         */
        status = loadBalancing();
        CHECK_ERROR(status, SDK_SUCCESS, "loadBalancing failed!!");
    }

    /**
     * Create and initialize memory objects
     */

    /**
    * When multiGPU support is enabled.
    */
    if (!noMultiGPUSupport)
    {
        for (int i = 0; i < numGPUDevices; i++)
        {
            // Set Presistent memory only for AMD platform
            cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;

            /**
            * Use CL_MEM_USE_PERSISTENT_MEM_AMD only in case of AMD GPU
            */
            if(isAmdPlatform())
                inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

            // Create memory object for stock price 
            randBuffers[i] = clCreateBuffer(context,
                                        inMemFlags,
                                        numSamplesPerGPU[i] * sizeof(cl_float4),
                                        NULL,
                                        &status);
            CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (randBuffers[i])");

            // Create memory object for output array 
            outputBuffers[i] = clCreateBuffer(context,
                                       CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                       numSamplesPerGPU[i] * sizeof(cl_float4),
                                       NULL,
                                       &status);
            CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffers[i])");
        }
    }
    else //Single GPU case
    {
        // Create memory object for stock price 
        randBuffer = clCreateBuffer(context,
                                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    numSamples * sizeof(cl_float4),
                                    randArray,
                                    &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (randBuffer)");

        // Create memory object for output array 
        outBuffer = clCreateBuffer(context,
                                   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                                   numSamples * sizeof(cl_float4),
                                   output,
                                   &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outBuffer)");
    }

    // create a CL program using the kernel source 
    streamsdk::SDKFile kernelFile;
    std::string kernelPath = sampleCommon->getPath();

    if(isLoadBinaryEnabled())
    {
        kernelPath.append(loadBinary.c_str());
    }
    else 
    {
        kernelPath.append("BinomialOptionMultiGPU_Kernels.cl");
    }

    /**
    * When multiGPU support is enabled.
    */
    if(!noMultiGPUSupport)
    {
        for (int i = 0; i < numGPUDevices; i++)
        {
            // create a CL program using the kernel source 
            streamsdk::buildProgramData buildData;
            buildData.kernelName = std::string("BinomialOptionMultiGPU_Kernels.cl");
            buildData.devices = gpuDeviceIDs;
            buildData.deviceId = i;
            buildData.flagsStr = std::string("");
            if(isLoadBinaryEnabled())
                buildData.binaryName = std::string(loadBinary.c_str());

            if(isComplierFlagsSpecified())
                buildData.flagsFileName = std::string(flags.c_str());

            int retValue = sampleCommon->buildOpenCLProgram(programs[i], context, buildData);
            CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

            // get a kernel object handle for a kernel with the given name 
            kernels[i] = clCreateKernel(programs[i], "binomial_options", &status);
            CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

            // Check group-size against what is returned by kernel 
            status = clGetKernelWorkGroupInfo(kernels[i],
                                              gpuDeviceIDs[i],
                                              CL_KERNEL_WORK_GROUP_SIZE,
                                              sizeof(size_t),
                                              &kernelWorkGroupSize,
                                              0);
            CHECK_OPENCL_ERROR(status, "clGetKernelWorkGroupInfo failed.");

            // If group-size is greater than maximum supported on kernel 
            if((size_t)(numSteps + 1) > kernelWorkGroupSize)
            {
                if(!quiet)
                {
                    std::cout << "Out of Resources!" << std::endl;
                    std::cout << "Group Size specified : " << (numSteps + 1) << std::endl;
                    std::cout << "Max Group Size supported on the kernel : " 
                              << kernelWorkGroupSize << std::endl;
                    std::cout << "Using appropiate group-size." << std::endl;
                    std::cout << "-------------------------------------------" << std::endl;
                }
                numSteps = (cl_int)kernelWorkGroupSize - 2;
            }
        }

    }
    else
    {
         // create a CL program using the kernel source 
        streamsdk::buildProgramData buildData;
        buildData.kernelName = std::string("BinomialOptionMultiGPU_Kernels.cl");
        buildData.devices = devices;
        buildData.deviceId = deviceId;
        buildData.flagsStr = std::string("");
        if(isLoadBinaryEnabled())
            buildData.binaryName = std::string(loadBinary.c_str());

        if(isComplierFlagsSpecified())
            buildData.flagsFileName = std::string(flags.c_str());

        int retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
        CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

        // get a kernel object handle for a kernel with the given name 
        kernel = clCreateKernel(program,
                                "binomial_options",
                                &status);
        CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

        // Get kernel work group size 
        status = clGetKernelWorkGroupInfo(kernel,
                                          devices[deviceId],
                                          CL_KERNEL_WORK_GROUP_SIZE,
                                          sizeof(size_t),
                                          &kernelWorkGroupSize,
                                          0);
        CHECK_OPENCL_ERROR(status, "clGetKernelWorkGroupInfo failed.");
        // If group-size is gerater than maximum supported on kernel 
        if((size_t)(numSteps + 1) > kernelWorkGroupSize)
        {
            if(!quiet)
            {
                std::cout << "Out of Resources!" << std::endl;
                std::cout << "Group Size specified : " << (numSteps + 1) << std::endl;
                std::cout << "Max Group Size supported on the kernel : " 
                          << kernelWorkGroupSize << std::endl;
                std::cout << "Using appropiate group-size." << std::endl;
                std::cout << "-------------------------------------------" << std::endl;
            }
            numSteps = (cl_int)kernelWorkGroupSize - 2;
        }
    }

    return SDK_SUCCESS;
}


int 
BinomialOptionMultiGPU::runCLKernels()
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
    CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt) Failed");
            
    memcpy(mapPtr, 
        randArray, 
        numSamples * sizeof(cl_float4));

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
    CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inUnmapEvent) Failed");
  
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

    if(localThreads[0] > maxWorkItemSizes[0] || 
        localThreads[0] > maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support"
                     "requested number of work items.";
        return SDK_FAILURE;
    }

    status = clGetKernelWorkGroupInfo(kernel,
                                      devices[deviceId],
                                      CL_KERNEL_LOCAL_MEM_SIZE,
                                      sizeof(cl_ulong),
                                      &usedLocalMemory,
                                      NULL);
    CHECK_OPENCL_ERROR(status, "clGetKernelWorkGroupInfo(CL_KERNEL_LOCAL_MEM_SIZE) failed.");

    if(usedLocalMemory > totalLocalMemory)
    {
        std::cout << "Unsupported: Insufficient local memory on device."
                  << std::endl;
        return SDK_FAILURE;
    }


    // This algorithm reduces each group of work-items to a single value on OpenCL device

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

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        ndrEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
    }

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
    CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(outMapEvt) Failed");

    memcpy(output, 
        outMapPtr, 
        numSamples * sizeof(cl_float4));

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
    CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(outUnmapEvt) Failed");
 
    // Release NDR event
    status = clReleaseEvent(ndrEvt);
    CHECK_OPENCL_ERROR(status, "clReleaseEvent(ndrEvt) failed.");
    return SDK_SUCCESS;
}

int 
BinomialOptionMultiGPU::loadBalancing()
{
    /**
    * Calculating the peak GFlops of each device
    **/

    peakGflopsGPU = new cl_double[numGPUDevices];

    CHECK_ALLOCATION(peakGflopsGPU, "Allocation failed(peakGflopGPU)");

    cumulativeSumPerGPU = new cl_int[numGPUDevices];
    CHECK_ALLOCATION(cumulativeSumPerGPU, "Allocation failed(cumulativeSumPerGPU)");

    for (int i = 0; i < numGPUDevices; i++)
    {
        cl_int numComputeUnits = devicesInfo[i].maxComputeUnits;

        cl_int maxClockFrequency = devicesInfo[i].maxClockFrequency;

        char *deviceName = devicesInfo[i].name;
        
        /**
        * In cayman device we have four processing elements in a stream processor
        **/
        int numProcessingElts;
        if (!strcmp(deviceName, "Cayman"))
        {
            numProcessingElts = 4;
        }
        else 
        {
            numProcessingElts = 5;
        }
        /**
        * We have 16 stream processors per compute unit and numProcessingelts number of 
        * processing elements in a stream processor. A processing elt can execute 2 floating point operations
        * per cycle. So the peakGflops fomula would be 
        *
        *   peakGflops = numComputeUnits * numStreamProcessor per Compute Unit * numProcessingElt per stream processor
        *                       * maxClockFrequency * num Floating Point operations per cycle per processing elt;
        *                       We have to divide the result by 1000 to get the Gflops as freq is in MHz.
        *
        **/
        peakGflopsGPU[i] = (numComputeUnits * 16 * 
                            numProcessingElts * maxClockFrequency * 2) / 1000;
    }

    //Work sharing
    double totalPeakGflops = 0;//Sum of peakGflops of all the devices
    for (int i = 0; i < numGPUDevices; i++)
    {
        totalPeakGflops += peakGflopsGPU[i];
    }

    /**
    * ratios function will be used in load balancing statically
    **/
    double *ratios = new cl_double[numGPUDevices];
    CHECK_ALLOCATION(ratios, "Allocation failed!!(ratios)");

    numSamplesPerGPU = new cl_int[numGPUDevices];//Divison of input 
    CHECK_ALLOCATION(numSamplesPerGPU, "Allocation failed!!(numSamplesPerGPU)");

    int cumulativeSumSamples = 0;

    for (int i = 0; i < numGPUDevices; i++)
    {
        ratios[i] = peakGflopsGPU[i] / totalPeakGflops;
        numSamplesPerGPU[i] = static_cast<cl_int>(numSamples * ratios[i]);
        cumulativeSumSamples = cumulativeSumSamples + numSamplesPerGPU[i];
        cumulativeSumPerGPU[i] = cumulativeSumSamples;

        /**
        * There can be possibility that some values are missed. To avoid, add the missing values to the last GPU
        **/
        if (i == numGPUDevices - 1 && cumulativeSumSamples < numSamples)
        {
            numSamplesPerGPU[i] = numSamplesPerGPU[i] + 
                                  (numSamples - cumulativeSumSamples);
            cumulativeSumPerGPU[i] = numSamples;
        }
    }

    if(ratios)
    {
        delete []ratios;
        ratios = NULL;
    }

    return SDK_SUCCESS;
}

/*
 * Reduces the input array (in place)
 * length specifies the length of the array
 */
int 
BinomialOptionMultiGPU::binomialOptionMultiGPUCPUReference()
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
         * The computation is similar for put options
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
    
    if(stepsArray)
    {
        free(stepsArray);
        stepsArray = NULL;
    }

    return SDK_SUCCESS;
}

int BinomialOptionMultiGPU::initialize()
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
    num_samples = NULL;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "Error. Failed to allocate memory (num_iterations)\n");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);
    delete num_iterations;
    num_iterations = NULL;

    return SDK_SUCCESS;
}

int BinomialOptionMultiGPU::setup()
{
    CHECK_ERROR(setupBinomialOptionMultiGPU(), SDK_SUCCESS, "OpenCL Resource BinomialOptionMultiGPU::setup failed");

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    CHECK_ERROR(setupCL(), SDK_SUCCESS, "OpenCL Resource setup failed");

    sampleCommon->stopTimer(timer);
    // Compute setup time
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}

/**
* This structure is used in multi threading 
*/
struct dataPerGPU
{
    BinomialOptionMultiGPU *boObj;
    int deviceNumber;
};

void* threadFuncPerGPU(void *data1)
{
    dataPerGPU *data = (dataPerGPU *)data1;
    int deviceNumber = data->deviceNumber;
    BinomialOptionMultiGPU *boObj = data->boObj;
    cl_int status;
    cl_event ndrEvt;
    cl_int eventStatus = CL_QUEUED;

    cl_event inMapEvt;
    void* mapPtr = clEnqueueMapBuffer(boObj->commandQueues[deviceNumber], 
                                      boObj->randBuffers[deviceNumber], 
                                      CL_FALSE, 
                                      CL_MAP_WRITE, 
                                      0,
                                      boObj->numSamplesPerGPU[deviceNumber] * sizeof(cl_float4),
                                      0,
                                      NULL,
                                      &inMapEvt,
                                      &status);

    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clEnqueueMapBuffer failed !!");

    status = clFlush(boObj->commandQueues[deviceNumber]);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clFlush failed !!");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        inMapEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR_RETURN_NULL(status, "clGetEventInfo failed !!");
    }

    status = clReleaseEvent(inMapEvt);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clReleaseEvent failed !!");

    memcpy(mapPtr,
        boObj->randArray + (boObj->cumulativeSumPerGPU[deviceNumber] - boObj->numSamplesPerGPU[deviceNumber]) * 4,
        (boObj->numSamplesPerGPU[deviceNumber] * sizeof(cl_float4)));

    cl_event inUnmapEvent;

    status = clEnqueueUnmapMemObject(boObj->commandQueues[deviceNumber],
                                    boObj->randBuffers[deviceNumber],
                                    mapPtr,
                                    0,
                                    NULL,
                                    &inUnmapEvent);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clEnqueueUnmapMemObject failed !!");

    status = clFlush(boObj->commandQueues[deviceNumber]);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clFlush failed !!");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        inUnmapEvent, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR_RETURN_NULL(status, "clGetEventInfo failed !!");
    }

    status = clReleaseEvent(inUnmapEvent);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clReleaseEvent failed !!");

    // Set appropriate arguments to the kernel 
    status = clSetKernelArg(boObj->kernels[deviceNumber], 
                0, 
                sizeof(int), 
                (void*)&boObj->numSteps);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clSetKernelArg failed !!");

    status = clSetKernelArg(boObj->kernels[deviceNumber], 
                1, 
                sizeof(cl_mem), 
                (void*)&boObj->randBuffers[deviceNumber]);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clSetKernelArg failed !!");

    status = clSetKernelArg(boObj->kernels[deviceNumber], 
                2, 
                sizeof(cl_mem), 
                (void*)&boObj->outputBuffers[deviceNumber]);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clSetKernelArg failed !!");

    status = clSetKernelArg(boObj->kernels[deviceNumber],
                            3,
                            (boObj->numSteps + 1) * sizeof(cl_float4),
                            NULL);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clSetKernelArg failed !!");

    status = clSetKernelArg(boObj->kernels[deviceNumber],
                            4,
                            boObj->numSteps * sizeof(cl_float4),
                            NULL);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clSetKernelArg failed !!");

    // Enqueue a kernel run call.
    /**
    * numsamples is divided amongst the available devices in loadBalancing function.
    */
    size_t globalThreads[] = {(boObj->numSamplesPerGPU[deviceNumber]) * 
                             (boObj->numSteps + 1)};

    size_t localThreads[] = {boObj->numSteps + 1};

    if(localThreads[0] > boObj->devicesInfo[deviceNumber].maxWorkItemSizes[0] || 
        localThreads[0] > boObj->devicesInfo[deviceNumber].maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support"
                     "requested number of work items.";
        return NULL;
    }

    status = clGetKernelWorkGroupInfo(boObj->kernels[deviceNumber],
                                      boObj->gpuDeviceIDs[deviceNumber],
                                      CL_KERNEL_LOCAL_MEM_SIZE,
                                      sizeof(cl_ulong),
                                      &boObj->usedLocalMemory,
                                      NULL);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clGetKernelWorkGroupInfo failed !!");

    if(boObj->usedLocalMemory > boObj->totalLocalMemory)
    {
        std::cout << "Unsupported: Insufficient local memory on device."
                  << std::endl;
        return NULL;
    }


    /** 
     * This algorithm reduces each group of work-items to a single value
     * on OpenCL device
     */
    status = clEnqueueNDRangeKernel(
                    boObj->commandQueues[deviceNumber],
                    boObj->kernels[deviceNumber],
                    1,
                    NULL,
                    globalThreads,
                    localThreads,
                    0,
                    NULL,
                    &ndrEvt);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clEnqueueNDRangeKernel failed !!");

    status = clFlush(boObj->commandQueues[deviceNumber]);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clFlush failed !!");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        ndrEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR_RETURN_NULL(status, "clGetEventInfo failed !!");
    }

    cl_event outMapEvt;
    cl_uint* outMapPtr = (cl_uint*)clEnqueueMapBuffer(boObj->commandQueues[deviceNumber],
                                                   boObj->outputBuffers[deviceNumber],
                                                   CL_FALSE,
                                                   CL_MAP_READ,
                                                   0,
                                                   (boObj->numSamplesPerGPU[deviceNumber]) * sizeof(cl_float4),
                                                   0,
                                                   NULL,
                                                   &outMapEvt,
                                                   &status);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clEnqueueMapBuffer failed !!");

    status = clFlush(boObj->commandQueues[deviceNumber]);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clFlush failed !!");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        outMapEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR_RETURN_NULL(status, "clGetEventInfo failed !!");
    }

    status = clReleaseEvent(outMapEvt);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clReleaseEvent failed !!");

    memcpy(boObj->output + ((boObj->cumulativeSumPerGPU[deviceNumber] - boObj->numSamplesPerGPU[deviceNumber]) * 4), 
        outMapPtr, 
        (boObj->numSamplesPerGPU[deviceNumber] * sizeof(cl_float4)));

    cl_event outUnmapEvt;
    status = clEnqueueUnmapMemObject(boObj->commandQueues[deviceNumber],
                                     boObj->outputBuffers[deviceNumber],
                                     (void*)outMapPtr,
                                     0,
                                     NULL,
                                     &outUnmapEvt);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clEnqueueUnmapMemObject failed !!");

    status = clFlush(boObj->commandQueues[deviceNumber]);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clFlush failed !!");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        outUnmapEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR_RETURN_NULL(status, "clGetEventInfo failed !!");
    }

    status = clReleaseEvent(outUnmapEvt);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clReleaseEvent failed !!");

    // Release NDR event
    status = clReleaseEvent(ndrEvt);
    CHECK_OPENCL_ERROR_RETURN_NULL(status, "clReleaseEvent failed !!");
    return NULL;
}

int 
BinomialOptionMultiGPU::runCLKernelsMultiGPU()
{
    streamsdk::SDKThread *threads = new streamsdk::SDKThread[numGPUDevices];
    CHECK_ALLOCATION(threads, "Allocation failed!!");

    /**
    * Creating one thread per GPU
    */
    dataPerGPU *data = new dataPerGPU[numGPUDevices];
    for (int i = 0 ; i < numGPUDevices; i++)
    {
        data[i].deviceNumber = i; 
        data[i].boObj = this;
        threads[i].create(threadFuncPerGPU, 
                        (void *) &data[i]);        
    }
    /**
    * Call join() function for synchronization
    * Main thread will wait for each thread to get completed. 
    */
    for (int i = 0; i < numGPUDevices; i++)
    {
        threads[i].join();
    }

    delete []data;
    delete []threads;
    return SDK_SUCCESS;
}

int BinomialOptionMultiGPU::run()
{
    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        if(!noMultiGPUSupport)
        {
            CHECK_ERROR(runCLKernelsMultiGPU(), SDK_SUCCESS, "OpenCL noMultiGPUSupport failed");
        }
        else 
        {
            CHECK_ERROR(runCLKernels(), SDK_SUCCESS, "OpenCL Run failed");
        }
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
        if(!noMultiGPUSupport)
        {
             CHECK_ERROR(runCLKernelsMultiGPU(), SDK_SUCCESS, "OpenCL noMultiGPUSupport failed");
        } 
        else 
        {
            CHECK_ERROR(runCLKernels(), SDK_SUCCESS, "OpenCL Run failed");
        }
    }

    sampleCommon->stopTimer(timer);

    // Compute average kernel time
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
        sampleCommon->printArray<cl_float>("Output", 
                        output, 
                        numSamples, 
                        1);
    return SDK_SUCCESS;
}

int BinomialOptionMultiGPU::verifyResults()
{
    if(verify)
    {
        /* reference implementation
         * it overwrites the input array with the output
         */
        int result = SDK_SUCCESS;
        result = binomialOptionMultiGPUCPUReference();
        CHECK_ERROR(result, SDK_SUCCESS, "OpenCL verifyResults failed");

        // compare the results and see if they match
        if(sampleCommon->compare(output, 
                            refOutput, 
                            numSamples, 
                            0.001f))
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

void BinomialOptionMultiGPU::printStats()
{
    std::string strArray[4] = 
    {
        "Option Samples", 
        "Time(sec)", 
        "KernelTime(sec)" , 
        "Options/sec"
    };

    totalTime = setupTime + kernelTime;
    std::string stats[4];
    stats[0] = sampleCommon->toString(numSamples, 
                                std::dec);
    stats[1] = sampleCommon->toString(totalTime, 
                                std::dec);
    stats[2] = sampleCommon->toString(kernelTime, 
                                std::dec);
    stats[3] = sampleCommon->toString(numSamples/totalTime, 
                                std::dec);
    this->SDKSample::printStats(strArray, 
                                stats, 
                                4);
}

int
BinomialOptionMultiGPU::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    if(noMultiGPUSupport)
    {
        status = clReleaseKernel(kernel);
        CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

        status = clReleaseProgram(program);
        CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

        status = clReleaseMemObject(randBuffer);
        CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

        status = clReleaseMemObject(outBuffer);
        CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed!!");

        status = clReleaseCommandQueue(commandQueue);
        CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed!!");

        status = clReleaseContext(context);
        CHECK_OPENCL_ERROR(status, "clReleaseContext failed!!");
    }
    else 
    {
        for (int i = 0; i < numGPUDevices; i++)
        {
            status = clReleaseKernel(kernels[i]);
            CHECK_OPENCL_ERROR(status, "clReleaseKernel failed!!");

            status = clReleaseProgram(programs[i]);
            CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

            status = clReleaseMemObject(randBuffers[i]);
            CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

            status = clReleaseMemObject(outputBuffers[i]);
            CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

            status = clReleaseCommandQueue(commandQueues[i]);
            CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");
        }
        status = clReleaseContext(context);
        CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");
    }
    return SDK_SUCCESS;
}

BinomialOptionMultiGPU::~BinomialOptionMultiGPU()
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

     if(!noMultiGPUSupport)
     {
         if(kernels)
         {
             delete []kernels;
             kernels = NULL;
         }

         if(programs)
         {
             delete []programs;
             programs = NULL;
         }

         if(commandQueues)
         {
             delete []commandQueues;
             commandQueues = NULL;
         }

         if(peakGflopsGPU)
         {
             delete []peakGflopsGPU;
             peakGflopsGPU = NULL;
         }

         if(cumulativeSumPerGPU)
         {
             delete []cumulativeSumPerGPU;
             cumulativeSumPerGPU = NULL;
         }

         if(numSamplesPerGPU)
         {
            delete []numSamplesPerGPU;
            numSamplesPerGPU = NULL;
         }
         
         if(randBuffers)
         {
            delete []randBuffers;
            randBuffers = NULL;
         }

         if(outputBuffers)
         {
            delete []outputBuffers;
            outputBuffers = NULL;
         }

         if(devicesInfo)
         {
             delete []devicesInfo;
             devicesInfo = NULL;
         }
     }

     delete []gpuDeviceIDs;
     gpuDeviceIDs = NULL;
}

int 
main(int argc, char * argv[])
{
    BinomialOptionMultiGPU clBinomialOptionMultiGPU("OpenCL BinomialOptionMultiGPU");
    // Initialize
    if (clBinomialOptionMultiGPU.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clBinomialOptionMultiGPU.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clBinomialOptionMultiGPU.isDumpBinaryEnabled())
    {
        if(clBinomialOptionMultiGPU.genBinaryImage() != SDK_SUCCESS)
            return SDK_FAILURE;
    }
    // Setup
    if(clBinomialOptionMultiGPU.setup() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Run
    if(clBinomialOptionMultiGPU.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    // VerifyResults
    if(clBinomialOptionMultiGPU.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Cleanup
    if (clBinomialOptionMultiGPU.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    clBinomialOptionMultiGPU.printStats();

    return SDK_SUCCESS;
}
