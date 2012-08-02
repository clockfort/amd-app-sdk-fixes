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


#include "ConstantBandwidth.hpp"
#include <string>

//Separator
std::string sep = "-----------------------------------------";

int 
ConstantBandwidth::setupConstantBandwidth()
{

    //Make vectorSize as 4 if -v option is 3. 

    //This memeory alignment is required as per OpenCL for type3 vectors 
    if(vectorSize == 3)
    {
        vec3 = true;
        vectorSize = 4;
    }

    // Allocate memory 
    cl_uint size = (WAVEFRONT + NUM_READS) * vectorSize * sizeof(cl_float);

    input = (cl_float*)malloc(size);
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    // Allocate memory for output buffer
    output = (cl_float*)malloc(length * vectorSize * sizeof(cl_float));
    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");

    if(verify)
    {
        cl_uint size = length * vectorSize * sizeof(cl_float);
        verificationOutput = (cl_float*)malloc(size);
        CHECK_ALLOCATION(verificationOutput, "Failed to allocate host memory. (verificationOutput)");
    }

    
    // random initialisation of input
    sampleCommon->fillRandom<cl_float>(input, 
                                       (WAVEFRONT + NUM_READS) * vectorSize, 
                                       1, 
                                       0, 
                                       10);

    // Random initialization of indices to kernel
    sampleCommon->fillRandom<cl_uint>(randomIndices, 
                                      NUM_READS + GROUP_SIZE, 
                                      1, 
                                      1, 
                                      256);

    return SDK_SUCCESS;
}

int 
ConstantBandwidth::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("ConstantBandwidth_Kernels.cl");
    
    // Pass vectorSize as DATATYPE to kernel
    cl_uint size = (WAVEFRONT + NUM_READS) * vectorSize * sizeof(cl_float);
    char buildOption[64];
    if(vectorSize == 1)
        sprintf(buildOption, "-D DATATYPE=float -D SIZE=%d ", size);
    else
        sprintf(buildOption, "-D DATATYPE=float%d -D SIZE=%d ", vec3 == true ? 3 : vectorSize, size);

    binaryData.flagsStr = std::string(buildOption);
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    CHECK_ERROR(status, SDK_SUCCESS, "OpenCL Generate Binary Image Failed");
    return SDK_SUCCESS;
}


int
ConstantBandwidth::setupCL(void)
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
    if(vStrVal.compare("1.0") >  0)
    {
        char openclVersion[1024];
        status = clGetDeviceInfo(devices[deviceId],
                                 CL_DEVICE_OPENCL_C_VERSION,
                                 sizeof(openclVersion),
                                 openclVersion,
                                 0);
        CHECK_OPENCL_ERROR(status, "clGetDeviceInfo failed.");
        
        std::string tempStr(deviceInfo.openclCVersion);
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

    cl_uint size = (WAVEFRONT + NUM_READS) * vectorSize * sizeof(cl_float);

    // Create constant buffer
    constantBuffer = clCreateBuffer(context, 
                                    CL_MEM_READ_ONLY,
                                    size,
                                    0, 
                                    &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (constantBuffer)");

    // Write data to buffer
    status = clEnqueueWriteBuffer(commandQueue,
                                  constantBuffer,
                                  1,
                                  0,
                                  size,
                                  input,
                                  0,
                                  0,
                                  0);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (constantBuffer)");

    // Random indices buffer
    randomIndicesBuffer = clCreateBuffer(context, 
                                         CL_MEM_READ_ONLY,
                                         (NUM_READS + GROUP_SIZE) * sizeof(cl_uint),
                                         0, 
                                         &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (randomIndicesBuffer)");

    // Write data to buffer
    status = clEnqueueWriteBuffer(commandQueue,
                                  randomIndicesBuffer,
                                  1,
                                  0,
                                  (NUM_READS + GROUP_SIZE) * sizeof(cl_uint),
                                  randomIndices,
                                  0,
                                  0,
                                  0);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (randomIndicesBuffer)");

    // Create output buffer
    outputBuffer = clCreateBuffer(context, 
                                  CL_MEM_WRITE_ONLY,
                                  sizeof(cl_float) * vectorSize * length,
                                  0, 
                                  &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffer)");

    // create a CL program using the kernel source

    // Pass vectorSize as DATATYPE to kernel
    char buildOption[64];
    if(vectorSize == 1)
        sprintf(buildOption, "-D DATATYPE=float -D SIZE=%d ", size);
    else
        sprintf(buildOption, "-D DATATYPE=float%d -D SIZE=%d ", vec3 == true ? 3 : vectorSize, size);

    // create a CL program using the kernel source
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("ConstantBandwidth_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string(buildOption);
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

    // ConstantBuffer bandwidth from single access
    kernel[0] = clCreateKernel(program, "constantBandwidth_single_static", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(constantBandwidth_single_static)");

    kernel[1] = clCreateKernel(program, "constantBandwidth_single_dynamic", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(constantBandwidth_single_dynamic)");

    // ConstantBuffer bandwidth from linear access
    kernel[2] = clCreateKernel(program, "constantBandwidth_linear", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(constantBandwidth_linear)");

    //ConstantBuffer bandwidth from random access
    kernel[3] = clCreateKernel(program, "constantBandwidth_random", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(constantBandwidth_random)");

    return SDK_SUCCESS;
}

int
ConstantBandwidth::bandwidth(cl_kernel &kernel)
{
    cl_int status;
    
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

    // Set appropriate arguments to the kernel 
    int argIndex = 0;

    if(dynamiArgFlag)
    {
        cl_uint index = 0;
        status = clSetKernelArg(kernel,
                                argIndex++,
                                sizeof(cl_uint), 
                                (void *)&index);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(index)");
    }
    
    status = clSetKernelArg(kernel,
                            argIndex++,
                            sizeof(cl_mem), 
                            (void *)&constantBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(constantBuffer)");

    status = clSetKernelArg(kernel,
                            argIndex++,
                            sizeof(cl_mem), 
                            (void *)&outputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(outputBuffer)");

    if(randomArgFlag)
    {
         status = clSetKernelArg(kernel,
                                 argIndex++,
                                 sizeof(cl_mem), 
                                 (void *)&randomIndicesBuffer);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(randomIndicesBuffer)");
    }

    double sec = 0;
    int iter = iterations;
    if(verify)
        iter = 1;


    // Run the kernel for a number of iterations
    for(int i = 0; i < iter; i++)
    {
        //Enqueue a kernel run call
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
        CHECK_OPENCL_ERROR(status, "clRelease Event Failed");
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

    return SDK_SUCCESS;
}

int 
ConstantBandwidth::runCLKernels(void)
{
    int status;

    if(vec3 == true)
        std::cout << "\nAccessType\t: single(static index)\nVectorElements\t: 3" << std::endl;
    else
        std::cout << "\nAccessType\t: single(static index)\nVectorElements\t: " << vectorSize << std::endl;

    std::cout << "Bandwidth\t";

    // Measure bandwidth of single reads(static index) from constant buffer 
    status = bandwidth(kernel[0]);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    if(verify)
    {
        int size = length * vectorSize * sizeof(cl_float);
        // Verify result for single access 
        memset(verificationOutput, 0, size);

        int index = 0;
        for(int i = 0; i < (int)length; i++)
        {
            for(int j = 0; j < NUM_READS; j++)
            {
                for(int k = 0; k < vectorSize; k++)
                    verificationOutput[i * vectorSize + k] += input[(index + j) * vectorSize + k];
            }
        }

        if(!memcmp(output, verificationOutput, size))
        {
            std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }
    }

    if(vec3 == true)
        std::cout << "\nAccessType\t: single(dynamic index)\nVectorElements\t: 3" << std::endl;
    else
        std::cout << "\nAccessType\t: single(dynamic index)\nVectorElements\t: " << vectorSize << std::endl;

    std::cout << "Bandwidth\t";
    dynamiArgFlag = true;

    // Measure bandwidth of single(dynamic index) reads from constant buffer 
    status = bandwidth(kernel[1]);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    if(verify)
    {
        int size = length * vectorSize * sizeof(cl_float);
        // Verify result for single access 
        if(!memcmp(output, verificationOutput, size))
        {
            std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }
    }

    if(vec3 == true)
        std::cout << "\nAccessType\t: linear\nVectorElements\t: 3" << std::endl;
    else
        std::cout << "\nAccessType\t: linear\nVectorElements\t: " << vectorSize << std::endl;

    std::cout << "Bandwidth\t";

    dynamiArgFlag = false;
    // Measure bandwidth of linear reads from constant buffer
    status = bandwidth(kernel[2]);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    if(verify)
    {
        int size = length * vectorSize * sizeof(cl_float);
        memset(verificationOutput, 0, size);
        // Verify result for Linear access
        for(int i = 0; i < (int)length; i++)
        {
            int index = i % 64;
            for(int j = 0; j < NUM_READS; j++)
            {
                for(int k = 0; k < vectorSize; k++)
                {
                    verificationOutput[i * vectorSize + k] += input[(index + j) * vectorSize + k];
                }
            }
        }

        if(!memcmp(output, verificationOutput, size))
        {
            std::cout << "Passed!\n" << std::endl;
        }
        else
        {
            std::cout << "Failed!\n" << std::endl;
            return SDK_FAILURE;
        }
    }

    std::cout << "\nAccessType\t: random\nVectorElements\t: " << vectorSize << std::endl;
    std::cout << "Bandwidth\t";

    randomArgFlag = true;
    // Measure bandwidth of linear reads from constant buffer 
    status = bandwidth(kernel[3]);
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    if(verify)
    {
        int size = length * vectorSize * sizeof(cl_float);
        memset(verificationOutput, 0, size);

        // Verify result for Random access 
        for(int i = 0; i < (int)length; i++)
        {
            int index = i % GROUP_SIZE;
            for(int j = 0; j < NUM_READS; j++)
            {
                int temp = randomIndices[index + j];
                for(int k = 0; k < vectorSize; k++)
                {
                    verificationOutput[i * vectorSize + k] += input[temp * vectorSize + k];
                }
            }
        }

        if(!memcmp(output, verificationOutput, size))
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
ConstantBandwidth::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "Memory Allocation failed (num_iterations)");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);
    delete num_iterations;

    streamsdk::Option* num_arguments = new streamsdk::Option;
    CHECK_ALLOCATION(num_arguments, "Memory Allocation failed (num_arguments)");

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
ConstantBandwidth::setup()
{
    if(setupConstantBandwidth() != SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    int status = setupCL();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_EXPECTED_FAILURE;
        
        return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    setupTime = (cl_double)sampleCommon->readTimer(timer);
    return SDK_SUCCESS;
}


int 
ConstantBandwidth::run()
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
ConstantBandwidth::verifyResults()
{
    return SDK_SUCCESS;
}


int 
ConstantBandwidth::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    for(int i = 0; i < NUM_KERNELS; i++)
    {
        status = clReleaseKernel(kernel[i]);
        CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");
    }

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");
 
    status = clReleaseMemObject(constantBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(outputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(randomIndicesBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseCommandQueue(commandQueue);
     CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // release program resources (input memory etc.)
    FREE(input);

    FREE(output);

    FREE(verificationOutput);

    // release device list
   FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    cl_int status = 0;
    ConstantBandwidth clConstantBandwidth("Constant BandWidth benchmark");

    if(clConstantBandwidth.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clConstantBandwidth.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clConstantBandwidth.isDumpBinaryEnabled())
    {
        return clConstantBandwidth.genBinaryImage();
    }

    status = clConstantBandwidth.setup();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_SUCCESS;

        return SDK_FAILURE;
    }

    if(clConstantBandwidth.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clConstantBandwidth.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;
    
    if(clConstantBandwidth.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    return SDK_SUCCESS;
}
