/* ============================================================

Copyright (c) 2012 Advanced Micro Devices, Inc.  All rights reserved.
 
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


#include "MatrixMultiplicationCPPKernel.hpp"

int 
MatrixMultiplication::setupMatrixMultiplicationInt()
{
    // allocate and init memory used by host  input0[width0][height0]
    cl_uint inputSizeBytes0 = width0 * height0 * sizeof(cl_int);

    inputInt0 = (cl_int *) malloc(inputSizeBytes0);
    CHECK_ALLOCATION(inputInt0, "Failed to allocate host memory. (input0)");

    // allocate and init memory used by host input1[width1][height1]
    cl_uint inputSizeBytes1 = width1 * height1 * sizeof(cl_int);

    inputInt1 = (cl_int *) malloc(inputSizeBytes1);
    CHECK_ALLOCATION(inputInt1, "Failed to allocate host memory. (input1)");
    
    // random initialisation of input 
    sampleCommon->fillRandom<cl_int>(inputInt0, width0, height0, 0, 10);
    sampleCommon->fillRandom<cl_int>(inputInt1, width1, height1, 0, 10);

    // allocate memory for output[width1][height0] 
    cl_uint outputSizeBytes = height0 * width1 * sizeof(cl_int);

    outputInt = (cl_int *) malloc(outputSizeBytes);
    CHECK_ALLOCATION(outputInt, "Failed to allocate host memory. (output)");

    // allocate memory for output[width1][height0] of reference implemenation
    if(verify)
    {
        verificationOutputInt = (cl_int *) malloc(outputSizeBytes);
        CHECK_ALLOCATION(verificationOutputInt, "Failed to allocate host memory. (verificationOutput)");
        memset(verificationOutputInt, 0, outputSizeBytes);
    }
    
    // Unless quiet mode has been enabled, print the INPUT arrays
    if(!quiet) 
    {
        sampleCommon->printArray<cl_int>(
            "InputInt0", 
            inputInt0, 
            width0, 
            1);
        sampleCommon->printArray<cl_int>(
            "InputInt1", 
            inputInt1, 
            width1, 
            1);
    }

    return SDK_SUCCESS;
}

int 
MatrixMultiplication::setupMatrixMultiplicationFloat()
{
    // allocate and init memory used by host  input0[width0][height0]
    cl_uint inputSizeBytes0 = width0 * height0 * sizeof(cl_float);

    inputFloat0 = (cl_float *) malloc(inputSizeBytes0);
    CHECK_ALLOCATION(inputFloat0, "Failed to allocate host memory. (input0)");

    // allocate and init memory used by host input1[width1][height1]
    cl_uint inputSizeBytes1 = width1 * height1 * sizeof(cl_float);

    inputFloat1 = (cl_float *) malloc(inputSizeBytes1);
    CHECK_ALLOCATION(inputFloat1, "Failed to allocate host memory. (input1)");
    
    // random initialisation of input 
    sampleCommon->fillRandom<cl_float>(inputFloat0, width0, height0, 0, 10);
    sampleCommon->fillRandom<cl_float>(inputFloat1, width1, height1, 0, 10);

    // allocate memory for output[width1][height0] 
    cl_uint outputSizeBytes = height0 * width1 * sizeof(cl_float);

    outputFloat = (cl_float *) malloc(outputSizeBytes);
    CHECK_ALLOCATION(outputFloat, "Failed to allocate host memory. (output)");

    // allocate memory for output[width1][height0] of reference implemenation
    if(verify)
    {
        verificationOutputFloat = (cl_float *) malloc(outputSizeBytes);
        CHECK_ALLOCATION(verificationOutputFloat, "Failed to allocate host memory. (verificationOutput)");
        memset(verificationOutputFloat, 0, outputSizeBytes);
    }
    
    // Unless quiet mode has been enabled, print the INPUT arrays
    if(!quiet) 
    {
        sampleCommon->printArray<cl_float>(
            "InputFloat0", 
            inputFloat0, 
            width0, 
            1);
        sampleCommon->printArray<cl_float>(
            "InputFloat1", 
            inputFloat1, 
            width1, 
            1);
    }

    return SDK_SUCCESS;
}

/**
 * genBinary Image function is used to when we want to create the binary 
 * for the kernel and run it at a later time. This is useful where offline  
 * compilation is the preferred mechanism. 
 */
int 
MatrixMultiplication::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("MatrixMultiplicationCPPKernel_Kernels.cl");
    binaryData.flagsStr = std::string("-x clc++ ");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}



int
MatrixMultiplication::setupCL(void)
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

    // creating context
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
 
    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    {
        // The block is to move the declaration of prop closer to its use 
        cl_command_queue_properties prop = 0;
        if(!eAppGFLOPS)
            prop |= CL_QUEUE_PROFILING_ENABLE;

        commandQueue = clCreateCommandQueue(
                           context, 
                           devices[deviceId], 
                           prop, 
                           &status);
        CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");
    }

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

	if(!arrayFlag)
	{
		 // Create buffer for matrix A 
		inputBufferInt0 = clCreateBuffer(
                      context, 
                      inMemFlags,
                      sizeof(cl_int) * width0 * height0,
                      0, 
                      &status);
		CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer0)");

		// Create buffer for matrix B 
		inputBufferInt1 = clCreateBuffer(
                      context, 
                      inMemFlags,
                      sizeof(cl_int) * width1 * height1,
                      0, 
                      &status);
		CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer1)");

		outputBufferInt = clCreateBuffer(
                      context, 
                      CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                      sizeof(cl_int) * height0 * width1,
                      0, 
                      &status);
		CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffer)");

		// create a CL program using the kernel source 
		streamsdk::buildProgramData buildData;
		buildData.kernelName = std::string("MatrixMultiplicationCPPKernel_Kernels.cl");
		buildData.devices = devices;
		buildData.deviceId = deviceId;
		buildData.flagsStr = std::string("-x clc++ ");
		if(isLoadBinaryEnabled())
			buildData.binaryName = std::string(loadBinary.c_str());

		if(isComplierFlagsSpecified())
			buildData.flagsFileName = std::string(flags.c_str());

		retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
		CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");
    
		// If local memory is present then use the specific kernel 
		if(lds)
			 kernel = clCreateKernel(program, "mmmKernel_localInt4", &status);
		else
			kernel = clCreateKernel(program, "mmmKernelInt4", &status);
		CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

		return SDK_SUCCESS;
	}
	else
	{
		// Create buffer for matrix A 
		inputBufferFloat0 = clCreateBuffer(
                      context, 
                      inMemFlags,
                      sizeof(cl_float) * width0 * height0,
                      0, 
                      &status);
		CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer0)");

		// Create buffer for matrix B 
		inputBufferFloat1 = clCreateBuffer(
                      context, 
                      inMemFlags,
                      sizeof(cl_float) * width1 * height1,
                      0, 
                      &status);
		CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer1)");

		outputBufferFloat = clCreateBuffer(
                      context, 
                      CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                      sizeof(cl_float) * height0 * width1,
                      0, 
                      &status);
		CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffer)");
		// create a CL program using the kernel source 
		streamsdk::buildProgramData buildData;
		buildData.kernelName = std::string("MatrixMultiplicationCPPKernel_Kernels.cl");
		buildData.devices = devices;
		buildData.deviceId = deviceId;
		buildData.flagsStr = std::string("-x clc++ ");
		if(isLoadBinaryEnabled())
			buildData.binaryName = std::string(loadBinary.c_str());

		if(isComplierFlagsSpecified())
			buildData.flagsFileName = std::string(flags.c_str());

		retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
		CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");
    
		// If local memory is present then use the specific kernel 
		if(lds)
			 kernel = clCreateKernel(program, "mmmKernel_localFloat4", &status);
		else
			kernel = clCreateKernel(program, "mmmKernelFloat4", &status);
		CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

		return SDK_SUCCESS;
	}
}

int
MatrixMultiplication::setWorkGroupSize()
{
	 /* 
	* Kernel runs over complete output matrix with blocks of blockSize x blockSize 
	* running concurrently
	*/
	cl_int status = 0;
	globalThreads[0] = width1 / 4;
	globalThreads[1] = height0/ 4;
	localThreads[0] = blockSize;
	localThreads[1] = blockSize;

	// Setting the KernelWorkGroupInfo values
	status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
	CHECK_ERROR(status,0, "setKernelWrkGroupInfo failed");
    
	availableLocalMemory = deviceInfo.localMemSize - kernelInfo.localMemoryUsed;

	if(!arrayFlag)
	{
		neededLocalMemory    = 2 * blockSize * blockSize * sizeof(cl_int); 
	}
	else
	{
		neededLocalMemory    = 2 * blockSize * blockSize * sizeof(cl_float); 
	}
    
    if(neededLocalMemory > availableLocalMemory)
    {
        std::cout << "Unsupported: Insufficient local memory on device." << std::endl;
        return SDK_SUCCESS;
    }

    if((cl_uint)(localThreads[0] * localThreads[1]) > kernelInfo.kernelWorkGroupSize)
    {
       if(kernelInfo.kernelWorkGroupSize >= 64)
        {
            blockSize = 8; 
            localThreads[0] = blockSize;
            localThreads[1] = blockSize;
        }
        else if(kernelInfo.kernelWorkGroupSize >= 32)
        {
            blockSize = 4; 
            localThreads[0] = blockSize;
            localThreads[1] = blockSize;
        }
        else
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << localThreads[0] * localThreads[1] << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelInfo.kernelWorkGroupSize<<std::endl;
            return SDK_FAILURE;
        }
    }

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] ||
       localThreads[1] > deviceInfo.maxWorkItemSizes[1] ||
       localThreads[0] * localThreads[1] > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support requested number of work items." << std::endl;
        return SDK_FAILURE;
    }
    return SDK_SUCCESS;
}

int 
MatrixMultiplication::runCLKernelsInt(void)
{
		cl_int   status;
		status = setWorkGroupSize();
		CHECK_ERROR(status, SDK_SUCCESS, "getWorkGroupSize() failed");

		cl_event ndrEvt;
		cl_int eventStatus = CL_QUEUED;

		// Set input data to matrix A and matrix B
		cl_event inMapEvt1, inMapEvt2, inUnmapEvt1, inUnmapEvt2, outMapEvt, outUnmapEvt;

		 void* mapPtr1 = clEnqueueMapBuffer(
                        commandQueue, 
                        inputBufferInt0, 
                        CL_FALSE, 
                        CL_MAP_WRITE, 
                        0, 
                        width0 * height0 * sizeof(cl_int), 
                        0, 
                        NULL, 
                        &inMapEvt1, 
                        &status);
		CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inputBuffer0)");

		void* mapPtr2 = clEnqueueMapBuffer(
                        commandQueue, 
                        inputBufferInt1, 
                        CL_FALSE, 
                        CL_MAP_WRITE, 
                        0, 
                        width1 * height1 * sizeof(cl_int), 
                        0, 
                        NULL, 
                        &inMapEvt2, 
                        &status);
		CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inputBuffer1)");    

		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&inMapEvt1);
		CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt1) Failed");
		memcpy(mapPtr1, inputInt0, sizeof(cl_int) * width0  * height0);

		status = sampleCommon->waitForEventAndRelease(&inMapEvt2);
		CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt2) Failed");
		memcpy(mapPtr2, inputInt1, sizeof(cl_int) * width1  * height1);

		status = clEnqueueUnmapMemObject(
                commandQueue, 
                inputBufferInt0, 
                mapPtr1, 
                0, 
                NULL, 
                &inUnmapEvt1);
		CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (inputBuffer0)");

		status = clEnqueueUnmapMemObject(
                commandQueue, 
                inputBufferInt1, 
                mapPtr2, 
                0, 
                NULL, 
                &inUnmapEvt2);
		CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (inputBuffer1)");

		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&inUnmapEvt1);
		CHECK_ERROR(status, SDK_SUCCESS, "waitForEventAndRelease(inUnmapEvt1) failed");

		status = sampleCommon->waitForEventAndRelease(&inUnmapEvt2);
		CHECK_ERROR(status,SDK_SUCCESS, "waitForEventAndRelease(inUnmapEvt2) failed");

		// Set appropriate arguments to the kernel

		// output array as the 1st argument : stores product of input0 and input1 
		status = clSetKernelArg(
                    kernel, 
                    0, 
                    sizeof(cl_mem), 
                    (void *)&inputBufferInt0);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputBuffer0)");

		// the input matrix  as 2nd argument - input0 
		status = clSetKernelArg(
                    kernel, 
                    1, 
                    sizeof(cl_mem), 
                    (void *)&inputBufferInt1);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputBuffer1)");

		// the input matrix as 3rd argument - input1
		status = clSetKernelArg(
                    kernel, 
                    2, 
                    sizeof(cl_mem), 
                    (void *)&outputBufferInt);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (outputBuffer)");

		// width0 of the input0 matrix as 4th argument - width0
		status = clSetKernelArg(
                    kernel, 
                    3, 
                    sizeof(cl_int),
                    (void*)&width0);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width0)");

		// Set local memory argument if Scratchpad is available
		if(lds)
		{
			status = clSetKernelArg(
                        kernel, 
                        4, 
                        (blockSize * 4) * (blockSize * 4) * sizeof(cl_int),
                        NULL);
			CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (local memory)");
		}
		else
		{
			status = clSetKernelArg(kernel, 4, sizeof(cl_int), &width1);
			CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width1)");
		}

		// Enqueue a kernel run call
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

		// wait for the kernel call to finish execution
		eventStatus = CL_QUEUED;
		while(eventStatus != CL_COMPLETE)
		{
			status = clGetEventInfo(
                        ndrEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
			CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
		}

		if(!eAppGFLOPS)
		{
			// Calculate performance
			cl_ulong startTime;
			cl_ulong endTime;
        
			// Get kernel profiling info
			status = clGetEventProfilingInfo(ndrEvt,
                                         CL_PROFILING_COMMAND_START,
                                         sizeof(cl_ulong),
                                         &startTime,
                                         0);
			CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo failed.(startTime)");

			status = clGetEventProfilingInfo(ndrEvt,
                                         CL_PROFILING_COMMAND_END,
                                         sizeof(cl_ulong),
                                         &endTime,
                                         0);
			CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo failed.(endTime)");

			// Print performance numbers
			double sec = 1e-9 * (endTime - startTime);
			kernelTime += sec;
		}

		status = clReleaseEvent(ndrEvt);
		CHECK_OPENCL_ERROR(status, "clReleaseEvent failed. (ndrEvt)");

		void* outMapPtrInt = clEnqueueMapBuffer(
                        commandQueue, 
                        outputBufferInt, 
                        CL_FALSE, 
                        CL_MAP_READ, 
                        0, 
                        width1 * height0 * sizeof(cl_int), 
                        0, 
                        NULL, 
                        &outMapEvt, 
                        &status);
		CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (outputBuffer)");

		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&outMapEvt);
		CHECK_ERROR(status,0, "waitForEventAndRelease(outMapEvt) failed");

		memcpy(outputInt, outMapPtrInt, sizeof(cl_int) * width1  * height0);

		status = clEnqueueUnmapMemObject(
                commandQueue, 
                outputBufferInt, 
                outMapPtrInt, 
                0, 
                NULL, 
                &outUnmapEvt);
		CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (outputBuffer)");

		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&outUnmapEvt);
		CHECK_ERROR(status,0, "waitForEventAndRelease(outUnmapEvt) failed");

		return SDK_SUCCESS;
}

int 
MatrixMultiplication::runCLKernelsFloat(void)
{
		cl_int   status;
		status = setWorkGroupSize();
		CHECK_ERROR(status, SDK_SUCCESS, "getWorkGroupSize() failed");

		cl_event ndrEvt;
		cl_int eventStatus = CL_QUEUED;

		// Set input data to matrix A and matrix B
		cl_event inMapEvt1, inMapEvt2, inUnmapEvt1, inUnmapEvt2, outMapEvt, outUnmapEvt;

		void* mapPtr1 = clEnqueueMapBuffer(
                        commandQueue, 
                        inputBufferFloat0, 
                        CL_FALSE, 
                        CL_MAP_WRITE, 
                        0, 
                        width0 * height0 * sizeof(cl_float), 
                        0, 
                        NULL, 
                        &inMapEvt1, 
                        &status);
		CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inputBuffer0)");

		void* mapPtr2 = clEnqueueMapBuffer(
                        commandQueue, 
                        inputBufferFloat1, 
                        CL_FALSE, 
                        CL_MAP_WRITE, 
                        0, 
                        width1 * height1 * sizeof(cl_float), 
                        0, 
                        NULL, 
                        &inMapEvt2, 
                        &status);
		CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inputBuffer1)");    

		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&inMapEvt1);
		CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt1) Failed");
		memcpy(mapPtr1, inputFloat0, sizeof(cl_float) * width0  * height0);

		status = sampleCommon->waitForEventAndRelease(&inMapEvt2);
		CHECK_ERROR(status,SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt2) Failed");
		memcpy(mapPtr2, inputFloat1, sizeof(cl_float) * width1  * height1);

		status = clEnqueueUnmapMemObject(
                commandQueue, 
                inputBufferFloat0, 
                mapPtr1, 
                0, 
                NULL, 
                &inUnmapEvt1);
		CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (inputBuffer0)");

		status = clEnqueueUnmapMemObject(
                commandQueue, 
                inputBufferFloat1, 
                mapPtr2, 
                0, 
                NULL, 
                &inUnmapEvt2);
		CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (inputBuffer1)");

		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&inUnmapEvt1);
		CHECK_ERROR(status, SDK_SUCCESS, "waitForEventAndRelease(inUnmapEvt1) failed");

		status = sampleCommon->waitForEventAndRelease(&inUnmapEvt2);
		CHECK_ERROR(status,SDK_SUCCESS, "waitForEventAndRelease(inUnmapEvt2) failed");

		// Set appropriate arguments to the kernel

		// output array as the 1st argument : stores product of input0 and input1 
		status = clSetKernelArg(
                    kernel, 
                    0, 
                    sizeof(cl_mem), 
                    (void *)&inputBufferFloat0);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputBuffer0)");

		// the input matrix  as 2nd argument - input0 
		status = clSetKernelArg(
                    kernel, 
                    1, 
                    sizeof(cl_mem), 
                    (void *)&inputBufferFloat1);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputBuffer1)");

		// the input matrix as 3rd argument - input1
		status = clSetKernelArg(
                    kernel, 
                    2, 
                    sizeof(cl_mem), 
                    (void *)&outputBufferFloat);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (outputBuffer)");

		// width0 of the input0 matrix as 4th argument - width0
		status = clSetKernelArg(
                    kernel, 
                    3, 
                    sizeof(cl_int),
                    (void*)&width0);
		CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width0)");

		// Set local memory argument if Scratchpad is available
		if(lds)
		{
			status = clSetKernelArg(
                        kernel, 
                        4, 
                        (blockSize * 4) * (blockSize * 4) * sizeof(cl_float),
                        NULL);
			CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (local memory)");
		}
		else
		{
			status = clSetKernelArg(kernel, 4, sizeof(cl_int), &width1);
			CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width1)");
		}

		// Enqueue a kernel run call
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

		// wait for the kernel call to finish execution
		eventStatus = CL_QUEUED;
		while(eventStatus != CL_COMPLETE)
		{
			status = clGetEventInfo(
                        ndrEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
			CHECK_OPENCL_ERROR(status, "clGetEventInfo failed.");
		}

		if(!eAppGFLOPS)
		{
			// Calculate performance
			cl_ulong startTime;
			cl_ulong endTime;
        
			// Get kernel profiling info
			status = clGetEventProfilingInfo(ndrEvt,
                                         CL_PROFILING_COMMAND_START,
                                         sizeof(cl_ulong),
                                         &startTime,
                                         0);
			CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo failed.(startTime)");

			status = clGetEventProfilingInfo(ndrEvt,
                                         CL_PROFILING_COMMAND_END,
                                         sizeof(cl_ulong),
                                         &endTime,
                                         0);
			CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo failed.(endTime)");

			// Print performance numbers
			double sec = 1e-9 * (endTime - startTime);
			kernelTime += sec;
		}

		status = clReleaseEvent(ndrEvt);
		CHECK_OPENCL_ERROR(status, "clReleaseEvent failed. (ndrEvt)");

		void* outMapPtrFloat = clEnqueueMapBuffer(
                        commandQueue, 
                        outputBufferFloat, 
                        CL_FALSE, 
                        CL_MAP_READ, 
                        0, 
                        width1 * height0 * sizeof(cl_float), 
                        0, 
                        NULL, 
                        &outMapEvt, 
                        &status);
		CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (outputBuffer)");
		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&outMapEvt);
		CHECK_ERROR(status,0, "waitForEventAndRelease(outMapEvt) failed");

		memcpy(outputFloat, outMapPtrFloat, sizeof(cl_float) * width1  * height0);

		status = clEnqueueUnmapMemObject(
                commandQueue, 
                outputBufferFloat, 
                outMapPtrFloat, 
                0, 
                NULL, 
                &outUnmapEvt);
		CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (outputBuffer)");

		status = clFlush(commandQueue);
		CHECK_OPENCL_ERROR(status, "clFlush failed.");

		status = sampleCommon->waitForEventAndRelease(&outUnmapEvt);
		CHECK_ERROR(status,0, "waitForEventAndRelease(outUnmapEvt) failed");

		return SDK_SUCCESS;
}

/*
 * This is a naive O(N^3) CPU implementatio of matrix multiplication
 */
void 
MatrixMultiplication::matrixMultiplicationCPUReference(
    cl_int * outputInt,
    cl_int * inputInt0,
    cl_int * inputInt1,
    const cl_uint y,
    const cl_uint x,
    const cl_uint z)
{
    for(cl_uint i = 0; i < y; i++)
    {
        for(cl_uint j = 0; j < z; j++)
        {
            for(cl_uint k = 0; k < x; k++)
            {
                outputInt[i * z + j] += (inputInt0[i * x + k] * inputInt1[k * z + j]);
            }
        }
    }
}

/*
 * This is a naive O(N^3) CPU implementatio of matrix multiplication
 */
void 
MatrixMultiplication::matrixMultiplicationCPUReference(
    cl_float * outputFloat,
    cl_float * inputFloat0,
    cl_float * inputFloat1,
    const cl_uint y,
    const cl_uint x,
    const cl_uint z)
{
    for(cl_uint i = 0; i < y; i++)
    {
        for(cl_uint j = 0; j < z; j++)
        {
            for(cl_uint k = 0; k < x; k++)
            {
                outputFloat[i * z + j] += (inputFloat0[i * x + k] * inputFloat1[k * z + j]);
            }
        }
    }
}

bool
MatrixMultiplication::compare(const int *refData, const int *data, 
                        const int length, const double epsilon)
{
    double error = 0.0;
    double ref = 0.0;

    for(int i = 1; i < length; ++i) 
    {
        double diff = (double)(refData[i] - data[i]);
        error += diff * diff;
        ref += (double)(refData[i] * refData[i]);
    }

    double normRef =::sqrt((double) ref);
    if (::fabs((double) ref) < 1e-7) {
        return false;
    }
    double normError = ::sqrt((double) error);
    error = normError / normRef;

    return error < epsilon;
}

int 
MatrixMultiplication::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // add an option for getting blockSize from commandline
    streamsdk::Option* xParam = new streamsdk::Option;
    CHECK_ALLOCATION(xParam, "Memory Allocation error.\n");
    xParam->_sVersion = "x";
    xParam->_lVersion = "height0";
    xParam->_description = "height of matrix A";
    xParam->_type     = streamsdk::CA_ARG_INT;
    xParam->_value    = &n;
    sampleArgs->AddOption(xParam);
    delete xParam;

    streamsdk::Option* yParam = new streamsdk::Option;
    CHECK_ALLOCATION(yParam, "Memory Allocation error.\n");
    yParam->_sVersion = "y";
    yParam->_lVersion = "width0";
    yParam->_description = "width of matrix A and Height of matrix B";
    yParam->_type     = streamsdk::CA_ARG_INT;
    yParam->_value    = &m;
    sampleArgs->AddOption(yParam);
    delete yParam;

    streamsdk::Option* zParam = new streamsdk::Option;
    CHECK_ALLOCATION(zParam, "Memory Allocation error.\n");
    zParam->_sVersion = "z";
    zParam->_lVersion = "width1";
    zParam->_description = "width of matrix B";
    zParam->_type     = streamsdk::CA_ARG_INT;
    zParam->_value    = &k;
    sampleArgs->AddOption(zParam);
    delete zParam;

    streamsdk::Option* blockSizeParam = new streamsdk::Option;
    CHECK_ALLOCATION(blockSizeParam, "Memory Allocation error.\n");
    blockSizeParam->_sVersion = "b";
    blockSizeParam->_lVersion = "blockSize";
    blockSizeParam->_description = "Use local memory of dimensions blockSize x blockSize";
    blockSizeParam->_type     = streamsdk::CA_ARG_INT;
    blockSizeParam->_value    = &blockSize;
    sampleArgs->AddOption(blockSizeParam);
    delete blockSizeParam;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "Memory Allocation error.\n");
    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;
    sampleArgs->AddOption(num_iterations);
    delete num_iterations;

    streamsdk::Option* appGflops_option = new streamsdk::Option;
    CHECK_ALLOCATION(appGflops_option, "Memory Allocation error.\n");
    appGflops_option->_sVersion = "";
    appGflops_option->_lVersion = "eAppGflops";
    appGflops_option->_description = "Prints GFLOPS calculated from transfer + kernel time";
    appGflops_option->_type = streamsdk::CA_NO_ARGUMENT;
    appGflops_option->_value = &eAppGFLOPS;
    sampleArgs->AddOption(appGflops_option);
    delete appGflops_option;

	streamsdk::Option* arrayType_option = new streamsdk::Option;
    CHECK_ALLOCATION(arrayType_option, "Memory Allocation error.\n");
    arrayType_option->_sVersion = "";
    arrayType_option->_lVersion = "arrayType";
    arrayType_option->_description = "type of matrix A and matrix B [int/float]";
	arrayType_option->_type     = streamsdk::CA_ARG_STRING;
    arrayType_option->_value    = &arrayType;
    sampleArgs->AddOption(arrayType_option);
    delete arrayType_option;

    return SDK_SUCCESS;
}

int 
MatrixMultiplication::setup()
{  
    // Make sure the dimensions are multiples of blockSize
    const int vectorSize = 4;
    if(n % (blockSize * vectorSize) != 0)
    {
        n = (n / (blockSize * vectorSize) + 1) * (blockSize * vectorSize);
    }

    if(m % (blockSize * vectorSize) != 0)
    {
        m = (m / (blockSize * vectorSize) + 1) * (blockSize * vectorSize);
    }

    if(k % (blockSize * vectorSize) != 0)
    {
       k = (k / (blockSize * vectorSize) + 1) * (blockSize * vectorSize);
    }

    width0  = m;
    height0 = n;

    width1  = k;
    height1 = m;

	if(!arrayFlag)
	{
		if(setupMatrixMultiplicationInt()!=SDK_SUCCESS)
			return SDK_FAILURE;
	}
	else
	{
		if(setupMatrixMultiplicationFloat()!=SDK_SUCCESS)
			return SDK_FAILURE;
	}

	int timer = sampleCommon->createTimer();
	sampleCommon->resetTimer(timer);
	sampleCommon->startTimer(timer);

	if(setupCL()!=SDK_SUCCESS)
		return SDK_FAILURE;

	sampleCommon->stopTimer(timer);

	setupTime = (cl_double)sampleCommon->readTimer(timer);

	return SDK_SUCCESS;
}


int 
MatrixMultiplication::run()
{
    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
		// Arguments are set and execution call is enqueued on command buffer
		if(!arrayFlag)
		{
			if(runCLKernelsInt() != SDK_SUCCESS)
				return SDK_FAILURE;
		}
		else
		{
			if(runCLKernelsFloat() != SDK_SUCCESS)
				return SDK_FAILURE;
		}
    }

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    std::cout << "Executing kernel for " << iterations << " iterations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    kernelTime = 0;
    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
		int kernelRun = 0;

		if(!arrayFlag)
		{
			kernelRun = runCLKernelsInt();
		}
		else
		{
			kernelRun = runCLKernelsFloat();
		}

        if(kernelRun != SDK_SUCCESS)
        {
            return kernelRun;
        }
    }

    sampleCommon->stopTimer(timer);
    appTime = (double)(sampleCommon->readTimer(timer)) / iterations;
    kernelTime = kernelTime / iterations;
	
	if(!quiet)
	{
		if(!arrayFlag)
		{
			sampleCommon->printArray<cl_int>("OutputInt", outputInt, width1, 1);
		}
		else
		{
			sampleCommon->printArray<cl_float>("OutputFloat", outputFloat, width1, 1);
		}
	}

   	return SDK_SUCCESS;
}

int 
MatrixMultiplication::verifyResults()
{
    if(verify)
    {
       if(!arrayFlag)
	   {
		   // reference implementation
			matrixMultiplicationCPUReference(verificationOutputInt, inputInt0, inputInt1, height0, width0,  width1);

			// compare the results and see if they match
			if(this->compare(outputInt, verificationOutputInt, height0*width1))
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
	   else
	   {
			// reference implementation
			matrixMultiplicationCPUReference(verificationOutputFloat, inputFloat0, inputFloat1, height0, width0,  width1);

			 // compare the results and see if they match
			if(sampleCommon->compare(outputFloat, verificationOutputFloat, height0*width1))
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
    }

    return SDK_SUCCESS;
}

void 
MatrixMultiplication::printStats()
{
    if(eAppGFLOPS)
    {
        std::string strArray[4] = {"MatrixA", "MatrixB", "Time(sec)", "[Transfer+kernel]Time(sec)"};
        std::string stats[4];

        double flops = 2 * width0 * width1;
        double perf = (flops / appTime) * height0 * 1e-9;
        if(timing)
            std::cout << "GFlops achieved : " << perf << std::endl << std::endl;

        totalTime = setupTime + appTime;

        stats[0]  = sampleCommon->toString(height0, std::dec)
                    +"x"+sampleCommon->toString(width0, std::dec);
        stats[1]  = sampleCommon->toString(height1, std::dec)
                    +"x"+sampleCommon->toString(width1, std::dec);
        stats[2]  = sampleCommon->toString(totalTime, std::dec);
        stats[3]  = sampleCommon->toString(appTime, std::dec);
        
        this->SDKSample::printStats(strArray, stats, 4);
    }
    else
    {
        std::string strArray[4] = {"MatrixA", "MatrixB", "Time(sec)", "kernelTime(sec)"};
        std::string stats[4];

        double flops = 2 * width0 * width1;
        double perf = (flops / kernelTime) * height0 * 1e-9;
        if(timing)
            std::cout << "GFlops achieved : " << perf << std::endl << std::endl;

        totalTime = setupTime + kernelTime;

        stats[0]  = sampleCommon->toString(height0, std::dec)
                    +"x"+sampleCommon->toString(width0, std::dec);
        stats[1]  = sampleCommon->toString(height1, std::dec)
                    +"x"+sampleCommon->toString(width1, std::dec);
        stats[2]  = sampleCommon->toString(totalTime, std::dec);
        stats[3]  = sampleCommon->toString(kernelTime, std::dec);

        this->SDKSample::printStats(strArray, stats, 4);
    }
}

int 
MatrixMultiplication::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

	if(!arrayFlag)
	{
		status = clReleaseMemObject(inputBufferInt0);
		CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBufferInt0)");

		status = clReleaseMemObject(inputBufferInt1);
		CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBufferInt1)");

		status = clReleaseMemObject(outputBufferInt);
		CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(outputBufferInt)");
	}
	else
	{
		status = clReleaseMemObject(inputBufferFloat0);
		CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBufferFloat0)");

		status = clReleaseMemObject(inputBufferFloat1);
		CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBufferFloat1)");

		status = clReleaseMemObject(outputBufferFloat);
		CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(outputBufferFloat)");
	}

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.(context)");

    // release program resources (input memory etc.)

    FREE(inputInt0);
    FREE(inputInt1);
    FREE(outputInt);
    FREE(verificationOutputInt);
	FREE(inputFloat0);
    FREE(inputFloat1);
    FREE(outputFloat);
    FREE(verificationOutputFloat);
    FREE(devices);

    return SDK_SUCCESS;
}

int
MatrixMultiplication::judgeArrayFlag()
{
	if(arrayType.compare("int")==0)
	{
		arrayFlag = 0;
	}
	else if(arrayType.compare("float")==0)
	{
		arrayFlag = 1;
	}
	else
	{
		std::cout << "Please input right argument[int/float] ! "<< std::endl;
		return SDK_FAILURE;
	}
	return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{

    MatrixMultiplication clMatrixMultiplication("OpenCL Matrix Multiplication");

    if(clMatrixMultiplication.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clMatrixMultiplication.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;
	
	if(clMatrixMultiplication.judgeArrayFlag() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clMatrixMultiplication.isDumpBinaryEnabled() != SDK_SUCCESS)
    {
        return clMatrixMultiplication.genBinaryImage();
    }
    else
    {
        // Setup
        if(clMatrixMultiplication.setup() != SDK_SUCCESS)
            return SDK_FAILURE;
        // Run
        if(clMatrixMultiplication.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        // VerifyResults
        if(clMatrixMultiplication.verifyResults() != SDK_SUCCESS)
            return SDK_FAILURE;

        // Cleanup
        if(clMatrixMultiplication.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;
        
        clMatrixMultiplication.printStats();
    }

    return SDK_SUCCESS;
}
