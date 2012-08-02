/* ============================================================

Copyright (c) 2011 Advanced Micro Devices, Inc.  All rights reserved.
 
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

#include "AtomicCounters.hpp"


int 
AtomicCounters::setupAtomicCounters()
{

    // Make sure length is multiples of GROUP_SIZE
    length = (length / GROUP_SIZE);
    length = length ? length * GROUP_SIZE : GROUP_SIZE;

    // Allocate the memory for input array
    input = (cl_uint*)malloc(length * sizeof(cl_uint));
    CHECK_ALLOCATION(input, "Allocation failed(input)");

    // Set the input data 
    value = 2;
    for(cl_uint i = 0; i < length; ++i)
        input[i] = (cl_uint)(rand() % 5);

    if(!quiet)
        sampleCommon->printArray<cl_uint>("Input Arry", input, 256, 1);

    return SDK_SUCCESS;
}

int 
AtomicCounters::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("AtomicCounters_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int
AtomicCounters::setupCL(void)
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
            std::cout << "GPU not found. Falling back to CPU" << std::endl;
            dType = CL_DEVICE_TYPE_CPU;
        }
    }

    cl_platform_id platform = NULL;
    int retValue = sampleCommon->getPlatform(platform, platformId, isPlatformEnabled());
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::getPlatform() failed.");
    
    // Display available devices.
    retValue = sampleCommon->displayDevices(platform, dType);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::displayDevices() failed.");

    cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0};
    context = clCreateContextFromType(cps,
                                      dType,
                                      NULL,
                                      NULL,
                                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed.");

    // getting device on which to run the sample
    status = sampleCommon->getDevices(context, &devices, deviceId, isDeviceIdEnabled());
    CHECK_ERROR(status, SDK_SUCCESS, "sampleCommon::getDevices() failed ");

    //Set device info of given cl_device_id
    retValue = deviceInfo.setDeviceInfo(devices[deviceId]);
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed" );

    // Check device extensions
    if(!strstr(deviceInfo.extensions, "cl_ext_atomic_counters_32"))
        OPENCL_EXPECTED_ERROR("Device does not support cl_ext_atomic_counters_32 extension!");

    if(!strstr(deviceInfo.extensions, "cl_khr_local_int32_base_atomics"))
        OPENCL_EXPECTED_ERROR("Device does not support cl_khr_local_int32_base_atomics extension!");

    // Get OpenCL device version
    std::string deviceVersionStr = std::string(deviceInfo.deviceVersion);
    size_t vStart = deviceVersionStr.find(" ", 0);
    size_t vEnd = deviceVersionStr.find(" ", vStart + 1);
    std::string vStrVal = deviceVersionStr.substr(vStart + 1, vEnd - vStart - 1);

    // Check of OPENCL_C_VERSION if device version is 1.1 or later
#ifdef CL_VERSION_1_1
    if(deviceInfo.openclCVersion)
    {
        // Exit if OpenCL C device version is 1.0
        deviceVersionStr = std::string(deviceInfo.openclCVersion);
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
    if(setupAtomicCounters() != SDK_SUCCESS)
        return SDK_FAILURE;

    cl_command_queue_properties props = CL_QUEUE_PROFILING_ENABLE;
    commandQueue = clCreateCommandQueue(context, devices[deviceId], props, &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed(commandQueue)");

    // Set Presistent memory only for AMD platform
    cl_mem_flags inMemFlags = CL_MEM_READ_ONLY;
    if(isAmdPlatform())
        inMemFlags |= CL_MEM_USE_PERSISTENT_MEM_AMD;

    // Create buffer for input array
    inBuf = clCreateBuffer(context, inMemFlags, length * sizeof(cl_uint), NULL, &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed.(inBuf)");

    // Set up data for input array
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                inBuf,
                CL_FALSE,
                0,
                length * sizeof(cl_uint),
                input,
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer(inBuf) failed..");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush(commandQueue) failed.");

    counterOutBuf = clCreateBuffer(
                        context, 
                        CL_MEM_READ_WRITE,
                        sizeof(cl_uint), 
                        NULL, 
                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed.(counterOutBuf).");

    globalOutBuf = clCreateBuffer(
                        context, 
                        CL_MEM_READ_WRITE,
                        sizeof(cl_uint),
                        NULL,
                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed.(globalOutBuf).");

    
    // create a CL program using the kernel source
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("AtomicCounters_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string("");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, SDK_SUCCESS, "sampleCommon::buildOpenCLProgram() failed");

    // ConstantBuffer bandwidth from single access
    counterKernel = clCreateKernel(program, "atomicCounters", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.(counterKernel).");

    globalKernel = clCreateKernel(program, "globalAtomics", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel(globalKernel) failed.");

    status = kernelInfoC.setKernelWorkGroupInfo(counterKernel, devices[deviceId]);
    CHECK_OPENCL_ERROR(status, "kernelInfo.setKernelWorkGroupInfo failed");

    status = kernelInfoG.setKernelWorkGroupInfo(globalKernel, devices[deviceId]);
    CHECK_OPENCL_ERROR(status, "kernelInfo.setKernelWorkGroupInfo failed");

    if(counterWorkGroupSize > kernelInfoC.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << counterWorkGroupSize << std::endl;
            std::cout << "Max Group Size supported on the kernel(readKernel) : " 
                      << kernelInfoC.kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelInfoC.kernelWorkGroupSize << std::endl;
        }
        counterWorkGroupSize = kernelInfoC.kernelWorkGroupSize;
    }

    
    if(globalWorkGroupSize > kernelInfoG.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << globalWorkGroupSize << std::endl;
            std::cout << "Max Group Size supported on the kernel(writeKernel) : " 
                      << kernelInfoG.kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelInfoG.kernelWorkGroupSize << std::endl;
        }
        globalWorkGroupSize = kernelInfoG.kernelWorkGroupSize;
    }

    // Wait for event and release event
    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_OPENCL_ERROR(status, "waitForEventAndRelease(writeEvt) failed.");

    return SDK_SUCCESS;
}

int 
AtomicCounters::initialize()
{
    // Call base class Initialize to get default configuration 
    CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL Resources Initialization failed");

    streamsdk::Option* array_length = new streamsdk::Option;
    CHECK_ALLOCATION(array_length, "Allocation failed(array_length)");

    array_length->_sVersion = "x";
    array_length->_lVersion = "length";
    array_length->_description = "Length of the Input array";
    array_length->_type = streamsdk::CA_ARG_INT;
    array_length->_value = &length;
    sampleArgs->AddOption(array_length);
    delete array_length;


    streamsdk::Option* numLoops = new streamsdk::Option;
    CHECK_ALLOCATION(numLoops, "Allocation failed(numLoops)");

    numLoops->_sVersion = "i";
    numLoops->_lVersion = "iterations";
    numLoops->_description = "Number of timing loops";
    numLoops->_type = streamsdk::CA_ARG_INT;
    numLoops->_value = &iterations;

    sampleArgs->AddOption(numLoops);
    delete numLoops;

    return SDK_SUCCESS;
}

int 
AtomicCounters::setup()
{
    int status = setupCL();
    if(status != SDK_SUCCESS)
    {
        if(status == SDK_EXPECTED_FAILURE)
            return SDK_EXPECTED_FAILURE;
        
        return SDK_FAILURE;
    }
    return SDK_SUCCESS;
}


void
AtomicCounters::cpuRefImplementation()
{
    for(cl_uint i = 0; i < length; ++i)
        if(value == input[i])
            refOut++;
}

int AtomicCounters::verifyResults()
{
    if(verify)
    {
        // Calculate the reference output
        cpuRefImplementation();

        // Compare the results and see if they match
        if(refOut == counterOut && refOut == globalOut)
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

int 
AtomicCounters::runAtomicCounterKernel()
{
    cl_int status = CL_SUCCESS;

    // Set Global and Local work items
    size_t globalWorkItems = length;
    size_t localWorkItems = counterWorkGroupSize;

    // Initialize the counter value
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                counterOutBuf,
                CL_FALSE,
                0,
                sizeof(cl_uint),
                &initValue,
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer(counterOutBuf) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush(commandQueue)failed.");

    // Wait for event and release event
    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_OPENCL_ERROR(status, "waitForEventAndRelease(writeEvt) failed.");

    // Set kernel arguments
    status = clSetKernelArg(counterKernel, 0, sizeof(cl_mem), &inBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(inBuf) failed.");

    status = clSetKernelArg(counterKernel, 1, sizeof(cl_uint), &value);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(value) failed.");

    status = clSetKernelArg(counterKernel, 2, sizeof(cl_mem), &counterOutBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(counterOutBuf) failed.");

    // Run Kernel
    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(
                commandQueue,
                counterKernel,
                1,
                NULL,
                &globalWorkItems,
                &localWorkItems,
                0,
                NULL,
                &ndrEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel(counterKernel) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush(commandQueue) failed.");

    cl_int eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        ndrEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetEventInfo(ndrEvt) failed.");
    }

    cl_ulong startTime;
    cl_ulong endTime;
    
    // Get profiling information
    status = clGetEventProfilingInfo(
                ndrEvt,
                CL_PROFILING_COMMAND_START,
                sizeof(cl_ulong),
                &startTime,
                NULL);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo(CL_PROFILING_COMMAND_START) failed.");

    status = clGetEventProfilingInfo(
                ndrEvt,
                CL_PROFILING_COMMAND_END,
                sizeof(cl_ulong),
                &endTime,
                NULL);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo(CL_PROFILING_COMMAND_END) failed.");

    double sec = 1e-9 * (endTime - startTime);
    kTimeAtomCounter += sec;

    status = clReleaseEvent(ndrEvt);
    CHECK_OPENCL_ERROR(status, "clReleaseEvent(ndrEvt) failed.");

    // Get the occurrences of Value from atomicKernel
    cl_event readEvt;
    status = clEnqueueReadBuffer(
                commandQueue,
                counterOutBuf,
                CL_FALSE,
                0,
                sizeof(cl_uint),
                &counterOut,
                0,
                NULL,
                &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(counterOutBuf) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush() failed.");

    // Wait for event and release event
    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_OPENCL_ERROR(status, "waitForEventAndRelease(readEvt) failed.");

    return SDK_SUCCESS;
}


int 
AtomicCounters::runGlobalAtomicKernel()
{
    cl_int status = CL_SUCCESS;

    // Set Global and Local work items
    size_t globalWorkItems = length;
    size_t localWorkItems = globalWorkGroupSize;

    // Initialize the counter value
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                globalOutBuf,
                CL_FALSE,
                0,
                sizeof(cl_uint),
                &initValue,
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer(globalOutBuf) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush() failed.");

    // Wait for event and release event
    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_OPENCL_ERROR(status, "waitForEventAndRelease(writeEvt) failed.");

    // Set kernel arguments
    status = clSetKernelArg(globalKernel, 0, sizeof(cl_mem), &inBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(inBuf) failed.");

    status = clSetKernelArg(globalKernel, 1, sizeof(cl_uint), &value);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(value) failed.");

    status = clSetKernelArg(globalKernel, 2, sizeof(cl_mem), &globalOutBuf);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg(globalOutBuf) failed.");

    // Run Kernel
    cl_event ndrEvt;
    status = clEnqueueNDRangeKernel(
                commandQueue,
                globalKernel,
                1,
                NULL,
                &globalWorkItems,
                &localWorkItems,
                0,
                NULL,
                &ndrEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel(globalKernel) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush(commandQueue) failed.");

    cl_int eventStatus = CL_QUEUED;
    while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        ndrEvt, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetEventInfo(ndrEvt) failed.");
    }

    cl_ulong startTime;
    cl_ulong endTime;
    
    // Get profiling information
    status = clGetEventProfilingInfo(
                ndrEvt,
                CL_PROFILING_COMMAND_START,
                sizeof(cl_ulong),
                &startTime,
                NULL);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo(CL_PROFILING_COMMAND_START) failed.");

    status = clGetEventProfilingInfo(
                ndrEvt,
                CL_PROFILING_COMMAND_END,
                sizeof(cl_ulong),
                &endTime,
                NULL);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo(CL_PROFILING_COMMAND_END) failed.");

    double sec = 1e-9 * (endTime - startTime);
    kTimeAtomGlobal += sec;

    status = clReleaseEvent(ndrEvt);
    CHECK_OPENCL_ERROR(status, "clReleaseEvent(ndrEvt) failed.");

    // Get the occurrences of Value from atomicKernel
    cl_event readEvt;
    status = clEnqueueReadBuffer(
                commandQueue,
                globalOutBuf,
                CL_FALSE,
                0,
                sizeof(cl_uint),
                &globalOut,
                0,
                NULL,
                &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer(globalOutBuf) failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush() failed.");

    // Wait for event and release event
    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_OPENCL_ERROR(status, "waitForEventAndRelease(readEvt) failed.");

    return SDK_SUCCESS;
}

int 
AtomicCounters::run()
{
    // Warm up Atomic counter kernel
    for(int i = 0; i < 2 && iterations != 1; i++)
        if(runAtomicCounterKernel())
            return SDK_FAILURE;

    std::cout << "Executing Kernels for " << iterations << " iterations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    kTimeAtomCounter = 0;
    // Run the kernel for a number of iterations
    for(int i = 0; i < iterations; i++)
         if(runAtomicCounterKernel())
             return SDK_FAILURE;

    // Compute total time
    kTimeAtomCounter /= iterations;

    if(!quiet)
        sampleCommon->printArray<cl_uint>("Atomic Counter Output", &counterOut, 1, 1);

    // Warm up Global atomics kernel
    for(int i = 0; i < 2 && iterations != 1; i++)
        if(runGlobalAtomicKernel())
            return SDK_FAILURE;
        
    kTimeAtomGlobal = 0;
    // Run the kernel for a number of iterations
    for(int i = 0; i < iterations; i++)
        if(runGlobalAtomicKernel())
            return SDK_FAILURE;
        
    // Compute total time
    kTimeAtomGlobal /= iterations;

    if(!quiet)
        sampleCommon->printArray<cl_uint>("Global atomics Output", &globalOut, 1, 1);

    return SDK_SUCCESS;
}

void 
AtomicCounters::printStats()
{
    std::string strArray[4] = {"Elements", "Occurrences", "AtomicsCounter(sec)", "GlobalAtomics(sec)"};
    std::string stats[4];
    
    stats[0]  = sampleCommon->toString(length, std::dec);
    stats[1]  = sampleCommon->toString(counterOut, std::dec);
    stats[2]  = sampleCommon->toString(kTimeAtomCounter, std::dec);
    stats[3]  = sampleCommon->toString(kTimeAtomGlobal, std::dec);
    
    this->SDKSample::printStats(strArray, stats, 4);
}

int 
AtomicCounters::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseMemObject(inBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject(inBuf) failed.");

    status = clReleaseMemObject(counterOutBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject(counterOutBuf) failed.");

    status = clReleaseMemObject(globalOutBuf);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject(globalOutBuf) failed.");

    status = clReleaseKernel(counterKernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel(counterKernel) failed.");

    status = clReleaseKernel(globalKernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel(globalKernel) failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram(program) failed.");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue(commandQueue) failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext(context) failed.");

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    int status = 0;
    AtomicCounters clAtomicCounters("OpenCL Atomic Counter");
    
     if(clAtomicCounters.initialize() != SDK_SUCCESS)
         return SDK_FAILURE;
    
    if(clAtomicCounters.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clAtomicCounters.isDumpBinaryEnabled())
    {
        return clAtomicCounters.genBinaryImage();
    }
    
    status = clAtomicCounters.setup();
    if(status != SDK_SUCCESS)
        return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;
    
    if(clAtomicCounters.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clAtomicCounters.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clAtomicCounters.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;
    
    clAtomicCounters.printStats();

    return SDK_SUCCESS;
}
