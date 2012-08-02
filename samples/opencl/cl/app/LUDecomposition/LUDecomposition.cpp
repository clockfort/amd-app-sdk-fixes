
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


#include "LUDecomposition.hpp"
#define VECTOR_SIZE 4
#define SIZE effectiveDimension * effectiveDimension * sizeof(double)

int LUD::setupLUD()
{
    /*
     *Changing dimension to a multiple of block if it is not.
     *Currently execution will be done on the effective dimention
     *and results will be shown upto what user requests 
     */


    if(actualDimension % VECTOR_SIZE != 0)
        effectiveDimension = actualDimension 
                                - (actualDimension % VECTOR_SIZE) 
                                + VECTOR_SIZE;
    else
        effectiveDimension = actualDimension;

    blockSize = effectiveDimension / VECTOR_SIZE;

#ifdef _WIN32
    input = static_cast<double*>(_aligned_malloc(SIZE, 4096));
#else
    input = static_cast<double*>(memalign(4096, SIZE));
#endif
    CHECK_ALLOCATION(input, "Unable to allocate input memory");

    //initialize with random double type elements
    sampleCommon->fillRandom<double>(
                        input,
                        effectiveDimension, 
                        effectiveDimension,
                        1,
                        2,
                        1);

#ifdef _WIN32
    matrixGPU = static_cast<double*>(_aligned_malloc(SIZE, 4096));
#else
    matrixGPU = static_cast<double*>(memalign(4096, SIZE));
#endif
    CHECK_ALLOCATION(matrixGPU, "Unable to allocate memory for GPU input");
    
    if(verify)
    {
        matrixCPU = static_cast<double*>(malloc(SIZE));
        CHECK_ALLOCATION(matrixCPU, "Unable to allocate memory for refernce implementation");
        memcpy((void*)matrixCPU, (const void*)input, SIZE);
    }

    if(!quiet)
    {
        sampleCommon->printArray<double>(
                        "Input Matrix",
                        input,
                        actualDimension,
                        actualDimension);
    }

    return SDK_SUCCESS;
}

int LUD::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("LUDecomposition_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}

int LUD::setupCL(void)
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
    CHECK_ERROR(retValue, SDK_SUCCESS, "SDKDeviceInfo::setDeviceInfo() failed");

    std::string buildOptions = std::string("");

    // Check if cl_khr_fp64 extension is supported
    if(strstr(deviceInfo.extensions, "cl_khr_fp64"))
    {
        buildOptions.append("-D KHR_DP_EXTENSION");
    }
    else
    {
        // Check if cl_amd_fp64 extension is supported
        if(!strstr(deviceInfo.extensions, "cl_amd_fp64"))
        {
            OPENCL_EXPECTED_ERROR("Device does not support cl_amd_fp64 extension!");
        }
    }

    //Local Memory Required in worst case
    localMemoryNeeded = (cl_ulong)((deviceInfo.maxWorkGroupSize / 2) * sizeof(cl_double));

    status = setupLUD();
    if(status != SDK_SUCCESS)
        return status;  

    //Creating Buffers
    inplaceBuffer = clCreateBuffer(
                        context,
                        CL_MEM_READ_WRITE,
                        sizeof(double) * effectiveDimension * effectiveDimension,
                        NULL,
                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (inputBuffer)");

    inputBuffer2 = clCreateBuffer(
                        context,
                        CL_MEM_READ_WRITE,
                        sizeof(double) * effectiveDimension * effectiveDimension,
                        NULL,
                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (outputBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("LUDecomposition_Kernels.cl");
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
    kernelLUD = clCreateKernel(program, "kernelLUDecompose", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed. : kernelLUD");
    
    kernelCombine = clCreateKernel(program, "kernelLUCombine", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed. : kernelCombine");

    return SDK_SUCCESS;
}

int LUD::runCLKernels(void)
{
    cl_int   status;
    cl_int eventStatus = CL_QUEUED; 
    cl_event ndrEvt,ndrEvt1;
    cl_event inMapEvt;
    cl_event inUnmapEvt;
    cl_event outMapEvt;
    cl_event outUnmapEvt;
    void* inMapPtr = NULL;
    void* outMapPtr = NULL;

    inMapPtr = clEnqueueMapBuffer(
                        commandQueue, 
                        inplaceBuffer, 
                        CL_FALSE, 
                        CL_MAP_WRITE, 
                        0, 
                        SIZE, 
                        0, 
                        NULL, 
                        &inMapEvt, 
                        &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inplaceBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&inMapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inMapEvt) Failed");

    memcpy(inMapPtr, input, SIZE);
    status = clEnqueueUnmapMemObject(
                commandQueue, 
                inplaceBuffer, 
                inMapPtr, 
                0, 
                NULL, 
                &inUnmapEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (inplaceBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.(commandQueue)");

    status = sampleCommon->waitForEventAndRelease(&inUnmapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(inUnmapEvt) Failed");

    size_t globalThreads[2]= {blockSize, effectiveDimension};
    size_t localThreads[2] = {blockSize, 1};
    size_t offset[2] = {0, 0};

    /*
     *Querying some important device paramneters
     *Finding out the appropritate Work group size for the kernel	 
     * See if local memory can be used 
     * Check group size against kernelWorkGroupSize 
     */

    status =  kernelInfo.setKernelWorkGroupInfo(kernelLUD, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    /*
     *Setting Common Kernel Arguments
     */
    {
        //kernelLUD
        // 1st kernel argument - output 
        status = clSetKernelArg(
                        kernelLUD,
                        0,
                        sizeof(cl_mem),
                        (void *)&inputBuffer2);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. :kernelLUD(inputBuffer2)");

        // 2nd kernel argument - input 
        status = clSetKernelArg(
                        kernelLUD,
                        1,
                        sizeof(cl_mem),
                        (void *)&inplaceBuffer);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. :kernelLUD(inplaceBuffer)");
    }

    for(int index = 0;index < effectiveDimension - 1;++index)
    {
        /*
         *  code to find :
         *  1. new offset 
         *  2. Local Work group size
         *  3. Global ND range
         */
        if(index % VECTOR_SIZE == 0)
        {
            offset[0] = (index / VECTOR_SIZE);
            offset[1] = VECTOR_SIZE * (index / VECTOR_SIZE);
        
            if(!index)
            {
                globalThreads[0] += 1;
                globalThreads[1] += VECTOR_SIZE;
            }
            globalThreads[0] -= 1;
            globalThreads[1] -= VECTOR_SIZE;
        
            if(globalThreads[0] <= kernelInfo.kernelWorkGroupSize)
                localThreads[0] = globalThreads[0];
            else
            {
                size_t temp = (int)kernelInfo.kernelWorkGroupSize;
                for(;temp > 1;temp--)
                {
                    if(globalThreads[0] % temp == 0)
                        break;
                }
                localThreads[0] = temp;
            }

            if( globalThreads[1] < kernelInfo.kernelWorkGroupSize / localThreads[0])
                localThreads[1] = globalThreads[1];
            else
            {
                size_t temp = kernelInfo.kernelWorkGroupSize / localThreads[0];
                for(;temp > 1; temp--)
                {
                    if(globalThreads[1] % temp == 0)
                        break;
                }
                localThreads[1] = temp;
            }
        }
        
        // 3rd kernel argument - index for the matrix 
        status = clSetKernelArg(
                        kernelLUD,
                        2,
                        sizeof(cl_uint),
                        &index);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. :kernelLUD(index)");

        // 4th kernel argument - local spcae for holding ratio  
        status = clSetKernelArg(
                        kernelLUD,
                        3,
                        sizeof(cl_double) * localThreads[1],
                        NULL);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. :kernelLUD(localBuffer)");

        /* 
         * Enqueue a kernel run call.
         * kernelLUD
         */
        status = clEnqueueNDRangeKernel(
                        commandQueue,
                        kernelLUD,
                        2,
                        offset,
                        globalThreads,
                        localThreads,
                        0,
                        NULL,
                        &ndrEvt);
        CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

        // wait for the kernel call to finish execution 
        status = clFlush(commandQueue);
        CHECK_OPENCL_ERROR(status, "clFlush failed.");

        status = clWaitForEvents(1,&ndrEvt);
        CHECK_OPENCL_ERROR(status, "WaitForEvent(ndrEvt) Failed");
    }

    /*
     * This will combine the L & U matrices at the GPU side
     * so that they can be bought in CPU space as a single matrix
     */

    // 2nd kernel argument - L matrix 
    status = clSetKernelArg(
                    kernelCombine, 
                    0, 
                    sizeof(cl_mem), 
                    (void *)&inputBuffer2);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. :kernelCombine(inputBuffer2)");

    // 2nd kernel argument - inplace matrix 
    status = clSetKernelArg(
                    kernelCombine,
                    1,
                    sizeof(cl_mem),
                    (void *)&inplaceBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. :kernelCombine(inplaceBuffer)");
   
    globalThreads[0] = effectiveDimension;
    globalThreads[1] = effectiveDimension;

    status = clEnqueueNDRangeKernel(
                    commandQueue,
                    kernelCombine,
                    2,
                    NULL,
                    globalThreads,
                    NULL,
                    0,
                    NULL,
                    &ndrEvt1);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed. ");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt1);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt1) Failed");

    // Get final matrix data 
    outMapPtr = clEnqueueMapBuffer(
                        commandQueue, 
                        inplaceBuffer, 
                        CL_FALSE, 
                        CL_MAP_READ, 
                        0, 
                        SIZE, 
                        0, 
                        NULL, 
                        &outMapEvt, 
                        &status);
    CHECK_OPENCL_ERROR(status, "clEnqueueMapBuffer failed. (inplaceBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&outMapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outMapEvt) Failed");

    memcpy(matrixGPU, outMapPtr, SIZE);

    status = clEnqueueUnmapMemObject(
                commandQueue, 
                inplaceBuffer, 
                outMapPtr, 
                0, 
                NULL, 
                &outUnmapEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueUnmapMemObject failed. (inplaceBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&outUnmapEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(outUnmapEvt) Failed");

    return SDK_SUCCESS;
}

void LUD::LUDCPUReference(double* matrixCPU, const cl_uint effectiveDimension)
{
    for(unsigned int d = 0 ; d < effectiveDimension-1 ; d++)
    {
        for(unsigned i = d + 1 ;i < effectiveDimension; i++)
        {
            double ratio = matrixCPU[i * effectiveDimension + d]
                            / matrixCPU[d * effectiveDimension + d];
            for(unsigned int j = d; j < effectiveDimension; j++)
            { 
                matrixCPU[ i * effectiveDimension + j] 
                    -= matrixCPU[d * effectiveDimension + j] * ratio;
                if(j == d)
                    matrixCPU[i * effectiveDimension + j] = ratio;
            }
        }
    }
}

int LUD::initialize()
{
    // Call base class Initialize to get default configuration 
    if(this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    // add command line option for matrix actualDimension 
    streamsdk::Option* xParam = new streamsdk::Option;
    CHECK_ALLOCATION(xParam, "Memory Allocation error.\n");

    xParam->_sVersion = "x";
    xParam->_lVersion = "dimension";
    xParam->_description = "actualDimension of input matrix";
    xParam->_type     = streamsdk::CA_ARG_INT;
    xParam->_value    = &actualDimension;

    sampleArgs->AddOption(xParam);
    delete xParam;

    streamsdk::Option* iter = new streamsdk::Option;
    CHECK_ALLOCATION(iter, "Memory allocation for Option failed\n");

    iter->_sVersion = "i";
    iter->_lVersion = "iterations";
    iter->_description = "Number of iteration of kernel";
    iter->_type = streamsdk::CA_ARG_INT;
    iter->_value = &iterations;

    sampleArgs->AddOption(iter);
    delete iter;

    return SDK_SUCCESS;
}


int LUD::setup()
{  
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    cl_int status = setupCL();
    if(status != SDK_SUCCESS)
        return status;

    sampleCommon->stopTimer(timer);
    setupTime = (cl_double)sampleCommon->readTimer(timer);

    return SDK_SUCCESS;
}


int LUD::run()
{
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    std::cout << "Executing kernel for " 
              << iterations << " iterations" << std::endl;
    std::cout << "-------------------------------------------"
              << std::endl;

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    totalKernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet)
        sampleCommon->printArray<double>(
                            "LU Matrix GPU implementation", 
                            matrixGPU, 
                            actualDimension, 
                            actualDimension);

    return SDK_SUCCESS;
}

int LUD::verifyResults()
{
    if(verify)
    {
        //reference implementation
        int refTimer = sampleCommon->createTimer();
        sampleCommon->resetTimer(refTimer);
        sampleCommon->startTimer(refTimer);
        LUDCPUReference(matrixCPU, effectiveDimension);
        sampleCommon->stopTimer(refTimer);
        referenceKernelTime = sampleCommon->readTimer(refTimer);

        if(!quiet)
            sampleCommon->printArray<double>(
                                "LU Matrix CPU Reference",
                                matrixCPU,
                                actualDimension,
                                actualDimension);

        // compare the results and see if they match 
        if(sampleCommon->compare(
                matrixGPU, 
                matrixCPU, 
                effectiveDimension * effectiveDimension))
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

void LUD::printStats()
{
    std::string strArray[4] = {"WxH" , "Time(sec)", "[Transfer+Kernel]Time(sec)"};
    std::string stats[3];

    totalTime = setupTime + totalKernelTime;

    stats[0]  = sampleCommon->toString(actualDimension, std::dec)
                + "x" + sampleCommon->toString(actualDimension, std::dec);
    stats[1]  = sampleCommon->toString(totalTime, std::dec);
    stats[2]  = sampleCommon->toString(totalKernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 3);
}


int LUD::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseKernel(kernelLUD);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernelLUD)");

    status = clReleaseKernel(kernelCombine);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernelCombine)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseMemObject(inplaceBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inplaceBuffer)");

    status = clReleaseMemObject(inputBuffer2);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBuffer2)");

    status = clReleaseCommandQueue(commandQueue);
     CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.(context)");

    // release program resources (input memory etc.) 
#ifdef WIN32
    ALIGNED_FREE(input);
    
    ALIGNED_FREE(matrixGPU);
#else
    FREE(input);
    
    FREE(matrixGPU);
#endif

    FREE(matrixCPU);
    FREE(devices);

    return SDK_SUCCESS;
}


int main(int argc, char * argv[])
{
    LUD clLUDecompose("OpenCL LU Decomposition");

    // Initialize
    if(clLUDecompose.initialize()!= SDK_SUCCESS)
        return SDK_FAILURE;

    if(clLUDecompose.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clLUDecompose.isDumpBinaryEnabled())
    {
        return clLUDecompose.genBinaryImage();
    }
    else
    {
        // Setup
        int status = clLUDecompose.setup();
        if(status != SDK_SUCCESS)
            return (status == SDK_EXPECTED_FAILURE) ? SDK_SUCCESS : SDK_FAILURE;

        // Run
        if(clLUDecompose.run() != SDK_SUCCESS)
            return SDK_FAILURE;

        // VerifyResults
        if(clLUDecompose.verifyResults() != SDK_SUCCESS)
                return SDK_FAILURE;

        // Cleanup
        if(clLUDecompose.cleanup() != SDK_SUCCESS)
            return SDK_FAILURE;
        clLUDecompose.printStats();
    }

    return SDK_SUCCESS;
}

