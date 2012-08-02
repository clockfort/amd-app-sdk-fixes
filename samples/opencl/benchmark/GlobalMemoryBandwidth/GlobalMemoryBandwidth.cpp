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


#include "GlobalMemoryBandwidth.hpp"

//Separator
std::string sep = "-----------------------------------------";

int 
GlobalMemoryBandwidth::setupGlobalMemoryBandwidth()
{
    //Make vectorSize as 4 if -v option is 3. 

    //This memeory alignment is required as per OpenCL for type3 vectors 
    if(vectorSize == 3)
    {
        vec3 = true;
        vectorSize = 4;
    }

    /**
     * Allocate memory required for global buffer 
     * This includes both single and linear(cached and uncached) reads 
     */
    cl_uint sizeElement = vectorSize * sizeof(cl_float);
    cl_uint readLength = length + (NUM_READS * 1024 / sizeElement) + EXTRA_BYTES;
    cl_uint size = readLength * vectorSize * sizeof(cl_float);

    input = (cl_float*)malloc(size);
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    outputReadSingle = (cl_float*)malloc(length * vectorSize * sizeof(cl_float));
    CHECK_ALLOCATION(outputReadSingle, "Failed to allocate host memory. (outputReadSingle)");

    memset(outputReadSingle, 0, length * vectorSize * sizeof(cl_float));

    outputReadLinear = (cl_float*)malloc(length * vectorSize * sizeof(cl_float));
    CHECK_ALLOCATION(outputReadLinear, "Failed to allocate host memory. (outputReadLinear)");

    memset(outputReadLinear, 0, length * vectorSize * sizeof(cl_float));

    outputReadLU = (cl_float*)malloc(length * vectorSize * sizeof(cl_float));
    CHECK_ALLOCATION(outputReadLU, "Failed to allocate host memory. (outputReadLU)");

    memset(outputReadLU, 0, length * vectorSize * sizeof(cl_float));

    outputWriteLinear = (cl_float*)malloc(size);
    CHECK_ALLOCATION(outputWriteLinear, "Failed to allocate host memory. (outputWriteLinear)");

    memset(outputWriteLinear, 0, size);

    // random initialisation of input 
    sampleCommon->fillRandom<cl_float>(input,  
                                       readLength * vectorSize, 
                                       1, 
                                       0, 
                                       10);

    return SDK_SUCCESS;
}

int 
GlobalMemoryBandwidth::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("GlobalMemoryBandwidth_Kernels.cl");

    // Pass vectorSize as DATATYPE to kernel
    char buildOption[128];
    if(vectorSize == 1)
        sprintf(buildOption, "-D DATATYPE=float -D OFFSET=%d ", OFFSET);
    else
        sprintf(buildOption, "-D DATATYPE=float%d -D OFFSET=%d ", (vec3 == true) ? 3 : vectorSize, OFFSET);


    binaryData.flagsStr = std::string(buildOption);
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    CHECK_ERROR(status, SDK_SUCCESS, "OpenCL Generate Binary Image Failed");
    return status;
}


int
GlobalMemoryBandwidth::setupCL(void)
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

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    std::string deviceStr(deviceInfo.deviceVersion);
    size_t vStart = deviceStr.find(" ", 0);
    size_t vEnd = deviceStr.find(" ", vStart + 1);
    std::string vStrVal = deviceStr.substr(vStart + 1, vEnd - vStart - 1);

#ifdef CL_VERSION_1_1
    if(vStrVal.compare("1.0") > 0)
    {
        char openclVersion[1024];
        status = clGetDeviceInfo(devices[deviceId],
                                 CL_DEVICE_OPENCL_C_VERSION,
                                 sizeof(openclVersion),
                                 openclVersion,
                                 0);
        CHECK_OPENCL_ERROR(status, "clGetDeviceInfo failed.");
        
        std::string tempStr(openclVersion);
        size_t dotPos = tempStr.find_first_of(".");
        size_t spacePos = tempStr.find_last_of(" ");
        tempStr = tempStr.substr(dotPos + 1, spacePos - dotPos);
        int minorVersion = atoi(tempStr.c_str());
        // OpenCL 1.1 has inbuilt support for vec3 data types
        if(minorVersion < 1 && vec3 == true)
        {
            OPENCL_EXPECTED_ERROR("Device doesn't support built-in 3 component vectors!");
        }
    }
    else
    {
        // OpenCL 1.1 has inbuilt support for vec3 data types
        if(vec3 == true)
        {
            OPENCL_EXPECTED_ERROR("Device doesn't support built-in 3 component vectors!");
        }
    }
#else
    // OpenCL 1.1 has inbuilt support for vec3 data types
    if(vec3 == true)
    {
        OPENCL_EXPECTED_ERROR("Device doesn't support built-in 3 component vectors!");
    }
#endif

    {
        // The block is to move the declaration of prop closer to its use 
        cl_command_queue_properties prop = 0;
        prop |= CL_QUEUE_PROFILING_ENABLE;

        commandQueue = clCreateCommandQueue(context, 
                                            devices[deviceId], 
                                            prop, 
                                            &status);
        CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");
    }

    cl_uint sizeElement = vectorSize * sizeof(cl_float);
    cl_uint readLength = length + (NUM_READS * 1024 / sizeElement) + EXTRA_BYTES;
    cl_uint size = readLength * vectorSize * sizeof(cl_float);

    // Create input buffer
    inputBuffer = clCreateBuffer(context, 
                                 CL_MEM_READ_ONLY,
                                 size,
                                 0, 
                                 &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer)");

    // Write data to buffer
    status = clEnqueueWriteBuffer(commandQueue,
                                  inputBuffer,
                                  1,
                                  0,
                                  size,
                                  input,
                                  0,
                                  0,
                                  0);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputBuffer)");

    outputBufferReadSingle = clCreateBuffer(context, 
                                            CL_MEM_WRITE_ONLY,
                                            sizeof(cl_float) * vectorSize * length,
                                            0, 
                                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBufferReadSingle)");

    // Write data to buffer
    status = clEnqueueWriteBuffer(commandQueue,
                                  outputBufferReadSingle,
                                  CL_TRUE,
                                  0,
                                  sizeof(cl_float) * vectorSize * length,
                                  outputReadSingle,
                                  0,
                                  NULL,
                                  NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (outputBufferReadSingle)");

    outputBufferReadLinear = clCreateBuffer(context, 
                                            CL_MEM_WRITE_ONLY,
                                            sizeof(cl_float) * vectorSize * length,
                                            0, 
                                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBufferReadLinear)");

    // Write data to buffer
    status = clEnqueueWriteBuffer(commandQueue,
                                  outputBufferReadLinear,
                                  CL_TRUE,
                                  0,
                                  sizeof(cl_float) * vectorSize * length,
                                  outputReadLinear,
                                  0,
                                  NULL,
                                  NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (outputBufferReadLinear)");

    outputBufferReadLU = clCreateBuffer(context, 
                                        CL_MEM_WRITE_ONLY,
                                        sizeof(cl_float) * vectorSize * length,
                                        0, 
                                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBufferReadLU)");

    // Write data to buffer 
    status = clEnqueueWriteBuffer(commandQueue,
                                  outputBufferReadLU,
                                  CL_TRUE,
                                  0,
                                  sizeof(cl_float) * vectorSize * length,
                                  outputReadLU,
                                  0,
                                  NULL,
                                  NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (outputBufferReadLU)");

     outputBufferWriteLinear = clCreateBuffer(context, 
                                              CL_MEM_WRITE_ONLY,
                                              size,
                                              0, 
                                              &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBufferWriteLinear)");

    // Write data to buffer 
    status = clEnqueueWriteBuffer(commandQueue,
                                  outputBufferWriteLinear,
                                  CL_TRUE,
                                  0,
                                  size,
                                  outputWriteLinear,
                                  0,
                                  NULL,
                                  NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (outputBufferWriteLinear)");

    // create a CL program using the kernel source
    char buildOption[128];
    if(vectorSize == 1)
        sprintf(buildOption, "-D DATATYPE=float -D OFFSET=%d ", OFFSET);
    else
        sprintf(buildOption, "-D DATATYPE=float%d -D OFFSET=%d ", (vec3 == true) ? 3 : vectorSize, OFFSET);

    // create a CL program using the kernel source
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("GlobalMemoryBandwidth_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string(buildOption);
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

    // Global memory bandwidth from read-single access
    kernel[0] = clCreateKernel(program, "read_single", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(read_single)");

    // Global memory  bandwidth from read-linear access
    kernel[1] = clCreateKernel(program, "read_linear", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(read_linear)");

    // Global memory  bandwidth from read-linear access
    kernel[2] = clCreateKernel(program, "read_linear_uncached", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(read_linear_uncached)");

    // Global memory  bandwidth from write-linear access
    kernel[3] = clCreateKernel(program, "write_linear", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(GlobalBandwidth_write_linear)");

    return SDK_SUCCESS;
}

int
GlobalMemoryBandwidth::bandwidth(cl_kernel &kernel, 
                             cl_mem &outputBuffer,
                             cl_float *output)
{
    cl_int status;
    cl_mem constValue =NULL;

    // Check group size against kernelWorkGroupSize
    status = clGetKernelWorkGroupInfo(kernel,
                                      devices[deviceId],
                                      CL_KERNEL_WORK_GROUP_SIZE,
                                      sizeof(size_t),
                                      &kernelWorkGroupSize,
                                      0);
    CHECK_OPENCL_ERROR(status, "clGetKernelWorkGroupInfo failed.");

    if(localThreads > kernelWorkGroupSize)
        localThreads = kernelWorkGroupSize;

    //Set appropriate arguments to the kernel
    int argIndex = 0;
    if(!writeFlag)
    {
        status = clSetKernelArg(kernel,
                                argIndex++,
                                sizeof(cl_mem), 
                                (void *)&inputBuffer);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(inputBuffer)");
    }
    else
    {
        // Pass a single constant value to kernel of type - float<vectorSize>
        cl_float *temp = (cl_float*)malloc(vectorSize * sizeof(cl_float));
        memset(temp, 0, vectorSize * sizeof(cl_float));

        constValue = clCreateBuffer(context,
                                           CL_MEM_READ_ONLY,
                                           vectorSize * sizeof(cl_float),
                                           0,
                                           &status);
        CHECK_OPENCL_ERROR(status, "clCreateBuffer failed.(constValue)");

        status = clEnqueueWriteBuffer(commandQueue,
                                      constValue,
                                      1,
                                      0,
                                      vectorSize * sizeof(cl_float),
                                      temp,
                                      0,
                                      0,
                                      0);
        if(sampleCommon->checkVal(status,
                                   CL_SUCCESS,
                                   "clEnqueueWriteBuffer failed.(constValue)"))
        {
            //Deallocate temp memory
            FREE(temp);
            return SDK_FAILURE;
        }

        // Deallocate temp memory
        FREE(temp);
        status = clSetKernelArg(kernel,
                                argIndex++,
                                sizeof(cl_mem), 
                                (void *)&constValue);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(constValue)");
    }

    status = clSetKernelArg(kernel,
                            argIndex++,
                            sizeof(cl_mem), 
                            (void *)&outputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(outputBuffer)");

    double sec = 0;
    int iter = iterations;
    if(verify)
        iter = 1;

    // Run the kernel for a number of iterations
    for(int i = 0; i < iter; i++)
    {
        // Enqueue a kernel run call
        cl_event ndrEvt;
        status = clEnqueueNDRangeKernel(commandQueue,
                                        kernel,
                                        1,
                                        NULL,
                                        &globalThreads,
                                        &localThreads,
                                        0,
                                        NULL,
                                        &ndrEvt);
        CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

        // wait for the kernel call to finish execution 
        status = clWaitForEvents(1, &ndrEvt);
        CHECK_OPENCL_ERROR(status, "clWaitForEvents failed.");

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

        // Cumulate time for each iteration 
        sec += 1e-9 * (endTime - startTime);

        status = clReleaseEvent(ndrEvt);
        CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo failed.(endTime)");
    }

    // Copy bytes 
    int bytesPerThread = 0;
    if(vec3 == true)
        bytesPerThread = NUM_READS * 3 * sizeof(cl_float);
    else
        bytesPerThread = NUM_READS * vectorSize * sizeof(cl_float);
    double bytes = (double)(iter * bytesPerThread);
    double perf = (bytes / sec) * 1e-9;
    perf *= globalThreads;
    
    std::cout << ": " << perf << " GB/s" << std::endl;

    // Enqueue readBuffer
    status = clEnqueueReadBuffer(commandQueue,
                                 outputBuffer,
                                 CL_TRUE,
                                 0,
                                 length * sizeof(cl_float) * vectorSize,
                                 output,
                                 0,
                                 NULL,
                                 0);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    if(constValue)
    {
        status = clReleaseMemObject(constValue);
        CHECK_OPENCL_ERROR(status, "clReleaseMemOnject failed.");
    }
    return SDK_SUCCESS;
}

int 
GlobalMemoryBandwidth::runCLKernels(void)
{
    int status;

    if(vec3 == true)
        std::cout << "\nGlobal Memory Read\nAccessType\t: single\nVectorElements\t: 3" << std::endl;
    else
        std::cout << "\nGlobal Memory Read\nAccessType\t: single\nVectorElements\t: " << vectorSize << std::endl;

    std::cout << "Bandwidth\t";

    // Measure bandwidth of single reads from global buffer
    status = bandwidth(kernel[0], outputBufferReadSingle, outputReadSingle);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    if(vec3 == true)
        std::cout << "\nGlobal Memory Read\nAccessType\t: linear\nVectorElements\t: 3" << std::endl;
    else
        std::cout << "\nGlobal Memory Read\nAccessType\t: linear\nVectorElements\t: " << vectorSize << std::endl;

    std::cout << "Bandwidth\t";

    // Measure bandwidth of linear reads from global buffer
    status = bandwidth(kernel[1], outputBufferReadLinear, outputReadLinear);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    if(vec3 == true)
        std::cout << "\nGlobal Memory Read\nAccessType\t: linear(uncached)\nVectorElements\t: 3" << std::endl;
    else
        std::cout << "\nGlobal Memory Read\nAccessType\t: linear(uncached)\nVectorElements\t: " << vectorSize << std::endl;

    std::cout << "Bandwidth\t";

    // Measure bandwidth of linear reads from global buffer 
    status = bandwidth(kernel[2], outputBufferReadLU, outputReadLU);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    if(vec3 == true)
        std::cout << "\nGlobal Memory Write\nAccessType\t: linear\nVectorElements\t: 3" << std::endl;
    else
        std::cout << "\nGlobal Memory Write\nAccessType\t: linear\nVectorElements\t: " << vectorSize << std::endl;

    std::cout << "Bandwidth\t";
    writeFlag = true;

    // Measure bandwidth of linear reads from global buffer 
    status = bandwidth(kernel[3], outputBufferWriteLinear, outputWriteLinear);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    return SDK_SUCCESS;
}


int 
GlobalMemoryBandwidth::initialize()
{
    // Call base class Initialize to get default configuration 
    if(this->SDKSample::initialize())
        return SDK_FAILURE;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations,"num_iterators memory allocation failed");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);
    delete num_iterations;

    streamsdk::Option* num_arguments = new streamsdk::Option;
    CHECK_ALLOCATION(num_arguments,"num_arguments memory allocation failed");

    num_arguments->_sVersion = "c";
    num_arguments->_lVersion = "components";
    num_arguments->_description = "Number of vector components";
    num_arguments->_type = streamsdk::CA_ARG_INT;
    num_arguments->_value = &vectorSize;

    sampleArgs->AddOption(num_arguments);
    delete num_arguments;

    return SDK_SUCCESS;
}

int 
GlobalMemoryBandwidth::setup()
{
    if(setupGlobalMemoryBandwidth()!=SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    int status = setupCL();
    if(status != SDK_SUCCESS)
        return status;

    sampleCommon->stopTimer(timer);

    setupTime = (cl_double)sampleCommon->readTimer(timer);

    return SDK_SUCCESS;
}


int 
GlobalMemoryBandwidth::run()
{
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);   

    // Arguments are set and execution call is enqueued on command buffer
    if(runCLKernels() != SDK_SUCCESS)
        return SDK_FAILURE;

    sampleCommon->stopTimer(timer);
    totalKernelTime = (double)(sampleCommon->readTimer(timer));

    return SDK_SUCCESS;
}

int 
GlobalMemoryBandwidth::verifyResults()
{
    if(verify)
    {
        int vecElements = (vec3 == true) ? 3 : vectorSize;
        // Verify result for single access
        verificationOutput = (cl_float*)malloc(length * vectorSize * sizeof(cl_float));
        CHECK_ALLOCATION(verificationOutput, "verificationOutput memory allocation failed");

        std::cout << "\nVerifying results for Read-Single : ";
        memset(verificationOutput, 0, length * vectorSize * sizeof(cl_float));

        int index = 0;
        for(int i = 0; i < (int)length; i++)
        {
            for(int j = 0; j < NUM_READS; j++)
            {
                for(int k = 0; k < vecElements; k++)
                    verificationOutput[i * vectorSize + k] += input[(index + j) * vectorSize + k];
            }
        }

        if(!memcmp(outputReadSingle, 
                   verificationOutput, 
                   length * vectorSize * sizeof(cl_float)))
        {
            std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }

        std::cout << "\nVerifying results for Read-Linear : ";
        memset(verificationOutput, 0, length * vectorSize * sizeof(cl_float));
        // Verify result for Linear access 
        for(int i = 0; i < (int)length; i++)
        {
            index = i;
            for(int j = 0; j < NUM_READS; j++)
            {
                for(int k = 0; k < vecElements; k++)
                    verificationOutput[i * vectorSize + k] += input[(index + j) * vectorSize + k];
            }
        }

        if(!memcmp(outputReadLinear, 
                   verificationOutput, 
                   length * vectorSize * sizeof(cl_float)))
        {
            std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }

        std::cout << "\nVerifying results for Read-Linear(uncached) : ";
        memset(verificationOutput, 0, length * vectorSize * sizeof(cl_float));
        // Verify result for Linear access
        int elementSize = sizeof(cl_float) * vectorSize;
        int stride = 512 / elementSize;
        for(int i = 0; i < (int)length; i++)
        {
            int readPos = i;
            for(int j = 0; j < NUM_READS; j++)
            {
                readPos += (4096 * vectorSize);
                for(int k = 0; k < vecElements; k++)
                {
                    verificationOutput[i * vectorSize + k] += input[readPos * vectorSize + k];
                }
            }
        }

        if(!memcmp(outputReadLU, 
                   verificationOutput, 
                   length * vectorSize * sizeof(cl_float)))
        {
           std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }

        std::cout << "\nVerifying results for Write-Linear : ";
        memset(verificationOutput, 0, length * vectorSize * sizeof(cl_float));
        if(!memcmp(outputWriteLinear, 
                   verificationOutput, 
                   length * vectorSize * sizeof(cl_float)))
        {
           std::cout << "Passed!\n" << std::endl;
            return SDK_SUCCESS;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }
    }
    return SDK_SUCCESS;
}


int 
GlobalMemoryBandwidth::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    for(int i = 0; i < NUM_KERNELS; i++)
    {
        status = clReleaseKernel(kernel[i]);
        CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");
    }

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");
 
    status = clReleaseMemObject(inputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBuffer)");

    status = clReleaseMemObject(outputBufferReadSingle);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(outputBufferReadSingle)");

    status = clReleaseMemObject(outputBufferReadLinear);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(outputBufferReadLinear)");

    status = clReleaseMemObject(outputBufferReadLU);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(outputBufferReadLU)");

    status = clReleaseMemObject(outputBufferWriteLinear);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(outputBufferWriteLinear)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed. (context)");

    // release program resources (input memory etc.)

    FREE(input);
    FREE(outputReadSingle);
    FREE(outputReadLinear);
    FREE(outputReadLU);
    FREE(outputWriteLinear);
    FREE(verificationOutput);
    FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    cl_int status = 0;
    GlobalMemoryBandwidth clGlobalMemoryBandwidth("OpenCL Reduction");

    if(clGlobalMemoryBandwidth.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clGlobalMemoryBandwidth.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clGlobalMemoryBandwidth.isDumpBinaryEnabled())
    {
        return clGlobalMemoryBandwidth.genBinaryImage();
    }

    status = clGlobalMemoryBandwidth.setup();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_SUCCESS;

        return SDK_FAILURE;
    }

    if(clGlobalMemoryBandwidth.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clGlobalMemoryBandwidth.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;
    
    if(clGlobalMemoryBandwidth.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    return SDK_SUCCESS;
}
