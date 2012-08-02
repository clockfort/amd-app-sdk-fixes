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


#include "SobelFilter.hpp"
#include <cmath>


int
SobelFilter::readInputImage(std::string inputImageName)
{

    // load input bitmap image 
    inputBitmap.load(inputImageName.c_str());

    // error if image did not load
    if(!inputBitmap.isLoaded())
    {
        std::cout << "Failed to load input image!";
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
    if(pixelData == NULL)
    {
        std::cout << "Failed to read pixel Data!";
        return SDK_FAILURE;
    }

    // Copy pixel data into inputImageData 
    memcpy(inputImageData, pixelData, width * height * pixelSize);

    // allocate memory for verification output
    verificationOutput = (cl_uchar*)malloc(width * height * pixelSize);
    CHECK_ALLOCATION(verificationOutput, "verificationOutput heap allocation failed!");

    // initialize the data to NULL 
    memset(verificationOutput, 0, width * height * pixelSize);

    return SDK_SUCCESS;

}


int
SobelFilter::writeOutputImage(std::string outputImageName)
{
    // copy output image data back to original pixel data
    memcpy(pixelData, outputImageData, width * height * pixelSize);

    // write the output bmp file
    if(!inputBitmap.write(outputImageName.c_str()))
    {
        std::cout << "Failed to write output image!";
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int 
SobelFilter::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("SobelFilter_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int 
SobelFilter::setupCL()
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
    status = sampleCommon->getDevices(context,&devices,deviceId,isDeviceIdEnabled());
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
    CHECK_ERROR(retValue, 0, "SDKDeviceInfo::setDeviceInfo() failed");


    // Create and initialize memory objects

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create memory object for input Image
    inputImageBuffer = clCreateBuffer(
        context,
        inMemFlags,
        width * height * pixelSize,
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputImageBuffer)");

    // Create memory objects for output Image
    outputImageBuffer = clCreateBuffer(context,
        CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
        width * height * pixelSize,
        outputImageData,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputImageBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("SobelFilter_Kernels.cl");
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
    kernel = clCreateKernel(
        program,
        "sobel_filter",
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    status = kernelInfo.setKernelWorkGroupInfo(kernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS,"kernelInfo.setKernelWorkGroupInfo() failed");


    if((blockSizeX * blockSizeY) > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : "
                      << blockSizeX * blockSizeY << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelInfo.kernelWorkGroupSize << std::endl;
        }

        // Three possible cases
        if(blockSizeX > kernelInfo.kernelWorkGroupSize)
        {
            blockSizeX = kernelInfo.kernelWorkGroupSize;
            blockSizeY = 1;
        }
    }
    return SDK_SUCCESS;
}

int 
SobelFilter::runCLKernels()
{
    cl_int status;

    // Set input data 
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                inputImageBuffer,
                CL_FALSE,
                0,
                width * height * pixelSize,
                inputImageData,
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputImageBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    // Set appropriate arguments to the kernel 

    // input buffer image 
    status = clSetKernelArg(
        kernel,
        0,
        sizeof(cl_mem),
        &inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputImageBuffer)")

    // outBuffer imager
    status = clSetKernelArg(
        kernel,
        1,
        sizeof(cl_mem),
        &outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (outputImageBuffer)");

    // Enqueue a kernel run call.
    size_t globalThreads[] = {width, height};
    size_t localThreads[] = {blockSizeX, blockSizeY};

    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        2,
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
        outputImageBuffer,
        CL_FALSE,
        0,
        width * height * pixelSize,
        outputImageData,
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
SobelFilter::initialize()
{
    cl_int status = 0;
    // Call base class Initialize to get default configuration
    status = this->SDKSample::initialize();
    CHECK_ERROR(status, SDK_SUCCESS, "OpenCL Initialization failed");

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory Allocation error.\n");

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
SobelFilter::setup()
{
    cl_int status = 0;
    // Allocate host memory and read input image 
    std::string filePath = sampleCommon->getPath() + std::string(INPUT_IMAGE);
    status = readInputImage(filePath);
    CHECK_ERROR(status, SDK_SUCCESS, "Read InputImage failed");

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
SobelFilter::run()
{
    cl_int status = 0;
    if(!byteRWSupport)
        return SDK_SUCCESS;

    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Set kernel arguments and run kernel
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << iterations
              << " iterations" <<std::endl;
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
    status = writeOutputImage(OUTPUT_IMAGE);
    CHECK_ERROR(status, SDK_SUCCESS, "write Output Image Failed");

    return SDK_SUCCESS;
}

int 
SobelFilter::cleanup()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseMemObject(inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // release program resources (input memory etc.)
    FREE(inputImageData);

    FREE(outputImageData);

    FREE(verificationOutput);

    FREE(devices);

    return SDK_SUCCESS;
}


void 
SobelFilter::sobelFilterCPUReference()
{
    // x-axis gradient mask 
    const int kx[][3] = 
    { 
        { 1, 2, 1},
        { 0, 0, 0},
        { -1,-2,-1}
    };

    // y-axis gradient mask
    const int ky[][3] = 
    { 
        { 1, 0, -1},
        { 2, 0, -2},
        { 1, 0, -1}
    };

    int gx = 0;
    int gy = 0;

    // pointer to input image data
    cl_uchar *ptr = (cl_uchar*)malloc(width * height * pixelSize);
    memcpy(ptr, inputImageData, width * height * pixelSize);

    // each pixel has 4 uchar components 
    int w = width * 4;

    int k = 1;

    // apply filter on each pixel (except boundary pixels)
    for(int i = 0; i < (int)(w * (height - 1)) ; i++) 
    {
        if(i < (k+1)*w - 4 && i >= 4 + k*w)
        {
            gx =  kx[0][0] * *(ptr + i - 4 - w)
                + kx[0][1] * *(ptr + i - w)
                + kx[0][2] * *(ptr + i + 4 - w)
                + kx[1][0] * *(ptr + i - 4)
                + kx[1][1] * *(ptr + i)
                + kx[1][2] * *(ptr + i + 4)
                + kx[2][0] * *(ptr + i - 4 + w)
                + kx[2][1] * *(ptr + i + w)
                + kx[2][2] * *(ptr + i + 4 + w);


            gy =  ky[0][0] * *(ptr + i - 4 - w)
                + ky[0][1] * *(ptr + i - w)
                + ky[0][2] * *(ptr + i + 4 - w)
                + ky[1][0] * *(ptr + i - 4)
                + ky[1][1] * *(ptr + i)
                + ky[1][2] * *(ptr + i + 4)
                + ky[2][0] * *(ptr + i - 4 + w)
                + ky[2][1] * *(ptr + i + w)
                + ky[2][2] * *(ptr + i + 4 + w);

            float gx2 = pow((float)gx, 2);
            float gy2 = pow((float)gy, 2);


            *(verificationOutput + i) = (cl_uchar)(sqrt(gx2 + gy2) / 2.0);
        }

        // if reached at the end of its row then incr k
        if(i == (k + 1) * w - 5)
        {
            k++;
        }
    } 

    free(ptr);
}


int 
SobelFilter::verifyResults()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    if(verify)
    {
        // reference implementation
        sobelFilterCPUReference();

        float *outputDevice = new float[width * height * pixelSize];
        CHECK_ALLOCATION(outputDevice, "Failed to allocate host memory! (outputDevice)");

        float *outputReference = new float[width * height * pixelSize];
        CHECK_ALLOCATION(outputReference, "Failed to allocate host memory!"
                "(outputReference)");

        // copy uchar data to float array
        for(int i = 0; i < (int)(width * height); i++)
        {
            outputDevice[i * 4 + 0] = outputImageData[i].s[0];
            outputDevice[i * 4 + 1] = outputImageData[i].s[1];
            outputDevice[i * 4 + 2] = outputImageData[i].s[2];
            outputDevice[i * 4 + 3] = outputImageData[i].s[3];

            outputReference[i * 4 + 0] = verificationOutput[i * 4 + 0];
            outputReference[i * 4 + 1] = verificationOutput[i * 4 + 1];
            outputReference[i * 4 + 2] = verificationOutput[i * 4 + 2];
            outputReference[i * 4 + 3] = verificationOutput[i * 4 + 3];
        }


        // compare the results and see if they match
        if(sampleCommon->compare(outputReference, 
                                 outputDevice, 
                                 width * height * 4))
        {
            std::cout << "Passed!\n" << std::endl;
            delete[] outputDevice;
            delete[] outputReference;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout << "Failed\n" << std::endl;
            delete[] outputDevice;
            delete[] outputReference;
            return SDK_FAILURE;
        }
    }

    return SDK_SUCCESS;
}

void 
SobelFilter::printStats()
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
main(int argc, char * argv[])
{
    cl_int status = 0;
    SobelFilter clSobelFilter("OpenCL SobelFilter");

    if(clSobelFilter.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clSobelFilter.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clSobelFilter.isDumpBinaryEnabled())
    {
        return clSobelFilter.genBinaryImage();
    }

    status = clSobelFilter.setup();
    if(status != SDK_SUCCESS)
        return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

    if(clSobelFilter.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clSobelFilter.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;
    
    if(clSobelFilter.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    clSobelFilter.printStats();
    return SDK_SUCCESS;
}