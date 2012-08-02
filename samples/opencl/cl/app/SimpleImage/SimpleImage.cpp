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


#include "SimpleImage.hpp"
#include <cmath>


int
SimpleImage::readInputImage(std::string inputImageName)
{

    // load input bitmap image 
    inputBitmap.load(inputImageName.c_str());

    // error if image did not load 
    if(!inputBitmap.isLoaded())
    {
        sampleCommon->error("Failed to load input image!");
        return SDK_FAILURE;
    }

    // get width and height of input image
    height = inputBitmap.getHeight();
    width = inputBitmap.getWidth();

    // allocate memory for input & outputimage data 
    inputImageData = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(inputImageData,"Failed to allocate memory! (inputImageData)");

    // allocate memory for 2D-copy output image data 
    outputImageData2D = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(outputImageData2D,"Failed to allocate memory! (outputImageData)");

    // allocate memory for 3D-copy output image data
    outputImageData3D = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(outputImageData3D,"Failed to allocate memory! (outputImageData)");

    // initializa the Image data to NULL 
    memset(outputImageData2D, 0, width * height * pixelSize);
    memset(outputImageData3D, 0, width * height * pixelSize);

    // get the pointer to pixel data 
    pixelData = inputBitmap.getPixels();
    CHECK_ALLOCATION(pixelData,"Failed to read pixel Data!");

    // Copy pixel data into inputImageData
    memcpy(inputImageData, pixelData, width * height * pixelSize);

    // allocate memory for verification output
    verificationOutput = (cl_uchar*)malloc(width * height * pixelSize);
    CHECK_ALLOCATION(pixelData,"verificationOutput heap allocation failed!");

    // initialize the data to NULL 
    //memset(verificationOutput, 0, width * height * pixelSize);
    memcpy(verificationOutput, inputImageData, width * height * pixelSize);

    return SDK_SUCCESS;

}


int
SimpleImage::writeOutputImage(std::string outputImageName)
{
    // copy output image data back to original pixel data
    memcpy(pixelData, outputImageData2D, width * height * pixelSize);

    // write the output bmp file
    if(!inputBitmap.write(outputImageName.c_str()))
    {
        std::cout << "Failed to write output image!";
        return SDK_FAILURE;
    }
    return SDK_SUCCESS;
}

int 
SimpleImage::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("SimpleImage_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
    }


int 
SimpleImage::setupCL()
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
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed.");

    // getting device on which to run the sample
    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");

    status = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_OPENCL_ERROR(status, "deviceInfo.setDeviceInfo failed");

    if(!deviceInfo.imageSupport)
    {
        OPENCL_EXPECTED_ERROR(" Expected Error: Device does not support Images");
    }

    // Create command queue
    cl_command_queue_properties prop = 0;
    commandQueue = clCreateCommandQueue(
        context,
        devices[deviceId],
        prop,
        &status);
    CHECK_OPENCL_ERROR(status,"clCreateCommandQueue failed.");

    // Create and initialize image objects
    cl_image_desc imageDesc;
    memset(&imageDesc, '\0', sizeof(cl_image_desc));
    imageDesc.image_type = CL_MEM_OBJECT_IMAGE2D;
    imageDesc.image_width = width;
    imageDesc.image_height = height;

    // Create 2D input image
    inputImage2D = clCreateImage(context,
                                 CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                 &imageFormat,
                                 &imageDesc,
                                 inputImageData,
                                 &status);
    CHECK_OPENCL_ERROR(status,"clCreateImage failed. (inputImage2D)");

    // Create 2D output image
    outputImage2D = clCreateImage(context,
                                  CL_MEM_WRITE_ONLY,
                                  &imageFormat,
                                  &imageDesc,
                                  0,
                                  &status);
    CHECK_OPENCL_ERROR(status,"clCreateImage failed. (outputImage2D)");

    // Writes to 3D images not allowed in spec currently
    outputImage3D = clCreateImage(context,
                                  CL_MEM_WRITE_ONLY,
                                  &imageFormat,
                                  &imageDesc,
                                  0,
                                  &status);
    CHECK_OPENCL_ERROR(status,"clCreateImage failed. (outputImage3D)");

    // Create 3D input image
    memset(&imageDesc, '\0', sizeof(cl_image_desc));
    imageDesc.image_type = CL_MEM_OBJECT_IMAGE3D;
    imageDesc.image_width = width;
    imageDesc.image_height = height / 2;
    imageDesc.image_depth = 2;

    inputImage3D = clCreateImage(context,
                                 CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                 &imageFormat,
                                 &imageDesc,
                                 inputImageData,
                                 &status);
    CHECK_OPENCL_ERROR(status,"clCreateImage failed. (inputImage3D)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("SimpleImage_Kernels.cl");
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
    kernel2D = clCreateKernel(program, "image2dCopy", &status);
    CHECK_OPENCL_ERROR(status,"clCreateKernel failed.(kernel2D)");
    
    kernel3D = clCreateKernel(program, "image3dCopy", &status);
    CHECK_OPENCL_ERROR(status,"clCreateKernel failed.(kernel3D)");
    
    // Check group size against group size returned by kernel
    status = clGetKernelWorkGroupInfo(kernel2D,
        devices[deviceId],
        CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t),
        &kernel2DWorkGroupSize,
        0);
    CHECK_OPENCL_ERROR(status,"clGetKernelWorkGroupInfo  failed.");
    
    // Check group size against group size returned by kernel
    status = clGetKernelWorkGroupInfo(kernel3D,
        devices[deviceId],
        CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t),
        &kernel3DWorkGroupSize,
        0);
    CHECK_OPENCL_ERROR(status,"clGetKernelWorkGroupInfo  failed.");

    cl_uint temp = (cl_uint)min(kernel2DWorkGroupSize, kernel3DWorkGroupSize);

    if((blockSizeX * blockSizeY) > temp)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : "
                      << blockSizeX * blockSizeY << std::endl;
            std::cout << "Max Group Size supported on the kernel(s) : " 
                      << temp << std::endl;
            std::cout << "Falling back to " << temp << std::endl;
        }

        if(blockSizeX > temp)
        {
            blockSizeX = temp;
            blockSizeY = 1;
        }
    }
    return SDK_SUCCESS;
}

int 
SimpleImage::runCLKernels()
{
    cl_int status;

    // Set appropriate arguments to the kernel2D

    // input buffer image
    status = clSetKernelArg(
        kernel2D,
        0,
        sizeof(cl_mem),
        &inputImage2D);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed. (inputImage2D)");

    // outBuffer image
    status = clSetKernelArg(
        kernel2D,
        1,
        sizeof(cl_mem),
        &outputImage2D);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed. (outputImage2D)");

    // Set appropriate arguments to the kernel3D 

    // input buffer image
    status = clSetKernelArg(kernel3D,
        0,
        sizeof(cl_mem),
        &inputImage3D);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed. (inputImage3D)");

    // outBuffer image
    status = clSetKernelArg(
        kernel3D,
        1,
        sizeof(cl_mem),
        &outputImage3D);
    CHECK_OPENCL_ERROR(status,"clSetKernelArg failed. (outputImage3D)");

    // Enqueue a kernel run call.
    size_t globalThreads[] = {width, height};
    size_t localThreads[] = {blockSizeX, blockSizeY};

    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel2D,
        2,
        NULL,
        globalThreads,
        localThreads,
        0,
        NULL,
        0);
    CHECK_OPENCL_ERROR(status,"clEnqueueNDRangeKernel failed.");

    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel3D,
        2,
        NULL,
        globalThreads,
        localThreads,
        0,
        NULL,
        0);
    CHECK_OPENCL_ERROR(status,"clEnqueueNDRangeKernel failed.");

    status = clFinish(commandQueue);
    CHECK_OPENCL_ERROR(status,"clFinish failed.");

    // Enqueue Read Image
    size_t origin[] = {0, 0, 0};
    size_t region[] = {width, height, 1};

    // Read output of 2D copy
    status = clEnqueueReadImage(commandQueue,
                                outputImage2D,
                                1,
                                origin,
                                region,
                                0,
                                0,
                                outputImageData2D,
                                0, 0, 0);
    CHECK_OPENCL_ERROR(status,"clEnqueueReadImage failed.");

    // Read output of 3D copy
    status = clEnqueueReadImage(commandQueue,
                                outputImage3D,
                                1,
                                origin,
                                region,
                                0,
                                0,
                                outputImageData3D,
                                0, 0, 0);
    CHECK_OPENCL_ERROR(status,"clEnqueueReadImage failed.");

    // Wait for the read buffer to finish execution
    status = clFinish(commandQueue);
    CHECK_OPENCL_ERROR(status,"clFinish failed.(commandQueue)");

    return SDK_SUCCESS;
}

int 
SimpleImage::initialize()
{

    // Call base class Initialize to get default configuration
    if (this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option,"Memory Allocation error. (iteration_option)");

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
SimpleImage::setup()
{
    int status = 0;
    // Allocate host memoryF and read input image
    std::string filePath = sampleCommon->getPath() + std::string(INPUT_IMAGE);
    status = readInputImage(filePath);
    CHECK_ERROR(status, SDK_SUCCESS, "Read Input Image failed");

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
SimpleImage::run()
{
    int status;
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // create and initialize timers
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    std::cout << "Executing kernel for " << iterations << 
        " iterations" <<std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    for(int i = 0; i < iterations; i++)
    {
        // Set kernel arguments and run kernel
        status = runCLKernels();
        CHECK_ERROR(status, SDK_SUCCESS, "OpenCL run Kernel failed");
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
SimpleImage::cleanup()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(kernel2D);
    CHECK_OPENCL_ERROR(status,"clReleaseKernel failed.(kernel2D)");

    status = clReleaseKernel(kernel3D);
    CHECK_OPENCL_ERROR(status,"clReleaseKernel failed.(kernel3D)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status,"clReleaseProgram failed.(program)");

    status = clReleaseMemObject(inputImage2D);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(inputImage2D)");

    status = clReleaseMemObject(outputImage2D);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(outputImage2D)");

    status = clReleaseMemObject(inputImage3D);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(inputImage3D)");

    status = clReleaseMemObject(outputImage3D);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(outputImage3D)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status,"clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status,"clReleaseContext failed.(context)");

    // release program resources (input memory etc.)

        FREE(inputImageData);
        FREE(outputImageData2D);
        FREE(outputImageData3D);
        FREE(verificationOutput);
        FREE(devices);

        return SDK_SUCCESS;
}


void 
SimpleImage::simpleImageCPUReference()
{

}


int 
SimpleImage::verifyResults()
{
    if(verify)
    {
        std::cout << "Verifying 2D copy result - ";
        // compare the results and see if they match
        if(!memcmp(inputImageData, outputImageData2D, width * height * 4))
        {
            std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed\n" << std::endl;
            return SDK_FAILURE;
        }

        std::cout << "Verifying 3D copy result - ";

        // compare the results and see if they match
        if(!memcmp(inputImageData, outputImageData3D, width * height * 4))
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

void 
SimpleImage::printStats()
{
    std::string strArray[4] = 
    {
        "Width", 
        "Height", 
        "Time(sec)", 
        "kernelTime(sec)"
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
    int status = 0;
    SimpleImage clSimpleImage("OpenCL SimpleImage");

    if (clSimpleImage.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clSimpleImage.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clSimpleImage.isDumpBinaryEnabled())
    {
        return clSimpleImage.genBinaryImage();
    }

    status = clSimpleImage.setup();
    if(status != SDK_SUCCESS)
        return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

    if (clSimpleImage.run() !=SDK_SUCCESS)
        return SDK_FAILURE;

    if (clSimpleImage.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clSimpleImage.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    clSimpleImage.printStats();
    return SDK_SUCCESS;
}
