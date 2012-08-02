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


#include "DeviceFission11Ext.hpp"


static clCreateSubDevicesEXT_fn pfn_clCreateSubDevicesEXT = NULL;
static clReleaseDeviceEXT_fn pfn_clReleaseDeviceEXT = NULL;

int
DeviceFission::setupDeviceFission()
{
    // make sure length is multiple of group size * numSubDevices
    unsigned int mulFactor = GROUP_SIZE * numSubDevices;
    length = (length < mulFactor) ? mulFactor : length;
    length = (length / mulFactor) * mulFactor;

    input = (float*)malloc(length * sizeof(float));
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    // Random initialisation of input
    sampleCommon->fillRandom<cl_float>(input, length, 1, 0, 5);

    // Unless quiet mode has been enabled, print the INPUT array
    if(!quiet) 
        sampleCommon->printArray<cl_float>("Input", input, VALUES_PRINTED, 1);

    rOutput = (float*)malloc(length * sizeof(float));
    CHECK_ALLOCATION(rOutput, "Failed to allocate host memory. (output)");

    memset(rOutput, 0, length * sizeof(cl_float));

    subOutput = (float*)malloc(length * sizeof(float));
    CHECK_ALLOCATION(subOutput, "Failed to allocate host memory. (subOutput)");

    memset(subOutput, 0, length * sizeof(cl_float));

    return SDK_SUCCESS;
}

int
DeviceFission::setupCLPlatform()
{
    cl_int status = CL_SUCCESS;
    cl_device_type dType;
    
    if(deviceType.compare("cpu") == 0)
        dType = CL_DEVICE_TYPE_CPU;
    else //deviceType = "gpu" 
    {
        std::cout << "Not supported on GPU. Falling back to CPU device" << std::endl;
        dType = CL_DEVICE_TYPE_CPU;
        deviceId = 0;
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

    rContext = clCreateContextFromType(
                  cps,
                  dType,
                  NULL,
                  NULL,
                  &status);
    CHECK_OPENCL_ERROR( status, "clCreateContextFromType failed.");

    // getting device on which to run the sample
    status = sampleCommon->getDevices(rContext, &rootDevices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(rootDevices[deviceId]);
    CHECK_ERROR(retValue, 0, "SDKDeviceInfo::setDeviceInfo() failed");

    // Check if byte-addressable store is supported
    if(!strstr(deviceInfo.extensions, "cl_ext_device_fission"))
    {
    	reqdExtSupport = CL_FALSE;
        OPENCL_EXPECTED_ERROR("Device does not support cl_ext_device_fission extension!");
    }

    // Initialize required partition property
    cl_device_partition_property_ext partitionPrty[3] =
    {
        CL_DEVICE_PARTITION_EQUALLY_EXT, 
        1, 
        CL_PROPERTIES_LIST_END_EXT
    };

    // Initialize clCreateSubDevicesEXT and clReleaseDeviceEXT function pointers
    INIT_CL_EXT_FCN_PTR(clCreateSubDevicesEXT);
    INIT_CL_EXT_FCN_PTR(clReleaseDeviceEXT);

    // Get number of sub-devices
    status = pfn_clCreateSubDevicesEXT(rootDevices[deviceId],
                                        partitionPrty, 
                                        0, 
                                        NULL, 
                                        &numSubDevices);
    CHECK_OPENCL_ERROR(status, "clCreateSubDevicesEXT failed.");

    subDevices = (cl_device_id*)malloc(numSubDevices * sizeof(cl_device_id));
    CHECK_ALLOCATION(subDevices, "Failed to allocate memory(subDevices)");

    status = pfn_clCreateSubDevicesEXT(rootDevices[deviceId],
                                        partitionPrty, 
                                        numSubDevices, 
                                        subDevices,
                                        NULL);
    CHECK_OPENCL_ERROR(status, "clCreateSubDevicesEXT failed.");

    // Create context for sub-devices
    subContext = clCreateContext(cps,
                                 numSubDevices,
                                 subDevices,
                                 NULL,
                                 NULL,
                                 &status);
    CHECK_OPENCL_ERROR(status, "clCreateContext failed.");

     return SDK_SUCCESS;
}

int 
DeviceFission::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("DeviceFission11Ext_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
DeviceFission::setupCLRuntime()
{
    cl_int status = CL_SUCCESS;

    // Create command queue 
    rCmdQueue = clCreateCommandQueue(rContext,
                                     rootDevices[deviceId],
                                     0,
                                     &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");

    // Create and initialize memory objects
    rInBuf = clCreateBuffer(rContext,
                            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            length * sizeof(cl_float),
                            input,
                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (rInBuf)");

    // Create memory objects for root device output 
    rOutBuf = clCreateBuffer(rContext,
                                    CL_MEM_WRITE_ONLY | 
                                    CL_MEM_USE_HOST_PTR,
                                    length * sizeof(cl_float),
                                    rOutput,
                                    &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (rOutBuf)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("DeviceFission11Ext_Kernels.cl");
    buildData.devices = rootDevices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string("");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    status = sampleCommon->buildOpenCLProgram(program, rContext, buildData);
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

    // Get a kernel object handle for a kernel with the given name 
    rKernel = clCreateKernel(program,
                            "copy",
                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    // Check whether specified groupSize is plausible on current kernel
    status = clGetKernelWorkGroupInfo(rKernel,
                                      rootDevices[deviceId],
                                      CL_KERNEL_WORK_GROUP_SIZE,
                                      sizeof(size_t),
                                      &kernelWorkGroupSize,
                                      0);
    CHECK_OPENCL_ERROR(status, "clGetKernelWorkGroupInfo failed.");

    // If groupSize exceeds the maximum supported on kernel, fall back 
    if(groupSize > kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << groupSize << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
        }
        groupSize = kernelWorkGroupSize;
    }

    //Setup sub-devices runtime resoureces

    subCmdQueue = (cl_command_queue*)malloc(numSubDevices * sizeof(cl_command_queue));
    CHECK_ALLOCATION(subCmdQueue,"Failed to allocate memory(subCmdQueue)");

    for(cl_uint i = 0; i < numSubDevices; i++)
    {
        // Create command queue 
        subCmdQueue[i] = clCreateCommandQueue(subContext,
                                         subDevices[i],
                                         0,
                                         &status);
        CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");
    }


    // Create and initialize memory objects
    
    subInBuf = (cl_mem*)malloc(numSubDevices * sizeof(cl_mem));
    CHECK_OPENCL_ERROR(status, "Failed to allocate memory(subInBuf)");

    for(cl_uint i = 0; i < numSubDevices; i++)
    {
        subInBuf[i] = clCreateBuffer(subContext,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  (length / numSubDevices) * sizeof(cl_float),
                                  &input[(length / numSubDevices) * i],
                                  &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (rInBuf)");
    }

    // Create memory objects for root device output 
    subOutBuf = clCreateBuffer(subContext,
                               CL_MEM_WRITE_ONLY,
                               (length / numSubDevices) * sizeof(cl_float) ,
                               NULL,
                               &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (rOutBuf)");

    streamsdk::SDKFile kernelFile;
    std::string kernelPath = sampleCommon->getPath();
    char * source = NULL;
    size_t sourceSize[] = {0};
    char * binary = NULL;
    size_t binarySize = 0;

    if(isLoadBinaryEnabled())
    {
		kernelPath += loadBinary;
        if(kernelFile.readBinaryFromFile(kernelPath.c_str()))
        {
            std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
            return SDK_FAILURE;
        }

        // Get binaries and binary sizes for all devices
        char** subBinaries = (char**)malloc(numSubDevices * sizeof(char*));
        if(subBinaries == NULL)
        {
            sampleCommon->error("Failed to allocate memory(subBinaries)");
            return SDK_FAILURE;
        }

        size_t* subBinariesSize = (size_t*)malloc(numSubDevices * sizeof(size_t*));
        if(subBinariesSize == NULL)
        {
            sampleCommon->error("Failed to allocate memory(subBinariesSize)");
            return SDK_FAILURE;
        }

        for(cl_uint i = 0; i < numSubDevices; ++i)
        {
            subBinaries[i] = (char*)kernelFile.source().c_str();
            subBinariesSize[i] = kernelFile.source().size();
        }

        subProgram = clCreateProgramWithBinary(subContext,
                                            numSubDevices,
                                            subDevices, 
                                            (const size_t *)subBinariesSize,
                                            (const unsigned char**)subBinaries,
                                            NULL,
                                            &status);
        if(sampleCommon->checkVal(status,
                                   CL_SUCCESS,
                                   "clCreateProgramWithBinary failed."))
        {
            return SDK_FAILURE;
        }

        free(subBinaries);
        free(subBinariesSize);
        subBinariesSize = NULL;
        subBinaries = NULL;

    }
    else
    {
        kernelPath.append("DeviceFission11Ext_Kernels.cl");
        if(!kernelFile.open(kernelPath.c_str()))//bool
        {
            std::cout << "Failed to load kernel file: " << kernelPath << std::endl;
            return SDK_FAILURE;
        }
        const char * source = kernelFile.source().c_str();
        size_t sourceSize[] = {strlen(source)};

        // create a CL program using the kernel source
        subProgram = clCreateProgramWithSource(subContext,
                                                1,
                                                (const char**)&source,
                                                sourceSize,
                                                &status);
        CHECK_OPENCL_ERROR(status, "clCreateProgramWithSource failed.");
    }

    // create a cl program executable for all the devices specified 
    status = clBuildProgram(subProgram,
                            numSubDevices,
                            subDevices,
                            NULL,
                            NULL,
                            NULL);
    if(status != CL_SUCCESS)
    {
        if(status == CL_BUILD_PROGRAM_FAILURE)
        {
            cl_int logStatus;
            char * buildLog = NULL;
            size_t buildLogSize = 0;
            logStatus = clGetProgramBuildInfo(subProgram, 
                                              subDevices[0], 
                                              CL_PROGRAM_BUILD_LOG, 
                                              buildLogSize, 
                                              buildLog, 
                                              &buildLogSize);
            if(!sampleCommon->checkVal(logStatus,
                                       CL_SUCCESS,
                                       "clGetProgramBuildInfo failed."))
                return SDK_FAILURE;

            buildLog = (char*)malloc(buildLogSize);
            if(NULL == buildLog)
            {
                sampleCommon->error("Failed to allocate host memory.(buildLog)");
                return SDK_FAILURE;
            }
            memset(buildLog, 0, buildLogSize);

            logStatus = clGetProgramBuildInfo(subProgram, 
                                              subDevices[0], 
                                              CL_PROGRAM_BUILD_LOG, 
                                              buildLogSize, 
                                              buildLog, 
                                              NULL);
            if(!sampleCommon->checkVal(logStatus,
                                       CL_SUCCESS,
                                       "clGetProgramBuildInfo failed."))
            {
                  free(buildLog);
                  return SDK_FAILURE;
            }

            std::cout << " \n\t\t\tBUILD LOG(SUB-DEVICES)\n";
            std::cout << " ************************************************\n";
            std::cout << buildLog << std::endl;
            std::cout << " ************************************************\n";
            free(buildLog);
        }

        if(!sampleCommon->checkVal(status,
                                   CL_SUCCESS,
                                   "clBuildProgram failed."))
            return SDK_FAILURE;
    }

    // Get a kernel object handle for a kernel with the given name 
    subKernel = clCreateKernel(subProgram,
                               "copy",
                               &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    return SDK_SUCCESS;
}


int
DeviceFission::runCLRootDeviceKerenls()
{
    cl_int status;
    cl_event events[1];

    // Set appropriate arguments to the kernel 

    // Set input 
    status = clSetKernelArg(rKernel, 0, sizeof(cl_mem),(void*)&rInBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (rInBuf)");

    // Set rOutBuf as second argument 
    status = clSetKernelArg(rKernel, 1, sizeof(cl_mem), (void*)&rOutBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (rOutBuf)");

    // Set global and local work items
    size_t globalThreads[] = {length};
    size_t localThreads[] = {groupSize};

    // Enqueue kernel
    status = clEnqueueNDRangeKernel(rCmdQueue,
                                    rKernel,
                                    1,
                                    NULL,
                                    globalThreads,
                                    localThreads,
                                    0,
                                    NULL,
                                    NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    status = clFinish(rCmdQueue);
    CHECK_OPENCL_ERROR(status, "clFinish failed.");

    /* Enqueue readBuffer*/
    status = clEnqueueReadBuffer(rCmdQueue,
                                 rOutBuf,
                                 CL_TRUE,
                                 0,
                                 length * sizeof(cl_float),
                                 rOutput,
                                 0,
                                 NULL,
                                 &events[0]);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = sampleCommon->waitForEventAndRelease(&events[0]);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(events[0]) Failed");

    return SDK_SUCCESS;
}

int
DeviceFission::runCLSubDeviceKerenls()
{
    cl_int status;
    cl_event events[1];

    // Set appropriate arguments to the kernel 

    // Set subOutBuf as second argument 
    status = clSetKernelArg(subKernel, 1, sizeof(cl_mem), (void*)&subOutBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (subOutBuf)");

    // Set global and local work items
    size_t globalThreads[] = {length / numSubDevices};
    size_t localThreads[] = {groupSize};

    for(cl_uint i = 0; i < numSubDevices; ++i)
    {
        // Set input 
        status = clSetKernelArg(subKernel, 0, sizeof(cl_mem),(void*)&subInBuf[i]);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (subInBuf)");

        // Enqueue kernel
        status = clEnqueueNDRangeKernel(subCmdQueue[i],
                                        subKernel,
                                        1,
                                        NULL,
                                        globalThreads,
                                        localThreads,
                                        0,
                                        NULL,
                                        NULL);
        CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

        status = clFinish(subCmdQueue[i]);
        CHECK_OPENCL_ERROR(status, "clFinish failed.");

        /* Enqueue readBuffer*/
        status = clEnqueueReadBuffer(subCmdQueue[i],
                                     subOutBuf,
                                     CL_TRUE,
                                     0,
                                     (length / numSubDevices) * sizeof(cl_float),
                                     &subOutput[(length / numSubDevices) * i],
                                     0,
                                     NULL,
                                     &events[0]);
        CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");
        
        status = sampleCommon->waitForEventAndRelease(&events[0]);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(events[0]) Failed");
    }
    return SDK_SUCCESS;
}

int 
DeviceFission::runCLKernels()
{
    // Run kernel on root device
    if(runCLRootDeviceKerenls() != CL_SUCCESS)
        return SDK_FAILURE;

    // Run kernsl on all sub-devices
    if(runCLSubDeviceKerenls() != CL_SUCCESS)
        return SDK_FAILURE;

    return SDK_SUCCESS;
}

int DeviceFission::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize())
        return SDK_FAILURE;

    // Now add customized options
    streamsdk::Option* array_length = new streamsdk::Option;
    CHECK_ALLOCATION(array_length, "Memory allocation error.\n");

    array_length->_sVersion = "x";
    array_length->_lVersion = "length";
    array_length->_description = "Length of the Input array";
    array_length->_type = streamsdk::CA_ARG_INT;
    array_length->_value = &length;
    sampleArgs->AddOption(array_length);
    delete array_length;

    return SDK_SUCCESS;
}

int 
DeviceFission::setup()
{
	cl_int retValue = setupCLPlatform();
    if(retValue != SDK_SUCCESS)
        return (retValue == SDK_EXPECTED_FAILURE)? SDK_EXPECTED_FAILURE:SDK_FAILURE;

	if(setupDeviceFission() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(setupCLRuntime() != SDK_SUCCESS)
        return SDK_FAILURE;

    return SDK_SUCCESS;
}

int DeviceFission::run()
{
	if(!reqdExtSupport)
        return SDK_SUCCESS;

	int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    // Execute kernel on device
    if(runCLKernels() != SDK_SUCCESS)
        return SDK_FAILURE;

	 sampleCommon->stopTimer(timer);
    // Compute average kernel time 
    kernelTimeGlobal = (double)(sampleCommon->readTimer(timer));

    if(!quiet)
        sampleCommon->printArray<cl_float>("Output", rOutput, VALUES_PRINTED, 1);

	sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    return SDK_SUCCESS;
}

int DeviceFission::verifyResults()
{
    if(verify)
    {
        bool rootDeviceRlt = sampleCommon->compare(input, rOutput, length);
        bool subDevicesRlt =sampleCommon->compare(input, subOutput, length);

        if(rootDeviceRlt && subDevicesRlt)
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

void DeviceFission::printStats()
{
	if(!reqdExtSupport)
        return;

    std::string strArray[2] = 
    {
        "Input length", 
        "RunTime(sec)"
    };
    std::string stats[2];

    stats[0] = sampleCommon->toString(length, std::dec);
    stats[1] = sampleCommon->toString(kernelTimeGlobal, std::dec);

    this->SDKSample::printStats(strArray, stats, 2);
}

int
DeviceFission::cleanup()
{

    // Releases all OpenCL resources of root device 
    cl_int status;

    status = clReleaseKernel(rKernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(rKernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseMemObject(rInBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed. (rInBuf)");

    status = clReleaseMemObject(rOutBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(rOutBuf)");

    status = clReleaseCommandQueue(rCmdQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(rCmdQueue)");

    status = clReleaseContext(rContext);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // Release all OpenCL resources for sub-devices
    status = clReleaseKernel(subKernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(subKernel)");

    status = clReleaseProgram(subProgram);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed. (subProgram)");

    status = clReleaseMemObject(subOutBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed. (subOutBuf)");

    for(cl_uint i = 0; i < numSubDevices; ++i)
    {
        status = clReleaseMemObject(subInBuf[i]);
        CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

        status = clReleaseCommandQueue(subCmdQueue[i]);
        CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");
    }

    status = clReleaseContext(subContext);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed. (subContext)");

    status = pfn_clReleaseDeviceEXT(rootDevices[deviceId]);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    return SDK_SUCCESS;
}

DeviceFission::~DeviceFission()
{
    /* release program resources (input memory etc.) */
    FREE(input);
    FREE(rOutput);
    FREE(subOutput);
    FREE(rootDevices);
    FREE(subDevices);
    FREE(subCmdQueue);
    FREE(subInBuf);
}

int 
main(int argc, char * argv[])
{
    DeviceFission clDeviceFission("OpenCL DeviceFission");

    // Initialize
    if(clDeviceFission.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // ParseCommandLine
    if(clDeviceFission.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clDeviceFission.isDumpBinaryEnabled())
    {
        if(clDeviceFission.genBinaryImage() != SDK_SUCCESS)
            return SDK_FAILURE;
        return SDK_SUCCESS;
    }
    else
    {
        // Setup
        cl_int retValue = clDeviceFission.setup();
        if(retValue != SDK_SUCCESS)
            return (retValue == SDK_EXPECTED_FAILURE)? SDK_SUCCESS: SDK_FAILURE;

        // Run
        if(clDeviceFission.run() != SDK_SUCCESS)
            return SDK_FAILURE;
        // VerifyResults
        if(clDeviceFission.verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;
        // Cleanup
        if(clDeviceFission.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

		clDeviceFission.printStats();
    }

    return SDK_SUCCESS;
}
