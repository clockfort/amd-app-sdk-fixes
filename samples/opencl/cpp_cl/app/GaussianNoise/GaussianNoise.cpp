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


#include "GaussianNoise.hpp"
#include <cmath>


int
GaussianNoise::readInputImage(std::string inputImageName)
{

    // load input bitmap image 
    std::string filePath = inputImageName;
    inputBitmap.load(filePath.c_str());

    // error if image did not load 
    if(!inputBitmap.isLoaded())
    {
        sampleCommon->error("Failed to load input image!");
        return SDK_FAILURE;
    }

    // get width and height of input image 
    height = inputBitmap.getHeight();
    width = inputBitmap.getWidth();

    // allocate memory for input & output image data  
    inputImageData  = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(inputImageData, "Failed to allocate memory! (inputImageData)");


    // allocate memory for output image data 
    outputImageData = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(outputImageData, "Failed to allocate memory! (outputImageData)");

    // initializa the Image data to NULL 
    memset(outputImageData, 0, width * height * pixelSize);

    // get the pointer to pixel data 
    pixelData = inputBitmap.getPixels();

    // error check 
    if(pixelData == NULL)
    {
        sampleCommon->error("Failed to read pixel Data!");
        return SDK_FAILURE;
    }

    // Copy pixel data into inputImageData 
    memcpy(inputImageData, pixelData, width * height * pixelSize);

    return SDK_SUCCESS;
}


int
GaussianNoise::writeOutputImage(std::string outputImageName)
{
    // copy output image data back to original pixel data 
    memcpy(pixelData, outputImageData, width * height * pixelSize);

    // write the output bmp file 
    if(!inputBitmap.write(outputImageName.c_str()))
    {
        sampleCommon->error("Failed to write output image!");
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int 
GaussianNoise::genBinaryImage()
{
    /*
     * Have a look at the available platforms and pick either
     * the AMD one if available or a reasonable default.
     */

    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("GaussianNoise_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
    binaryData.flagsFileName = std::string(flags.c_str());
    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int 
GaussianNoise::setupCL()
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
        std::cerr << "No device available\n";
        return SDK_FAILURE;
    }

    if(sampleCommon->validateDeviceId(deviceId, deviceCount))
    {
        sampleCommon->error("sampleCommon::validateDeviceId() failed");
        return SDK_FAILURE;
    }

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
    inputImageBuffer = cl::Buffer(context, 
                                  inMemFlags, 
                                  width * height * pixelSize,
                                  0,
                                  &err);
    CHECK_OPENCL_ERROR(err, "Buffer::Buffer() failed. (inputImageBuffer)");

    // Create memory object for output Image 
    outputImageBuffer = cl::Buffer(context, 
                                   CL_MEM_WRITE_ONLY, 
                                   width * height * pixelSize,
                                   NULL,
                                   &err);
    CHECK_OPENCL_ERROR(err, "Buffer::Buffer() failed. (outputImageBuffer)");    

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

        program = cl::Program(context, device, programBinary, NULL, &err);
        CHECK_OPENCL_ERROR(err, "Program::Program(Binary) failed.");
    }
    else
    {
        kernelPath.append("GaussianNoise_Kernels.cl");
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
    kernel = cl::Kernel(program, "gaussian_transform",  &err);
    CHECK_OPENCL_ERROR(err, "Kernel::Kernel() failed.");


    // Check group size against group size returned by kernel 
    kernelWorkGroupSize = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[deviceId], &err);
    CHECK_OPENCL_ERROR(err, "Kernel::getWorkGroupInfo()  failed.");

    if((blockSizeX * blockSizeY) > kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : "
                      << blockSizeX * blockSizeY << std::endl;
            std::cout << "Max Group Size supported on the kernel : "
                      << kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
        }

        if(blockSizeX > kernelWorkGroupSize)
        {
            blockSizeX = kernelWorkGroupSize;
            blockSizeY = 1;
        }
    }

    return SDK_SUCCESS;
}

int 
GaussianNoise::runCLKernels()
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;

    cl::Event writeEvt;
    status = commandQueue.enqueueWriteBuffer(
                inputImageBuffer,
                CL_FALSE,
                0,
                width * height * pixelSize,
                inputImageData,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "CommandQueue::enqueueWriteBuffer() failed. (inputImageBuffer)");
    
    status = commandQueue.flush();
    CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");

    eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = writeEvt.getInfo<cl_int>(
                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                    &eventStatus);
        CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
    }

    // Set appropriate arguments to the kernel 

    // input buffer image 
    status = kernel.setArg(0, inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (inputImageBuffer)");

    // outBuffer imager 
    status = kernel.setArg(1, outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (outputImageBuffer)");

    // factor 
    status = kernel.setArg(2, factor);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (factor)");

    /* 
    * Enqueue a kernel run call.
    */

    // Each thread calculates 2 gaussian numbers 
    cl::NDRange globalThreads(width / 2, height);
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

    // Enqueue readBuffer
    cl::Event readEvt;
    status = commandQueue.enqueueReadBuffer(
                outputImageBuffer,
                CL_FALSE,
                0,
                width * height * pixelSize,
                outputImageData,
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

    return SDK_SUCCESS;
}



int 
GaussianNoise::initialize()
{
    // Call base class Initialize to get default configuration
    CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL resource  initilization failed");
    streamsdk::Option* iteration_option = new streamsdk::Option;
    if(!iteration_option)
    {
        sampleCommon->error("Memory Allocation error.\n");
        return SDK_FAILURE;
    }
    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);

    delete iteration_option;

    streamsdk::Option* factor_option = new streamsdk::Option;
    if(!factor_option)
    {
        sampleCommon->error("Memory Allocation error.\n");
        return SDK_FAILURE;
    }
    factor_option->_sVersion = "f";
    factor_option->_lVersion = "factor";
    factor_option->_description = "Noise factor";
    factor_option->_type = streamsdk::CA_ARG_INT;
    factor_option->_value = &factor;

    sampleArgs->AddOption(factor_option);
    delete factor_option;

    return SDK_SUCCESS;
}

int 
GaussianNoise::setup()
{
    // Allocate host memory and read input image 
    if(readInputImage(INPUT_IMAGE) != SDK_SUCCESS)
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

int 
GaussianNoise::run()
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
    if(writeOutputImage(OUTPUT_IMAGE) != SDK_SUCCESS)
    {
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int 
GaussianNoise::cleanup()
{
    // release program resources (input memory etc.) 

    FREE(inputImageData);
    FREE(outputImageData);

    return SDK_SUCCESS;
}



void 
GaussianNoise::gaussianNoiseCPUReference()
{

}


int 
GaussianNoise::verifyResults()
{
    if(verify)
    {
        float mean = 0;
        for(int i = 0; i < (int)(width * height); i++)
        {
            mean += outputImageData[i].s[0] - inputImageData[i].s[0];
        }
        mean /= (width * height * factor);

        if(fabs(mean) < 0.1)
        {
            std::cout << "Passed! \n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout << "Failed! \n" << std::endl;
            return SDK_FAILURE;
        }
    }
    return SDK_SUCCESS;
}

void 
GaussianNoise::printStats()
{
    std::string strArray[4] = 
    {
        "Width", 
        "Height", 
        "Time(sec)", 
        "[Transfer+Kernel]Time(sec)"
    };
    std::string stats[4];

    totalTime = setupTime + kernelTime;

    stats[0] = sampleCommon->toString(width, std::dec);
    stats[1] = sampleCommon->toString(height, std::dec);
    stats[2] = sampleCommon->toString(totalTime, std::dec);
    stats[3] = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}


int 
main(int argc, char *argv[])
{
    GaussianNoise clGaussianNoise("OpenCL GaussianNoise");

    if(clGaussianNoise.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clGaussianNoise.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clGaussianNoise.isDumpBinaryEnabled())
    {
        return clGaussianNoise.genBinaryImage();
    }
    else
    {
        if(clGaussianNoise.setup() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clGaussianNoise.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clGaussianNoise.verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clGaussianNoise.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        clGaussianNoise.printStats();
    }

    return SDK_SUCCESS;
}