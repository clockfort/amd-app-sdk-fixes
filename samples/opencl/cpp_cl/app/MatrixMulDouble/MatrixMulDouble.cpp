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


#include "MatrixMulDouble.hpp"

int 
MatrixMulDouble::setupMatrixMulDouble()
{
    // allocate and init memory used by host  inputA[widthA][heightA]
    cl_uint inputSizeBytes0 = widthA * heightA * sizeof(cl_double);

    inputA = (cl_double*) malloc(inputSizeBytes0);
    CHECK_ALLOCATION(inputA, "Failed to allocate host memory. (inputA)");
    

    // allocate and init memory used by host inputB[widthB][heightB]
    cl_uint inputSizeBytes1 = widthB * heightB * sizeof(cl_double);

    inputB = (cl_double*) malloc(inputSizeBytes1);
    CHECK_ALLOCATION(inputB, "Failed to allocate host memory. (inputB)");
    
    
    // random initialisation of input 
    sampleCommon->fillRandom<cl_double>(inputA, widthA, heightA, 0, 10);
    sampleCommon->fillRandom<cl_double>(inputB, widthB, heightB, 0, 10);

    // allocate memory for output[widthB][heightA] 
    cl_uint outputSizeBytes = heightA * widthB * sizeof(cl_double);

    output = (cl_double*) malloc(outputSizeBytes);
    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");
    

    // allocate memory for output[widthB][heightA] of reference implemenation
    if(verify)
    {
        refOutput = (cl_double*) malloc(outputSizeBytes);
        CHECK_ALLOCATION(refOutput, "Failed to allocate host memory. (refOutput)");
        memset(refOutput, 0, outputSizeBytes);
    }
    /* 
     * Unless quiet mode has been enabled, print the INPUT arrays
     */
    if(!quiet) 
    {
        sampleCommon->printArray<cl_double>(
            "inputA", 
            inputA, 
            widthA, 
            1);
        sampleCommon->printArray<cl_double>(
            "inputB", 
            inputB, 
            widthB, 
            1);

    }

    return SDK_SUCCESS;
}

int 
MatrixMulDouble::genBinaryImage()
{
   streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("MatrixMulDouble_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}



int
MatrixMulDouble::setupCL(void)
{
    cl_int status = 0;
    cl_device_type dType;
    
    if(deviceType.compare("cpu") == 0)
        dType = CL_DEVICE_TYPE_CPU;
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
    status = cl::Platform::get(&platforms);
    CHECK_OPENCL_ERROR(status, "Platform::get() failed.");
    
    std::vector<cl::Platform>::iterator i;
    if(platforms.size() > 0)
    {
        if(isPlatformEnabled())
        {
            i = platforms.begin() + platformId;
        }
        else
        {
            for(i = platforms.begin(); i != platforms.end(); ++i)
            {
                if(!strcmp((*i).getInfo<CL_PLATFORM_VENDOR>().c_str(), 
                    "Advanced Micro Devices, Inc."))
                {
                    break;
                }
            }
        }
    }

    cl_context_properties cps[3] = 
    { 
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)(*i)(),
        0 
    };


    if(NULL == (*i)())
    {
        sampleCommon->error("NULL platform found so Exiting Application.");
        return SDK_FAILURE;
    }

    context = cl::Context(dType, cps, NULL, NULL, &status);
    CHECK_OPENCL_ERROR(status, "Context::Context() failed.");
    
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    CHECK_OPENCL_ERROR(status, "Context::getInfo() failed.");
    
    std::cout << "Platform :" << (*i).getInfo<CL_PLATFORM_VENDOR>().c_str() << "\n";
    int deviceCount = (int)devices.size();
    int j = 0;
    for (std::vector<cl::Device>::iterator i = devices.begin(); i != devices.end(); ++i, ++j)
    {
        std::cout << "Device " << j << " : ";
        std::string deviceName = (*i).getInfo<CL_DEVICE_NAME>();
        std::cout << deviceName.c_str() << "\n";
    }
    std::cout << "\n";

    if (deviceCount == 0) 
    {
        std::cerr << "No device available\n";
        return SDK_FAILURE;
    }

    if(sampleCommon->validateDeviceId(deviceId, deviceCount))
    {
        sampleCommon->error("sampleCommon::validateDeviceId() failed");
        return SDK_FAILURE;
    }

    std::string extensions = devices[deviceId].getInfo<CL_DEVICE_EXTENSIONS>();

    std::string buildOptions = std::string("");
    // Check if cl_khr_fp64 extension is supported 
    if(strstr(extensions.c_str(), "cl_khr_fp64"))
    {
        buildOptions.append("-D KHR_DP_EXTENSION");
    }
    else
    {
        // Check if cl_amd_fp64 extension is supported 
        if(!strstr(extensions.c_str(), "cl_amd_fp64"))
        {
            OPENCL_EXPECTED_ERROR("Device does not support cl_amd_fp64 extension!");
        }
    }
    cl_uint localMemType;
    // Get device specific information 
    status = devices[deviceId].getInfo<cl_uint>(
             CL_DEVICE_LOCAL_MEM_TYPE,
            &localMemType);
    CHECK_OPENCL_ERROR(status, "Device::getInfo CL_DEVICE_LOCAL_MEM_TYPE) failed.");
    
    // If scratchpad is available then update the flag 
    if(localMemType == CL_LOCAL)
        lds = true;

    // Get Device specific Information 
    status = devices[deviceId].getInfo<size_t>(
              CL_DEVICE_MAX_WORK_GROUP_SIZE, 
              &maxWorkGroupSize);

    CHECK_OPENCL_ERROR(status, "Device::getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed.");
    
    status = devices[deviceId].getInfo<cl_uint>(
             CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
             &maxDimensions);
    CHECK_OPENCL_ERROR(status, "Device::getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS) failed.");
    

    maxWorkItemSizes = (size_t*)malloc(maxDimensions * sizeof(size_t));
    
    std::vector<size_t> workItems = devices[deviceId].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();

    for(cl_uint i = 0; i < maxDimensions; ++i)
        maxWorkItemSizes[i] = workItems[i];

    status = devices[deviceId].getInfo<cl_ulong>(
             CL_DEVICE_LOCAL_MEM_SIZE,
             &totalLocalMemory);
    CHECK_OPENCL_ERROR(status, "Device::getInfo(CL_DEVICE_LOCAL_MEM_SIZES) failed.");
    
    // Set command queue properties
    cl_command_queue_properties prop = 0;
    if(!eAppGFLOPS)
        prop |= CL_QUEUE_PROFILING_ENABLE;

    commandQueue = cl::CommandQueue(context, devices[deviceId], prop, &status);
    CHECK_OPENCL_ERROR(status, "CommandQueue::CommandQueue() failed.");
    
    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create buffer for matrix A 
    inputBufA = cl::Buffer(
                    context, 
                    inMemFlags, 
                    sizeof(cl_double) * widthA * heightA, 
                    NULL, 
                    &status);
    CHECK_OPENCL_ERROR(status, "cl::Buffer failed. (inputBufA)");
    
    // Create buffer for matrix B 
    inputBufB = cl::Buffer(
                    context,
                    inMemFlags,
                    sizeof(cl_double) * widthB * heightB,
                    NULL,
                    &status);
    CHECK_OPENCL_ERROR(status, "cl::Buffer failed. (inputBufB)");
    
    outputBuf = cl::Buffer(
                    context,
                    CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                    sizeof(cl_double) * heightA * widthB,
                    NULL,
                    &status);
    CHECK_OPENCL_ERROR(status, "cl::Buffer failed. (outputBuf)");
   
    device.push_back(devices[deviceId]);

    // create a CL program using the kernel source
    streamsdk::SDKFile kernelFile;
    std::string kernelPath = sampleCommon->getPath();

    if(isLoadBinaryEnabled())
    {
        kernelPath.append(loadBinary.c_str());
        if(!kernelFile.readBinaryFromFile(kernelPath.c_str()))
        {
            std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
            return SDK_FAILURE;
        }
        cl::Program::Binaries programBinary(1,std::make_pair(
                                              (const void*)kernelFile.source().data(), 
                                              kernelFile.source().size()));
        
        program = cl::Program(context, device, programBinary, NULL, &status);
        CHECK_OPENCL_ERROR(status, "Program::Program(Binary) failed.");
        
    }
    else
    {
        kernelPath.append("MatrixMulDouble_Kernels.cl");
        if(!kernelFile.open(kernelPath.c_str()))
        {
            std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
            return SDK_FAILURE;
        }
        cl::Program::Sources programSource(
                                1,
                                std::make_pair(kernelFile.source().data(), 
                                kernelFile.source().size()));
        
        program = cl::Program(context, programSource, &status);
        CHECK_OPENCL_ERROR(status, "Program::Program(Source) failed.");
        
    }

    std::string flagsStr = std::string("");

    // Get build options if any
    flagsStr.append(buildOptions.c_str());

    // Get additional options
    if(isComplierFlagsSpecified())
    {
        streamsdk::SDKFile flagsFile;
        std::string flagsPath = sampleCommon->getPath();
        flagsPath.append(flags.c_str());
        if(!flagsFile.open(flagsPath.c_str()))
        {
            std::cout << "Failed to load flags file: " << flagsPath << std::endl;
            return SDK_FAILURE;
        }
        flagsFile.replaceNewlineWithSpaces();
        const char * flags = flagsFile.source().c_str();
        flagsStr.append(flags);
    }

    if(flagsStr.size() != 0)
        std::cout << "Build Options are : " << flagsStr.c_str() << std::endl;

    status = program.build(device, flagsStr.c_str());

    if(status != CL_SUCCESS)
    {
        if(status == CL_BUILD_PROGRAM_FAILURE)
        {
            std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[deviceId]);

            std::cout << " \n\t\t\tBUILD LOG\n";
            std::cout << " ************************************************\n";
            std::cout << str << std::endl;
            std::cout << " ************************************************\n";
        }
    }
    CHECK_OPENCL_ERROR(status, "Program::build() failed.");

    // Create kernel  

    // If local memory is present then use the specific kernel 
    if(lds)
        kernel = cl::Kernel(program, "mmmKernel_local", &status);
    else
        kernel = cl::Kernel(program, "mmmKernel", &status);

    CHECK_OPENCL_ERROR(status, "cl::Kernel failed.");
    status =  kernel.getWorkGroupInfo<cl_ulong>(

        devices[deviceId],
                CL_KERNEL_LOCAL_MEM_SIZE,
                &usedLocalMemory);
    CHECK_OPENCL_ERROR(status, "Kernel::getWorkGroupInfo(CL_KERNEL_LOCAL_MEM_SIZE) failed"
                ".(usedLocalMemory)");

    availableLocalMemory = totalLocalMemory - usedLocalMemory;
    if(lds)
        neededLocalMemory = (blockSize * 4) * (blockSize * 4) * sizeof(cl_double); 
    else
        neededLocalMemory = 0; 

    if(neededLocalMemory > availableLocalMemory)
    {
        std::cout << "Unsupported: Insufficient local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    // Check group size against group size returned by kernel 
    kernelWorkGroupSize = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[deviceId], &status);
    CHECK_OPENCL_ERROR(status, "Kernel::getWorkGroupInfo()  failed.");
    
    if((cl_uint)(blockSize * blockSize) > kernelWorkGroupSize)
    {
       if(kernelWorkGroupSize >= 64)
            blockSize = 8; 
        else if(kernelWorkGroupSize >= 32)
            blockSize = 4; 
        else
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << blockSize * blockSize << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelWorkGroupSize<<std::endl;
            return SDK_FAILURE;
        }
    }

    if(blockSize > maxWorkItemSizes[0] ||
       blockSize > maxWorkItemSizes[1] ||
       blockSize * blockSize > maxWorkGroupSize)
    {
        sampleCommon->error("Unsupported: Device does not support requested number of work items.");
        return SDK_FAILURE;
    }

     return SDK_SUCCESS;
}


int 
MatrixMulDouble::runCLKernels(void)
{
    cl_int   status;

    /* 
     * Kernel runs over complete output matrix with blocks of blockSize x blockSize 
     * running concurrently
     */
    size_t globalThreads[2]= {widthB / 4, heightA / 4};
    size_t localThreads[2] = {blockSize, blockSize};

    cl_int eventStatus = CL_QUEUED;

    // Set input data to matrix A and matrix B
    cl::Event inMapEvtA;
    cl::Event inMapEvtB;
    cl::Event inUnmapEvtA;
    cl::Event inUnmapEvtB;
    cl::Event outMapEvt;
    cl::Event outUnmapEvt;
    cl::Event ndrEvt;

    void* inMapPtrA = NULL;
    void* inMapPtrB = NULL;
    void* outMapPtr = NULL;

    inMapPtrA = commandQueue.enqueueMapBuffer(
                    inputBufA,
                    CL_FALSE,
                    CL_MAP_WRITE,
                    0,
                    widthA * heightA * sizeof(cl_double),
                    NULL,
                    &inMapEvtA,
                    &status);
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.enqueueMapBuffer failed. (inputBufA)");

    inMapPtrB = commandQueue.enqueueMapBuffer(
                    inputBufB,
                    CL_FALSE,
                    CL_MAP_WRITE,
                    0,
                    widthA * heightA * sizeof(cl_double),
                    NULL,
                    &inMapEvtB,
                    &status);
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.enqueueMapBuffer failed. (inputBufB)");

    status = commandQueue.flush();
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = inMapEvtA.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }

    memcpy(inMapPtrA, inputA, sizeof(cl_double) * widthA  * heightA);

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = inMapEvtB.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }

    memcpy(inMapPtrB, inputB, sizeof(cl_double) * widthB  * heightB);

    status = commandQueue.enqueueUnmapMemObject(
                inputBufA,
                inMapPtrA,
                NULL,
                &inUnmapEvtA);
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.enqueueUnmapMemObject (inputBufA).");

    status = commandQueue.enqueueUnmapMemObject(
                inputBufB,
                inMapPtrB,
                NULL,
                &inUnmapEvtB);
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.enqueueUnmapMemObject (inputBufB)");

    status = commandQueue.flush();
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = inUnmapEvtA.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = inUnmapEvtB.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }


    // Set appropriate arguments to the kernel 

    // output array as the 1st argument : stores product of inputA and inputB 
    status = kernel.setArg(0, inputBufA);
    CHECK_OPENCL_ERROR(status, "cl::setArg failed. (inputBufA)");

    status = kernel.setArg(1, inputBufB);
    CHECK_OPENCL_ERROR(status, "cl::setArg failed. (inputBufB)");

    status = kernel.setArg(2, outputBuf);
    CHECK_OPENCL_ERROR(status, "cl::setArg failed. (outputBuf)");

    // widthA of the inputA matrix as 4th argument - widthA 
    status = kernel.setArg(3, widthA);
    CHECK_OPENCL_ERROR(status, "cl::setArg failed. (widthA)");

    // Set local memory argument if Scratchpad is available 
    if(lds)
    {
        status = kernel.setArg(
                    4, 
                    (blockSize * 4) * (blockSize * 4) * sizeof(cl_double),
                    NULL);
        CHECK_OPENCL_ERROR(status, "cl::setArg failed. (local memory)");

    }
    else
    {
        status = kernel.setArg(4, sizeof(cl_int), &widthB);
        CHECK_OPENCL_ERROR(status, "cl::setArg failed. (widthB)");

    }

    /* 
     * Enqueue a kernel run call.
     */

    // Each thread calculates 2 gaussian numbers 
    cl::NDRange gThreads(globalThreads[0], globalThreads[1]);
    cl::NDRange lThreads(localThreads[0], localThreads[1]);

    status = commandQueue.enqueueNDRangeKernel(kernel,
                                               cl::NullRange,
                                               gThreads,
                                               lThreads,
                                               0, 
                                               &ndrEvt);
    CHECK_OPENCL_ERROR(status, "CommandQueue::enqueueNDRangeKernel() failed.");

    status = commandQueue.flush();
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = ndrEvt.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }

    if(!eAppGFLOPS)
    {
        // Calculate performance 
        cl_ulong startTime;
        cl_ulong endTime;
        status = ndrEvt.getProfilingInfo<cl_ulong>(
                    CL_PROFILING_COMMAND_START,
                    &startTime);
        CHECK_OPENCL_ERROR(status, "cl::Event.getProfilingInfo failed.(startTime).");

        status = ndrEvt.getProfilingInfo<cl_ulong>(
                    CL_PROFILING_COMMAND_END,
                    &endTime);
        CHECK_OPENCL_ERROR(status, "cl::Event.getProfilingInfo failed.(endTime).");

        // Print performance numbers 
        double sec = 1e-9 * (endTime - startTime);
        kernelTime += sec;
    }

    outMapPtr = commandQueue.enqueueMapBuffer(
                    outputBuf,
                    CL_FALSE,
                    CL_MAP_READ,
                    0,
                    widthB * heightA * sizeof(cl_double),
                    NULL,
                    &outMapEvt,
                    &status);
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.enqueueMapBuffer failed. (outputBuf).");

    status = commandQueue.flush();
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = outMapEvt.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }

    memcpy(output, outMapPtr, sizeof(cl_double) * widthB  * heightA);

    status = commandQueue.enqueueUnmapMemObject(
                outputBuf,
                outMapPtr,
                NULL,
                &outUnmapEvt);
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.enqueueUnmapMemObject (outputBuf)");

    status = commandQueue.flush();
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = outUnmapEvt.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }

    return SDK_SUCCESS;
}

/*
 * This is a naive O(N^3) CPU implementatio of matrix multiplication
 */
void 
MatrixMulDouble::MatrixMulDoubleCPUReference(
    cl_double * output,
    cl_double * inputA,
    cl_double * inputB,
    const cl_uint y,
    const cl_uint x,
    const cl_uint z)
{
    for(cl_uint i=0; i < y; i++)
    {
        for(cl_uint j=0; j < z; j++)
        {
            for(cl_uint k=0; k < x; k++)
            {
                output[i*z + j] += (inputA[i*x + k]*inputB[k*z + j]);
            }
        }
    }
}

int 
MatrixMulDouble::initialize()
{
    // Call base class Initialize to get default configuration 
    CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL resource initilization failed");

    // add an option for getting blockSize from commandline 
    streamsdk::Option* xParam = new streamsdk::Option;
    CHECK_ALLOCATION(xParam, "Memory Allocation error. (xParam)");

    xParam->_sVersion = "x";
    xParam->_lVersion = "heightA";
    xParam->_description = "height of matrix A";
    xParam->_type     = streamsdk::CA_ARG_INT;
    xParam->_value    = &n;

    sampleArgs->AddOption(xParam);
    delete xParam;

    streamsdk::Option* yParam = new streamsdk::Option;
    CHECK_ALLOCATION(yParam, "Memory Allocation error. (yParam)");

    yParam->_sVersion = "y";
    yParam->_lVersion = "widthA";
    yParam->_description = "width of matrix A and Height of matrix B";
    yParam->_type     = streamsdk::CA_ARG_INT;
    yParam->_value    = &m;

    sampleArgs->AddOption(yParam);
    delete yParam;

    streamsdk::Option* zParam = new streamsdk::Option;
    CHECK_ALLOCATION(zParam, "Memory Allocation error. (zParam)");

    zParam->_sVersion = "z";
    zParam->_lVersion = "widthB";
    zParam->_description = "width of matrix B";
    zParam->_type     = streamsdk::CA_ARG_INT;
    zParam->_value    = &k;

    sampleArgs->AddOption(zParam);
    delete zParam;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "Memory Allocation error. (num_iterations)");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);
    delete num_iterations;

    streamsdk::Option* appGflops_option = new streamsdk::Option;
    CHECK_ALLOCATION(appGflops_option, "Memory Allocation error. (appGflops_option)");

    appGflops_option->_sVersion = "";
    appGflops_option->_lVersion = "eAppGflops";
    appGflops_option->_description = "Prints GFLOPS calculated from transfer + kernel time";
    appGflops_option->_type = streamsdk::CA_NO_ARGUMENT;
    appGflops_option->_value = &eAppGFLOPS;

    sampleArgs->AddOption(appGflops_option);
    delete appGflops_option;

    return SDK_SUCCESS;
}

int 
MatrixMulDouble::setup()
{
    // Make sure the dimensions are multiples of blockSize 
    const int vectorSize = 4;
    if(n % (blockSize * vectorSize) != 0)
    {
        n = (n / (blockSize * vectorSize) + 1) * (blockSize * vectorSize);
    }

    if(m % blockSize != 0)
    {
        m = (m / blockSize + 1) * blockSize;
    }

    if(k % (blockSize * vectorSize) != 0)
    {
       k = (k / (blockSize * vectorSize) + 1) * (blockSize * vectorSize);
    }

    widthA  = m;
    heightA = n;
    
    widthB  = k;
    heightB = m;

    if(setupMatrixMulDouble() !=  SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    int status = setupCL();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_EXPECTED_FAILURE;
        return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    setupTime = (cl_double)sampleCommon->readTimer(timer);

    return SDK_SUCCESS;
}


int 
MatrixMulDouble::run()
{
    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        int kernelRun = runCLKernels();
        CHECK_ERROR(kernelRun, SDK_SUCCESS, "OpenCL's runCLKernels failed");
    }

    kernelTime = 0;
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    std::cout << "Executing kernel for " << iterations 
              << " iterations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    appTime = (double)(sampleCommon->readTimer(timer)) / iterations;
    kernelTime /= iterations;

    if(!quiet)
        sampleCommon->printArray<cl_double>("Output", output, widthB, 1);

    return SDK_SUCCESS;
}

int 
MatrixMulDouble::verifyResults()
{
    if(verify)
    {
        // reference implementation 
        MatrixMulDoubleCPUReference(refOutput, inputA, inputB, heightA, widthA,  widthB);

        // compare the results and see if they match 
        if(sampleCommon->compare(output, refOutput, heightA * widthB))
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
MatrixMulDouble::printStats()
{
    if(eAppGFLOPS)
    {
        std::string strArray[4] = {"MatrixA", "MatrixB", "Time(sec)", "[Transfer+kernel]Time(sec)"};
        std::string stats[4];

        double flops = 2 * widthA * widthB;
        double perf = (flops / appTime) * heightA * 1e-9;
        if(timing)
            std::cout << "GFlops achieved : " << perf 
                      << "  Note : Transfer time included to calculate GFLOPS" 
                      << std::endl << std::endl;

        totalTime = setupTime + appTime;

        stats[0]  = sampleCommon->toString(heightA, std::dec)
                    +"x"+sampleCommon->toString(widthA, std::dec);
        stats[1]  = sampleCommon->toString(heightB, std::dec)
                    +"x"+sampleCommon->toString(widthB, std::dec);
        stats[2]  = sampleCommon->toString(totalTime, std::dec);
        stats[3]  = sampleCommon->toString(appTime, std::dec);
        
        this->SDKSample::printStats(strArray, stats, 4);
    }
    else
    {
        std::string strArray[4] = {"MatrixA", "MatrixB", "Time(sec)", "KernelTime(sec)"};
        std::string stats[4];

        double flops = 2 * widthA * widthB;
        double perf = (flops / kernelTime) * heightA * 1e-9;
        if(timing)
            std::cout << "GFlops achieved : " << perf << std::endl << std::endl;

        totalTime = setupTime + kernelTime;

        stats[0]  = sampleCommon->toString(heightA, std::dec)
                    +"x"+sampleCommon->toString(widthA, std::dec);
        stats[1]  = sampleCommon->toString(heightB, std::dec)
                    +"x"+sampleCommon->toString(widthB, std::dec);
        stats[2]  = sampleCommon->toString(totalTime, std::dec);
        stats[3]  = sampleCommon->toString(kernelTime, std::dec);

        this->SDKSample::printStats(strArray, stats, 4);
    }
}

int 
MatrixMulDouble::cleanup()
{
    // release program resources (input memory etc.) 
        FREE(inputA);
        FREE(inputB);
        FREE(output);
        FREE(refOutput);
    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    MatrixMulDouble clMatrixMulDouble("OpenCL Matrix Multiplication for Double");

    if(clMatrixMulDouble.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clMatrixMulDouble.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clMatrixMulDouble.isDumpBinaryEnabled())
    {
        return clMatrixMulDouble.genBinaryImage();
    }
    else
    {
        // Setup
        int status = clMatrixMulDouble.setup();
        if(status != SDK_SUCCESS)
            return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

        // Run
        if(clMatrixMulDouble.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        // VerifyResults
        if(clMatrixMulDouble.verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;

        // Cleanup
        if(clMatrixMulDouble.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        clMatrixMulDouble.printStats();
    }

    return SDK_SUCCESS;
}
