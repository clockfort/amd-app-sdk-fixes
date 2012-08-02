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


#include "FloydWarshall.hpp"

int FloydWarshall::setupFloydWarshall()
{
    cl_uint matrixSizeBytes;

    // allocate and init memory used by host
    matrixSizeBytes = numNodes * numNodes * sizeof(cl_uint);
    pathDistanceMatrix = (cl_uint *) malloc(matrixSizeBytes);
    CHECK_ALLOCATION(pathDistanceMatrix, "Failed to allocate host memory. (pathDistanceMatrix)");

    pathMatrix = (cl_uint *) malloc(matrixSizeBytes);
    CHECK_ALLOCATION(pathMatrix, "Failed to allocate host memory. (pathMatrix)");

    // random initialisation of input

    /*
     * pathMatrix is the intermediate node from which the path passes
     * pathMatrix(i,j) = k means the shortest path from i to j
     * passes through an intermediate node k
     * Initialized such that pathMatrix(i,j) = i
     */
   
    sampleCommon->fillRandom<cl_uint>(pathDistanceMatrix, numNodes, numNodes, 0, MAXDISTANCE); 
    for(cl_int i = 0; i < numNodes; ++i)
    {
        cl_uint iXWidth = i * numNodes;
        pathDistanceMatrix[iXWidth + i] = 0;
    }

    /*
     * pathMatrix is the intermediate node from which the path passes
     * pathMatrix(i,j) = k means the shortest path from i to j
     * passes through an intermediate node k
     * Initialized such that pathMatrix(i,j) = i
     */
    for(cl_int i = 0; i < numNodes; ++i)
    {
        for(cl_int j = 0; j < i; ++j)
        {
            pathMatrix[i * numNodes + j] = i;
            pathMatrix[j * numNodes + i] = j;
        }
        pathMatrix[i * numNodes + i] = i;
    }

    /* 
     * Unless quiet mode has been enabled, print the INPUT array.
     */
    if(!quiet) 
    {
        sampleCommon->printArray<cl_uint>(
            "Path Distance", 
            pathDistanceMatrix, 
            numNodes, 
            1);

        sampleCommon->printArray<cl_uint>(
            "Path ", 
            pathMatrix, 
            numNodes, 
            1);
    }

    if(verify)
    {
        verificationPathDistanceMatrix = (cl_uint *) malloc(numNodes * numNodes * sizeof(cl_int));
        CHECK_ALLOCATION(verificationPathDistanceMatrix, "Failed to allocate host memory. (verificationPathDistanceMatrix)");

        verificationPathMatrix = (cl_uint *) malloc(numNodes * numNodes * sizeof(cl_int));
        CHECK_ALLOCATION(verificationPathMatrix, "Failed to allocate host memory. (verificationPathMatrix)");

        memcpy(verificationPathDistanceMatrix, pathDistanceMatrix, numNodes * numNodes * sizeof(cl_int));
        memcpy(verificationPathMatrix, pathMatrix, numNodes*numNodes*sizeof(cl_int));
    }

    return SDK_SUCCESS;
}

int 
FloydWarshall::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("FloydWarshall_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
FloydWarshall::setupCL(void)
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
            std::cout << "GPU not found. Fall back to CPU device" << std::endl;
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

    {
        // The block is to move the declaration of prop closer to its use 
        cl_command_queue_properties prop = 0;
        commandQueue = clCreateCommandQueue(context, 
                                            devices[deviceId], 
                                            prop, 
                                            &status);
        CHECK_OPENCL_ERROR(status, "clCreateCommandQueue failed.");
    }

    pathDistanceBuffer = clCreateBuffer(context, 
                                        CL_MEM_READ_WRITE,
                                        sizeof(cl_uint) * numNodes * numNodes,
                                        NULL, 
                                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (pathDistanceBuffer)");

    pathBuffer = clCreateBuffer(context, 
                                CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                sizeof(cl_uint) * numNodes * numNodes,
                                NULL, 
                                &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (pathBuffer)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("FloydWarshall_Kernels.cl");
    buildData.devices = devices;
    buildData.deviceId = deviceId;
    buildData.flagsStr = std::string("");
    if(isLoadBinaryEnabled())
        buildData.binaryName = std::string(loadBinary.c_str());

    if(isComplierFlagsSpecified())
        buildData.flagsFileName = std::string(flags.c_str());

    retValue = sampleCommon->buildOpenCLProgram(program, context, buildData);
    CHECK_ERROR(retValue, 0, "sampleCommon::buildOpenCLProgram() failed");


    // get a kernel object handle for a kernel with the given name
    kernel = clCreateKernel(program, "floydWarshallPass", &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    return SDK_SUCCESS;
}

int 
FloydWarshall::runCLKernels(void)
{
    cl_int   status;
    cl_uint numPasses = numNodes;
    size_t globalThreads[2] = {numNodes, numNodes};
    size_t localThreads[2] = {blockSize, blockSize};

    totalKernelTime = 0;

    // Check group size against kernelWorkGroupSize 
    status = kernelInfo.setKernelWorkGroupInfo(kernel,devices[deviceId]);
    CHECK_OPENCL_ERROR(status, "kernelInfo.setKernelWorkGroupInfo failed.");
    
    if((cl_uint)(localThreads[0] * localThreads[0]) > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : "<<localThreads[0]<<std::endl;
            std::cout << "Max Group Size supported on the kernel : " 
                << kernelInfo.kernelWorkGroupSize<<std::endl;
            std::cout << "Changing the group size to " << kernelInfo.kernelWorkGroupSize 
                << std::endl;
        }

        blockSize = 4;

        localThreads[0] = blockSize;
        localThreads[1] = blockSize;
    }

     /*
     * The floyd Warshall algorithm is a multipass algorithm
     * that calculates the shortest path between each pair of
     * nodes represented by pathDistanceBuffer. 
     *
     * In each pass a node k is introduced and the pathDistanceBuffer
     * which has the shortest distance between each pair of nodes
     * considering the (k-1) nodes (that are introduced in the previous
     * passes) is updated such that
     *
     * ShortestPath(x,y,k) = min(ShortestPath(x,y,k-1), ShortestPath(x,k,k-1) + ShortestPath(k,y,k-1))
     * where x and y are the pair of nodes between which the shortest distance 
     * is being calculated.
     * 
     * pathBuffer stores the intermediate nodes through which the shortest
     * path goes for each pair of nodes.
     */

    // Set input data
    cl_event writeEvt;
    status = clEnqueueWriteBuffer(
                commandQueue,
                pathDistanceBuffer,
                CL_FALSE,
                0,
                sizeof(cl_uint) * numNodes * numNodes,
                pathDistanceMatrix,
                0,
                NULL,
                &writeEvt);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. (pathDistanceBuffer)");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt) Failed");

    /*
     * Set appropriate arguments to the kernel
     *
     * First argument of the kernel is the adjacency matrix
     */
    status = clSetKernelArg(kernel, 
                            0, 
                            sizeof(cl_mem), 
                            (void*)&pathDistanceBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed.(pathDistanceBuffer)");

    /*
     * Second argument to the kernel is the path matrix
     * the matrix that stores the nearest node through which the shortest path
     * goes.
     */
    status = clSetKernelArg(kernel, 
                            1, 
                            sizeof(cl_mem), 
                            (void*)&pathBuffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (PathBuffer)");

    /*
     * Third argument is the number of nodes in the graph
     */
    status = clSetKernelArg(kernel, 
                            2, 
                            sizeof(cl_uint), 
                            (void*)&numNodes);
   CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (numNodes)");

    // numNodes - i.e number of elements in the array 
    status = clSetKernelArg(kernel, 
                            3, 
                            sizeof(cl_uint), 
                            (void*)&numNodes);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (numNodes)");

    for(cl_uint i = 0; i < numPasses; i += 1)
    {
        /*
         * Kernel needs which pass of the algorithm is running 
         * which is sent as the Fourth argument
         */
        status = clSetKernelArg(kernel, 
                                3, 
                                sizeof(cl_uint), 
                                (void*)&i);
        CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (pass)");


         // Enqueue a kernel run call.

        cl_event ndrEvt;
        status = clEnqueueNDRangeKernel(commandQueue,
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
    }

    // Enqueue readBuffer
    cl_event readEvt1;
    status = clEnqueueReadBuffer(commandQueue,
                                 pathBuffer,
                                 CL_TRUE,
                                 0,
                                 numNodes * numNodes * sizeof(cl_uint),
                                 pathMatrix,
                                 0,
                                 NULL,
                                 &readEvt1);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt1);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt1) Failed");


    // Enqueue readBuffer
    cl_event readEvt2;
    status = clEnqueueReadBuffer(commandQueue,
                                 pathDistanceBuffer,
                                 CL_TRUE,
                                 0,
                                 numNodes * numNodes * sizeof(cl_uint), 
                                 pathDistanceMatrix,
                                 0,
                                 NULL,
                                 &readEvt2);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    status = sampleCommon->waitForEventAndRelease(&readEvt2);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(readEvt2) Failed");

    return SDK_SUCCESS;
}

/*
 * Returns the lesser of the two unsigned integers a and b
 */
cl_uint 
FloydWarshall::minimum(cl_uint a, cl_uint b)
{
    return (b < a) ? b : a;
}

/*
 * Calculates the shortest path between each pair of nodes in a graph
 * pathDistanceMatrix gives the shortest distance between each node
 * in the graph.
 * pathMatrix gives the path intermediate node through which the shortest
 * distance in calculated
 * numNodes is the number of nodes in the graph
 */
void 
FloydWarshall::floydWarshallCPUReference(cl_uint * pathDistanceMatrix,
                                         cl_uint * pathMatrix,
                                         const cl_uint numNodes) 
{
    cl_uint distanceYtoX, distanceYtoK, distanceKtoX, indirectDistance;

    /*
     * pathDistanceMatrix is the adjacency matrix(square) with
     * the dimension equal to the number of nodes in the graph.
     */
    cl_uint width = numNodes;
    cl_uint yXwidth;

    /*
     * for each intermediate node k in the graph find the shortest distance between
     * the nodes i and j and update as
     *
     * ShortestPath(i,j,k) = min(ShortestPath(i,j,k-1), ShortestPath(i,k,k-1) + ShortestPath(k,j,k-1))
     */
    for(cl_uint k = 0; k < numNodes; ++k)
    {
        for(cl_uint y = 0; y < numNodes; ++y)
        {
            yXwidth =  y*numNodes;
            for(cl_uint x = 0; x < numNodes; ++x)
            {
                distanceYtoX = pathDistanceMatrix[yXwidth + x];
                distanceYtoK = pathDistanceMatrix[yXwidth + k];
                distanceKtoX = pathDistanceMatrix[k * width + x];

                indirectDistance = distanceYtoK + distanceKtoX;

                if(indirectDistance < distanceYtoX)
                {
                    pathDistanceMatrix[yXwidth + x] = indirectDistance;
                    pathMatrix[yXwidth + x]         = k;
                }
            }
        }
    }
}

int FloydWarshall::initialize()
{
    // Call base class Initialize to get default configuration
    if(this->SDKSample::initialize())
        return SDK_FAILURE;

    streamsdk::Option* num_nodes = new streamsdk::Option;
    CHECK_ALLOCATION(num_nodes, "Memory allocation error.\n");
    
    num_nodes->_sVersion = "x";
    num_nodes->_lVersion = "nodes";
    num_nodes->_description = "number of nodes";
    num_nodes->_type = streamsdk::CA_ARG_INT;
    num_nodes->_value = &numNodes;
    sampleArgs->AddOption(num_nodes);
    delete num_nodes;

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

int FloydWarshall::setup()
{
     // numNodes should be multiples of blockSize 
    if(numNodes % blockSize != 0)
    {
        numNodes = (numNodes / blockSize + 1) * blockSize;
    }

    if(setupFloydWarshall() != SDK_SUCCESS)
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


int FloydWarshall::run()
{
    for(int i = 0; i < 2 && iterations != 1; i++)
    {
        // Arguments are set and execution call is enqueued on command buffer 
        if(runCLKernels() != SDK_SUCCESS)
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
        if(runCLKernels() != SDK_SUCCESS)
            return SDK_FAILURE;
    }

    sampleCommon->stopTimer(timer);
    totalKernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;

    if(!quiet) {
        sampleCommon->printArray<cl_uint>("Output Path Distance Matrix", pathDistanceMatrix, numNodes, 1);
        sampleCommon->printArray<cl_uint>("Output Path Matrix", pathMatrix, numNodes, 1);
    }

    return SDK_SUCCESS;
}

int FloydWarshall::verifyResults()
{
    if(verify)
    {
        /*
         * reference implementation
         * it overwrites the input array with the output
         */
        int refTimer = sampleCommon->createTimer();
        sampleCommon->resetTimer(refTimer);
        sampleCommon->startTimer(refTimer);
        floydWarshallCPUReference(verificationPathDistanceMatrix, verificationPathMatrix, numNodes);
        sampleCommon->stopTimer(refTimer);
        referenceKernelTime = sampleCommon->readTimer(refTimer);
        
        std::cout << "CPU time " << referenceKernelTime << std::endl;

        // compare the results and see if they match 
        if(memcmp(pathDistanceMatrix, verificationPathDistanceMatrix, numNodes*numNodes*sizeof(cl_uint)) == 0)
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

void FloydWarshall::printStats()
{
    std::string strArray[3] = {"Nodes", "Time(sec)", "[Transfer+Kernel]Time(sec)"};
    std::string stats[3];

    totalTime = setupTime + totalKernelTime;

    stats[0] = sampleCommon->toString(numNodes, std::dec);
    stats[1] = sampleCommon->toString(totalTime, std::dec);
    stats[2] = sampleCommon->toString(totalKernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 3);
}

int FloydWarshall::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.) 
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseMemObject(pathDistanceBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseMemObject(pathBuffer);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    // release program resources (input memory etc.) 
    FREE(pathDistanceMatrix);
    FREE(pathMatrix);
    FREE(verificationPathDistanceMatrix);
    FREE(verificationPathMatrix);
    FREE(devices);

    return SDK_SUCCESS;
}

int 
main(int argc, char * argv[])
{
    FloydWarshall clFloydWarshall("OpenCL FloydWarshall");

    // Initialize
    if(clFloydWarshall.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clFloydWarshall.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clFloydWarshall.isDumpBinaryEnabled())
    {
        return clFloydWarshall.genBinaryImage();
    }

    // Setup
    if(clFloydWarshall.setup() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Run
    if(clFloydWarshall.run() != SDK_SUCCESS)
        return SDK_FAILURE;

    // VerifyResults
    if(clFloydWarshall.verifyResults() != SDK_SUCCESS)
        return SDK_FAILURE;

    // Cleanup
    if(clFloydWarshall.cleanup() != SDK_SUCCESS)
        return SDK_FAILURE;
    clFloydWarshall.printStats();

    return SDK_SUCCESS;
}
