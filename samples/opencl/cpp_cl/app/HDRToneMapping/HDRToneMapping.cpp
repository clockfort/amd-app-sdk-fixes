/* ============================================================

Copyright (c) 2011 Advanced Micro Devices, Inc.  All rights reserved.

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

/***
* \File HDRToneMapping.cpp
* \brief definition of the class HDRToneMapping
**/

#include "HDRToneMapping.hpp"

int 
HDRToneMapping::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("HDRToneMapping_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;

}

int 
HDRToneMapping::setupCL()
{
    cl_int err = CL_SUCCESS;
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
    err = cl::Platform::get(&platforms);
    CHECK_OPENCL_ERROR(err, "Platform::get() failed.");

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

    context = cl::Context(dType, cps, NULL, NULL, &err);
    CHECK_OPENCL_ERROR(err, "Context::Context() failed.");

    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    CHECK_OPENCL_ERROR(err, "Context::getInfo() failed.");

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
        std::cout << "No device available\n";
        return SDK_FAILURE;
    }

    if(sampleCommon->validateDeviceId(deviceId, deviceCount) != SDK_SUCCESS)
    {
        std::cout << "sampleCommon::validateDeviceId() failed";
        return SDK_FAILURE;
    }

    // Get Device specific Information 
    err = devices[deviceId].getInfo<size_t>(
              CL_DEVICE_MAX_WORK_GROUP_SIZE, 
              &maxWorkGroupSize);
    CHECK_OPENCL_ERROR(err, "Device::getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed.");
    
    err = devices[deviceId].getInfo<cl_uint>(
             CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
             &maxDimensions);
    CHECK_OPENCL_ERROR(err, "Device::getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS) failed.");

    maxWorkItemSizes = (size_t*)malloc(maxDimensions * sizeof(size_t));
    
    std::vector<size_t> workItems = devices[deviceId].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();

    for(cl_uint i = 0; i < maxDimensions; ++i)
        maxWorkItemSizes[i] = workItems[i];

    err = devices[deviceId].getInfo<cl_ulong>(
             CL_DEVICE_LOCAL_MEM_SIZE,
             &totalLocalMemory);
    CHECK_OPENCL_ERROR(err, "Device::getInfo(CL_DEVICE_LOCAL_MEM_SIZES) failed.");

    commandQueue = cl::CommandQueue(context, devices[deviceId], 0, &err);
    CHECK_OPENCL_ERROR(err, "CommandQueue::CommandQueue() failed.");
    
    /*
    * Create and initialize memory objects
    */

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create memory object for input Image 
    /**
    * We use CL_MEM_USE_HOST_PTR for CPU as the CPU device is running the kernel 
    * on the actual buffer provided by the application 
    */
    if (dType == CL_DEVICE_TYPE_CPU)
    {
        inputImageBuffer = cl::Buffer(context,
                                CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                width * height * numChannels * sizeof(cl_float),
                                input,
                                &err);
        CHECK_OPENCL_ERROR(err, "Buffer::Buffer() failed. (inputImageBuffer)");
        // Create memory object for output Image 
        outputImageBuffer = cl::Buffer(context, 
                                       CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, 
                                       width * height * numChannels * sizeof(cl_float),
                                       output,
                                       &err);
        CHECK_OPENCL_ERROR(err, "Buffer::Buffer() failed. (outputImageBuffer)");
    }
    else if (dType == CL_DEVICE_TYPE_GPU)
    {
        inputImageBuffer = cl::Buffer(context, 
                                      inMemFlags, 
                                      width * height * numChannels * sizeof(cl_float),
                                      0,
                                      &err);
        CHECK_OPENCL_ERROR(err, "Buffer::Buffer() failed. (inputImageBuffer)");
        // Create memory object for output Image 
        outputImageBuffer = cl::Buffer(context, 
                                       CL_MEM_WRITE_ONLY, 
                                       width * height * numChannels * sizeof(cl_float),
                                       NULL,
                                       &err);
        CHECK_OPENCL_ERROR(err, "Buffer::Buffer() failed. (outputImageBuffer)");
    }

    device.push_back(devices[deviceId]);

    // create a CL program using the kernel source 
    streamsdk::SDKFile kernelFile;
    std::string kernelPath = sampleCommon->getPath();

    if(isLoadBinaryEnabled())
    {
        kernelPath.append(loadBinary.c_str());
        if(kernelFile.readBinaryFromFile(kernelPath.c_str()))
        {
            std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
            return SDK_FAILURE;
        }
        cl::Program::Binaries programBinary(1,std::make_pair(
                                              (const void*)kernelFile.source().data(), 
                                              kernelFile.source().size()));
        
        program = cl::Program(context, device, programBinary, NULL, &err);
        CHECK_OPENCL_ERROR(err, "Program::Program(Binary) failed.");
    }
    else
    {
        kernelPath.append("HDRToneMapping_Kernels.cl");
        if(!kernelFile.open(kernelPath.c_str()))
        {
            std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
            return SDK_FAILURE;
        }

        cl::Program::Sources programSource(1, 
            std::make_pair(kernelFile.source().data(), 
            kernelFile.source().size()));

        program = cl::Program(context, programSource, &err);
        CHECK_OPENCL_ERROR(err, "Program::Program(Source) failed.");
    }

    std::string flagsStr = std::string("");

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

    err = program.build(device, flagsStr.c_str());
    if(err != CL_SUCCESS)
    {
        if(err == CL_BUILD_PROGRAM_FAILURE)
        {
            std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[deviceId]);

            std::cout << " \n\t\t\tBUILD LOG\n";
            std::cout << " ************************************************\n";
            std::cout << str << std::endl;
            std::cout << " ************************************************\n";
        }
    }

    CHECK_OPENCL_ERROR(err, "Program::build() failed.");

    // Create kernel 
    kernel = cl::Kernel(program, "toneMappingPattanaik",  &err);
    CHECK_OPENCL_ERROR(err, "Kernel::Kernel() failed.");

    // Check group size against group size returned by kernel 
    kernelWorkGroupSize = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[deviceId], &err);
    CHECK_OPENCL_ERROR(err, "Kernel::getWorkGroupInfo()  failed.");

    /**
    * For CPU device the kernel work group size is 1024.
    * Workgroup creation/replacement is an overhead - 
    * avoid workgroups with small number of workitems (we pay more for replacing a WG than running more WI in a for loop).
    */
    if (kernelWorkGroupSize >= 1024)
    {
        blockSizeX = 32;
        blockSizeY = 32;
    }

    if((cl_uint)(blockSizeX * blockSizeY) > kernelWorkGroupSize)
    {
        if(kernelWorkGroupSize >= 64)
        {
            blockSizeX = 8;
            blockSizeY = 8;
        }
        else if(kernelWorkGroupSize >= 32)
        {
            blockSizeX = 4;
            blockSizeY = 4;
        }
        else
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << blockSizeX * blockSizeY << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelWorkGroupSize<<std::endl;
            return SDK_FAILURE;
        }
    }

    if(blockSizeX > maxWorkItemSizes[0] ||
       blockSizeY > maxWorkItemSizes[1] ||
       blockSizeX * blockSizeY > maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support requested number of work items." << std::endl;
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int 
HDRToneMapping::runCLKernels()
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;

    if (deviceType.compare("gpu") == 0)
    {
        cl::Event mapEvt; 
        void *mapPtr = commandQueue.enqueueMapBuffer(inputImageBuffer, 
                                CL_FALSE,
                                CL_MAP_WRITE,
                                0,
                                width * height * numChannels * sizeof(cl_float),
                                NULL,
                                &mapEvt,
                                &status);
        CHECK_OPENCL_ERROR(status, "CommandQueue::enqueueWriteBuffer() failed. (inputImageBuffer)");

        status = commandQueue.flush();
        CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

        eventStatus = CL_QUEUED;
        while(eventStatus != CL_COMPLETE)
        {
            status = mapEvt.getInfo<cl_int>(
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        &eventStatus);
            CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
        }

        memcpy(mapPtr, 
            input, 
            height * width * numChannels * sizeof(cl_float));

        cl::Event unMapEvt;

        status = commandQueue.enqueueUnmapMemObject(inputImageBuffer, 
                                mapPtr, 
                                NULL, 
                                &unMapEvt);
        CHECK_OPENCL_ERROR(status, "commandQueue.enqueueUnmapObject failed");

        status = commandQueue.flush();
        CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

        eventStatus = CL_QUEUED;
        while(eventStatus != CL_COMPLETE)
        {
            status = unMapEvt.getInfo<cl_int>(
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        &eventStatus);
            CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
        }
    }

    // input buffer image 
    status = kernel.setArg(0, inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (inputImageBuffer)");

    // outBuffer imager 
    status = kernel.setArg(1, outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (outputImageBuffer)");

    // Average Luminance 
    status = kernel.setArg(2, averageLuminance);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (averageLuminance)");

    // Gamma 
    status = kernel.setArg(3, gammaPattanaik);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (gammaPattanaik)");

    // C 
    status = kernel.setArg(4, cPattanaik);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (cPattanaik)");

    // Delta 
    status = kernel.setArg(5, deltaPattanaik);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (deltaPattanaik)");

    // Width 
    status = kernel.setArg(6, width);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (width)");

    // numChannels 
    status = kernel.setArg(7, numChannels);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (numChannels)");

    // Height 
    status = kernel.setArg(8, height);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (height)");

    /* 
    * Enqueue a kernel run call.
    */

    cl::NDRange globalThreads(width, height);
    cl::NDRange localThreads(blockSizeX, blockSizeY);

    cl::Event ndrEvt;
    status = commandQueue.enqueueNDRangeKernel(
                kernel,
                cl::NullRange,
                globalThreads,
                localThreads,
                NULL, 
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

    if (deviceType.compare("gpu") == 0)
    {
        // Enqueue readBuffer
        cl::Event readEvt;
        status = commandQueue.enqueueReadBuffer(
                    outputImageBuffer,
                    CL_FALSE,
                    0,
                    width * height * numChannels * sizeof(cl_float),
                    output,
                    NULL,
                    &readEvt);
        CHECK_OPENCL_ERROR(status, "CommandQueue::enqueueReadBuffer failed. (outputImageBuffer)");

        status = commandQueue.flush();
        CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

        eventStatus = CL_QUEUED;
        while(eventStatus != CL_COMPLETE)
        {
            status = readEvt.getInfo<cl_int>(
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        &eventStatus);
            CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
        }
    }
    return SDK_SUCCESS;
}

void 
HDRToneMapping::printStats()
{
    std::string strArray[4] = {"Width", "Height", "Time(sec)", "[Transfer+Kernel]Time(sec)"};
    std::string stats[4];

    totalTime = setupTime + kernelTime;
    
    stats[0] = sampleCommon->toString(width , std::dec);
    stats[1] = sampleCommon->toString(height , std::dec);
    stats[2] = sampleCommon->toString(totalTime, std::dec);
    stats[3] = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}

int 
HDRToneMapping::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory Allocation error.\n");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);

    delete iteration_option;

    streamsdk::Option* c_option = new streamsdk::Option;
    CHECK_ALLOCATION(c_option, "Memory Allocation error.\n");

    c_option->_sVersion = "c";
    c_option->_lVersion = "cpattanaik";
    c_option->_description = "c value(A parameter in pattanaik operator)";
    c_option->_type = streamsdk::CA_ARG_FLOAT;
    c_option->_value = &cPattanaik;

    sampleArgs->AddOption(c_option);

    delete c_option;

    streamsdk::Option* delta_option = new streamsdk::Option;
    CHECK_ALLOCATION(delta_option, "Memory Allocation error.\n");

    delta_option->_sVersion = "l";
    delta_option->_lVersion = "delta";
    delta_option->_description = "delta value(A parameter in pattanaik operator)";
    delta_option->_type = streamsdk::CA_ARG_FLOAT;
    delta_option->_value = &deltaPattanaik;

    sampleArgs->AddOption(delta_option);

    delete delta_option;

    streamsdk::Option* gamma_option = new streamsdk::Option;
    CHECK_ALLOCATION(gamma_option, "Memory Allocation error.\n");

    gamma_option->_sVersion = "g";
    gamma_option->_lVersion = "gamma";
    gamma_option->_description = "gamma value(A parameter in pattanaik operator)";
    gamma_option->_type = streamsdk::CA_ARG_FLOAT;
    gamma_option->_value = &gammaPattanaik;

    sampleArgs->AddOption(gamma_option);

    delete gamma_option;

    return SDK_SUCCESS;
}

int 
HDRToneMapping::setup()
{
    // Allocate host memory and read input image 
    int status = readInputImage();
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    // create and initialize timers 
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    status = setupCL();
    if(status != SDK_SUCCESS)
        return status;

    sampleCommon->stopTimer(timer);
    // Compute setup time 
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}

int 
HDRToneMapping::readInputImage()
{
    //Current implementations simply reads from the .txt. It should be changed later.
    std::ifstream inputFile;
    std::cout << "Input file name " << inputImageName << std::endl;
    inputFile.open(inputImageName.c_str(), std::ifstream::binary);

    if (inputFile.is_open())
    {
        inputFile >> width;
        inputFile >> height;
        if(deviceType.compare("gpu") == 0)
        { 
            input = new cl_float[height * width * sizeof(cl_float) * numChannels];
            CHECK_ALLOCATION(input, "Allocation failed(input)!!");

            output = new cl_float[height * width * sizeof(cl_float) * numChannels];
            CHECK_ALLOCATION(input, "Allocation failed(output)!!");
        }
        else if (deviceType.compare("cpu") == 0)
        {
#ifdef _WIN32
            input = (cl_float *)_aligned_malloc(height * width * sizeof(cl_float) * numChannels, 
                                    128);
            CHECK_ALLOCATION(input, "Allocation failed(input)!!");
    
            output = (cl_float *)_aligned_malloc(height * width * sizeof(cl_float) * numChannels, 
                                    128);
            CHECK_ALLOCATION(input, "Allocation failed(output)!!");
#else 
            input = (cl_float *)memalign(128, 
                                    height * width * sizeof(cl_float) * numChannels);
            CHECK_ALLOCATION(input, "Allocation failed(input)!!");
    
            output = (cl_float *)memalign(128, 
                                    height * width * sizeof(cl_float) * numChannels);
            CHECK_ALLOCATION(input, "Allocation failed(output)!!");
#endif
        }

        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                inputFile >> input[(y * width * numChannels) + (x * numChannels + 0)];
                inputFile >> input[(y * width * numChannels) + (x * numChannels + 1)];
                inputFile >> input[(y * width * numChannels) + (x * numChannels + 2)];
                inputFile >> input[(y * width * numChannels) + (x * numChannels + 3)];
            }
        }
    }
    else 
    {
        std::cout << "not able to open the file  " << inputImageName << std::endl;
        return SDK_FAILURE;
    }
    std::cout << "Width of the image " << width << std::endl;
    std::cout << "Height of the image " << height << std::endl;
    inputFile.close();
    //Calculating average luminance value 

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            float r = input[(y * width * numChannels) + (x * numChannels + 0)];
            float g = input[(y * width * numChannels) + (x * numChannels + 1)];
            float b = input[(y * width * numChannels) + (x * numChannels + 2)];

            float luminance = (0.2126f * r ) + ( 0.7152f * g ) + ( 0.0722f * b );
            averageLuminance += luminance;
        }
    }

    averageLuminance = averageLuminance / (width * height);
    std::cout << "Average luminance value in the image " << averageLuminance << std::endl;
    return SDK_SUCCESS;
}

int 
HDRToneMapping::run()
{
    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Set kernel arguments and run kernel 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << iterations << 
        " iterations" <<std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    // create and initialize timers 
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    for(int i = 0; i < iterations; i++)
    {
        // Set kernel arguments and run kernel 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }
    sampleCommon->stopTimer(timer);
    // Compute kernel time 
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    // write the output image to bitmap file 
    cl_float tempChannel = 0.0f;
    cl_uint* outputData = NULL;
    outputData = (cl_uint*)malloc(width * height * sizeof(cl_uint));
    CHECK_ALLOCATION(outputData, "Failed to allocate memory for output BMP image!\n");

    for(cl_uint y = 0; y < height; y++)
    {
        for(cl_uint x = 0; x < width; x++)
        {
            cl_uint r = 255, g = 255, b = 255, alpha = 255;
            tempChannel = 255.0f * (output[(y * width + x) * 4 + 0]);
            if (tempChannel <= 255.0f)
            {
                r = (cl_uint)(tempChannel);
            }

            tempChannel = 255.0f * (output[(y * width + x) * 4 + 1]);
            if (tempChannel <= 255.0f)
            {
                g = (cl_uint)(tempChannel);
            }

            tempChannel = 255.0f * (output[(y * width + x) * 4 + 2]);
            if (tempChannel <= 255.0f)
            {
                b = (cl_uint)(tempChannel);
            }

            tempChannel = 255.0f * (output[(y * width + x) * 4 + 3]);
            if (tempChannel <= 255.0f)
            {
                alpha = (cl_uint)(tempChannel);
            }

            outputData[(height - 1 - y) * width + x] = 0x000000FF & b;
            outputData[(height - 1 - y) * width + x] |= 0x0000FF00 & (g << 8);
            outputData[(height - 1 - y) * width + x] |= 0x00FF0000 & (r << 16);
            outputData[(height - 1 - y) * width + x] |= 0xFF000000 & (alpha << 24);
        }
    }

    streamsdk::SDKBitMap bitMapOutput;

    bitMapOutput.write(outputImageName.c_str(), 
                    width, 
                    height, 
                    outputData);

    return SDK_SUCCESS;
}

int 
HDRToneMapping::cleanup()
{
    if (input)
    {
        if(deviceType.compare("gpu") == 0)
        { 
            delete []input;
            input = NULL;
        }
        else if (deviceType.compare("cpu") == 0)
        {
#ifdef _WIN32
            ALIGNED_FREE(input);
#else 
            FREE(input);
#endif 
        }
    }

    if (referenceOutput)
    {
        delete []referenceOutput;
        referenceOutput = NULL;
    }

    if (output)
    {
        if(deviceType.compare("gpu") == 0)
        { 
            delete []output;
            output = NULL;
        }
        else if (deviceType.compare("cpu") == 0)
        {
#ifdef _WIN32
            ALIGNED_FREE(output);
#else 
            FREE(output);
#endif 
        }
    }
    return SDK_SUCCESS;
}

int 
HDRToneMapping::cpuPattanaikReference()
{
    referenceOutput = new cl_float[height * width * numChannels];
    CHECK_ALLOCATION(referenceOutput, "Allocation failed(referenceOutput)!!");

    cl_float gcPattanaik = cPattanaik * averageLuminance; 

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            cl_float yLPattanaik = 0.0f;
            cl_float cLPattanaik = 0.0f;

            cl_float r, g, b, r1, g1, b1;
            //Finding R, G, B values of a pixel
            r1 = input[y * width * numChannels + (x * numChannels + 0)];
            g1 = input[y * width * numChannels + (x * numChannels + 1)];
            b1 = input[y * width * numChannels + (x * numChannels + 2)];
            //Calculating the luminance value 
            cl_float yLuminance = (0.2126f * r1) + (0.7152f * g1) + (0.0722f * b1);

            if (x != 0 && y != 0 && x != width - 1 && y != height - 1)
            {
                //Calculating mean
                float leftUp = 0.0f;
                float up = 0.0f;
                float rightUp = 0.0f;
                float left = 0.0f;
                float right = 0.0f;
                float leftDown = 0.0f;
                float down = 0.0f;
                float rightDown = 0.0f;

                r = input[width * numChannels * (y - 1) + (((x - 1) * numChannels) + 0 )];
                g = input[width * numChannels * (y - 1) + ((x - 1) * numChannels) + 1 ];
                b = input[width * numChannels * (y - 1) + ((x - 1) * numChannels) + 2 ];

                leftUp = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                r = input[width * numChannels * (y - 1) + ((x) * numChannels) + 0 ];
                g = input[width * numChannels * (y - 1) + ((x) * numChannels) + 1 ];
                b = input[width * numChannels * (y - 1) + ((x) * numChannels) + 2 ];

                up = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                r = input[width * numChannels * (y - 1) + ((x + 1) * numChannels) + 0 ];
                g = input[width * numChannels * (y - 1) + ((x + 1) * numChannels) + 1 ];
                b = input[width * numChannels * (y - 1) + ((x + 1) * numChannels) + 2 ];

                rightUp = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                r = input[width * numChannels * (y) + ((x - 1) * numChannels) + 0 ];
                g = input[width * numChannels * (y) + ((x - 1) * numChannels) + 1 ];
                b = input[width * numChannels * (y) + ((x - 1) * numChannels) + 2 ];

                left = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                r = input[width * numChannels * (y) + ((x + 1) * numChannels) + 0 ];
                g = input[width * numChannels * (y) + ((x + 1) * numChannels) + 1 ];
                b = input[width * numChannels * (y) + ((x + 1) * numChannels) + 2 ];

                right = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                r = input[width * numChannels * (y + 1) + ((x - 1) * numChannels) + 0 ];
                g = input[width * numChannels * (y + 1) + ((x - 1) * numChannels) + 1 ];
                b = input[width * numChannels * (y + 1) + ((x - 1) * numChannels) + 2 ];

                leftDown = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                r = input[width * numChannels * (y + 1) + ((x) * numChannels) + 0 ];
                g = input[width * numChannels * (y + 1) + ((x) * numChannels) + 1 ];
                b = input[width * numChannels * (y + 1) + ((x) * numChannels) + 2 ];

                down = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                r = input[width * numChannels * (y + 1) + ((x + 1) * numChannels) + 0 ];
                g = input[width * numChannels * (y + 1) + ((x + 1) * numChannels) + 1 ];
                b = input[width * numChannels * (y + 1) + ((x + 1) * numChannels) + 2 ];

                rightDown = (0.2126f * r) + (0.7152f * g) + (0.0722f * b);

                //Calculate median

                yLPattanaik = (leftUp + up + rightUp + left + right + leftDown + down + rightDown) / 8;
            }
            else 
            {
                yLPattanaik = yLuminance;
            }

            cLPattanaik =  yLPattanaik * log(deltaPattanaik + yLPattanaik / yLuminance) + gcPattanaik;

            float yDPattanaik = yLuminance / (yLuminance + cLPattanaik);

            r = pow((r1 / yLuminance), gammaPattanaik) * yDPattanaik;
            g = pow((g1 / yLuminance), gammaPattanaik) * yDPattanaik;	
            b = pow((b1 / yLuminance), gammaPattanaik) * yDPattanaik;

            referenceOutput[width * numChannels * y + (x * numChannels + 0)] = r;
            referenceOutput[width * numChannels * y + (x * numChannels + 1)] = g;
            referenceOutput[width * numChannels * y + (x * numChannels + 2)] = b;
            referenceOutput[width * numChannels * y + (x * numChannels + 3)] = input[width * numChannels * y + (x * numChannels + 3)];
        }
    }
    return SDK_SUCCESS;
}

int 
HDRToneMapping::verifyResults()
{
    if (verify)
    {
        int timer = sampleCommon->createTimer();
        sampleCommon->startTimer(timer);
        if (cpuPattanaikReference() != SDK_SUCCESS)
        {
            return SDK_FAILURE;
        }
        sampleCommon->stopTimer(timer);

        double referenceTime = (double)sampleCommon->readTimer(timer);

        std::cout<<"Reference time is "<<referenceTime<<" secs"<<std::endl;

        cl_float error = 0.0f;

        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                error += referenceOutput[width * numChannels * y + (x * numChannels + 0)] - output[width * numChannels * y + (x * numChannels + 0)];
                error += referenceOutput[width * numChannels * y + (x * numChannels + 1)] - output[width * numChannels * y + (x * numChannels + 1)];
                error += referenceOutput[width * numChannels * y + (x * numChannels + 2)] - output[width * numChannels * y + (x * numChannels + 2)];
                error += referenceOutput[width * numChannels * y + (x * numChannels + 3)] - output[width * numChannels * y + (x * numChannels + 3)];
            }
        }

        cl_float tempChannel = 0.0f;
        cl_uint* outputData = 0;
        outputData = (cl_uint*)malloc(width * height * sizeof(cl_uint));
        CHECK_ALLOCATION(outputData, "Failed to allocate memory for output BMP image!\n");

        for(cl_uint y = 0; y < height; y++)
        {
            for(cl_uint x = 0; x < width; x++)
            {
                cl_uint r = 255, g = 255, b = 255, alpha = 255;
                tempChannel = 255.0f * (referenceOutput[(y * width + x) * 4 + 0]);
                if (tempChannel <= 255.0f)
                {
                    r = (cl_uint)(tempChannel);
                }

                tempChannel = 255.0f * (referenceOutput[(y * width + x) * 4 + 1]);
                if (tempChannel <= 255.0f)
                {
                    g = (cl_uint)(tempChannel);
                }

                tempChannel = 255.0f * (referenceOutput[(y * width + x) * 4 + 2]);
                if (tempChannel <= 255.0f)
                {
                    b = (cl_uint)(tempChannel);
                }

                tempChannel = 255.0f * (referenceOutput[(y * width + x) * 4 + 3]);
                if (tempChannel <= 255.0f)
                {
                    alpha = (cl_uint)(tempChannel);
                }

                outputData[(height - 1 - y) * width + x] = 0x000000FF & b;
                outputData[(height - 1 - y) * width + x] |= 0x0000FF00 & (g << 8);
                outputData[(height - 1 - y) * width + x] |= 0x00FF0000 & (r << 16);
                outputData[(height - 1 - y) * width + x] |= 0xFF000000 & (alpha << 24);
            }
        }

        streamsdk::SDKBitMap bitMapOutput;

        bitMapOutput.write("reference_output.bmp", 
                        width, 
                        height, 
                        outputData);

        error = error / (height * width);

        if(error > 0.000001f)
        {
            std::cout<<"Failed!!"<<std::endl;
            return SDK_FAILURE;
        }
        else 
        {
            std::cout<<"Passed!!"<<std::endl;
        }
    }
    return SDK_SUCCESS;
}

int main(int argc, char *argv[])
{
    HDRToneMapping *hdrObj = new HDRToneMapping("HDR ToneMapping Application");
    CHECK_ALLOCATION(hdrObj, "Object instantiation failed(hdrObj)");

    if(hdrObj->initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(hdrObj->parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(hdrObj->isDumpBinaryEnabled())
    {
        return hdrObj->genBinaryImage();
    }
    else
    {
        // Setup
        if (hdrObj->setup() != SDK_SUCCESS)
            return SDK_FAILURE;

        // Run
        if(hdrObj->run() != SDK_SUCCESS)
            return SDK_FAILURE;

        // VerifyResults
        if(hdrObj->verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;

        // Cleanup
        if(hdrObj->cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        hdrObj->printStats();
    }

    return SDK_SUCCESS;
}