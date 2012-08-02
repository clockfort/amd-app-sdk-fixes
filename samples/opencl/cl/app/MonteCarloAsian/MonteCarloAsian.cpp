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


#include "MonteCarloAsian.hpp"

#include <math.h>
#include <malloc.h>


/*
 *  structure for attributes of Monte calro 
 *  simulation
 */

 typedef struct _MonteCalroAttrib
{
     cl_float4 strikePrice;
     cl_float4 c1;
     cl_float4 c2;
     cl_float4 c3;
     cl_float4 initPrice;
     cl_float4 sigma;
     cl_float4 timeStep;
}MonteCarloAttrib;


int 
MonteCarloAsian::setupMonteCarloAsian()
{

    if(!disableAsync)
    {
        steps = (steps < 4) ? 4 : steps;
        steps = (steps / 2) * 2;
    }

    // Validate flags
    if(dUseOutAllocHostPtr)
        if(!(disableAsync && disableMapping))
        {
            std::cout << "Note : Neglected --dOutAllocHostPtr flag" << std::endl;
            dUseOutAllocHostPtr = false;
        }

    if(dUseInPersistent)
        if(!(disableAsync && disableMapping))
        {
            std::cout << "Note : Neglected --dInPersistent flag" << std::endl;
            dUseInPersistent = false;
        }

    int i = 0;
    const cl_float finalValue = 0.8f;
    const cl_float stepValue = finalValue / (cl_float)steps;

    // Allocate and init memory used by host
    sigma = (cl_float*)malloc(steps * sizeof(cl_float));
    CHECK_ALLOCATION(sigma, "Failed to allocate host memory. (sigma)");

    sigma[0] = 0.01f;
    for(i = 1; i < steps; i++)
        sigma[i] = sigma[i - 1] + stepValue;

    price = (cl_float*) malloc(steps * sizeof(cl_float));
    CHECK_ALLOCATION(price, "Failed to allocate host memory. (price)");
    memset((void*)price, 0, steps * sizeof(cl_float));

    vega = (cl_float*) malloc(steps * sizeof(cl_float));
    CHECK_ALLOCATION(price, "Failed to allocate host memory. (vega)");
    memset((void*)vega, 0, steps * sizeof(cl_float));

    refPrice = (cl_float*) malloc(steps * sizeof(cl_float));
    CHECK_ALLOCATION(refPrice, "Failed to allocate host memory. (refPrice)");
    memset((void*)refPrice, 0, steps * sizeof(cl_float));

    refVega = (cl_float*) malloc(steps * sizeof(cl_float));
    CHECK_ALLOCATION(refVega, "Failed to allocate host memory. (refVega)");
    memset((void*)refVega, 0, steps * sizeof(cl_float));
    
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

    priceVals = (cl_float*)malloc(width * height * 2 * sizeof(cl_float4));
    CHECK_ALLOCATION(priceVals, "Failed to allocate host memory. (priceVals)");
    memset((void*)priceVals, 0, width * height * 2 * sizeof(cl_float4));

    priceDeriv = (cl_float*)malloc(width * height * 2 * sizeof(cl_float4));
    CHECK_ALLOCATION(priceDeriv, "Failed to allocate host memory. (priceDeriv)");
    memset((void*)priceDeriv, 0, width * height * 2 * sizeof(cl_float4));

    if(!disableAsync && disableMapping)
    {
        priceValsAsync = (cl_float*)malloc(width * height * 2 * sizeof(cl_float4));
        CHECK_ALLOCATION(priceValsAsync, "Failed to allocate host memory. (priceValsAsync)");
        memset((void*)priceValsAsync, 0, width * height * 2 * sizeof(cl_float4));

        priceDerivAsync = (cl_float*)malloc(width * height * 2 * sizeof(cl_float4));
        CHECK_ALLOCATION(priceDerivAsync, "Failed to allocate host memory. (priceDerivAsync)");
        memset((void*)priceDerivAsync, 0, width * height * 2 * sizeof(cl_float4));
    }
    /* 
     * Unless quiet mode has been enabled, print the INPUT array.
     * No more than 256 values are printed because it clutters the screen
     * and it is not practical to manually compare a large set of numbers
     */

    if(!quiet) 
        sampleCommon->printArray<cl_float>(
                            "sigma values", 
                            sigma, 
                            steps, 
                            1);

    return SDK_SUCCESS;
    
}

int
MonteCarloAsian::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("MonteCarloAsian_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
MonteCarloAsian::setupCL(void)
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

    // Get platform
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
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType() failed.");

    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed");

    //Set device info of given cl_device_id

    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    commandQueue = clCreateCommandQueue(context, 
                                        devices[deviceId], 
                                        0, 
                                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueue(commandQueue) failed.");

    if(!dUseInPersistent)
    {
        // Set Presistent memory only for AMD platform
        cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
        if(isAmdPlatform())
            inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;
        
        // Create buffer for randBuf
        randBuf = clCreateBuffer(context, 
                                 inMemFlags,
                                 sizeof(cl_uint4) * width  * height,
                                 NULL, 
                                 &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer(randBuf) failed.");

        if(!disableAsync)
        {
            // create Buffer for randBuf in asynchronose mode
            randBufAsync = clCreateBuffer(context, 
                                     inMemFlags,
                                     sizeof(cl_uint4) * width  * height,
                                     NULL, 
                                     &status);
            CHECK_OPENCL_ERROR(status, "clCreateBuffer(randBufAsync) failed.");
        }
    }
    else
    {
        // create Normal Buffer, if presistent memory is not in used
        randBuf = clCreateBuffer(context, 
                                 CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                 sizeof(cl_uint4) * width  * height,
                                 randNum, 
                                 &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer(randBuf) failed.");
    }

    if(!dUseOutAllocHostPtr)
    {
        // create Buffer for PriceBuf
        priceBuf = clCreateBuffer(context, 
                                  CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                  sizeof(cl_float4) * width * height * 2,
                                  NULL, 
                                  &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer(priceBuf) failed.");

        // create Buffer for PriceDeriveBuffer
        priceDerivBuf = clCreateBuffer(context, 
                                       CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                       sizeof(cl_float4) * width * height * 2,
                                       NULL, 
                                       &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer(priceDerivBuf) failed.");
        
        if(!disableAsync)
        {
            // create Buffuer for priceBufAsync
            priceBufAsync = clCreateBuffer(context, 
                                      CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                      sizeof(cl_float4) * width * height * 2,
                                      NULL, 
                                      &status);
            CHECK_OPENCL_ERROR(status, "clCreateBuffer(priceBufAsync) failed.");

            // create Buffuer for priceDerivBufAsync
            priceDerivBufAsync = clCreateBuffer(context, 
                                           CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                           sizeof(cl_float4) * width * height * 2,
                                           NULL, 
                                           &status);
            CHECK_OPENCL_ERROR(status, "clCreateBuffer(priceDerivBufAsync) failed.");
        }
    }
    else
    {
        // create Buffer for priceBuf
        priceBuf = clCreateBuffer(context, 
                                  CL_MEM_WRITE_ONLY,
                                  sizeof(cl_float4) * width * height * 2,
                                  NULL, 
                                  &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer(priceBuf) failed.");

        // create Buffer for priceDerivBuf
        priceDerivBuf = clCreateBuffer(context, 
                                       CL_MEM_WRITE_ONLY,
                                       sizeof(cl_float4) * width * height * 2,
                                       NULL, 
                                       &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer(priceDerivBuf) failed.");
    }    

    // Create a CL program using the kernel source
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("MonteCarloAsian_Kernels.cl");
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
    CHECK_OPENCL_ERROR(status, "clCreateKernel(calPriceVega) failed.");

    // Check group-size against what is returned by kernel
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
MonteCarloAsian::runCLKernels(void)
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;
    size_t globalThreads[2] = {width, height};
    size_t localThreads[2] = {blockSizeX, blockSizeY};

    // Declare attribute structure
    MonteCarloAttrib attributes;

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] || 
       localThreads[1] > deviceInfo.maxWorkItemSizes[1] || 
       (localThreads[0] * localThreads[1]) > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support requested"
                  << ":number of work items.";
        return SDK_FAILURE;
    }

    // width - i.e number of elements in the array
    status = clSetKernelArg(kernel, 2, sizeof(cl_uint), (void*)&width);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(width) failed.");

    status = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*)&noOfSum);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(noOfSum) failed.");

    if(disableAsync)
    {
        status = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&randBuf); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(randBuf) failed.");

        status = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&priceBuf); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(priceBuf) failed.");

        status = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&priceDerivBuf); 
        CHECK_OPENCL_ERROR(status, "clSetKernelArg(priceDerivBuf) failed.");
    }

    float timeStep = maturity / (noOfSum - 1);

    // Initialize random number generator
    srand(1);
    if(!disableAsync)
    {
        if(disableMapping)
        {
            cl_event inWriteEvt1;
            cl_event inWriteEvt2;
            cl_event outReadEvt11;
            cl_event outReadEvt12;
            cl_event outReadEvt21;
            cl_event outReadEvt22;
            cl_event ndrEvt;
            size_t size = width * height * sizeof(cl_float4);
            for(int k = 0; k < steps / 2; k++)
            {
                // Generate data for input for kernel 1
                for(int j = 0; j < (width * height * 4); j++)
                    randNum[j] = (cl_uint)rand();

                // Fill data of input buffer for kernel 1
                status = clEnqueueWriteBuffer(
                                commandQueue,
                                randBuf,
                                CL_FALSE,
                                0,
                                size,
                                randNum,
                                0,
                                NULL,
                                &inWriteEvt1);
                CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer(randBuf) failed.");

                status = clFlush(commandQueue);
                CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                // Get data from outut buffers of kernel 2
                if(k != 0)
                {
                    // Wait for kernel 2 to complete
                    status = clWaitForEvents(1, &ndrEvt);
                    CHECK_OPENCL_ERROR(status, "clWaitForEvents(ndrEvt) failed.");
                    
                    status = clReleaseEvent(ndrEvt);
                    CHECK_OPENCL_ERROR(status, "clReleaseEvent(ndrEvt) failed.");

                    status = clEnqueueReadBuffer(
                                    commandQueue,
                                    priceBufAsync,
                                    CL_FALSE,
                                    0,
                                    size * 2,
                                    priceValsAsync,
                                    0,
                                    NULL,
                                    &outReadEvt21);
                    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceBufAsync) failed.");

                    status = clEnqueueReadBuffer(
                                    commandQueue,
                                    priceDerivBufAsync,
                                    CL_FALSE,
                                    0,
                                    size * 2,
                                    priceDerivAsync,
                                    0,
                                    NULL,
                                    &outReadEvt22);
                    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceDerivBufAsync) failed.");

                    status = clFlush(commandQueue);
                    CHECK_OPENCL_ERROR(status, "clFlush() failed.");
                }

                // Set up arguments required for kernel 1
                float c1 = (interest - 0.5f * sigma[k * 2] * sigma[k * 2]) * timeStep;
                float c2 = sigma[k * 2] * sqrt(timeStep);
                float c3 = (interest + 0.5f * sigma[k * 2] * sigma[k * 2]);
                
                const cl_float4 c1F4 = {c1, c1, c1, c1};
                attributes.c1 = c1F4;

                const cl_float4 c2F4 = {c2, c2, c2, c2};
                attributes.c2 = c2F4;

                const cl_float4 c3F4 = {c3, c3, c3, c3};
                attributes.c3 = c3F4;

                const cl_float4 initPriceF4 = {initPrice, initPrice, initPrice, initPrice};
                attributes.initPrice = initPriceF4;

                const cl_float4 strikePriceF4 = {strikePrice, strikePrice, strikePrice, strikePrice};
                attributes.strikePrice = strikePriceF4;

                const cl_float4 sigmaF4 = {sigma[k * 2], sigma[k * 2], sigma[k * 2], sigma[k * 2]};
                attributes.sigma = sigmaF4;

                const cl_float4 timeStepF4 = {timeStep, timeStep, timeStep, timeStep};
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


                // Wait for input of kernel to complete
                status = sampleCommon->waitForEventAndRelease(&inWriteEvt1);
                CHECK_OPENCL_ERROR(status, "clWaitForEventsAndRelease(inWriteEvt1) failed."); 
                
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

                /* Generate data of input for kernel 2
                 Fill data of input buffer for kernel 2*/
                if(k <= steps - 1)
                {
                    // Generate data of input for kernel 2
                    for(int j = 0; j < (width * height * 4); j++)
                        randNum[j] = (cl_uint)rand();

                    // Fill data of input buffer for kernel 2
                    status = clEnqueueWriteBuffer(
                                    commandQueue,
                                    randBufAsync,
                                    CL_FALSE,
                                    0,
                                    size,
                                    randNum,
                                    0,
                                    NULL,
                                    &inWriteEvt2);
                    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer(randBufAsync) failed.");

                    status = clFlush(commandQueue);
                    CHECK_OPENCL_ERROR(status, "clFlush() failed.");
                }

                // Wait for output buffers of kernel 2 to complete

                // Calcualte the results from output of kernel 2
                if(k != 0)
                {
                    // Wait for output buffers of kernel 2 to complete
                    status = sampleCommon->waitForEventAndRelease(&outReadEvt21);
                    CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(outReadEvt21) failed.");

                    status = sampleCommon->waitForEventAndRelease(&outReadEvt22);
                    CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(outReadEvt22) failed.");

                    // Calcualte the results from output of kernel 2
                    for(int i = 0; i < noOfTraj * noOfTraj; i++)
                    {
                        price[k * 2 - 1] += priceValsAsync[i];
                        vega[k * 2 - 1] += priceDerivAsync[i];
                    }

                    price[k * 2 - 1] /= (noOfTraj * noOfTraj);
                    vega[k * 2 - 1] /= (noOfTraj * noOfTraj);

                    price[k * 2 - 1] = exp(-interest * maturity) * price[k * 2 - 1];
                    vega[k * 2 - 1] = exp(-interest * maturity) * vega[k * 2 - 1];
                }

                // Wait for kernel 1 to complete

                status = sampleCommon->waitForEventAndRelease(&ndrEvt);
                CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(ndrEvt) failed.");

                // Get data from output buffers of kernel 1
                status = clEnqueueReadBuffer(commandQueue, 
                                             priceBuf, 
                                             CL_TRUE,
                                             0,
                                             size * 2,
                                             priceVals,
                                             0,
                                             NULL,
                                             &outReadEvt11);
                CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceBuf) failed.");

                status = clEnqueueReadBuffer(commandQueue, 
                                             priceDerivBuf, 
                                             CL_TRUE,
                                             0,
                                             size * 2,
                                             priceDeriv,
                                             0,
                                             NULL,
                                             &outReadEvt12);
                CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceDerivBuf) failed.");

                status = clFlush(commandQueue);
                CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                // Set up arguments required for kernel 2
                float c21 = (interest - 0.5f * sigma[k * 2 + 1] * sigma[k * 2 + 1]) * timeStep;
                float c22 = sigma[k * 2 + 1] * sqrt(timeStep);
                float c23 = (interest + 0.5f * sigma[k * 2 + 1] * sigma[k * 2 + 1]);

                const cl_float4 c1F42 = {c21, c21, c21, c21};
                attributes.c1 = c1F42;

                const cl_float4 c2F42 = {c22, c22, c22, c22};
                attributes.c2 = c2F42;

                const cl_float4 c3F42 = {c23, c23, c23, c23};
                attributes.c3 = c3F42;

                const cl_float4 initPriceF42 = {initPrice, initPrice, initPrice, initPrice};
                attributes.initPrice = initPriceF42;

                const cl_float4 strikePriceF42 = {strikePrice, strikePrice, strikePrice, strikePrice};
                attributes.strikePrice = strikePriceF42;

                const cl_float4 sigmaF42 = {sigma[k * 2 + 1], sigma[k * 2 + 1], sigma[k * 2 + 1], sigma[k * 2 + 1]};
                attributes.sigma = sigmaF42;

                const cl_float4 timeStepF42 = {timeStep, timeStep, timeStep, timeStep};
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
                status = sampleCommon->waitForEventAndRelease(&inWriteEvt2);
                CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(inWriteEvt2) failed.");

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
                status = sampleCommon->waitForEventAndRelease(&outReadEvt11);
                CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(outReadEvt11) failed.");

                status = sampleCommon->waitForEventAndRelease(&outReadEvt12);
                CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(outReadEvt12) failed.");

                for(int i = 0; i < noOfTraj * noOfTraj; i++)
                {
                    price[k * 2] += priceVals[i];
                    vega[k * 2] += priceDeriv[i];
                }

                price[k * 2] /= (noOfTraj * noOfTraj);
                vega[k * 2] /= (noOfTraj * noOfTraj);

                price[k * 2] = exp(-interest * maturity) * price[k * 2];
                vega[k * 2] = exp(-interest * maturity) * vega[k * 2];
            }

            // Wait for kernel 1 to complete
            status = sampleCommon->waitForEventAndRelease(&ndrEvt);
            CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(ndrEvt) failed.");

            // Gather last kernel 2 execution here
            status = clEnqueueReadBuffer(
                            commandQueue,
                            priceBufAsync,
                            CL_FALSE,
                            0,
                            size * 2,
                            priceValsAsync,
                            0,
                            NULL,
                            &outReadEvt21);
            CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceBufAsync) failed.");
            status = clEnqueueReadBuffer(
                            commandQueue,
                            priceDerivBufAsync,
                            CL_FALSE,
                            0,
                            size * 2,
                            priceDerivAsync,
                            0,
                            NULL,
                            &outReadEvt22);
            CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceDerivBufAsync) failed.");

            status = clFlush(commandQueue);
            CHECK_OPENCL_ERROR(status, "clFlush() failed.");

            // Wait for output buffers of kernel 2 to complete

            status = sampleCommon->waitForEventAndRelease(&outReadEvt21);
            CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(outReadEvt21) failed.");

            status = sampleCommon->waitForEventAndRelease(&outReadEvt22);
            CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(outReadEvt22) failed.");

            // Calcualte the results from output of kernel 2
            for(int i = 0; i < noOfTraj * noOfTraj; i++)
            {
                price[steps - 1] += priceValsAsync[i];
                vega[steps - 1] += priceDerivAsync[i];
            }

            price[steps - 1] /= (noOfTraj * noOfTraj);
            vega[steps - 1] /= (noOfTraj * noOfTraj);

            price[steps - 1] = exp(-interest * maturity) * price[steps - 1];
            vega[steps - 1] = exp(-interest * maturity) * vega[steps - 1];
        }
        else
        {

            void* inMapPtr1 = NULL;
            void* inMapPtr2 = NULL;
            void* outMapPtr11 = NULL;
            void* outMapPtr12 = NULL;
            void* outMapPtr21 = NULL;
            void* outMapPtr22 = NULL;
            cl_float* ptr21 = NULL;
            cl_float* ptr22 = NULL;

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
            
            size_t size = width * height * sizeof(cl_float4);
            for(int k = 0; k < steps / 2; k++)
            {
                // Map input buffer for kernel 1
                inMapPtr1 = clEnqueueMapBuffer(
                                commandQueue,
                                randBuf,
                                CL_FALSE,
                                CL_MAP_WRITE,
                                0,
                                size,
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

                // Wait for map of input of kernel 1
                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    inMapEvt1, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }

                status = clReleaseEvent(inMapEvt1);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(inMapEvt1) failed.");

                memcpy(inMapPtr1, (void*)randNum, size);

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


                // Get data from outut buffers of kernel 2
                if(k != 0)
                {
                    // Wait for kernel 2 to complete
                    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
                    CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(ndrEvt) failed.");
                    
                    outMapPtr21 = clEnqueueMapBuffer(
                                        commandQueue,
                                        priceBufAsync,
                                        CL_FALSE,
                                        CL_MAP_READ,
                                        0,
                                        size * 2,
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
                                        size * 2,
                                        0,
                                        NULL,
                                        &outMapEvt22,
                                        &status);
                    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceDerivBufAsync) failed.");

                    status = clFlush(commandQueue);
                    CHECK_OPENCL_ERROR(status, "clFlush() failed.");
                }

                // Set up arguments required for kernel 1
                float c1 = (interest - 0.5f * sigma[k * 2] * sigma[k * 2]) * timeStep;
                float c2 = sigma[k * 2] * sqrt(timeStep);
                float c3 = (interest + 0.5f * sigma[k * 2] * sigma[k * 2]);
                
                const cl_float4 c1F4 = {c1, c1, c1, c1};
                attributes.c1 = c1F4;

                const cl_float4 c2F4 = {c2, c2, c2, c2};
                attributes.c2 = c2F4;

                const cl_float4 c3F4 = {c3, c3, c3, c3};
                attributes.c3 = c3F4;

                const cl_float4 initPriceF4 = {initPrice, initPrice, initPrice, initPrice};
                attributes.initPrice = initPriceF4;

                const cl_float4 strikePriceF4 = {strikePrice, strikePrice, strikePrice, strikePrice};
                attributes.strikePrice = strikePriceF4;

                const cl_float4 sigmaF4 = {sigma[k * 2], sigma[k * 2], sigma[k * 2], sigma[k * 2]};
                attributes.sigma = sigmaF4;

                const cl_float4 timeStepF4 = {timeStep, timeStep, timeStep, timeStep};
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
                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    inUnmapEvt1, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                
                status = clReleaseEvent(inUnmapEvt1);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(inUnmapEvt1) failed.");
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
                                    size,
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
                    eventStatus = CL_QUEUED;
                    while(eventStatus != CL_COMPLETE)
                    {
                        status = clGetEventInfo(
                                        inMapEvt2, 
                                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                        sizeof(cl_int),
                                        &eventStatus,
                                        NULL);
                        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                    }
                    
                    status = clReleaseEvent(inMapEvt2);
                    CHECK_OPENCL_ERROR(status, "clReleaseEvent(inMapEvt2) failed.");

                    memcpy(inMapPtr2, (void*)randNum, size);

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
                    eventStatus = CL_QUEUED;
                    while(eventStatus != CL_COMPLETE)
                    {
                        status = clGetEventInfo(
                                        outMapEvt21, 
                                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                        sizeof(cl_int),
                                        &eventStatus,
                                        NULL);
                        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                    }
                  
                    status = clReleaseEvent(outMapEvt21);
                    CHECK_OPENCL_ERROR(status, "clReleaseEvent(outMapEvt21) failed.");

                    eventStatus = CL_QUEUED;
                    while(eventStatus != CL_COMPLETE)
                    {
                        status = clGetEventInfo(
                                        outMapEvt22, 
                                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                        sizeof(cl_int),
                                        &eventStatus,
                                        NULL);
                        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                    }
                  
                    status = clReleaseEvent(outMapEvt22);
                    CHECK_OPENCL_ERROR(status, "clReleaseEvent(outMapEvt22) failed.");


                    // Calcualte the results from output of kernel 2
                    ptr21 = (cl_float*)outMapPtr21;
                    ptr22 = (cl_float*)outMapPtr22;
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

                    eventStatus = CL_QUEUED;
                    while(eventStatus != CL_COMPLETE)
                    {
                        status = clGetEventInfo(
                                        outUnmapEvt21, 
                                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                        sizeof(cl_int),
                                        &eventStatus,
                                        NULL);
                        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                    }
                  
                    status = clReleaseEvent(outUnmapEvt21);
                    CHECK_OPENCL_ERROR(status, "clReleaseEvent(outUnmapEvt21) failed.");

                    eventStatus = CL_QUEUED;
                    while(eventStatus != CL_COMPLETE)
                    {
                        status = clGetEventInfo(
                                        outUnmapEvt22, 
                                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                        sizeof(cl_int),
                                        &eventStatus,
                                        NULL);
                        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                    }
                  
                    status = clReleaseEvent(outUnmapEvt22);
                    CHECK_OPENCL_ERROR(status, "clReleaseEvent(outUnmapEvt22) failed.");

                }

                // Wait for kernel 1 to complete
                status = sampleCommon->waitForEventAndRelease(&ndrEvt);
                CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(ndrEvt) failed.");

               
                // Get data from output buffers of kernel 1
                outMapPtr11 = clEnqueueMapBuffer(
                                    commandQueue,
                                    priceBuf,
                                    CL_FALSE,
                                    CL_MAP_READ,
                                    0,
                                    size * 2,
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
                                    size * 2,
                                    0,
                                    NULL,
                                    &outMapEvt12,
                                    &status);
                CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceDerivBuf) failed.");
         
                status = clFlush(commandQueue);
                CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                // Set up arguments required for kernel 2
                float c21 = (interest - 0.5f * sigma[k * 2 + 1] * sigma[k * 2 + 1]) * timeStep;
                float c22 = sigma[k * 2 + 1] * sqrt(timeStep);
                float c23 = (interest + 0.5f * sigma[k * 2 + 1] * sigma[k * 2 + 1]);
                
                const cl_float4 c1F42 = {c21, c21, c21, c21};
                attributes.c1 = c1F42;

                const cl_float4 c2F42 = {c22, c22, c22, c22};
                attributes.c2 = c2F42;

                const cl_float4 c3F42 = {c23, c23, c23, c23};
                attributes.c3 = c3F42;

                const cl_float4 initPriceF42 = {initPrice, initPrice, initPrice, initPrice};
                attributes.initPrice = initPriceF42;

                const cl_float4 strikePriceF42 = {strikePrice, strikePrice, strikePrice, strikePrice};
                attributes.strikePrice = strikePriceF42;

                const cl_float4 sigmaF42 = {sigma[k * 2 + 1], sigma[k * 2 + 1], sigma[k * 2 + 1], sigma[k * 2 + 1]};
                attributes.sigma = sigmaF42;

                const cl_float4 timeStepF42 = {timeStep, timeStep, timeStep, timeStep};
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
                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    inUnmapEvt2, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                
                status = clReleaseEvent(inUnmapEvt2);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(inUnmapEvt2) failed.");
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
                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    outMapEvt11, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                
                status = clReleaseEvent(outMapEvt11);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(outMapEvt11) failed.");

                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    outMapEvt12, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                
                status = clReleaseEvent(outMapEvt12);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(outMapEvt12) failed.");

                // Calcualte the results from output of kernel 2
                ptr21 = (cl_float*)outMapPtr11;
                ptr22 = (cl_float*)outMapPtr12;
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

                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    outUnmapEvt11, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                
                status = clReleaseEvent(outUnmapEvt11);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(outUnmapEvt11) failed.");

                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    outUnmapEvt12, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                
                status = clReleaseEvent(outUnmapEvt12);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(outUnmapEvt12) failed.");
            }

            // Wait for kernel 1 to complete
            status = clWaitForEvents(1, &ndrEvt);
            CHECK_OPENCL_ERROR(status, "clWaitForEvents(ndrEvt) failed.");
            
            status = clReleaseEvent(ndrEvt);
            CHECK_OPENCL_ERROR(status, "clReleaseEvent(ndrEvt) failed.");

            // Gather last kernel 2 execution here
            outMapPtr21 = clEnqueueMapBuffer(
                                commandQueue,
                                priceBufAsync,
                                CL_FALSE,
                                CL_MAP_READ,
                                0,
                                size * 2,
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
                                size * 2,
                                0,
                                NULL,
                                &outMapEvt22,
                                &status);
            CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceDerivBuf) failed.");

            status = clFlush(commandQueue);
            CHECK_OPENCL_ERROR(status, "clFlush() failed.");
            eventStatus = CL_QUEUED;
            while(eventStatus != CL_COMPLETE)
            {
                status = clGetEventInfo(
                                outMapEvt21, 
                                CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                sizeof(cl_int),
                                &eventStatus,
                                NULL);
                CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
            }
            
            status = clReleaseEvent(outMapEvt21);
            CHECK_OPENCL_ERROR(status, "clReleaseEvent(outMapEvt21) failed.");

            eventStatus = CL_QUEUED;
            while(eventStatus != CL_COMPLETE)
            {
                status = clGetEventInfo(
                                outMapEvt22, 
                                CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                sizeof(cl_int),
                                &eventStatus,
                                NULL);
                CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
            }
            
            status = clReleaseEvent(outMapEvt22);
            CHECK_OPENCL_ERROR(status, "clReleaseEvent(outMapEvt22) failed.");

            // Calcualte the results from output of kernel 2
            ptr21 = (cl_float*)outMapPtr21;
            ptr22 = (cl_float*)outMapPtr22;
            for(int i = 0; i < noOfTraj * noOfTraj; i++)
            {
                price[steps - 1] += ptr21[i];
                vega[steps - 1] += ptr22[i];
            }

            price[steps - 1] /= (noOfTraj * noOfTraj);
            vega[steps - 1] /= (noOfTraj * noOfTraj);

            price[steps - 1] = exp(-interest * maturity) * price[steps - 1];
            vega[steps - 1] = exp(-interest * maturity) * vega[steps - 1];
        }
    }
    else
    {
        cl_event events[1];
        for(int k = 0; k < steps; k++)
        {
            for(int j = 0; j < (width * height * 4); j++)
                randNum[j] = (cl_uint)rand();

            if(!dUseInPersistent)
            {
                if(disableMapping)
                {
                    status = clEnqueueWriteBuffer(
                                    commandQueue,
                                    randBuf,
                                    CL_TRUE,
                                    0,
                                    width * height * sizeof(cl_float4),
                                    randNum,
                                    0,
                                    NULL,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer(randBuf) failed.");
                }
                else
                {
                    cl_event inEvent;
                    cl_event inUnEvent;
                    void* mapPtr = clEnqueueMapBuffer(
                                        commandQueue,
                                        randBuf,
                                        CL_FALSE,
                                        CL_MAP_WRITE,
                                        0,
                                        width * height * sizeof(cl_float4),
                                        0,
                                        NULL,
                                        &inEvent,
                                        &status);
                    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(randBuf) failed.");

                    status = clFlush(commandQueue);
                    CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                    cl_int eventStatus = CL_QUEUED;
                    while(eventStatus != CL_COMPLETE)
                    {
                        status = clGetEventInfo(
                                        inEvent, 
                                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                        sizeof(cl_int),
                                        &eventStatus,
                                        NULL);
                        CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                    }

                    memcpy(mapPtr, (void*)randNum, width * height * sizeof(cl_float4));

                    status = clEnqueueUnmapMemObject(
                                    commandQueue,
                                    randBuf,
                                    mapPtr,
                                    0,
                                    NULL,
                                    &inUnEvent);
                    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(randBuf) failed.");

                    status = clReleaseEvent(inEvent);
                    CHECK_OPENCL_ERROR(status, "clReleaseEvent(inEvent) failed.");

					status = clFlush(commandQueue);
                    CHECK_OPENCL_ERROR(status, "clFlush() failed.");
                    
                    status = sampleCommon->waitForEventAndRelease(&inUnEvent);
                    CHECK_OPENCL_ERROR(status, "clWaitForEventAndRelease(inUnEvent) failed.");

					 
                }
            }

            float c1 = (interest - 0.5f * sigma[k] * sigma[k]) * timeStep;
            float c2 = sigma[k] * sqrt(timeStep);
            float c3 = (interest + 0.5f * sigma[k] * sigma[k]);
            
            const cl_float4 c1F4 = {c1, c1, c1, c1};
            attributes.c1 = c1F4;

            const cl_float4 c2F4 = {c2, c2, c2, c2};
            attributes.c2 = c2F4;

            const cl_float4 c3F4 = {c3, c3, c3, c3};
            attributes.c3 = c3F4;

            const cl_float4 initPriceF4 = {initPrice, initPrice, initPrice, initPrice};
            attributes.initPrice = initPriceF4;

            const cl_float4 strikePriceF4 = {strikePrice, strikePrice, strikePrice, strikePrice};
            attributes.strikePrice = strikePriceF4;

            const cl_float4 sigmaF4 = {sigma[k], sigma[k], sigma[k], sigma[k]};
            attributes.sigma = sigmaF4;

            const cl_float4 timeStepF4 = {timeStep, timeStep, timeStep, timeStep};
            attributes.timeStep = timeStepF4;

            // Set appropriate arguments to the kernel 
            status = clSetKernelArg(kernel, 0, sizeof(attributes), (void*)&attributes);
            CHECK_OPENCL_ERROR(status, "clSetKernelArg(attributes) failed.");


			// Enqueue a kernel run call.
            status = clEnqueueNDRangeKernel(commandQueue,
                                            kernel,
                                            2,
                                            NULL,
                                            globalThreads,
                                            localThreads,
                                            0,
                                            NULL,
                                            &events[0]);
            
            CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel() failed.");

			status = clFlush(commandQueue);
            CHECK_OPENCL_ERROR(status, "clFlush() failed.");

            // wait for the kernel call to finish execution
            status = sampleCommon->waitForEventAndRelease(&events[0]);
            CHECK_ERROR(status,0, "WaitForEventAndRelease(events[0]) Failed");  

            if(!disableMapping)
            {
                cl_event inEvent;
                cl_event inUnEvent;
                void* mapPtr = clEnqueueMapBuffer(
                                commandQueue,
                                priceBuf,
                                CL_FALSE,
                                CL_MAP_READ,
                                0,
                                width * height * 2 * sizeof(cl_float4),
                                0,
                                NULL,
                                &inEvent,
                                &status);
                CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(randBuf) failed.");

                status = clFlush(commandQueue);
                CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    inEvent, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                memcpy((void*)priceVals, mapPtr, width * height * 2 * sizeof(cl_float4));
                status = clEnqueueUnmapMemObject(
                                commandQueue,
                                priceBuf,
                                mapPtr,
                                0,
                                NULL,
                                &inUnEvent);
                CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(priceBuf) failed.");

                status = clReleaseEvent(inEvent);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(inEvent) failed.");

				status = clFlush(commandQueue);
                CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                status = sampleCommon->waitForEventAndRelease(&inUnEvent);
                CHECK_ERROR(status,0, "WaitForEventAndRelease(inUnEvent) Failed");  

                mapPtr = clEnqueueMapBuffer(
                                commandQueue,
                                priceDerivBuf,
                                CL_FALSE,
                                CL_MAP_READ,
                                0,
                                width * height * 2 * sizeof(cl_float4),
                                0,
                                NULL,
                                &inEvent,
                                &status);
                CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(priceDerivBuf) failed.");

                status = clFlush(commandQueue);
                CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                eventStatus = CL_QUEUED;
                while(eventStatus != CL_COMPLETE)
                {
                    status = clGetEventInfo(
                                    inEvent, 
                                    CL_EVENT_COMMAND_EXECUTION_STATUS, 
                                    sizeof(cl_int),
                                    &eventStatus,
                                    NULL);
                    CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
                }
                memcpy((void*)priceDeriv, mapPtr, width * height * 2 * sizeof(cl_float4));
                status = clEnqueueUnmapMemObject(
                                commandQueue,
                                priceDerivBuf,
                                mapPtr,
                                0,
                                NULL,
                                &inUnEvent);
                CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(priceDerivBuf) failed.");

                status = clReleaseEvent(inEvent);
                CHECK_OPENCL_ERROR(status, "clReleaseEvent(inEvent) failed.");

				status = clFlush(commandQueue);
                CHECK_OPENCL_ERROR(status, "clFlush() failed.");

                status = sampleCommon->waitForEventAndRelease(&inUnEvent);
                CHECK_OPENCL_ERROR(status, "WaitForEventAndRelease(inUnEvent) Failed"); 
            }
            else
            {
                // Enqueue the results to application pointer
                status = clEnqueueReadBuffer(commandQueue, 
                                             priceBuf, 
                                             CL_TRUE,
                                             0,
                                             width * height * 2 * sizeof(cl_float4),
                                             priceVals,
                                             0,
                                             NULL,
                                             &events[0]);
                CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceBuf) failed.");
           
                // wait for the read buffer to finish execution
                status = sampleCommon->waitForEventAndRelease(&events[0]);
                CHECK_OPENCL_ERROR(status, "clWaitForEventsAndRelease(events[0]) failed."); 

                // Enqueue the results to application pointer
                status = clEnqueueReadBuffer(commandQueue, 
                                             priceDerivBuf, 
                                             CL_TRUE,
                                             0,
                                             width * height * 2 * sizeof(cl_float4),
                                             priceDeriv,
                                             0,
                                             NULL,
                                             &events[0]);
                CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(priceDerivBuf) failed.");

                // wait for the read buffer to finish execution
                status = sampleCommon->waitForEventAndRelease(&events[0]);
                CHECK_OPENCL_ERROR(status, "clWaitForEventsAndRelease(events[0]) failed."); 

            }        
            // Replace Following "for" loop with reduction kernel

            for(int i = 0; i < noOfTraj * noOfTraj; i++)
            {
                price[k] += priceVals[i];
                vega[k] += priceDeriv[i];
            }

            price[k] /= (noOfTraj * noOfTraj);
            vega[k] /= (noOfTraj * noOfTraj);

            price[k] = exp(-interest * maturity) * price[k];
            vega[k] = exp(-interest * maturity) * vega[k];
        }
    }

    return SDK_SUCCESS;
}

int
MonteCarloAsian::initialize()
{
    // Call base class Initialize to get default configuration 
    CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL resource initilization failed");
    const int optionsCount = 5;
    streamsdk::Option *optionList = new streamsdk::Option[optionsCount];
    CHECK_ALLOCATION(optionList, " Allocate memory failed (optionList)\n");

    optionList[0]._sVersion = "c";
    optionList[0]._lVersion = "steps";
    optionList[0]._description = "Steps of Monte carlo simuation";
    optionList[0]._type = streamsdk::CA_ARG_INT;
    optionList[0]._value = &steps;

    optionList[1]._sVersion = "P";
    optionList[1]._lVersion = "initPrice";
    optionList[1]._description = "Initial price(Default value 50)";
    optionList[1]._type = streamsdk::CA_ARG_FLOAT;//STRING;
    optionList[1]._value = &initPrice;

    optionList[2]._sVersion = "s";
    optionList[2]._lVersion = "strikePrice";
    optionList[2]._description = "Strike price (Default value 55)";
    optionList[2]._type = streamsdk::CA_ARG_FLOAT;//STRING;
    optionList[2]._value = &strikePrice;

    optionList[3]._sVersion = "r";
    optionList[3]._lVersion = "interest";
    optionList[3]._description = "interest rate (Default value 0.06)";
    optionList[3]._type = streamsdk::CA_ARG_FLOAT;//STRING;
    optionList[3]._value = &interest;

    optionList[4]._sVersion = "m";
    optionList[4]._lVersion = "maturity";
    optionList[4]._description = "Maturity (Default value 1)";
    optionList[4]._type = streamsdk::CA_ARG_FLOAT;//STRING;
    optionList[4]._value = &maturity;

    for(cl_int i = 0; i < optionsCount; ++i)
        sampleArgs->AddOption(&optionList[i]);

    delete[] optionList;

    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Failed to allocate memory (iteration_option)\n");

    iteration_option->_sVersion = "i";
    iteration_option->_lVersion = "iterations";
    iteration_option->_description = "Number of iterations to execute kernel";
    iteration_option->_type = streamsdk::CA_ARG_INT;
    iteration_option->_value = &iterations;

    sampleArgs->AddOption(iteration_option);

    delete iteration_option;
    
    streamsdk::Option* inPersistent_option = new streamsdk::Option;
    CHECK_ALLOCATION(inPersistent_option, "Failed to allocate memory (inPersistent_option)\n");

    inPersistent_option->_sVersion = "";
    inPersistent_option->_lVersion = "dInPersistent";
    inPersistent_option->_description = "Disables the Persistent memory for input buffers";
    inPersistent_option->_type = streamsdk::CA_NO_ARGUMENT;
    inPersistent_option->_value = &dUseInPersistent;

    sampleArgs->AddOption(inPersistent_option);

    delete inPersistent_option;

    streamsdk::Option* outAllocHostPtr_option = new streamsdk::Option;
    CHECK_ALLOCATION(outAllocHostPtr_option, "Failed to allocate memory (outAllocHostPtr_option)\n");

    outAllocHostPtr_option->_sVersion = "";
    outAllocHostPtr_option->_lVersion = "dOutAllocHostPtr";
    outAllocHostPtr_option->_description = "Disables the Alloc host ptr for output buffers";
    outAllocHostPtr_option->_type = streamsdk::CA_NO_ARGUMENT;
    outAllocHostPtr_option->_value = &dUseOutAllocHostPtr;

    sampleArgs->AddOption(outAllocHostPtr_option);

    delete outAllocHostPtr_option;

    streamsdk::Option* disableMapping_option = new streamsdk::Option;
    CHECK_ALLOCATION(disableMapping_option, "Failed to allocate memory (disableMapping_option)\n");

    disableMapping_option->_sVersion = "";
    disableMapping_option->_lVersion = "dMapping";
    disableMapping_option->_description = "Disables mapping/unmapping and uses read/write buffers.";
    disableMapping_option->_type = streamsdk::CA_NO_ARGUMENT;
    disableMapping_option->_value = &disableMapping;

    sampleArgs->AddOption(disableMapping_option);

    delete disableMapping_option;

    streamsdk::Option* disableAsync_option = new streamsdk::Option;
    CHECK_ALLOCATION(disableAsync_option, "Failed to allocate memory (disableAsync_option)\n");

    disableAsync_option->_sVersion = "";
    disableAsync_option->_lVersion = "dAsync";
    disableAsync_option->_description = "Disables Asynchronous.";
    disableAsync_option->_type = streamsdk::CA_NO_ARGUMENT;
    disableAsync_option->_value = &disableAsync;

    sampleArgs->AddOption(disableAsync_option);

    delete disableAsync_option;

    return SDK_SUCCESS;
}

int MonteCarloAsian::setup()
{
    if(setupMonteCarloAsian() != SDK_SUCCESS)
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


int MonteCarloAsian::run()
{
    // Warmup
    for(int i = 0; i < 2 && iterations != 1; i++)
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
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    // Compute average kernel time
    kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
    {
        sampleCommon->printArray<cl_float>("price", price, steps, 1);
        sampleCommon->printArray<cl_float>("vega", vega, steps, 1);
    }

    return SDK_SUCCESS;
}

void
MonteCarloAsian::printStats()
{
    std::string strArray[4] = 
    {
        "Steps", 
        "Time(sec)", 
        "[Transfer+Kernel](sec)", 
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
MonteCarloAsian::lshift128(unsigned int* input,
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
MonteCarloAsian::rshift128(unsigned int* input, 
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
MonteCarloAsian::generateRand(unsigned int* seed,
                              float *gaussianRand1,
                              float *gaussianRand2,
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
    const float one = 1.0f;
    const float intMax = 4294967296.0f;
    const float PI = 3.14159265358979f;
    const float two = 2.0f;

    float r[4] = {0.0f, 0.0f, 0.0f, 0.0f}; 
    float phi[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    float temp1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float temp2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
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
MonteCarloAsian::calOutputs(float strikePrice, 
                            float* meanDeriv1, 
                            float*  meanDeriv2, 
                            float* meanPrice1,
                            float* meanPrice2, 
                            float* pathDeriv1, 
                            float* pathDeriv2, 
                            float* priceVec1, 
                            float* priceVec2)
{
    float temp1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float temp2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float temp3[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float temp4[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    
    float tempDiff1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float tempDiff2[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for(int c = 0; c < 4; ++c)
    {
        tempDiff1[c] = meanPrice1[c] - strikePrice;
        tempDiff2[c] = meanPrice2[c] - strikePrice;
    }
    if(tempDiff1[0] > 0.0f)
    {
        temp1[0] = 1.0f;
        temp3[0] = tempDiff1[0];
    }
    if(tempDiff1[1] > 0.0f)
    {
        temp1[1] = 1.0f;
        temp3[1] = tempDiff1[1];
    }
    if(tempDiff1[2] > 0.0f)
    {
        temp1[2] = 1.0f;
        temp3[2] = tempDiff1[2];
    }
    if(tempDiff1[3] > 0.0f)
    {
        temp1[3] = 1.0f;
        temp3[3] = tempDiff1[3];
    }

    if(tempDiff2[0] > 0.0f)
    {
        temp2[0] = 1.0f;
        temp4[0] = tempDiff2[0];
    }
    if(tempDiff2[1] > 0.0f)
    {
        temp2[1] = 1.0f;
        temp4[1] = tempDiff2[1];
    }
    if(tempDiff2[2] > 0.0f)
    {
        temp2[2] = 1.0f;
        temp4[2] = tempDiff2[2];
    }
    if(tempDiff2[3] > 0.0f)
    {
        temp2[3] = 1.0f;
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

void MonteCarloAsian::cpuReferenceImpl()
{
    float timeStep = maturity / (noOfSum - 1);
    // Initialize random number generator
    srand(1);

    for(int k = 0; k < steps; k++)
    {
        float c1 = (interest - 0.5f * sigma[k] * sigma[k]) * timeStep;
        float c2 = sigma[k] * sqrt(timeStep);
        float c3 = (interest + 0.5f * sigma[k] * sigma[k]);  

        for(int j = 0; j < (width * height); j++)
        {
            unsigned int nextRand[4] = {0u, 0u, 0u, 0u};
            for(int c = 0; c < 4; ++c)
                nextRand[c] = (cl_uint)rand();

            float trajPrice1[4] = {initPrice, initPrice, initPrice, initPrice};
            float sumPrice1[4] = {initPrice, initPrice, initPrice, initPrice};
            float sumDeriv1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float meanPrice1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float meanDeriv1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float price1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float pathDeriv1[4] = {0.0f, 0.0f, 0.0f, 0.0f};

            float trajPrice2[4] = {initPrice, initPrice, initPrice, initPrice};
            float sumPrice2[4] = {initPrice, initPrice, initPrice, initPrice};
            float sumDeriv2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float meanPrice2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float meanDeriv2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float price2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float pathDeriv2[4] = {0.0f, 0.0f, 0.0f, 0.0f};

            //Run the Monte Carlo simulation a total of Num_Sum - 1 times
            for(int i = 1; i < noOfSum; i++)
            {
                unsigned int tempRand[4] =  {0u, 0u, 0u, 0u};
                for(int c = 0; c < 4; ++c)
                    tempRand[c] = nextRand[c];

                float gaussian1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                float gaussian2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                generateRand(tempRand, gaussian1, gaussian2, nextRand);

                //Calculate the trajectory price and sum price for all trajectories
                for(int c = 0; c < 4; ++c)
                {
                    trajPrice1[c] = trajPrice1[c] * exp(c1 + c2 * gaussian1[c]);
                    trajPrice2[c] = trajPrice2[c] * exp(c1 + c2 * gaussian2[c]);

                    sumPrice1[c] = sumPrice1[c] + trajPrice1[c];
                    sumPrice2[c] = sumPrice2[c] + trajPrice2[c];

                    float temp = c3 * timeStep * i;

                    // Calculate the derivative price for all trajectories
                    sumDeriv1[c] = sumDeriv1[c] + trajPrice1[c] 
                                * ((log(trajPrice1[c] / initPrice) - temp) / sigma[k]);
    
                    sumDeriv2[c] = sumDeriv2[c] + trajPrice2[c] 
                                * ((log(trajPrice2[c] / initPrice) - temp) / sigma[k]);			            
                }

        }
    
            //Calculate the average price and “average derivative?of each simulated path
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

int MonteCarloAsian::verifyResults()
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
            if(fabs(price[i] - refPrice[i]) > 0.2f)
            {
                std::cout << "Failed\n";
                return SDK_FAILURE;
            }
            if(fabs(vega[i] - refVega[i]) > 0.2f)
            {
                std::cout << "Failed\n" << std::endl;
                return SDK_FAILURE;
            }
        }
        std::cout << "Passed!\n" << std::endl;
    }

    return SDK_SUCCESS;
}

int MonteCarloAsian::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseMemObject(priceBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject(priceBuf) failed.");

    status = clReleaseMemObject(priceDerivBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject(priceDerivBuf) failed.");

    status = clReleaseMemObject(randBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject(randBuf) failed.");

    if(!disableAsync)
    {
        status = clReleaseMemObject(priceBufAsync);
        CHECK_OPENCL_ERROR(status, "clReleaseMemObject(priceBufAsync) failed.");

        status = clReleaseMemObject(priceDerivBufAsync);
        CHECK_OPENCL_ERROR(status, "clReleaseMemObject(priceDerivBufAsync) failed.");

        status = clReleaseMemObject(randBufAsync);
        CHECK_OPENCL_ERROR(status, "clReleaseMemObject(randBufAsync) failed.");
    }

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel(kernel) failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram(program) failed.");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue(readKernel) failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext(context) failed.");

    // Release program resources (input memory etc.)

        FREE(sigma);
        FREE(price);
        FREE(vega);
        FREE(refPrice);
        FREE(refVega);
        FREE(priceVals);
        FREE(priceDeriv);

    if(!disableAsync && disableMapping)
    {

        FREE(priceValsAsync);
        FREE(priceDerivAsync);

    }
        FREE(devices);
    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    // Create MonteCalroAsian object 
    MonteCarloAsian clMonteCarloAsian(
        "OpenCL Monte Carlo simulation for Asian Option Pricing");

    // Initialization 
    if(clMonteCarloAsian.initialize()!=SDK_SUCCESS)
        return SDK_FAILURE;

    // Parse command line options 
    if(clMonteCarloAsian.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clMonteCarloAsian.isDumpBinaryEnabled())
        return clMonteCarloAsian.genBinaryImage();
    else
    {
        // Setup 
        if(clMonteCarloAsian.setup() != SDK_SUCCESS)
            return SDK_FAILURE;

        // Run 
        if(clMonteCarloAsian.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        //VerifyResults.
        if(clMonteCarloAsian.verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;

        // Cleanup resources created 
        if(clMonteCarloAsian.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;

        // Print performance statistics 
        clMonteCarloAsian.printStats();
    }

    return SDK_SUCCESS;
}
