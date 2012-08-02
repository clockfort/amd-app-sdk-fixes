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


#include "MatrixTranspose.hpp"


// auxiliary functions to read event time
double ReadEventTime(cl_event& event);

int 
MatrixTranspose::setupMatrixTranspose()
{
    cl_uint inputSizeBytes;

    // allocate and init memory used by host 
    inputSizeBytes = width * height * sizeof(cl_float);
    input = (cl_float *) malloc(inputSizeBytes);
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    // random initialisation of input 
    sampleCommon->fillRandom<cl_float>(input, width, height, 0, 255);

    output = (cl_float *) malloc(inputSizeBytes);
    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");

    if(verify)
    {
        verificationOutput = (cl_float *) malloc(inputSizeBytes);
        CHECK_ALLOCATION(verificationOutput, "Failed to allocate host memory. (verificationOutput)");
        }
    
    // Unless quiet mode has been enabled, print the INPUT array.
    if(!quiet) 
    {
        sampleCommon->printArray<cl_float>(
            "Input", 
            input, 
            width, 
            1);
    }

    return SDK_SUCCESS;
}

int 
MatrixTranspose::genBinaryImage()
{
    /*
     * Have a look at the available platforms and pick either
     * the AMD one if available or a reasonable default.
     */

    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("MatrixTranspose_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
    binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
    }

int
MatrixTranspose::setupCL(void)
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

    // Get Device specific Information, Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    {
        // The block is to move the declaration of prop closer to its use 
        cl_command_queue_properties prop = CL_QUEUE_PROFILING_ENABLE;
        commandQueue = clCreateCommandQueue(
                           context, 
                           devices[deviceId], 
                           prop, 
                           &status);
        CHECK_ERROR(status, 0, "clCreateCommandQueue failed.");
        }

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
		// To achieve best performance, use persistent memory together with
		// clEnqueueMapBuffer (instead of clEnqeueRead/Write). 
		// At the same time, in general, the best performance is the function
		// of access pattern and size of the buffer.
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    inputBuffer = clCreateBuffer(
                      context, 
                      inMemFlags,
                      sizeof(cl_float) * width * height,
                      NULL, 
                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer)");

    outputBuffer = clCreateBuffer(
                      context, 
					  CL_MEM_WRITE_ONLY,
                      sizeof(cl_float) * width * height,
                      NULL, 
                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("MatrixTranspose_Kernels.cl");
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
    kernel = clCreateKernel(program, "matrixTranspose", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");		

    status =  kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    availableLocalMemory = deviceInfo.localMemSize - kernelInfo.localMemoryUsed;

	// each work item is going to work on [elemsPerThread1Dim x elemsPerThread1Dim] matrix elements,
	// therefore the total size of needed local memory is calculated as
	// # of WIs in a group multiplied by # of matrix elements per a WI
    neededLocalMemory    = blockSize * blockSize * elemsPerThread1Dim * elemsPerThread1Dim * sizeof(cl_float);

    if(neededLocalMemory > availableLocalMemory)
    {
        std::cout << "Unsupported: Insufficient local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    if((cl_uint)(blockSize * blockSize) > kernelInfo.kernelWorkGroupSize)
    {
        if(kernelInfo.kernelWorkGroupSize >= 64)
            blockSize = 8; 
        else if(kernelInfo.kernelWorkGroupSize >= 32)
            blockSize = 4; 
        else
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << blockSize * blockSize << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelInfo.kernelWorkGroupSize << std::endl;
            return SDK_FAILURE;
        }
    }

    if(blockSize > deviceInfo.maxWorkItemSizes[0] ||
       blockSize > deviceInfo.maxWorkItemSizes[1] ||
       (size_t)blockSize * blockSize > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support requested number of work items." << std::endl;
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}


int 
MatrixTranspose::runCLKernels(void)
{
    cl_int   status;

	// every thread in [blockSize x blockSize] workgroup will execute [elemsPerThread1Dim x elemsPerThread1Dim] elements 
    size_t globalThreads[2]= {width/elemsPerThread1Dim, height/elemsPerThread1Dim};
    size_t localThreads[2] = {blockSize, blockSize};

    cl_int eventStatus = CL_QUEUED;

    cl_event inMapEvt;
    cl_event inUnmapEvt;
    cl_event outMapEvt;
    cl_event outUnmapEvt;
    void* inMapPtr = NULL;
    void* outMapPtr = NULL;

    inMapPtr = clEnqueueMapBuffer(
                        commandQueue, 
                        inputBuffer, 
                        CL_TRUE, 
                        CL_MAP_WRITE, 
                        0, 
                        width * height * sizeof(cl_float), 
                        0, 
                        NULL, 
                        &inMapEvt, 
                        &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inputBuffer)");

    memcpy(inMapPtr, input, sizeof(cl_float) * width * height);

    status = clEnqueueUnmapMemObject(
                commandQueue, 
                inputBuffer, 
                inMapPtr, 
                0, 
                NULL, 
                &inUnmapEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (inputBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&inUnmapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inUnmapEvt) Failed");

    // Set appropriate arguments to the kernel
    
    // 1st kernel argument - output 
    status = clSetKernelArg(
                    kernel, 
                    0, 
                    sizeof(cl_mem), 
                    (void *)&outputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (outputBuffer)");

    // 2nd kernel argument - input 
    status = clSetKernelArg(
                    kernel, 
                    1, 
                    sizeof(cl_mem), 
                    (void *)&inputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputBuffer)");

    // 3rd kernel argument - size of input buffer 
    status = clSetKernelArg(
                    kernel, 
                    2, 
                    (size_t)neededLocalMemory, 
                    NULL);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (block)");

    // Enqueue a kernel run call.
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

    status = clFinish(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFinish failed.");

	// accumulate NDRange time
	totalNDRangeTime += ReadEventTime(ndrEvt);

	status = clReleaseEvent(ndrEvt);
    CHECK_OPENCL_ERROR(status, "clReleaseEvent failed.(endTime)");

    outMapPtr = clEnqueueMapBuffer(
                        commandQueue, 
                        outputBuffer, 
                        CL_FALSE, 
                        CL_MAP_READ, 
                        0, 
                        width * height * sizeof(cl_float), 
                        0, 
                        NULL, 
                        &outMapEvt, 
                        &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (resultBuf)");           

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&outMapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapEvt) Failed");
    memcpy(output, outMapPtr, sizeof(cl_float) * width * height);

    status = clEnqueueUnmapMemObject(
                commandQueue, 
                outputBuffer, 
                outMapPtr, 
                0, 
                NULL, 
                &outUnmapEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (resultBuf)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&outUnmapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outUnmapEvt) Failed");

    return SDK_SUCCESS;
}

/*
 * Naive matrix transpose implementation
 */
void 
MatrixTranspose::matrixTransposeCPUReference(
                            cl_float * output,
                            cl_float * input,
                            const cl_uint width,
                            const cl_uint height)
{
    for(cl_uint j=0; j < height; j++)
    {
        for(cl_uint i=0; i < width; i++)
        {
            output[i*height + j] = input[j*width + i];
        }
    }
}

int 
MatrixTranspose::initialize()
{
    // Call base class Initialize to get default configuration 
    if(this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // add command line option for blockSize 
    streamsdk::Option* xParam = new streamsdk::Option;
    if(!xParam)
    {
        sampleCommon->error("Memory Allocation error.\n");
        return SDK_FAILURE;
    }

    xParam->_sVersion = "x";
    xParam->_lVersion = "width";
    xParam->_description = "width of input matrix";
    xParam->_type     = streamsdk::CA_ARG_INT;
    xParam->_value    = &width;

    sampleArgs->AddOption(xParam);
    delete xParam;

    streamsdk::Option* yParam = new streamsdk::Option;
    if(!yParam)
    {
        sampleCommon->error("Memory Allocation error.\n");
        return SDK_FAILURE;
    }

    yParam->_sVersion = "y";
    yParam->_lVersion = "height";
    yParam->_description = "height of input matrix";
    yParam->_type     = streamsdk::CA_ARG_INT;
    yParam->_value    = &height;

    sampleArgs->AddOption(yParam);
    delete yParam;

    streamsdk::Option* blockSizeParam = new streamsdk::Option;
    if(!blockSizeParam)
    {
        sampleCommon->error("Memory Allocation error.\n");
        return SDK_FAILURE;
    }

    blockSizeParam->_sVersion = "b";
    blockSizeParam->_lVersion = "blockSize";
    blockSizeParam->_description = "Use local memory of dimensions blockSize x blockSize";
    blockSizeParam->_type     = streamsdk::CA_ARG_INT;
    blockSizeParam->_value    = &blockSize;
    sampleArgs->AddOption(blockSizeParam);
    delete blockSizeParam;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    if(!num_iterations)
    {
        sampleCommon->error("Memory allocation error.\n");
        return SDK_FAILURE;
    }

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);
    delete num_iterations;

    return SDK_SUCCESS;
}


int 
MatrixTranspose::setup()
{
    // width should be multiples of blockSize 
    if(width%blockSize !=0)
    {
        width = (width/blockSize + 1)*blockSize;
    }

    height = width;
    int status=setupMatrixTranspose();
    if(status!=SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    if(setupCL() != SDK_SUCCESS)
        return SDK_FAILURE;

    sampleCommon->stopTimer(timer);

    setupTime = (cl_double)sampleCommon->readTimer(timer);

    return SDK_SUCCESS;
}


int 
MatrixTranspose::run()
{
    //Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        //. Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);   

    std::cout << "Executing kernel for " << iterations << 
        " iterations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

	totalNDRangeTime = 0;

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    totalKernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

	totalNDRangeTime /= iterations;

    if(!quiet) {
        sampleCommon->printArray<cl_float>("Output", output, width, 1);
    }

    return SDK_SUCCESS;
}

int 
MatrixTranspose::verifyResults()
{
    if(verify)
    {
        /* 
         * reference implementation
         */
        int refTimer = sampleCommon->createTimer();
        sampleCommon->resetTimer(refTimer);
        sampleCommon->startTimer(refTimer);
        matrixTransposeCPUReference(verificationOutput, input, width, height);
        sampleCommon->stopTimer(refTimer);
        referenceKernelTime = sampleCommon->readTimer(refTimer);

        // compare the results and see if they match 
        if(sampleCommon->compare(output, verificationOutput, width*height))
        {
            std::cout<<"Passed!\n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout<<"Failed verification test\n" << std::endl;
            return SDK_FAILURE;
        }
    }
    return SDK_SUCCESS;
}

void 
MatrixTranspose::printStats()
{
    std::string strArray[4] = {"WxH" , "Time(sec)", "[Transfer+Kernel]Time(sec)", "Kernel Speed(GB/s)"};
    std::string stats[4]; 

    totalTime = setupTime + totalKernelTime;

    stats[0]  = sampleCommon->toString(width, std::dec)
                +"x"+sampleCommon->toString(height, std::dec);
    stats[1]  = sampleCommon->toString(totalTime, std::dec);
    stats[2]  = sampleCommon->toString(totalKernelTime, std::dec);

	double kernelSpeed = height*width*sizeof(float)*2/totalNDRangeTime;
	stats[3]  = sampleCommon->toString(kernelSpeed, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}

int 
MatrixTranspose::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseMemObject(inputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // release program resources (input memory etc.) 
        FREE(input);
        FREE(output);
        FREE(verificationOutput);
        FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    // Create MonteCalroAsian object 
    MatrixTranspose clMatrixTranspose("OpenCL Matrix Transpose");

    // Initialization 
    if(clMatrixTranspose.initialize()!=SDK_SUCCESS)
        return SDK_FAILURE;

    // Parse command line options 
    if(clMatrixTranspose.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clMatrixTranspose.isDumpBinaryEnabled())
    {
        return clMatrixTranspose.genBinaryImage();
    }
    else
    {
        // Setup 
        if(clMatrixTranspose.setup()==SDK_FAILURE)
            return SDK_FAILURE;

        // Run 
        if(clMatrixTranspose.run() == SDK_FAILURE)
          return SDK_FAILURE;

        else
        {
            // Verifty 
            if(clMatrixTranspose.verifyResults()==SDK_FAILURE)
                return SDK_FAILURE;
        }
        // Cleanup resources created 
        if(clMatrixTranspose.cleanup()==SDK_FAILURE)
            return SDK_FAILURE;
        // Print performance statistics 
        clMatrixTranspose.printStats();
    }

    return SDK_SUCCESS;
}

double 
ReadEventTime(cl_event& event)
{
	// Calculate performance 
    cl_ulong startTime;
    cl_ulong endTime;

	cl_int status;

    // Get kernel profiling info 
    status = clGetEventProfilingInfo(event,
                                        CL_PROFILING_COMMAND_START,
                                        sizeof(cl_ulong),
                                        &startTime,
                                        0);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo failed.(startTime)");

    status = clGetEventProfilingInfo(event,
                                        CL_PROFILING_COMMAND_END,
                                        sizeof(cl_ulong),
                                        &endTime,
                                        0);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo failed.(endTime)");

	return (double)(endTime - startTime);
}
