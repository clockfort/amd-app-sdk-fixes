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


#include "HistogramAtomics.hpp"

#include <math.h>

void 
Histogram::calculateHostBin()
{
    // compute CPU histogram
    cl_int *p = (cl_int*)input;

    memset(cpuhist, 0, sizeof(cl_uint) * NBINS);

    for(unsigned int i = 0; i < inputNBytes / sizeof(cl_uint); i++) 
    {
        cpuhist[ (p[i] >> 24) & 0xff ]++;
        cpuhist[ (p[i] >> 16) & 0xff ]++;
        cpuhist[ (p[i] >> 8) & 0xff ]++;
        cpuhist[ (p[i] >> 0) & 0xff ]++;
    }
}

int
Histogram::setupHistogram()
{
    cl_int status = 0;

	nThreads =           64 * 1024;
	n4Vectors =          2048 * 2048;
	n4VectorsPerThread = n4Vectors / nThreads;
	inputNBytes =        n4Vectors * sizeof(cl_uint4);

	input =  (cl_uint*)malloc(inputNBytes);
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    // random initialization of input 
    time_t ltime;
    time(&ltime);
    cl_uint a = (cl_uint) ltime, b = (cl_uint) ltime;
    cl_uint *p = (cl_uint *) input;

    for(unsigned int i = 0; i < inputNBytes / sizeof(cl_uint); i++)
        p[i] = ( b = ( a * ( b & 65535 )) + ( b >> 16 ));

    return SDK_SUCCESS;
}

int 
Histogram::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("HistogramAtomics_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int
Histogram::setupCL(void)
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


	context = cl::Context(dType, cps, NULL, NULL, &status);
	CHECK_OPENCL_ERROR(status, "Context::Context() failed.");

    // getting device on which to run the sample
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
		deviceExtensions = (*i).getInfo<CL_DEVICE_EXTENSIONS>();
	}
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
	commandQueue = cl::CommandQueue(context, devices[deviceId], 0, &status);
	CHECK_OPENCL_ERROR(status, "CommandQueue::CommandQueue() failed.");

    // Check if byte-addressable store is supported 
    if(!strstr(deviceExtensions.c_str(), "cl_khr_byte_addressable_store"))
    {
        reqdExtSupport = false;
        OPENCL_EXPECTED_ERROR("Device does not support cl_khr_byte_addressable_store extension!!");
    }

    // Check if byte-addressable store is supported 
	if(!strstr(deviceExtensions.c_str(), "cl_khr_global_int32_base_atomics"))
    {
        reqdExtSupport = false;
        OPENCL_EXPECTED_ERROR("Device does not support global_int32_base_atomics extension!");
    }
    // Check if byte-addressable store is supported 
    if(!strstr(deviceExtensions.c_str(), "cl_khr_local_int32_base_atomics"))
    {
        reqdExtSupport = false;
        OPENCL_EXPECTED_ERROR("Device does not support local_int32_base_atomics extension!");
    }

 	cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
	if(isAmdPlatform())
		inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

	inputBuffer = cl::Buffer(context, 
		inMemFlags, 
		inputNBytes,
		0,
		&status);
	CHECK_OPENCL_ERROR(status, "Buffer::Buffer() failed. (inputBuffer)");
	device.push_back(devices[deviceId]);

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

		program = cl::Program(context, device, programBinary, NULL, &status);
		CHECK_OPENCL_ERROR(status, "Program::Program(Binary) failed.");
	}
	else
	{
		kernelPath.append("HistogramAtomics_Kernels.cl");
		if(!kernelFile.open(kernelPath.c_str()))
		{
			std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
			return SDK_FAILURE;
		}

		cl::Program::Sources programSource(1, 
			std::make_pair(kernelFile.source().data(), 
			kernelFile.source().size()));

		program = cl::Program(context, programSource, &status);
		CHECK_OPENCL_ERROR(status, "Program::Program(Source) failed.");

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

	histogram = cl::Kernel(program, "histogramKernel",  &status);
	CHECK_OPENCL_ERROR(status, "Kernel::Kernel() failed.");

	reduce = cl::Kernel(program, "reduceKernel",  &status);
	CHECK_OPENCL_ERROR(status, "Kernel::Kernel() failed.");

	kernelWorkGroupSize = histogram.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[deviceId], &status);
	CHECK_OPENCL_ERROR(status, "Kernel::getWorkGroupInfo()  failed.");
	
	 compileWorkGroupSize = histogram.getWorkGroupInfo<CL_KERNEL_COMPILE_WORK_GROUP_SIZE>(devices[deviceId], &status)[0];
	CHECK_OPENCL_ERROR(status, "Kernel::getWorkGroupInfo()  failed.");

	if(kernelWorkGroupSize % 256 != 0)
	{
		OPENCL_EXPECTED_ERROR("Device does not support work-group size of 256 on histogram kernel. Exiting!");
	}

	nThreadsPerGroup = (cl_uint)compileWorkGroupSize;
	nGroups =            nThreads / nThreadsPerGroup;
	outputNBytes =       nGroups * NBINS * sizeof(cl_uint);

    output = (cl_uint*)malloc(outputNBytes);
    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");

	outputBuffer =  cl::Buffer(context, 
								CL_MEM_WRITE_ONLY, 
								outputNBytes,
								NULL,
								&status);
	CHECK_OPENCL_ERROR(status, "Buffer::Buffer() failed. (outputImageBuffer)");    
    return SDK_SUCCESS;
}

int 
Histogram::runCLKernels(void)
{
    cl_int status;
    cl_int eventStatus = CL_QUEUED;

	cl::NDRange globalThreads(nThreads);
	cl::NDRange localThreads(nThreadsPerGroup);

	cl::NDRange globalThreadsReduce(NBINS)  ;
	cl::NDRange localThreadsReduce(nThreadsPerGroup)  ;

	cl::Event writeEvt;

	status = commandQueue.enqueueWriteBuffer(
		inputBuffer,
		CL_FALSE,
		0,
		inputNBytes,
		input,
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

    int Arg = 0;

	status = histogram.setArg(0, inputBuffer);
	CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (inputBuffer)");
	status = histogram.setArg(1, outputBuffer);
	CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (outputBuffer)");
	status = histogram.setArg(2, n4VectorsPerThread);
	CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (n4VectorsPerThread)");

	status = reduce.setArg(0, outputBuffer);
	CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (outputBuffer)");
	status = reduce.setArg(1, nGroups);
	CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed. (nGroups)");
    /* 
    * Enqueue a kernel run call.
    */

	cl::Event ndrEvt1;
	cl::Event ndrEvt2;

	status = commandQueue.enqueueNDRangeKernel(
		histogram,
		cl::NullRange,
		globalThreads,
		localThreads,
		NULL, 
		&ndrEvt1);
	CHECK_OPENCL_ERROR(status, "CommandQueue::enqueueNDRangeKernel() failed.");

	status = commandQueue.enqueueNDRangeKernel(
		reduce,
		cl::NullRange,
		globalThreadsReduce,
		localThreadsReduce,
		NULL, 
		&ndrEvt2);
	CHECK_OPENCL_ERROR(status, "CommandQueue::enqueueNDRangeKernel() failed.");

	status = commandQueue.flush();
	CHECK_OPENCL_ERROR(status, "cl::CommandQueue.flush failed.");
	eventStatus = CL_QUEUED;
	while(eventStatus != CL_COMPLETE)
	{
		status = ndrEvt1.getInfo<cl_int>(
			CL_EVENT_COMMAND_EXECUTION_STATUS, 
			&eventStatus);
		CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
	}
	eventStatus = CL_QUEUED;
	while(eventStatus != CL_COMPLETE)
	{
		status = ndrEvt2.getInfo<cl_int>(
			CL_EVENT_COMMAND_EXECUTION_STATUS, 
			&eventStatus);
		CHECK_OPENCL_ERROR(status, "cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS) failed.");
	}

	cl::Event readEvt;
	status = commandQueue.enqueueReadBuffer(
		outputBuffer,
		CL_FALSE,
		0,
		outputNBytes,
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

    return SDK_SUCCESS;
}

int
Histogram::initialize()
{
    // Call base class Initialize to get default configuration
	CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL resource  initilization failed");


    streamsdk::Option* iteration_option = new streamsdk::Option;
    CHECK_ALLOCATION(iteration_option, "Memory allocation error.\n");

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
Histogram::setup()
{
    if(setupHistogram() != SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    int status = setupCL();
    if(status != SDK_SUCCESS)
        return status;

    sampleCommon->stopTimer(timer);
    // Compute setup time 
    setupTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}


int 
Histogram::run()
{
    if(!reqdExtSupport)
        return SDK_SUCCESS;

    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    if(!quiet)
    {
        std::cout << "Executing kernel for "
            << iterations << " iterations" << std::endl;
        std::cout << "-------------------------------------------" << std::endl;
    }

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
    kernelTimeGlobal = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
        sampleCommon->printArray<cl_uint>("Output", output, NBINS, 1);

    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    return SDK_SUCCESS;
}

int
Histogram::verifyResults()
{
    if(!reqdExtSupport)
        return SDK_SUCCESS;

    if(verify)
    {
        /* reference implementation on host device
         * calculates the histogram bin on host
         */
        calculateHostBin();

        // compare the results and see if they match 
        bool flag = true;
        for(cl_uint i = 0; i < NBINS; ++i)
        {
            if(cpuhist[i] != output[i])
            {
                flag = false;
                break;
            }
        }

        if(flag)
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

void Histogram::printStats()
{
    if(!reqdExtSupport)
        return;

    // calculate total time 
    totalTime = setupTime + kernelTimeGlobal;

    std::string strArray[3] = 
    {
        "Input", 
        "Time(sec)", 
        "[Transfer+Kernel]Time(sec)"
    };
    std::string stats[3];

    cl_int elem = inputNBytes / sizeof(cl_uint);
    stats[0] = sampleCommon->toString(elem, std::dec);
    stats[1] = sampleCommon->toString(totalTime, std::dec);
    stats[2] = sampleCommon->toString(kernelTimeGlobal, std::dec);

    this->SDKSample::printStats(strArray, stats, 3);

    totalTime = setupTime + kernelTimeLocal;

}


int Histogram::cleanup()
{
    if(!reqdExtSupport)
        return SDK_SUCCESS;

    // Releases OpenCL resources (Context, Memory etc.) 
	free(input);
	free(output);
    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    // Create MonteCalroAsian object
    Histogram clHistogram("HistogramAtomics OpenCL sample");

    // Initialization
    if(clHistogram.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Parse command line options
    if(clHistogram.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clHistogram.isDumpBinaryEnabled())
    {
        return clHistogram.genBinaryImage();
    }
    
    // Setup
    int status = clHistogram.setup();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_SUCCESS;
        else
            return SDK_FAILURE;
    }

    // Run 
    if(clHistogram.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Verify
    if(clHistogram.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Cleanup resources created 
    if(clHistogram.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Print performance statistics 
    clHistogram.printStats();

    return SDK_SUCCESS;
}
