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


#include "DCT.hpp"

using namespace dct;

int 
DCT::setupDCT()
{
    cl_uint inputSizeBytes;

    // allocate and init memory used by host 
    inputSizeBytes = width * height * sizeof(cl_float);
    input = (cl_float *) malloc(inputSizeBytes);
    CHECK_ALLOCATION(input, "Failed to allocate host memory. (input)");

    cl_uint outputSizeBytes =  width * height * sizeof(cl_float);
    output = (cl_float *)malloc(outputSizeBytes);
    CHECK_ALLOCATION(output, "Failed to allocate host memory. (output)");

    // random initialisation of input
    sampleCommon->fillRandom<cl_float>(input, width, height, 0, 255);

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
DCT::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("DCT_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());
    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
DCT::setupCL(void)
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

    inputBuffer = clCreateBuffer(
                      context, 
                      CL_MEM_READ_ONLY,
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

    dctBuffer = clCreateBuffer(
                      context, 
                      CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                      sizeof(cl_float) * blockSize,
                      dct8x8, 
                      &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (dctBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("DCT_Kernels.cl");
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
    kernel = clCreateKernel(program, "DCT", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    return SDK_SUCCESS;
}


int 
DCT::runCLKernels(void)
{
    cl_int   status;

    size_t globalThreads[2] = {width, height};
    size_t localThreads[2] = {blockWidth, blockWidth};

    status =  kernelInfo.setKernelWorkGroupInfo(kernel,devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "setKErnelWorkGroupInfo() failed");

    availableLocalMemory = deviceInfo.localMemSize - kernelInfo.localMemoryUsed;

    neededLocalMemory    = blockWidth * blockWidth* sizeof(cl_float); 

    if(neededLocalMemory > availableLocalMemory)
    {
        std::cout << "Unsupported: Insufficient local memory on device." << std::endl;
        return SDK_FAILURE;
    }

    if(localThreads[0]                 > deviceInfo.maxWorkItemSizes[0] ||
       localThreads[1]                 > deviceInfo.maxWorkItemSizes[1] ||
       localThreads[0] * localThreads[1] > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device does not support requested number of work items."<<std::endl;
        return SDK_FAILURE;
    }

    if((cl_uint)(localThreads[0] * localThreads[1]) > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size required : {" 
                << localThreads[0] << "x" << localThreads[1] << "}" << std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                      << kernelInfo.kernelWorkGroupSize << std::endl;
            std::cout << "Exiting!\n";
        }
            return SDK_EXPECTED_FAILURE;
    }

    // Set input data
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                inputBuffer,
                CL_FALSE,
                0,
                sizeof(cl_float) * width * height,
                input, 
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (inputBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");


    // Set appropriate arguments to the kernel

    /** 
     * 1st argument to the kernel which stores the output of the DCT 
     */
    status = clSetKernelArg(
                    kernel, 
                    0, 
                    sizeof(cl_mem), 
                    (void *)&outputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (outputBuffer)");

    /**
     * 2nd argument to the kernel , the input matrix 
     */
    status = clSetKernelArg(
                    kernel, 
                    1, 
                    sizeof(cl_mem), 
                    (void *)&inputBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inputBuffer)");

    /**
     * 3rd argument to the kernel , the dct8x8 cosine basis function matrix 
     */
    status = clSetKernelArg(
                    kernel, 
                    2, 
                    sizeof(cl_mem), 
                    (void *)&dctBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (dctBuffer)");

    /** 
     * 4th argument to the kernel , local memory which stores intermediate values 
     */
    status = clSetKernelArg(
                    kernel, 
                    3, 
                    blockWidth * blockWidth * sizeof(cl_float), 
                    NULL);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (Intermediate local buffer)");
    
    /** 
     * 5th argument to the kernel , width of the input matrix 
     */
    status = clSetKernelArg(
                    kernel,
                    4,
                    sizeof(cl_uint),
                    (void *)&width);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (width)");

    /** 
     * 6th argument to the kernel , width of the block = 8 
     */
    status = clSetKernelArg(
                    kernel,
                    5,
                    sizeof(cl_uint),
                    (void *)&blockWidth);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (blockWidth)");

    /**
     * 7th argument to the kernel , flag to perform inverse DCT 
     */
    status = clSetKernelArg(
                    kernel, 
                    6, 
                    sizeof(cl_uint), 
                    (void *)&inverse);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (inverse)");

    /**
     * Enqueue a kernel run call.
     */
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

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&ndrEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(ndrEvt) Failed");

    // Enqueue readBuffer
    cl_event readEvt;
    status = clEnqueueReadBuffer(
                commandQueue,
                outputBuffer,
                CL_FALSE,
                0,
                width * height * sizeof(cl_float),
                output,
                0,
                NULL,
                &readEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt) Failed");

    return SDK_SUCCESS;
}



cl_uint
DCT::getIdx(cl_uint blockIdx, cl_uint blockIdy, cl_uint localIdx, cl_uint localIdy, cl_uint blockWidth, cl_uint globalWidth)
{
    cl_uint globalIdx = blockIdx * blockWidth + localIdx;
    cl_uint globalIdy = blockIdy * blockWidth + localIdy;
    return (globalIdy * globalWidth  + globalIdx);
}


/*
 * Reference implementation of the Discrete Cosine Transfrom on the CPU
 */
void 
DCT::DCTCPUReference( cl_float * verificationOutput,
                      const cl_float * input , 
                      const cl_float * dct8x8 ,
                      const cl_uint    width,
                      const cl_uint    height,
                      const cl_uint   numBlocksX,
                      const cl_uint   numBlocksY,
                      const cl_uint    inverse)
{
    cl_float * temp = (cl_float *)malloc(width*height*sizeof(cl_float));
    
    // for each block in the image
    for(cl_uint blockIdy=0; blockIdy < numBlocksY; ++blockIdy)
        for(cl_uint blockIdx=0; blockIdx < numBlocksX; ++blockIdx)
        {
            //  First calculate A^T * X
            for(cl_uint j=0; j < blockWidth ; ++j)
                for(cl_uint i = 0; i < blockWidth ; ++i)
                {
                    cl_uint index = getIdx(blockIdx, blockIdy, i, j, blockWidth, width);
                    cl_float tmp = 0.0f;
                    for(cl_uint k=0; k < blockWidth; ++k)
                    {
                        // multiply with dct8x8(k,i) if forward DCT and dct8x8(i,k) if inverse DCT
                        cl_uint index1 =  (inverse) ? i*blockWidth +k : k*blockWidth + i;
                        //input(k,j)
                        cl_uint index2 = getIdx(blockIdx, blockIdy, j, k, blockWidth, width);

                        tmp += dct8x8[index1]*input[index2];
                    }
                    temp[index] = tmp;
                }

            // And now multiply the result of previous step with A i.e. calculate (A^T * X) * A
            for(cl_uint j=0; j < blockWidth ; ++j)
                for(cl_uint i = 0; i < blockWidth ; ++i)
                {
                    cl_uint index = getIdx(blockIdx, blockIdy, i, j, blockWidth, width);
                    cl_float tmp = 0.0f;
                    for(cl_uint k=0; k < blockWidth; ++k)
                    {
                        //input(i,k)
                        cl_uint index1 = getIdx(blockIdx, blockIdy, k, i, blockWidth, width);
                        
                        // multiply with dct8x8(k,j) if forward DCT and dct8x8(j,k) if inverse DCT
                        cl_uint index2 =  (inverse) ? j*blockWidth +k : k*blockWidth + j;
                        
                        tmp += temp[index1]*dct8x8[index2];
                    }
                    verificationOutput[index] = tmp;
                }
        }

    free(temp);
}

int DCT::initialize()
{
    // Call base class Initialize to get default configuration
    CHECK_ERROR(this->SDKSample::initialize(), SDK_SUCCESS, "OpenCL resource initilization failed");
    streamsdk::Option* width_option = new streamsdk::Option;
    CHECK_ALLOCATION(width_option, "Memory allocation error.\n");

    width_option->_sVersion = "x";
    width_option->_lVersion = "width";
    width_option->_description = "Width of the input matrix";
    width_option->_type = streamsdk::CA_ARG_INT;
    width_option->_value = &width;

    sampleArgs->AddOption(width_option);
    delete width_option;

    streamsdk::Option* height_option = new streamsdk::Option;
    CHECK_ALLOCATION(height_option, "Memory allocation error.\n");

    height_option->_sVersion = "y";
    height_option->_lVersion = "height";
    height_option->_description = "Height of the input matrix";
    height_option->_type = streamsdk::CA_ARG_INT;
    height_option->_value = &height;

    sampleArgs->AddOption(height_option);
    delete height_option;

    streamsdk::Option* num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "Memory allocation error.\n");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations for kernel execution";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);
    delete num_iterations;
    return SDK_SUCCESS;
}

int DCT::setup()
{
    // Make sure the width is a multiple of blockWidth 8 here
    if(width % blockWidth != 0)
        width = (width/blockWidth + 1)*blockWidth;

    // Make sure the height is a multiple of blockWidth 8 here
    if(height%blockWidth !=0)
        height = (height/blockWidth + 1) * blockWidth;
    CHECK_ERROR(setupDCT(), SDK_SUCCESS, "OpenCL setupDCT failed");
    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);
    if(setupCL() != SDK_SUCCESS)
        return SDK_FAILURE;
    sampleCommon->stopTimer(timer);

    setupTime = (cl_double)sampleCommon->readTimer(timer);
    return SDK_SUCCESS;
}


int DCT::run()
{
    // Warm up
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
         if (runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    std::cout << "Executing kernel for " << iterations 
              << " iterations" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);   

    for(int i = 0; i < iterations; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer
        if (runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    totalKernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet) {
        sampleCommon->printArray<cl_float>("Output", output, width,1);
    }

    return SDK_SUCCESS;
}

int DCT::verifyResults()
{
    if(verify)
    {
        verificationOutput = (cl_float *) malloc(width*height*sizeof(cl_float));
        CHECK_ALLOCATION(verificationOutput,"Failed to allocate host memory. (verificationOutput)");

        /**
         * reference implementation
         */
        int refTimer = sampleCommon->createTimer();

        sampleCommon->resetTimer(refTimer);
        sampleCommon->startTimer(refTimer);
        DCTCPUReference(verificationOutput, input, dct8x8, width, height, 
                            width/blockWidth, height/blockWidth, 0);

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
            std::cout<<"Failed\n" << std::endl;
            return SDK_FAILURE;
        }
    }
    return SDK_SUCCESS;
}

void DCT::printStats()
{
    std::string strArray[4] = {"Width", "Height", "Time(sec)", "[Transfer+Kernel]Time(sec)"};
    std::string stats[4];

    totalTime = setupTime + totalKernelTime;

    stats[0]  = sampleCommon->toString(width    , std::dec);
    stats[1]  = sampleCommon->toString(height   , std::dec);
    stats[2]  = sampleCommon->toString(totalTime, std::dec);
    stats[3]  = sampleCommon->toString(totalKernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}

int DCT::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseMemObject(inputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(inputBuffer)");

    status = clReleaseMemObject(outputBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(dctBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(dctBuffer)");

    status = clReleaseCommandQueue(commandQueue);
     CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.(context)");

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
    DCT clDCT("OpenCL DCT8x8");
    if(clDCT.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clDCT.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clDCT.isDumpBinaryEnabled())
    {
        return clDCT.genBinaryImage();
    }

    if(clDCT.setup() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clDCT.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clDCT.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clDCT.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;

    clDCT.printStats();

    return SDK_SUCCESS;
}
