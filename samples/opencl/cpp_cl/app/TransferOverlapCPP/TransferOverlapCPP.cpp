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

#include "TransferOverlapCPP.hpp"
#include <string>

struct _memFlags {cl_mem_flags f;
                  const char *s;} 
                  memFlags[] = 
                  {
                     { CL_MEM_READ_ONLY,              "CL_MEM_READ_ONLY" },
                     { CL_MEM_WRITE_ONLY,             "CL_MEM_WRITE_ONLY" },
                     { CL_MEM_READ_WRITE,             "CL_MEM_READ_WRITE" },
                     { CL_MEM_ALLOC_HOST_PTR,         "CL_MEM_ALLOC_HOST_PTR" },
                     { CL_MEM_USE_PERSISTENT_MEM_AMD, "CL_MEM_USE_PERSISTENT_MEM_AMD"}
                  };

int nFlags = sizeof(memFlags) / sizeof(memFlags[0]);

int 
TransferOverlap::setupTransferOverlap()
{
    // Increase the input valuse if -e option is not used
    if(verify)
    {
        nLoops = 1;
        nSkip = 0; 
        nKLoops = 10;
        nBytes = 1024 * 1024;
    }

    timeLog = new TestLog(nLoops * 50);
    CHECK_ALLOCATION(timeLog, "Failed to allocate host memory. (timeLog)");

    // Educated guess of optimal work size
    int minBytes = MAX_WAVEFRONT_SIZE * sizeof(cl_uint) * 4;

    nBytes = (nBytes / minBytes) * minBytes;
    nBytes = nBytes < minBytes ? minBytes : nBytes;
    nItems = nBytes / (4 * sizeof(cl_uint));
    
    int maxThreads = nBytes / (4 * sizeof( cl_uint ));
    nThreads = deviceInfo.maxComputeUnits * numWavefronts * MAX_WAVEFRONT_SIZE;
    
    if(nThreads > maxThreads)
        nThreads = maxThreads;
    else
    {
        while(nItems % nThreads != 0)
            nThreads += MAX_WAVEFRONT_SIZE;
    }

    nBytesResult = (nThreads / MAX_WAVEFRONT_SIZE) * sizeof(cl_uint) * 2;

    return SDK_SUCCESS;
}

int 
TransferOverlap::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("TransferOverlapCPP_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int
TransferOverlap::setupCL(void)
{
    cl_int status = 0;
	int retValue = 0;
   // size_t deviceListSize;
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

    
	status = cl::Platform::get(&this->platforms);
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

   
    //Set device info of given cl_device_id
    retValue = setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    // Check device extensions
    if(!strstr(extensions.c_str(), "cl_khr_local_int32_base_atomics"))
        OPENCL_EXPECTED_ERROR("Device does not support cl_khr_local_int32_base_atomics extension!");

    // Get OpenCL device version
    std::string deviceVersionStr = deviceVersion;
    size_t vStart = deviceVersionStr.find(" ", 0);
    size_t vEnd = deviceVersionStr.find(" ", vStart + 1);
    std::string vStrVal = deviceVersionStr.substr(vStart + 1, vEnd - vStart - 1);

    // Check of OPENCL_C_VERSION if device version is 1.1 or later
#ifdef CL_VERSION_1_1
    if(openclCVersion.c_str())
    {
        // Exit if OpenCL C device version is 1.0
        deviceVersionStr = openclCVersion;
        vStart = deviceVersionStr.find(" ", 0);
        vStart = deviceVersionStr.find(" ", vStart + 1);
        vEnd = deviceVersionStr.find(" ", vStart + 1);
        vStrVal = deviceVersionStr.substr(vStart + 1, vEnd - vStart - 1);
        if(vStrVal.compare("1.0") <= 0)
            OPENCL_EXPECTED_ERROR("Unsupported device! Required CL_DEVICE_OPENCL_C_VERSION as 1.1");
    }
    else
        OPENCL_EXPECTED_ERROR("Unsupported device! Required CL_DEVICE_OPENCL_C_VERSION as 1.1");
#else
    OPENCL_EXPECTED_ERROR("Unsupported device! Required CL_DEVICE_OPENCL_C_VERSION as 1.1");
#endif

    //Setup application data
   if(setupTransferOverlap() != SDK_SUCCESS)
       return SDK_FAILURE;

   commandQueue = cl::CommandQueue(context, devices[deviceId], 0, &status);
   CHECK_OPENCL_ERROR(status, "CommandQueue::CommandQueue() failed.");


	inputBuffer1 = cl::Buffer(context,inFlags,nBytes,NULL,&status);
	CHECK_OPENCL_ERROR(status, "clCreateBuffer() failed.(inputBuffer1).");

   
	inputBuffer2 = cl::Buffer(context, inFlags, nBytes, NULL, &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer() failed.(inputBuffer2).");

   
	resultBuffer1 = cl::Buffer(context,
                                   CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                   nBytesResult,
                                   NULL, 
                                   &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer() failed.(resultBuffer1).");

   
	resultBuffer2 = cl::Buffer(context,
                                   CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                   nBytesResult,
                                   NULL, 
                                   &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer() failed.(resultBuffer2).");

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
        kernelPath.append("TransferOverlapCPP_Kernels.cl");
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



    /* ConstantBuffer bandwidth from single access */
   
	readKernel = cl::Kernel(program, "readKernel", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(readKernel).");

    size_t kernelWorkGroupSize = 0;

    /* Check whether specified local group size is possible on current kernel */
   
	kernelWorkGroupSize = readKernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[deviceId], &status);
    CHECK_OPENCL_ERROR(status, "clGetKernelWorkGroupInfo() failed.");

    // If local groupSize exceeds the maximum supported on kernel  fall back
    if(MAX_WAVEFRONT_SIZE > kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << MAX_WAVEFRONT_SIZE << std::endl;
            std::cout << "Max Group Size supported on the kernel(readKernel) : " 
                      << kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
        }
        return SDK_FAILURE;
    }

   
	writeKernel = cl::Kernel(program, "writeKernel", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel() failed.(writeKernel)");

    // Check whether specified local group size is possible on current kernel
   
	kernelWorkGroupSize = writeKernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[deviceId], &status);
    CHECK_OPENCL_ERROR(status, "clGetKernelWorkGroupInfo() failed.");

    // If local group size exceeds the maximum supported on kernel fall back 
    if(MAX_WAVEFRONT_SIZE > kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << MAX_WAVEFRONT_SIZE << std::endl;
            std::cout << "Max Group Size supported on the kernel(writeKernel) : " 
                      << kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelWorkGroupSize << std::endl;
        }
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int 
TransferOverlap::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize())
        return SDK_FAILURE;

    streamsdk::Option* numLoops = new streamsdk::Option;

    numLoops->_sVersion = "i";
    numLoops->_lVersion = "iterations";
    numLoops->_description = "Number of timing loops";
    numLoops->_type = streamsdk::CA_ARG_INT;
    numLoops->_value = &nLoops;

    sampleArgs->AddOption(numLoops);
    delete numLoops;

    streamsdk::Option* skipLoops = new streamsdk::Option;

    skipLoops->_sVersion = "s";
    skipLoops->_lVersion = "skip";
    skipLoops->_description = "skip first n iterations for average";
    skipLoops->_type = streamsdk::CA_ARG_INT;
    skipLoops->_value = &nSkip;

    sampleArgs->AddOption(skipLoops);
    delete skipLoops;

    streamsdk::Option* kernelLoops = new streamsdk::Option;

    kernelLoops->_sVersion = "k";
    kernelLoops->_lVersion = "kernelLoops";
    kernelLoops->_description = "Number of loops in kernel";
    kernelLoops->_type = streamsdk::CA_ARG_INT;
    kernelLoops->_value = &nKLoops;

    sampleArgs->AddOption(kernelLoops);
    delete kernelLoops;

    streamsdk::Option* size = new streamsdk::Option;

    size->_sVersion = "x";
    size->_lVersion = "size";
    size->_description = "Size in bytes";
    size->_type = streamsdk::CA_ARG_INT;
    size->_value = &nBytes;

    sampleArgs->AddOption(size);
    delete size;

    streamsdk::Option* wavefronts = new streamsdk::Option;

    wavefronts->_sVersion = "w";
    wavefronts->_lVersion = "wavefronts";
    wavefronts->_description = "Number of wavefronts per compute unit";
    wavefronts->_type = streamsdk::CA_ARG_INT;
    wavefronts->_value = &numWavefronts;

    sampleArgs->AddOption(wavefronts);
    delete wavefronts;

    streamsdk::Option* inputFlags = new streamsdk::Option;

    inputFlags->_sVersion = "I";
    inputFlags->_lVersion = "inMemFlag";
    inputFlags->_description = "Memory flags for input buffer "
                         "\n\t\t 0 for CL_MEM_READ_ONLY"
                         "\n\t\t 1 CL_MEM_WRITE_ONLY"
                         "\n\t\t 2 CL_MEM_READ_WRITE"
                         "\n\t\t 3 CL_MEM_ALLOC_HOST_PTR"
                         "\n\t\t 4 CL_MEM_USE_PERSISTENT_MEM_AMD\n";
    inputFlags->_type = streamsdk::CA_ARG_INT;
    inputFlags->_value = &inFlagsValue;

    sampleArgs->AddOption(inputFlags);
    delete inputFlags;

    streamsdk::Option* noOverlapStr = new streamsdk::Option;

    noOverlapStr->_sVersion = "n";
    noOverlapStr->_lVersion = "noOverlap";
    noOverlapStr->_description = "Do not overlap memset() with kernel";
    noOverlapStr->_type = streamsdk::CA_NO_ARGUMENT;
    noOverlapStr->_value = &noOverlap;

    sampleArgs->AddOption(noOverlapStr);
    delete noOverlapStr;


    streamsdk::Option* pringLogStr = new streamsdk::Option;

    pringLogStr->_sVersion = "l";
    pringLogStr->_lVersion = "log";
    pringLogStr->_description = "Prints complete timing log";
    pringLogStr->_type = streamsdk::CA_NO_ARGUMENT;
    pringLogStr->_value = &printLog;

    sampleArgs->AddOption(pringLogStr);
    delete pringLogStr;

    return SDK_SUCCESS;
}

int 
TransferOverlap::parseExtraCommandLineOptions(int argc, char**argv)
{
    if(parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;
    
    //Handle extra options which are not default in SDKUtils
    while(--argc) 
    {
        if(strcmp(argv[argc], "-I") == 0 ||  strcmp(argv[argc], "-inMemFlag") == 0)
        {
            int f = atoi( argv[ argc+1 ] );
            if( f < nFlags )
                inFlags |= memFlags[ f ].f;
        }
    }

    cl_mem_flags f = CL_MEM_ALLOC_HOST_PTR |
                     CL_MEM_USE_PERSISTENT_MEM_AMD |
                     CL_MEM_READ_ONLY |
                     CL_MEM_WRITE_ONLY |
                     CL_MEM_READ_WRITE;

    if( (inFlags & f) == 0 )
             inFlags |= CL_MEM_READ_ONLY | CL_MEM_USE_PERSISTENT_MEM_AMD;

    f  = CL_MEM_READ_ONLY |
         CL_MEM_WRITE_ONLY |
         CL_MEM_READ_WRITE;

    if( (inFlags & f) == 0 )
             inFlags |= CL_MEM_READ_ONLY;

    nSkip = nLoops > nSkip ? nSkip : 0;

    return SDK_SUCCESS;
}

int 
TransferOverlap::setup()
{
    int status = setupCL();
    if(status != SDK_SUCCESS)
        return status;

    return SDK_SUCCESS;
}

int 
TransferOverlap::verifyResultBuffer(cl::Buffer resultBuffer, bool firstLoop)
{
    cl_int   status;
	cl::Event event;
    void *ptrResult = NULL;

    t.Reset(); 
    t.Start();

   
	ptrResult = (void *)commandQueue.enqueueMapBuffer(
		                                              resultBuffer,
													  CL_TRUE,
													  CL_MAP_READ,
													  0,
													  nBytesResult,
													  NULL,
													  NULL,
													  &status);

    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer() failed.(resultBuffer)");

    t.Stop();
    timeLog->Timer(
        "%32s  %lf s %8.2lf GB/s\n", "clEnqueueMapBuffer(MAP_WRITE)", 
        t.GetElapsedTime(), 
        nBytesResult, 
        1); 

    t.Reset(); 
    t.Start();

    cl_uint sum = 0;
    for( int i = 0; i < nThreads / MAX_WAVEFRONT_SIZE; i++ )
        sum += ((cl_uint*)ptrResult)[i];

    bool results;
    if( (sum != nBytes / sizeof(cl_uint)) && !firstLoop )
        results = false;
    else
        results = true;

    t.Stop();
    timeLog->Timer("%32s  %lf s\n", "CPU reduction", t.GetElapsedTime(), nBytes, 1);
    
    if(results)
        timeLog->Msg( "%32s\n", "verification ok" );
    else
    {
        correctness = false;
        timeLog->Error( "%32s\n", "verification FAILED" );
        std::cout << "Failed\n";
        return SDK_FAILURE;
    }
    
    t.Reset();
    t.Start();

    
	status = commandQueue.enqueueUnmapMemObject(
		                                        resultBuffer,
												(void *) ptrResult,
												NULL,
												&event
											    );
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject() failed.(resultBuffer)");
 
    
	status = event.wait();
    CHECK_OPENCL_ERROR(status, "clWaitForEvents()");

    t.Stop();
    timeLog->Timer(
        "%32s  %lf s %8.2lf GB/s\n", "clEnqueueUnmapMemObject()", 
        t.GetElapsedTime(), 
        nBytesResult,
        1);

    

    return SDK_SUCCESS;
}

int 
TransferOverlap::launchKernel(cl::Buffer inputBuffer, cl::Buffer resultBuffer, unsigned char v)
{
    cl_uint vKernel = 0;
    cl_int status = 0;

    for(int i = 0; i < sizeof(cl_uint); i++)
       vKernel |= v << (i * 8);

    nItemsPerThread = nItems / nThreads;

    globalWorkSize = nThreads;
    localWorkSize = MAX_WAVEFRONT_SIZE;

    
	status = readKernel.setArg(0,inputBuffer);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed.(inputBuffer)");

    
	status = readKernel.setArg(1,resultBuffer);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed.(resultBuffer)");

  
	status = readKernel.setArg(2,nItemsPerThread);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed.(nItemsPerThread)");

   
	status = readKernel.setArg(3,vKernel);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed.(vKernel)");

   
	status = readKernel.setArg(4,nKLoops);
    CHECK_OPENCL_ERROR(status, "Kernel::setArg() failed.(nKLoops)");


	cl::Event  event;
	cl::Event *evPtr;

    if( noOverlap )
        evPtr = &event;
    else
        evPtr = NULL;

    t.Reset(); 
    t.Start();

	cl::NDRange globalThreads(globalWorkSize);
	cl::NDRange localThreads(localWorkSize);

   
	status = commandQueue.enqueueNDRangeKernel(
                readKernel,
				cl::NullRange,              
                globalThreads,
                localThreads,             
                NULL, 
                evPtr);

    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel() failed.");

    if(noOverlap)
    {
        
		status = evPtr->wait();
        CHECK_OPENCL_ERROR(status, "clWaitForEvents() failed.");
    }
    else
    {
       
		status = commandQueue.flush();
        CHECK_OPENCL_ERROR(status, "clFlush() failed.");
    }

    t.Stop();
    timeLog->Timer(
        "%32s  %lf s\n", "clEnqueueNDRangeKernel()", 
        t.GetElapsedTime(), 
        nBytes, 
        nKLoops);

    return SDK_SUCCESS;
}

void*
TransferOverlap::launchMapBuffer(cl::Buffer buffer, cl::Event *mapEvent)
{
    cl_int status;
    void *ptr = NULL;

    t.Reset(); 
    t.Start();

   
	ptr = (void *)commandQueue.enqueueMapBuffer(
		                                              buffer,
													  CL_FALSE,
													  CL_MAP_WRITE,
													  0,
													  nBytes,
													  0,
													  mapEvent,
													  &status);
    if(ptr == NULL)
    {
        std::cout << "clEnqueueMapBuffer(buffer) failed.";
        return NULL;
    }

	status = commandQueue.flush();
    if(status != CL_SUCCESS)
    {
        std::cout << "clFlush() failed.";
        return NULL;
    }

    t.Stop();
    timeLog->Timer(
        "%32s  %lf s %8.2lf GB/s\n", "clEnqueueMapBuffer(MAP_WRITE)", 
        t.GetElapsedTime(), 
        nBytes,
        1);

    return ptr;
}

int
TransferOverlap::fillBuffer(cl::Buffer buffer, cl::Event *mapEvent, void *ptr, unsigned char v)
{
    cl_int status;
	cl::Event event;

    t.Reset();
    t.Start();

   
	status = mapEvent->wait();
    CHECK_OPENCL_ERROR(status, "clWaitForEvents() failed.");

    memset(ptr, v, nBytes);

    t.Stop();
    timeLog->Timer(
        "%32s  %lf s %8.2lf GB/s\n", "clWaitForEvents() + memset()", 
        t.GetElapsedTime(), 
        nBytes, 
        1);    

   
    t.Reset();
    t.Start();

   
	status = commandQueue.enqueueUnmapMemObject(
		                                        buffer,
												(void *) ptr,
												NULL,
												&event
											    );
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject(buffer) failed.");
 
    
    status = event.wait();

    CHECK_OPENCL_ERROR(status, "clWaitForEvents() failed.");

    t.Stop();
    timeLog->Timer(
        "%32s  %lf s %8.2lf GB/s\n", "clEnqueueUnmapMemObject()",
        t.GetElapsedTime(),
        nBytes,
        1);
    
   

    return SDK_SUCCESS;
}

int 
TransferOverlap::runOverlapTest()
{
    int nl = nLoops;
    int status = SDK_SUCCESS;

    void *inPtr1 = NULL;
    void *inPtr2 = NULL;

	cl::Event lastBuf1MapEvent;
	cl::Event lastBuf2MapEvent;

    bool firstLoop = true;

    // Start with inputBuffer1 mapped
  
	inPtr1 = (void *)commandQueue.enqueueMapBuffer(
		                                              inputBuffer1,
													  CL_FALSE,
													  CL_MAP_WRITE,
													  0,
													  nBytes,
													  0,
													  &lastBuf1MapEvent,
													  &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer(inputBuffer1) failed.");

    
	status = commandQueue.flush();
    CHECK_OPENCL_ERROR(status, "clFlush() failed.");

    CPerfCounter lt;

    lt.Reset(); 
    lt.Start();

    while(nl--)
    {
       timeLog->loopMarker();
       unsigned char v = (unsigned char)(nl & 0xff);

       /* 1. Host acquires and fills inputBuffer1. Unless this
        is the first loop, this happens concurrently with the
        preceding kernel execution.*/
       timeLog->Msg( "\n%s\n\n", "Acquire and fill: inputBuffer1" );
       status = fillBuffer(inputBuffer1, &lastBuf1MapEvent, inPtr1, v);
       if(status != SDK_SUCCESS)
           return SDK_FAILURE;
    
       /* This is a CPU/GPU synchronization point, as all commands in the
        in-order queue before the preceding cl*Unmap() are now finished.
        We can accurately sample the per-loop timer here.*/
       lt.Stop();
       timeLog->Timer(
           "\n%s %f s\n", "Loop time", 
           lt.GetElapsedTime(), 
           nBytes,
           1);
       lt.Reset();
       lt.Start();

       /* 2. Launch map of inputBuffer2. The map needs to precede
        the next kernel launch in the in-order queue, otherwise waiting
        for the map to finish would also wait for the kernel to
        finish.*/
       timeLog->Msg("\n%s\n\n", "Launch map: inputBuffer2");
       inPtr2 = launchMapBuffer(inputBuffer2, &lastBuf2MapEvent);
       if(inPtr2 == NULL)
           return SDK_FAILURE;

       // Verify result of kernel for inputBuffer2
       timeLog->Msg("\n%s\n\n", "Verify: resultBuffer2");
       status = verifyResultBuffer(resultBuffer2, firstLoop);
       if(status != SDK_SUCCESS)
           return SDK_FAILURE;

       // 3. Asynchronous launch of kernel for inputBuffer1
       timeLog->Msg("\n%s\n\n", "Launch GPU kernel: inputBuffer1");
       status = launchKernel(inputBuffer1, resultBuffer1, v);
       if(status != SDK_SUCCESS)
           return SDK_FAILURE;

       /* 4. Host acquires and fills inputBuffer2. This happens
        concurrently with the preceding kernel execution.*/
       timeLog->Msg("\n%s\n\n", "Acquire and fill: inputBuffer2");
       status = fillBuffer(inputBuffer2, &lastBuf2MapEvent, inPtr2, v); 
       if(status != SDK_SUCCESS)
           return SDK_FAILURE;

       /* 5. Launch map of inputBuffer1. The map needs to precede
        the next kernel launch in the in-order queue, otherwise waiting
        for the map to finish would also wait for the kernel to
        finish.*/
       timeLog->Msg("\n%s\n\n", "Launch map: inputBuffer1");
       inPtr1 = launchMapBuffer(inputBuffer1, &lastBuf1MapEvent);
       if(inPtr1 == NULL)
           return SDK_FAILURE;

       // Verify result of kernel for inputBuffer1
       timeLog->Msg("\n%s\n\n", "Verify: resultBuffer1");
       status = verifyResultBuffer(resultBuffer1, firstLoop);
       if(status != SDK_SUCCESS)
           return SDK_FAILURE;

       // 6. Asynchronous launch of kernel for inputBuffer2
       timeLog->Msg("\n%s\n\n", "Launch GPU kernel: inputBuffer2");

       status = launchKernel(inputBuffer2, resultBuffer2, v);
       if(status != SDK_SUCCESS)
           return SDK_FAILURE;

       timeLog->Msg("%s\n", "");
       firstLoop = false;
    }

    
	status = commandQueue.finish();
    CHECK_OPENCL_ERROR(status, "clFlush() failed.");
    
    return SDK_SUCCESS;
}

int 
TransferOverlap::run()
{
    CPerfCounter gt;
    gt.Reset();
    gt.Start();

    int status = runOverlapTest();
    if(status != SDK_SUCCESS)
        return status;

    gt.Stop();
    testTime = gt.GetElapsedTime();

    if(verify)
    {
        if(correctness)
        {
            std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }
    }

    return SDK_SUCCESS;
}

void 
TransferOverlap::printStats()
{
    if(!quiet)
    {
#ifdef _WIN32
        std::cout << "Build:               _WINxx" ;
#ifdef _DEBUG
        std::cout << " DEBUG";
#else
        std::cout <<  " release";
#endif
        std::cout << "\n" ;
#else
#ifdef NDEBUG
        std::cout << "Build:               release\n";
#else
        std::cout << "Build:               DEBUG\n";
#endif
#endif

        std::cout << "GPU work items:        " << nThreads << std::endl;
        std::cout << "Buffer size:           " << nBytes << std::endl;
        std::cout << "Timing loops:          " << nLoops << std::endl;
        std::cout << "Kernel loops:          " << nKLoops << std::endl;
        std::cout << "Wavefronts/SIMD:       " << numWavefronts << std::endl;
        std::cout << "memset/kernel overlap: " << (noOverlap ? "no" : "yes") << std::endl;
        std::cout << "inputBuffer:           ";

        for( int i = 0; i < sizeof(memFlags) / sizeof(memFlags[0]); i++)
           if(inFlags & memFlags[i].f)
               std::cout << memFlags[i].s;

        std::cout << std::endl;

        if(printLog) 
        {
            std:: cout << "\nLOOP ITERATIONS\n" 
                       << "---------------\n\n";
           timeLog->printLog();
        }

        std::cout <<
            "\nAVERAGES (over loops" << nSkip << " - " << nLoops - 1 << ", use -l to show complete log)\n" <<
            "--------\n\n";

        timeLog->printSummary(nSkip);
        std::cout << "\nComplete test time:" << testTime << " s\n\n";
    }
}

int 
TransferOverlap::cleanup()
{
    
    delete timeLog;
    

    return SDK_SUCCESS;
}


int TransferOverlap::setDeviceInfo(cl::Device device)
{
	cl_int status = CL_SUCCESS;

	//Get device type
	status = device.getInfo(CL_DEVICE_TYPE,&this->deviceInfo.dType);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_TYPE) failed");

	//Get vender ID
	
	status = device.getInfo(CL_DEVICE_VENDOR_ID,&this->deviceInfo.venderId);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_VENDOR_ID) failed");

	//Get max compute units
	
	status = device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS,&this->deviceInfo.maxComputeUnits);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_COMPUTE_UNITS) failed");

	//Get max work item dimensions
	
	status = device.getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,&this->deviceInfo.maxWorkItemDims);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS) failed");

	this->maxWorkItemSizes = std::vector< ::size_t>(this->deviceInfo.maxWorkItemDims);
	
	status = device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES,&this->maxWorkItemSizes);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS) failed");

	// Maximum work group size
    status = device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE,&this->deviceInfo.maxWorkGroupSize);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed");

	// Preferred vector sizes of all data types
	 status = device.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,&this->deviceInfo.preferredCharVecWidth);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR) failed");

	
	status = device.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,&this->deviceInfo.preferredShortVecWidth);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT) failed");

	
	status = device.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,&this->deviceInfo.preferredIntVecWidth);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT) failed");

	
	status = device.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,&this->deviceInfo.preferredLongVecWidth);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG) failed");

	
	status = device.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,&this->deviceInfo.preferredFloatVecWidth);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT) failed");

	
	status = device.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE,&this->deviceInfo.preferredDoubleVecWidth);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE) failed");

	
	status = device.getInfo(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,&this->deviceInfo.preferredHalfVecWidth);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF) failed");

	// Clock frequency
	
	status = device.getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY,&this->deviceInfo.maxClockFrequency);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_CLOCK_FREQUENCY) failed");

	// Address bits
	
	status = device.getInfo(CL_DEVICE_ADDRESS_BITS,&this->deviceInfo.addressBits);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_ADDRESS_BITS) failed");

	// Maximum memory alloc size
	
	status = device.getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE,&this->deviceInfo.maxMemAllocSize);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_MEM_ALLOC_SIZE) failed");

	

	// Device version
	
    status = device.getInfo(CL_DEVICE_VERSION,&this->deviceVersion);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_VERSION) failed");

	// Device extensions
	
	
	status = device.getInfo(CL_DEVICE_EXTENSIONS,&this->extensions);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_EXTENSIONS) failed");

	// Device OpenCL C version
	
	
	status = device.getInfo(CL_DEVICE_OPENCL_C_VERSION,&this->openclCVersion);
	CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_OPENCL_C_VERSION) failed");


	return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    TransferOverlap clTransferOverlap("OpenCL Constant Bandwidth");

    if(clTransferOverlap.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clTransferOverlap.parseExtraCommandLineOptions(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clTransferOverlap.isDumpBinaryEnabled())
    {
        return clTransferOverlap.genBinaryImage();
    }
    else
    {
        int state = clTransferOverlap.setup();
        if(state != SDK_SUCCESS)
            return (state == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

        if(clTransferOverlap.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        clTransferOverlap.printStats();
        if(clTransferOverlap.cleanup() != SDK_SUCCESS)
          return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}
