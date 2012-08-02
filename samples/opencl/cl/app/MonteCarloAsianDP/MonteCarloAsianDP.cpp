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


#include "MonteCarloAsianDP.hpp"

#include <math.h>
#include <malloc.h>


/*
 *  structure for attributes of Monte calro 
 *  simulation
 */

 typedef struct _MonteCalroAttrib
{
     cl_double4 strikePrice;
     cl_double4 c1;
     cl_double4 c2;
     cl_double4 c3;
     cl_double4 initPrice;
     cl_double4 sigma;
     cl_double4 timeStep;
 }MonteCarloAttrib;


int 
MonteCarloAsianDP::setupMonteCarloAsianDP()
{
    steps = (steps < 4) ? 4 : steps;
    steps = (steps / 2) * 2;
    
    int i = 0;
    const cl_double finalValue = 0.8;
    const cl_double stepValue = finalValue / (cl_double)steps;
    
    // Allocate and init memory used by host
    sigma = (cl_double*)malloc(steps * sizeof(cl_double));
    CHECK_ALLOCATION(sigma, "Failed to allocate host memory. (sigma)");
    

    sigma[0] = 0.01;
    for(i = 1; i < steps; i++)
    {
        sigma[i] = sigma[i - 1] + stepValue;
    }
    
    price = (cl_double*) malloc(steps * sizeof(cl_double));
    CHECK_ALLOCATION(price, "Failed to allocate host memory. (price)");

    memset((void*)price, 0, steps * sizeof(cl_double));

    vega = (cl_double*) malloc(steps * sizeof(cl_double));
    CHECK_ALLOCATION(vega, "Failed to allocate host memory. (vega)");

    memset((void*)vega, 0, steps * sizeof(cl_double));

    refPrice = (cl_double*) malloc(steps * sizeof(cl_double));
    CHECK_ALLOCATION(refPrice, "Failed to allocate host memory. (refPrice)");
    memset((void*)refPrice, 0, steps * sizeof(cl_double));

    refVega = (cl_double*) malloc(steps * sizeof(cl_double));
    CHECK_ALLOCATION(refVega, "Failed to allocate host memory. (refVega)");
    memset((void*)refVega, 0, steps * sizeof(cl_double));

    // Set samples and exercize points 
    noOfSum = 12;
    noOfTraj = 1024;

    width = noOfTraj / 4;
    height = noOfTraj / 2;

#if defined (_WIN32)
    randNum = (cl_uint*)_aligned_malloc(width * height * sizeof(cl_uint4), 16);
#else
    randNum = (cl_uint*)memalign(16, width * height * sizeof(cl_uint4));
#endif

    CHECK_ALLOCATION(randNum, "Failed to allocate host memory. (randNum)");

    priceVals = (cl_double*)malloc(width * height * 2 * sizeof(cl_double4));
    CHECK_ALLOCATION(priceVals, "Failed to allocate host memory. (priceVals)");
    memset((void*)priceVals, 0, width * height * 2 * sizeof(cl_double4));

    priceDeriv = (cl_double*)malloc(width * height * 2 * sizeof(cl_double4));
    CHECK_ALLOCATION(priceDeriv, "Failed to allocate host memory. (priceDeriv)");
    memset((void*)priceDeriv, 0, width * height * 2 * sizeof(cl_double4));

    /* 
     * Unless quiet mode has been enabled, print the INPUT array.
     * No more than 256 values are printed because it clutters the screen
     * and it is not practical to manually compare a large set of numbers
     */
    if(!quiet) 
    {
        sampleCommon->printArray<cl_double>("sigma values", 
                                           sigma, 
                                           steps, 
                                           1);
    }

    return SDK_SUCCESS;
}

int
MonteCarloAsianDP::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("MonteCarloAsianDP_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
MonteCarloAsianDP::setupCL(void)
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

    context = clCreateContextFromType(cps,
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

    {
        // The block is to move the declaration of prop closer to its use
        cl_command_queue_properties prop = 0;
        commandQueue = clCreateCommandQueue(context, 
                                            devices[deviceId], 
                                            prop, 
                                            &status);
        CHECK_OPENCL_ERROR(status,"clCreateCommandQueue failed.");
        
    }

    std::string buildOptions = std::string("");
    // Check if cl_khr_fp64 extension is supported
    if(strstr(deviceInfo.extensions, "cl_khr_fp64"))
    {
        buildOptions.append("-D KHR_DP_EXTENSION");
    }
    else
    {
        /* Check if cl_amd_fp64 extension is supported */
        if(!strstr(deviceInfo.extensions, "cl_amd_fp64"))
        {
            sampleCommon->expectedError("Device does not support cl_amd_fp64 extension!");
            return SDK_EXPECTED_FAILURE;
        }
    }
    
    // Exit if SDK version is 2.2 or less
    std::string deviceVersionStr = std::string(deviceInfo.deviceVersion);
    size_t vStart = deviceVersionStr.find_last_of("v");
    size_t vEnd = deviceVersionStr.find(" ", vStart);
    std::string vStrVal = deviceVersionStr.substr(vStart + 1, vEnd - vStart - 1);
    if(vStrVal.compare("2.2") <= 0 && dType == CL_DEVICE_TYPE_GPU)
    {
        sampleCommon->expectedError("Few double math functions are not supported in SDK2.2 or less!");
        return SDK_EXPECTED_FAILURE;
    }

    // Exit if OpenCL version is 1.0
    vStart = deviceVersionStr.find(" ", 0);
    vEnd = deviceVersionStr.find(" ", vStart + 1);
    vStrVal = deviceVersionStr.substr(vStart + 1, vEnd - vStart - 1);
    if(vStrVal.compare("1.0") <= 0 && dType == CL_DEVICE_TYPE_GPU)
    {
        sampleCommon->expectedError("Unsupported device!");
        return SDK_EXPECTED_FAILURE;
    }

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    randBuf = clCreateBuffer(context, 
                             inMemFlags,
                             sizeof(cl_uint4) * width  * height,
                             NULL, 
                             &status);
    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(randBuf)");

    randBufAsync = clCreateBuffer(context, 
                             inMemFlags,
                             sizeof(cl_uint4) * width  * height,
                             NULL, 
                             &status);

    CHECK_OPENCL_ERROR(status, "clCreateBuffer(randBufAsync) failed.");


    priceBuf = clCreateBuffer(context, 
                              CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                              sizeof(cl_double4) * width * height * 2,
                              NULL, 
                              &status);
    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(priceBuf)");

    priceDerivBuf = clCreateBuffer(context, 
                                   CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                   sizeof(cl_double4) * width * height * 2,
                                   NULL, 
                                   &status);
    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(priceDerivBuf)");

    priceBufAsync = clCreateBuffer(context, 
                              CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                              sizeof(cl_double4) * width * height * 2,
                              NULL, 
                              &status);
    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(priceBufAsync)");

    priceDerivBufAsync = clCreateBuffer(context, 
                                   CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                   sizeof(cl_double4) * width * height * 2,
                                   NULL, 
                                   &status);
    CHECK_OPENCL_ERROR(status,"clCreateBuffer failed.(priceDerivBufAsync)");

    // create a CL program using the kernel source
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("MonteCarloAsianDP_Kernels.cl");
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
    kernel = clCreateKernel(program, "calPriceVega", &status);
    CHECK_OPENCL_ERROR(status,"clCreateKernel failed.");

    // Settinf the kernel Information
    status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_OPENCL_ERROR(status, "kernelInfo.setKernelWorkGroupInfo failed");

    if((blockSizeX * blockSizeY) > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " 
                      << blockSizeX * blockSizeY << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelInfo.kernelWorkGroupSize << std::endl;
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
MonteCarloAsianDP::runCLKernels(void)
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;

    size_t globalThreads[2] = {width, height};
    size_t localThreads[2] = {blockSizeX, blockSizeY};

    /*
     * Declare attribute structure
     */
    MonteCarloAttrib attributes;

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] || 
       localThreads[1] > deviceInfo.maxWorkItemSizes[1] || 
       (size_t)blockSizeX * blockSizeY > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support requested"
            ":number of work items.";
        return SDK_FAILURE;
    }

    status = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*)&noOfSum);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (noOfSum)");

    status = clSetKernelArg(kernel, 2, sizeof(cl_uint), (void*)&width);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width)");


    double timeStep = maturity / (noOfSum - 1);

    // Initialize random number generator
    srand(1);
    void* inMapPtr1 = NULL;
    void* inMapPtr2 = NULL;
    void* outMapPtr11 = NULL;
    void* outMapPtr12 = NULL;
    void* outMapPtr21 = NULL;
    void* outMapPtr22 = NULL;
    cl_double* ptr21 = NULL;
    cl_double* ptr22 = NULL;

    cl_event inMapEvt1;
    cl_event inMapEvt2;
    cl_event inUnmapEvt1;
    cl_event inUnmapEvt2;

    cl_event outMapEvt11;
    cl_event outMapEvt12;
    cl_event outUnmapEvt11;
    cl_event outUnmapEvt12;

    cl_event outMapEvt21;
    cl_event outMapEvt22;
    cl_event outUnmapEvt21;
    cl_event outUnmapEvt22;
    cl_event ndrEvt;

    size_t inSize = width * height * sizeof(cl_uint4);
    size_t outSize = width * height * sizeof(cl_double4);
    for(int k = 0; k < steps / 2; k++)
    {
        // Map input buffer for kernel 1
        inMapPtr1 = clEnqueueMapBuffer(
                        commandQueue,
                        randBuf,
                        CL_FALSE,
                        CL_MAP_WRITE,
                        0,
                        inSize,
                        0,
                        NULL,
                        &inMapEvt1,
                        &status);
        CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(randBuf) failed.");

        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush() failed.");

        // Generate data for input for kernel 1
        for(int j = 0; j < (width * height * 4); j++)
            randNum[j] = (cl_uint)rand();

        status = sampleCommon->waitForEventAndRelease(&inMapEvt1);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt1) Failed");
        memcpy(inMapPtr1, (void*)randNum, inSize);

        // Unmap of input buffer of kernel 1
        status = clEnqueueUnmapMemObject(
                        commandQueue,
                        randBuf,
                        inMapPtr1,
                        0,
                        NULL,
                        &inUnmapEvt1);
        CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(randBuf) failed.");

        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush() failed.");


        // Get data from output buffers of kernel 2
        if(k != 0)
        {
            // Wait for kernel 2 to complete
            status = sampleCommon->waitForEventAndRelease(&ndrEvt);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

            outMapPtr21 = clEnqueueMapBuffer(
                                commandQueue,
                                priceBufAsync,
                                CL_FALSE,
                                CL_MAP_READ,
                                0,
                                outSize * 2,
                                0,
                                NULL,
                                &outMapEvt21,
                                &status);
            CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceBufAsync) failed.");
            outMapPtr22 = clEnqueueMapBuffer(
                                commandQueue,
                                priceDerivBufAsync,
                                CL_FALSE,
                                CL_MAP_READ,
                                0,
                                outSize * 2,
                                0,
                                NULL,
                                &outMapEvt22,
                                &status);
            CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceDerivBufAsync) failed.");
    
            status = clFlush(commandQueue);
            CHECK_OPENCL_ERROR(status, "clFlush() failed.");
        }

        // Set up arguments required for kernel 1
        double c1 = (interest - 0.5 * sigma[k * 2] * sigma[k * 2]) * timeStep;
        double c2 = sigma[k * 2] * sqrt(timeStep);
        double c3 = (interest + 0.5 * sigma[k * 2] * sigma[k * 2]);
        
        const cl_double4 c1F4 = {c1, c1, c1, c1};
        attributes.c1 = c1F4;

        const cl_double4 c2F4 = {c2, c2, c2, c2};
        attributes.c2 = c2F4;

        const cl_double4 c3F4 = {c3, c3, c3, c3};
        attributes.c3 = c3F4;

        const cl_double4 initPriceF4 = {initPrice, initPrice, initPrice, initPrice};
        attributes.initPrice = initPriceF4;

        const cl_double4 strikePriceF4 = {strikePrice, strikePrice, strikePrice, strikePrice};
        attributes.strikePrice = strikePriceF4;

        const cl_double4 sigmaF4 = {sigma[k * 2], sigma[k * 2], sigma[k * 2], sigma[k * 2]};
        attributes.sigma = sigmaF4;

        const cl_double4 timeStepF4 = {timeStep, timeStep, timeStep, timeStep};
        attributes.timeStep = timeStepF4;

        // Set appropriate arguments to the kernel 
        status = clSetKernelArg(kernel, 0, sizeof(attributes), (void*)&attributes);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(attributes) failed.");

        status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&randBuf); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(randBuf) failed.");

        status = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&priceBuf); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(priceBuf) failed.");

        status = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&priceDerivBuf); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(priceDerivBuf) failed.");

        // Wait for input of kernel 1 to complete
        status = sampleCommon->waitForEventAndRelease(&inUnmapEvt1);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inUnmapEvt1) Failed");
        inMapPtr1 = NULL;

        // Enqueue kernel 1
        status = clEnqueueNDRangeKernel(commandQueue,
                                        kernel,
                                        2,
                                        NULL,
                                        globalThreads,
                                        localThreads,
                                        0,
                                        NULL,
                                        &ndrEvt);
        CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel() failed.");

        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush() failed.");

        // Generate data of input for kernel 2
        // Fill data of input buffer for kernel 2
        if(k <= steps - 1)
        {
            // Map input buffer for kernel 1
            inMapPtr2 = clEnqueueMapBuffer(
                            commandQueue,
                            randBufAsync,
                            CL_FALSE,
                            CL_MAP_WRITE,
                            0,
                            inSize,
                            0,
                            NULL,
                            &inMapEvt2,
                            &status);
            CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(randBufAsync) failed.");

            status = clFlush(commandQueue);
            CHECK_OPENCL_ERROR(status, "clFlush() failed.");

            // Generate data for input for kernel 1
            for(int j = 0; j < (width * height * 4); j++)
                randNum[j] = (cl_uint)rand();

            // Wait for map of input of kernel 1
            status = sampleCommon->waitForEventAndRelease(&inMapEvt2);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt2) Failed");

            memcpy(inMapPtr2, (void*)randNum, inSize);

            // Unmap of input buffer of kernel 1
            status = clEnqueueUnmapMemObject(
                            commandQueue,
                            randBufAsync,
                            inMapPtr2,
                            0,
                            NULL,
                            &inUnmapEvt2);
            CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(randBufAsync) failed.");

            status = clFlush(commandQueue);
            CHECK_OPENCL_ERROR(status, "clFlush() failed.");
        }

        // Wait for output buffers of kernel 2 to complete
        // Calcualte the results from output of kernel 2
        if(k != 0)
        {
            // Wait for output buffers of kernel 2 to complete
            status = sampleCommon->waitForEventAndRelease(&outMapEvt21);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapevt21) Failed");

            status = sampleCommon->waitForEventAndRelease(&outMapEvt22);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapevt22) Failed");

            // Calcualte the results from output of kernel 2
            ptr21 = (cl_double*)outMapPtr21;
            ptr22 = (cl_double*)outMapPtr22;
            for(int i = 0; i < noOfTraj * noOfTraj; i++)
            {
                price[k * 2 - 1] += ptr21[i];
                vega[k * 2 - 1] += ptr22[i];
            }

            price[k * 2 - 1] /= (noOfTraj * noOfTraj);
            vega[k * 2 - 1] /= (noOfTraj * noOfTraj);

            price[k * 2 - 1] = exp(-interest * maturity) * price[k * 2 - 1];
            vega[k * 2 - 1] = exp(-interest * maturity) * vega[k * 2 - 1];
            
            // Unmap of output buffers of kernel 2
            status = clEnqueueUnmapMemObject(
                            commandQueue,
                            priceBufAsync,
                            outMapPtr21,
                            0,
                            NULL,
                            &outUnmapEvt21);
            CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(priceBufAsync) failed.");

            status = clEnqueueUnmapMemObject(
                            commandQueue,
                            priceDerivBufAsync,
                            outMapPtr22,
                            0,
                            NULL,
                            &outUnmapEvt22);
            CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(priceDerivBufAsync) failed.");

            status = clFlush(commandQueue);
            CHECK_OPENCL_ERROR(status, "clFlush() failed.");

            status = sampleCommon->waitForEventAndRelease(&outUnmapEvt21);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outUnmapevt21) Failed");

            status = sampleCommon->waitForEventAndRelease(&outUnmapEvt22);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outUnmapevt22) Failed");
        }

        // Wait for kernel 1 to complete
        status = sampleCommon->waitForEventAndRelease(&ndrEvt);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

        // Get data from output buffers of kernel 1
        outMapPtr11 = clEnqueueMapBuffer(
                            commandQueue,
                            priceBuf,
                            CL_FALSE,
                            CL_MAP_READ,
                            0,
                            outSize * 2,
                            0,
                            NULL,
                            &outMapEvt11,
                            &status);
        CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceBuf) failed.");
        outMapPtr12 = clEnqueueMapBuffer(
                            commandQueue,
                            priceDerivBuf,
                            CL_FALSE,
                            CL_MAP_READ,
                            0,
                            outSize * 2,
                            0,
                            NULL,
                            &outMapEvt12,
                            &status);
        CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceDerivBuf) failed.");
    
        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush() failed.");

        // Set up arguments required for kernel 2
        double c21 = (interest - 0.5 * sigma[k * 2 + 1] * sigma[k * 2 + 1]) * timeStep;
        double c22 = sigma[k * 2 + 1] * sqrt(timeStep);
        double c23 = (interest + 0.5 * sigma[k * 2 + 1] * sigma[k * 2 + 1]);
        
        const cl_double4 c1F42 = {c21, c21, c21, c21};
        attributes.c1 = c1F42;

        const cl_double4 c2F42 = {c22, c22, c22, c22};
        attributes.c2 = c2F42;

        const cl_double4 c3F42 = {c23, c23, c23, c23};
        attributes.c3 = c3F42;

        const cl_double4 initPriceF42 = {initPrice, initPrice, initPrice, initPrice};
        attributes.initPrice = initPriceF42;

        const cl_double4 strikePriceF42 = {strikePrice, strikePrice, strikePrice, strikePrice};
        attributes.strikePrice = strikePriceF42;

        const cl_double4 sigmaF42 = {sigma[k * 2 + 1], sigma[k * 2 + 1], sigma[k * 2 + 1], sigma[k * 2 + 1]};
        attributes.sigma = sigmaF42;

        const cl_double4 timeStepF42 = {timeStep, timeStep, timeStep, timeStep};
        attributes.timeStep = timeStepF42;

        // Set appropriate arguments to the kernel 
        status = clSetKernelArg(kernel, 0, sizeof(attributes), (void*)&attributes);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(attributes) failed.");

        status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&randBufAsync); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(randBuf) failed.");

        status = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&priceBufAsync); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(priceBuf) failed.");

        status = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&priceDerivBufAsync); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(priceDerivBuf) failed.");

        // Wait for input of kernel 2 to complete
        status = sampleCommon->waitForEventAndRelease(&inUnmapEvt2);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inUnmapEvt2) Failed");
        inMapPtr2 = NULL;

        // Enqueue kernel 2
        status = clEnqueueNDRangeKernel(commandQueue,
                                        kernel,
                                        2,
                                        NULL,
                                        globalThreads,
                                        localThreads,
                                        0,
                                        NULL,
                                        &ndrEvt);
        CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel() failed.");

        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush() failed.");

        // Wait for output buffers of kernel 1 to complete
        status = sampleCommon->waitForEventAndRelease(&outMapEvt11);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapevt11) Failed");


        status = sampleCommon->waitForEventAndRelease(&outMapEvt12);
            CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapevt12 ) Failed");

        // Calcualte the results from output of kernel 2
        ptr21 = (cl_double*)outMapPtr11;
        ptr22 = (cl_double*)outMapPtr12;
        for(int i = 0; i < noOfTraj * noOfTraj; i++)
        {
            price[k * 2] += ptr21[i];
            vega[k * 2] += ptr22[i];
        }

        price[k * 2] /= (noOfTraj * noOfTraj);
        vega[k * 2] /= (noOfTraj * noOfTraj);

        price[k * 2] = exp(-interest * maturity) * price[k * 2];
        vega[k * 2] = exp(-interest * maturity) * vega[k * 2];
        
        // Unmap of output buffers of kernel 2
        status = clEnqueueUnmapMemObject(
                        commandQueue,
                        priceBuf,
                        outMapPtr11,
                        0,
                        NULL,
                        &outUnmapEvt11);
        CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(priceBuf) failed.");

        status = clEnqueueUnmapMemObject(
                        commandQueue,
                        priceDerivBuf,
                        outMapPtr12,
                        0,
                        NULL,
                        &outUnmapEvt12);
        CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(priceDerivBuf) failed.");

        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush() failed.");

        status = sampleCommon->waitForEventAndRelease(&outUnmapEvt11);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outUnmapEvt11) Failed");

        status = sampleCommon->waitForEventAndRelease(&outUnmapEvt12);
        CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outUnmapEvt12) Failed");
    }

    // Wait for kernel 1 to complete
    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

    // Gather last kernel 2 execution here
    outMapPtr21 = clEnqueueMapBuffer(
                        commandQueue,
                        priceBufAsync,
                        CL_FALSE,
                        CL_MAP_READ,
                        0,
                        outSize * 2,
                        0,
                        NULL,
                        &outMapEvt21,
                        &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceBuf) failed.");

    outMapPtr22 = clEnqueueMapBuffer(
                        commandQueue,
                        priceDerivBufAsync,
                        CL_FALSE,
                        CL_MAP_READ,
                        0,
                        outSize * 2,
                        0,
                        NULL,
                        &outMapEvt22,
                        &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceDerivBuf) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush() failed.");

    status = sampleCommon->waitForEventAndRelease(&outMapEvt21);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapEvt21) Failed");

    status = sampleCommon->waitForEventAndRelease(&outMapEvt22);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapEvt22) Failed");

    // Calcualte the results from output of kernel 2
    ptr21 = (cl_double*)outMapPtr21;
    ptr22 = (cl_double*)outMapPtr22;
    for(int i = 0; i < noOfTraj * noOfTraj; i++)
    {
        price[steps - 1] += ptr21[i];
        vega[steps - 1] += ptr22[i];
    }

    price[steps - 1] /= (noOfTraj * noOfTraj);
    vega[steps - 1] /= (noOfTraj * noOfTraj);

    price[steps - 1] = exp(-interest * maturity) * price[steps - 1];
    vega[steps - 1] = exp(-interest * maturity) * vega[steps - 1];

    return SDK_SUCCESS;
}

int
MonteCarloAsianDP::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize())
        return SDK_FAILURE;
        
    const int optionsCount = 5;
    streamsdk::Option *optionList = new streamsdk::Option[optionsCount];
    CHECK_ALLOCATION(optionList,"Failed to allocate host memory. (optionList)");

    optionList[0]._sVersion = "c";
    optionList[0]._lVersion = "steps";
    optionList[0]._description = "Steps of Monte carlo simuation";
    optionList[0]._type = streamsdk::CA_ARG_INT;
    optionList[0]._value = &steps;

    optionList[1]._sVersion = "P";
    optionList[1]._lVersion = "initPrice";
    optionList[1]._description = "Initial price(Default value 50)";
    optionList[1]._type = streamsdk::CA_ARG_DOUBLE;
    optionList[1]._value = &initPrice;

    optionList[2]._sVersion = "s";
    optionList[2]._lVersion = "strikePrice";
    optionList[2]._description = "Strike price (Default value 55)";
    optionList[2]._type = streamsdk::CA_ARG_DOUBLE;
    optionList[2]._value = &strikePrice;

    optionList[3]._sVersion = "r";
    optionList[3]._lVersion = "interest";
    optionList[3]._description = "interest rate (Default value 0.06)";
    optionList[3]._type = streamsdk::CA_ARG_DOUBLE;
    optionList[3]._value = &interest;

    optionList[4]._sVersion = "m";
    optionList[4]._lVersion = "maturity";
    optionList[4]._description = "Maturity (Default value 1)";
    optionList[4]._type = streamsdk::CA_ARG_DOUBLE;
    optionList[4]._value = &maturity;


    for(cl_int i = 0; i < optionsCount; ++i)
    {
        sampleArgs->AddOption(&optionList[i]);
    }

    delete[] optionList;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option,"Failed to allocate host memory. (iteration_option)");
    
    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);

    delete iteration_option;
    
    return SDK_SUCCESS;
}

int MonteCarloAsianDP::setup()
{
    int status=setupMonteCarloAsianDP();
    CHECK_ERROR(status, SDK_SUCCESS, "MonteCarloAsianDP::setup) failed");

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


int MonteCarloAsianDP::run()
{
    int status = 0;
    // Warmup
    for(int i = 0; i < 2; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout<<"Executing kernel for " << 
        iterations << " iterations" << std::endl;
    std::cout<<"-------------------------------------------" << 
        std::endl;

    // create and initialize timers
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels()!=SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    // Compute average kernel time
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
    {
        sampleCommon->printArray<cl_double>("price", price, steps, 1);
        sampleCommon->printArray<cl_double>("vega", vega, steps, 1);
    }

    return SDK_SUCCESS;
}

void
MonteCarloAsianDP::printStats()
{
    std::string strArray[4] = 
    {
        "Steps", 
        "Time(sec)",
        "[Transfer+kernel](sec)", 
        "Samples used /sec"
    };
    std::string stats[4];

    totalTime = setupTime + kernelTime;
    stats[0] = sampleCommon->toString(steps, std::dec);
    stats[1] = sampleCommon->toString(totalTime, std::dec);
    stats[2] = sampleCommon->toString(kernelTime, std::dec);
    stats[3] = sampleCommon->toString((noOfTraj * (noOfSum - 1) * steps) / 
                                      kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}

void
MonteCarloAsianDP::lshift128(unsigned int* input,
                           unsigned int shift, 
                           unsigned int * output)
{
    unsigned int invshift = 32u - shift;

    output[0] = input[0] << shift;
    output[1] = (input[1] << shift) | (input[0] >> invshift);
    output[2] = (input[2] << shift) | (input[1] >> invshift);
    output[3] = (input[3] << shift) | (input[2] >> invshift);
}

void
MonteCarloAsianDP::rshift128(unsigned int* input, 
                           unsigned int shift, 
                           unsigned int* output)
{
    unsigned int invshift = 32u - shift;
    output[3]= input[3] >> shift;
    output[2] = (input[2] >> shift) | (input[0] >> invshift);
    output[1] = (input[1] >> shift) | (input[1] >> invshift);
    output[0] = (input[0] >> shift) | (input[2] >> invshift);
}

void 
MonteCarloAsianDP::generateRand(unsigned int* seed,
                              double *gaussianRand1,
                              double *gaussianRand2,
                              unsigned int* nextRand)
{

    unsigned int mulFactor = 4;
    unsigned int temp[8][4];

    unsigned int state1[4] = {seed[0], seed[1], seed[2], seed[3]};
    unsigned int state2[4] = {0u, 0u, 0u, 0u}; 
    unsigned int state3[4] = {0u, 0u, 0u, 0u}; 
    unsigned int state4[4] = {0u, 0u, 0u, 0u}; 
    unsigned int state5[4] = {0u, 0u, 0u, 0u}; 

    unsigned int stateMask = 1812433253u;
    unsigned int thirty = 30u;
    unsigned int mask4[4] = {stateMask, stateMask, stateMask, stateMask};
    unsigned int thirty4[4] = {thirty, thirty, thirty, thirty};
    unsigned int one4[4] = {1u, 1u, 1u, 1u};
    unsigned int two4[4] = {2u, 2u, 2u, 2u};
    unsigned int three4[4] = {3u, 3u, 3u, 3u};
    unsigned int four4[4] = {4u, 4u, 4u, 4u};

    unsigned int r1[4] = {0u, 0u, 0u, 0u}; 
    unsigned int r2[4] = {0u, 0u, 0u, 0u}; 

    unsigned int a[4] = {0u, 0u, 0u, 0u}; 
    unsigned int b[4] = {0u, 0u, 0u, 0u}; 

    unsigned int e[4] = {0u, 0u, 0u, 0u}; 
    unsigned int f[4] = {0u, 0u, 0u, 0u}; 
    
    unsigned int thirteen  = 13u;
    unsigned int fifteen = 15u;
    unsigned int shift = 8u * 3u;

    unsigned int mask11 = 0xfdff37ffu;
    unsigned int mask12 = 0xef7f3f7du;
    unsigned int mask13 = 0xff777b7du;
    unsigned int mask14 = 0x7ff7fb2fu;

    const double one = 1.0;
    const double intMax = 4294967296.0;
    const double PI = 3.14159265358979;
    const double two = 2.0;

    double r[4] = {0.0, 0.0, 0.0, 0.0}; 
    double phi[4] = {0.0, 0.0, 0.0, 0.0};

    double temp1[4] = {0.0, 0.0, 0.0, 0.0};
    double temp2[4] = {0.0, 0.0, 0.0, 0.0};

    //Initializing states.
    for(int c = 0; c < 4; ++c)
    {
        state2[c] = mask4[c] * (state1[c] ^ (state1[c] >> thirty4[c])) + one4[c];
        state3[c] = mask4[c] * (state2[c] ^ (state2[c] >> thirty4[c])) + two4[c];
        state4[c] = mask4[c] * (state3[c] ^ (state3[c] >> thirty4[c])) + three4[c];
        state5[c] = mask4[c] * (state4[c] ^ (state4[c] >> thirty4[c])) + four4[c];
    }

    unsigned int i = 0;
    for(i = 0; i < mulFactor; ++i)
    {
        switch(i)
        {
            case 0:
                for(int c = 0; c < 4; ++c)
                {
                    r1[c] = state4[c];
                    r2[c] = state5[c];
                    a[c] = state1[c];
                    b[c] = state3[c];
                }
                break;
            case 1:
                for(int c = 0; c < 4; ++c)
                {
                    r1[c] = r2[c];
                    r2[c] = temp[0][c];
                    a[c] = state2[c];
                    b[c] = state4[c];
                }
                break;
            case 2:
                for(int c = 0; c < 4; ++c)
                {
                    r1[c] = r2[c];
                    r2[c] = temp[1][c];
                    a[c] = state3[c];
                    b[c] = state5[c];
                }
                break;
            case 3:
                for(int c = 0; c < 4; ++c)
                {
                    r1[c] = r2[c];
                    r2[c] = temp[2][c];
                    a[c] = state4[c];
                    b[c] = state1[c];
                }
                break;
            default:
                break;            

        }

        lshift128(a, shift, e);
        rshift128(r1, shift, f);
        
        temp[i][0] = a[0] ^ e[0] ^ ((b[0] >> thirteen) & mask11) ^ f[0] ^ (r2[0] << fifteen);
        temp[i][1] = a[1] ^ e[1] ^ ((b[1] >> thirteen) & mask12) ^ f[1] ^ (r2[1] << fifteen);
        temp[i][2] = a[2] ^ e[2] ^ ((b[2] >> thirteen) & mask13) ^ f[2] ^ (r2[2] << fifteen);
        temp[i][3] = a[3] ^ e[3] ^ ((b[3] >> thirteen) & mask14) ^ f[3] ^ (r2[3] << fifteen);

    }

    for(int c = 0; c < 4; ++c)
    {    
        temp1[c] = temp[0][c] * one / intMax;
        temp2[c] = temp[1][c] * one / intMax;
    }

    for(int c = 0; c < 4; ++c)
    {
        // Applying Box Mullar Transformations.
        r[c] = sqrt((-two) * log(temp1[c]));
        phi[c]  = two * PI * temp2[c];
        gaussianRand1[c] = r[c] * cos(phi[c]);
        gaussianRand2[c] = r[c] * sin(phi[c]);
        
        nextRand[c] = temp[2][c];
    }
}

void 
MonteCarloAsianDP::calOutputs(double strikePrice, 
                            double* meanDeriv1, 
                            double*  meanDeriv2, 
                            double* meanPrice1,
                            double* meanPrice2, 
                            double* pathDeriv1, 
                            double* pathDeriv2, 
                            double* priceVec1, 
                            double* priceVec2)
{
    double temp1[4] = {0.0, 0.0, 0.0, 0.0};
    double temp2[4] = {0.0, 0.0, 0.0, 0.0};
    double temp3[4] = {0.0, 0.0, 0.0, 0.0};
    double temp4[4] = {0.0, 0.0, 0.0, 0.0};
    
    double tempDiff1[4] = {0.0, 0.0, 0.0, 0.0};
    double tempDiff2[4] = {0.0, 0.0, 0.0, 0.0};

    for(int c = 0; c < 4; ++c)
    {
        tempDiff1[c] = meanPrice1[c] - strikePrice;
        tempDiff2[c] = meanPrice2[c] - strikePrice;
    }
    if(tempDiff1[0] > 0.0)
    {
        temp1[0] = 1.0;
        temp3[0] = tempDiff1[0];
    }
    if(tempDiff1[1] > 0.0)
    {
        temp1[1] = 1.0;
        temp3[1] = tempDiff1[1];
    }
    if(tempDiff1[2] > 0.0)
    {
        temp1[2] = 1.0;
        temp3[2] = tempDiff1[2];
    }
    if(tempDiff1[3] > 0.0)
    {
        temp1[3] = 1.0;
        temp3[3] = tempDiff1[3];
    }

    if(tempDiff2[0] > 0.0)
    {
        temp2[0] = 1.0;
        temp4[0] = tempDiff2[0];
    }
    if(tempDiff2[1] > 0.0)
    {
        temp2[1] = 1.0;
        temp4[1] = tempDiff2[1];
    }
    if(tempDiff2[2] > 0.0)
    {
        temp2[2] = 1.0;
        temp4[2] = tempDiff2[2];
    }
    if(tempDiff2[3] > 0.0)
    {
        temp2[3] = 1.0;
        temp4[3] = tempDiff2[3];
    }

    for(int c = 0; c < 4; ++c)
    {
        pathDeriv1[c] = meanDeriv1[c] * temp1[c]; 
        pathDeriv2[c] = meanDeriv2[c] * temp2[c]; 
        priceVec1[c] = temp3[c]; 
        priceVec2[c] = temp4[c];	
    }
}

void MonteCarloAsianDP::cpuReferenceImpl()
{
    double timeStep = maturity / (noOfSum - 1);
    
    // Initialize random number generator
    srand(1);

    for(int k = 0; k < steps; k++)
    {
        double c1 = (interest - 0.5 * sigma[k] * sigma[k]) * timeStep;
        double c2 = sigma[k] * sqrt(timeStep);
        double c3 = (interest + 0.5 * sigma[k] * sigma[k]);  

        for(int j = 0; j < (width * height); j++)
        {
            unsigned int nextRand[4] = {0u, 0u, 0u, 0u};
            for(int c = 0; c < 4; ++c)
                nextRand[c] = (cl_uint)rand();

            double trajPrice1[4] = {initPrice, initPrice, initPrice, initPrice};
            double sumPrice1[4] = {initPrice, initPrice, initPrice, initPrice};
            double sumDeriv1[4] = {0.0, 0.0, 0.0, 0.0};
            double meanPrice1[4] = {0.0, 0.0, 0.0, 0.0};
            double meanDeriv1[4] = {0.0, 0.0, 0.0, 0.0};
            double price1[4] = {0.0, 0.0, 0.0, 0.0};
            double pathDeriv1[4] = {0.0, 0.0, 0.0, 0.0};

            double trajPrice2[4] = {initPrice, initPrice, initPrice, initPrice};
            double sumPrice2[4] = {initPrice, initPrice, initPrice, initPrice};
            double sumDeriv2[4] = {0.0, 0.0, 0.0, 0.0};
            double meanPrice2[4] = {0.0, 0.0, 0.0, 0.0};
            double meanDeriv2[4] = {0.0, 0.0, 0.0, 0.0};
            double price2[4] = {0.0, 0.0, 0.0, 0.0};
            double pathDeriv2[4] = {0.0, 0.0, 0.0, 0.0};

            //Run the Monte Carlo simulation a total of Num_Sum - 1 times
            for(int i = 1; i < noOfSum; i++)
            {
                unsigned int tempRand[4] =  {0u, 0u, 0u, 0u};
                for(int c = 0; c < 4; ++c)
                    tempRand[c] = nextRand[c];

                double gaussian1[4] = {0.0, 0.0, 0.0, 0.0};
                double gaussian2[4] = {0.0, 0.0, 0.0, 0.0};
                generateRand(tempRand, gaussian1, gaussian2, nextRand);
                //Calculate the trajectory price and sum price for all trajectories
                for(int c = 0; c < 4; ++c)
                {
                    trajPrice1[c] = trajPrice1[c] * exp(c1 + c2 * gaussian1[c]);
                    trajPrice2[c] = trajPrice2[c] * exp(c1 + c2 * gaussian2[c]);
                    
                    sumPrice1[c] = sumPrice1[c] + trajPrice1[c];
                    sumPrice2[c] = sumPrice2[c] + trajPrice2[c];

                    double temp = c3 * timeStep * i;

                    // Calculate the derivative price for all trajectories
                    sumDeriv1[c] = sumDeriv1[c] + trajPrice1[c] 
                                * ((log(trajPrice1[c] / initPrice) - temp) / sigma[k]);
                    sumDeriv2[c] = sumDeriv2[c] + trajPrice2[c] 
                                * ((log(trajPrice2[c] / initPrice) - temp) / sigma[k]);			            
                }

        }

            //Calculate the average price and “average derivative” of each simulated path
            for(int c = 0; c < 4; ++c)
            {
                meanPrice1[c] = sumPrice1[c] / noOfSum;
                meanPrice2[c] = sumPrice2[c] / noOfSum;
                meanDeriv1[c] = sumDeriv1[c] / noOfSum;
                meanDeriv2[c] = sumDeriv2[c] / noOfSum;
            }

            calOutputs(strikePrice, meanDeriv1, meanDeriv2, meanPrice1, meanPrice2, 
                        pathDeriv1, pathDeriv2, price1, price2);

            for(int c = 0; c < 4; ++c)
            {
                priceVals[j * 8 + c] = price1[c];
                priceVals[j * 8 + 1 * 4 + c] = price2[c];
                priceDeriv[j * 8 + c] = pathDeriv1[c]; 
                priceDeriv[j * 8 + 1 * 4 + c] = pathDeriv2[c];
            }
        }

        // Replace Following "for" loop with reduction kernel
        for(int i = 0; i < noOfTraj * noOfTraj; i++)
        {
            refPrice[k] += priceVals[i];
            refVega[k] += priceDeriv[i];
        }

        refPrice[k] /= (noOfTraj * noOfTraj);
        refVega[k] /= (noOfTraj * noOfTraj);

        refPrice[k] = exp(-interest * maturity) * refPrice[k];
        refVega[k] = exp(-interest * maturity) * refVega[k];
    }
}

int MonteCarloAsianDP::verifyResults()
{
    if(verify)
    {
        /* reference implementation
         * it overwrites the input array with the output
         */
        cpuReferenceImpl();

        // compare the results and see if they match
        for(int i = 0; i < steps; ++i)
        {
            if(fabs(price[i] - refPrice[i]) > 0.2)
            {
                std::cout << "Failed\n";
                return SDK_FAILURE;
            }
            if(fabs(vega[i] - refVega[i]) > 0.2)
            {
                std::cout << "Failed\n" << std::endl;
                return SDK_FAILURE;
            }
        }
        std::cout << "Passed!\n" << std::endl;
    }

    return SDK_SUCCESS;
}

int MonteCarloAsianDP::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseMemObject(priceBuf);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(priceBuf)");

    status = clReleaseMemObject(priceDerivBuf);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(priceDerivBuf)");

    status = clReleaseMemObject(randBuf);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(randBuf)");

    status = clReleaseMemObject(priceBufAsync);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(priceBufAsync)");

    status = clReleaseMemObject(priceDerivBufAsync);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(priceDerivBufAsync)");

    status = clReleaseMemObject(randBufAsync);
    CHECK_OPENCL_ERROR(status,"clReleaseMemObject failed.(randBufAsync)");

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status,"clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status,"clReleaseProgram failed.(program)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status,"clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status,"clReleaseContext failed.(context)");


    // Release program resources (input memory etc.)

        FREE(sigma);
        FREE(price);
        FREE(vega);
        FREE(refPrice);
        FREE(refVega);


    if(randNum)
    {
        #if defined (_WIN32)
            ALIGNED_FREE(randNum);
        #else
            FREE(randNum);
        #endif
        randNum = NULL;
    }

        FREE(priceVals);
        FREE(priceDeriv);
        FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    // Create MonteCalroAsian object
    MonteCarloAsianDP clMonteCarloAsianDP(
        "OpenCL Monte Carlo simulation for Asian Option Pricing");
    
    // Initialization
    if(clMonteCarloAsianDP.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Parse command line options
    if(clMonteCarloAsianDP.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clMonteCarloAsianDP.isDumpBinaryEnabled())
    {
        return clMonteCarloAsianDP.genBinaryImage();
    }
    else
    {
        // Setup
        int returnVal = clMonteCarloAsianDP.setup();

        if(returnVal != SDK_SUCCESS)
            return (returnVal == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

        // Run
        if(clMonteCarloAsianDP.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        if(clMonteCarloAsianDP.verifyResults()!=SDK_SUCCESS)
            return SDK_FAILURE;

        // Cleanup resources created 
        if(clMonteCarloAsianDP.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        clMonteCarloAsianDP.printStats();
    }
    return SDK_SUCCESS;
}