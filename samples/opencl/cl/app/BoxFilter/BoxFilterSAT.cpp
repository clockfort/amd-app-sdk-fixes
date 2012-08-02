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


#include "BoxFilterSAT.hpp"
#include <cmath>


int
BoxFilterSAT::readInputImage(std::string inputImageName)
{

    // load input bitmap image
    std::string filePath = sampleCommon->getPath() + std::string(INPUT_IMAGE);
    inputBitmap.load(filePath.c_str());
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
    verificationOutput = (cl_uchar4*)malloc(width * height * sizeof(cl_uchar4));
    CHECK_ALLOCATION(verificationOutput, "verificationOutput heap allocation failed!");

    // initialize the data to NULL
    memset(verificationOutput, 0, width * height * pixelSize);

    // Calculate passes required for SAT computation
    cl_float tempN = (cl_float)((cl_float)log((float)width) / 
        log((float)rHorizontal));
    cl_float tempM = (cl_float)((cl_float)log((float)height) / 
        log((float)rVertical));

    // Calculate horizontal passes
    while(fabs(tempN - (cl_int)tempN) > 1e-7)
    {
        if(rHorizontal < 2)
        {
            std::cout << 
                "Width should be a multiple of power of 2 atleast!" << 
                std::endl;
            return SDK_FAILURE;
        }
        rHorizontal /= 2;
        tempN = (cl_float)((cl_float)log((float)width) /
            log((float)rHorizontal));
    }
    n = (cl_uint)tempN;

    // Calculate vertical passes
    while(fabs(tempM - (cl_int)tempM) > 1e-7)
    {
        if(rVertical < 2)
        {
            std::cout <<
                "Height should be a multiple of power of 2 atleast!" << 
                std::endl;
            return SDK_FAILURE;
        }
        rVertical /= 2;
        tempM = (cl_float)((cl_float)log((float)width) 
            / log((float)rVertical));
    }
    m = (cl_uint)tempM;

    return SDK_SUCCESS;

}


int
BoxFilterSAT::writeOutputImage(std::string outputImageName)
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
BoxFilterSAT::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("BoxFilter_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int 
BoxFilterSAT::setupCL()
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
    CHECK_ERROR(retValue, 0, "SDKDeviceInfo::setDeviceInfo() failed");

    // Create and initialize memory objects

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create memory object for input Image
    inputImageBuffer = clCreateBuffer(context,
                                      inMemFlags,
                                      width * height * pixelSize,
                                      0,
                                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputImageBuffer)");

    // Create memory object for temp0
    tempImageBuffer0 = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        width * height * sizeof(cl_uint4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (tempImageBuffer0)");

    // Create memory object for temp1
    tempImageBuffer1 = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        width * height * sizeof(cl_uint4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (tempImageBuffer1)");

    // Create memory objects for output Image
    outputImageBuffer = clCreateBuffer(context,
        CL_MEM_WRITE_ONLY,
        width * height * pixelSize,
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputImageBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("BoxFilter_Kernels.cl");
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
    kernel = clCreateKernel(program, "box_filter", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(box)");

    // Create a kernel object for first horizontal pass of SAT computation
    horizontalSAT0 = clCreateKernel(program, "horizontalSAT0", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(horizontalSAT0)");

    // Create a kernel object for rest horizontal pass of SAT computation
    horizontalSAT = clCreateKernel(program, "horizontalSAT", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(horizontalSAT)");

    // Create a kernel object for vertical pass of SAT computation
    verticalSAT = clCreateKernel(program, "verticalSAT", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(verticalSAT)");

    status =  kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    status =  kernelInfoHSAT0.setKernelWorkGroupInfo(horizontalSAT0, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    status =  kernelInfoHSAT.setKernelWorkGroupInfo(horizontalSAT, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    status =  kernelInfoVSAT.setKernelWorkGroupInfo(verticalSAT, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    size_t minA = min(kernelInfoVSAT.kernelWorkGroupSize, kernelInfoHSAT.kernelWorkGroupSize);
    size_t minB = min(kernelInfo.kernelWorkGroupSize, kernelInfoHSAT0.kernelWorkGroupSize);
    kernelWorkGroupSize = min(minA, minB);

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

        // Three possible cases
        if(blockSizeX > kernelWorkGroupSize)
        {
            blockSizeX = kernelWorkGroupSize;
            blockSizeY = 1;
        }
    }
    return SDK_SUCCESS;
}

int
BoxFilterSAT::runSatKernel(cl_kernel kernel,
                        cl_mem *input,
                        cl_mem *output,
                        cl_uint pass,
                        cl_uint r)
{
    int status;

    // Setup arguments to the kernel

    // input buffer
    status = clSetKernelArg(kernel, 
                            0, 
                            sizeof(cl_mem), 
                            input);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (input)");

    // output buffer
    status = clSetKernelArg(kernel, 
                            1, 
                            sizeof(cl_mem), 
                            output);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (satHorizontalPass[0])");

    // current pass
    status = clSetKernelArg(kernel, 
                            2, 
                            sizeof(cl_uint), 
                            &pass);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (pass)");

    // Memory Fetches for SAT computation
    status = clSetKernelArg(kernel, 
                            3, 
                            sizeof(cl_uint), 
                            &r);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (r)");

    // width of input image
    status = clSetKernelArg(kernel, 
                            4, 
                            sizeof(cl_uint), 
                            &width);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width)");

    size_t globalThreads[] = {width, height};
    size_t localThreads[] = {blockSizeX, blockSizeY};

    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(commandQueue,
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

    return SDK_SUCCESS;

}

int 
BoxFilterSAT::runBoxFilterKernel()
{
    int status;
    cl_mem *input;

    if((n - 1 + m) % 2)
        input = &tempImageBuffer1;
    else
        input = &tempImageBuffer0;


    // Setup arguments to the kernel

    // input buffer
    status = clSetKernelArg(kernel, 
                            0, 
                            sizeof(cl_mem), 
                            input);

    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (input)");

    // output buffer
    status = clSetKernelArg(kernel, 
                            1, 
                            sizeof(cl_mem), 
                            &outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(output)");

    // current pass
    status = clSetKernelArg(kernel, 
                            2, 
                            sizeof(cl_uint), 
                            &filterWidth);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (pass)");

    size_t globalThreads[] = 
    {
        width,
        height
    };
    size_t localThreads[] = {blockSizeX, blockSizeY};

    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(commandQueue,
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

     return SDK_SUCCESS;

}


int 
BoxFilterSAT::runCLKernels()
{
    // Write Data to inputImageBuffer
    cl_int status = CL_SUCCESS;
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                inputImageBuffer,
                CL_FALSE,
                0,
                width * height * sizeof(cl_uchar4),
                inputImageData,
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputImageBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    cl_uint pass = 0;

    /* SAT : Summed Area table computation 
     * This is done in log(N) time
     * This is done in 2 steps : HorizontalPasses and VerticalPasses 
     * This will compute a data structure(i.e SAT) to do fast blur on the image
     * as it allows to apply filter of any size with only 4 fetches from SAT
     * Hence this algorithm is ideal for interactive applications */

    //Compute Horizontal pass = 0
    if(!runSatKernel(horizontalSAT0, 
                     &inputImageBuffer, 
                     &tempImageBuffer0,
                     pass,
                     rHorizontal))
    {
        cl_mem *inputHorizontal = &tempImageBuffer0;
        cl_mem *outputHorizontal = &tempImageBuffer1;

        //Do rest of the horizontal passes
        for(pass = 1; pass < n; pass++)
        {
            if(!runSatKernel(horizontalSAT, 
                             inputHorizontal, 
                             outputHorizontal,
                             pass,
                             rHorizontal))
            {
            }

            // Swap buffers : tempImageBuffer0 and tempImageBuffer1
            cl_mem *temp = inputHorizontal;
            inputHorizontal = outputHorizontal;
            outputHorizontal = temp;
        }
    }

    cl_mem *inputVertical;
    cl_mem *outputVertical;

    if(n % 2)
    {
        inputVertical = &tempImageBuffer0;
        outputVertical = &tempImageBuffer1;
    }
    else
    {
        inputVertical = &tempImageBuffer1;
        outputVertical = &tempImageBuffer0;
    }

    //Do vertical passes
    for(pass = 0; pass < m; pass++)
    {
            if(!runSatKernel(verticalSAT,
                             inputVertical,
                             outputVertical,
                             pass,
                             rVertical))
            {             

            }
            // Swap Buffers here
            cl_mem *temp = inputVertical;
            inputVertical = outputVertical;
            outputVertical = temp;
    }

    if(!runBoxFilterKernel())
    {
    }

    // Read back the value
    cl_event readEvt;
    status = clEnqueueReadBuffer(
                commandQueue,
                outputImageBuffer,
                CL_FALSE,
                0,
                width * height * sizeof(cl_uchar4),
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
BoxFilterSAT::initialize()
{
    // Call base class Initialize to get default configuration
    CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL resource initilization failed");

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory Allocation error.\n");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);
    delete iteration_option;

    streamsdk::Option* filter_width = new streamsdk::Option;
    CHECK_ALLOCATION(filter_width, "Memory Allocation error.\n");

    filter_width->_sVersion = "x";
    filter_width->_lVersion = "width";
    filter_width->_description = "Filter width";
    filter_width->_type = streamsdk::CA_ARG_INT;
    filter_width->_value = &filterWidth;

    sampleArgs->AddOption(filter_width);
    delete filter_width;
    return SDK_SUCCESS;
}

int 
BoxFilterSAT::setup()
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
BoxFilterSAT::run()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Set kernel arguments and run kernel
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << iterations << " iterations" <<std::endl;
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
BoxFilterSAT::cleanup()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseKernel(horizontalSAT0);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseKernel(horizontalSAT);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseKernel(verticalSAT);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");	

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseMemObject(inputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outputImageBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(tempImageBuffer0);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(tempImageBuffer1);
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
BoxFilterSAT::boxFilterCPUReference()
{
    std::cout << "Verifying results...";
    int t = (filterWidth - 1) / 2;
    int filterSize = filterWidth * filterWidth;

    for(int y = 0; y < (int)height; y++)
    {
        for(int x = 0; x < (int)width; x++)
        {
            // Only threads inside apron will calculate their pixel value
            if(x >= t && x < (int)(width - t) && y >= t && y < (int)(height - t))
            {
                cl_int4 sum = {0, 0, 0, 0};
                // For all pixels inside box
                for(int y1 = -t; y1 <= t; y1++)
                {
                    for(int x1 = -t; x1 <= t; x1++)
                    {
                        sum.s[0] += inputImageData[x + x1 + (y + y1) * width].s[0];
                        sum.s[1] += inputImageData[x + x1 + (y + y1) * width].s[1];
                        sum.s[2] += inputImageData[x + x1 + (y + y1) * width].s[2];
                        sum.s[3] += inputImageData[x + x1 + (y + y1) * width].s[3];
                    }
                }
                verificationOutput[x + y * width].s[0] = sum.s[0] / filterSize;
                verificationOutput[x + y * width].s[1] = sum.s[1] / filterSize;
                verificationOutput[x + y * width].s[2] = sum.s[2] / filterSize;
                verificationOutput[x + y * width].s[3] = sum.s[3] / filterSize;
            }
        }
    }
    std::cout<<"done!" <<std::endl;
}

int 
BoxFilterSAT::verifyResults()
{
    if(!byteRWSupport)
        return SDK_SUCCESS;

    if(verify)
    {
        // reference implementation
        boxFilterCPUReference();

        // Compare between outputImageData and verificationOutput
        if(!memcmp(outputImageData, 
                   verificationOutput, 
                   width * height * sizeof(cl_uchar4)))
        {
            std::cout << "Passed!\n" <<std::endl;
        }
        else
        {
            std::cout << "Failed!\n" <<std::endl;
        }
    }
    return SDK_SUCCESS;
}

void 
BoxFilterSAT::printStats()
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