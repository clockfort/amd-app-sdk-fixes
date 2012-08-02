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


#include "NBody.hpp"
#include <GL/glut.h>
#include <cmath>
#include <malloc.h>

int numBodies;      /**< No. of particles*/
cl_float* pos;      /**< Output position */
void* me;           /**< Pointing to NBody class */
cl_bool display;

float
NBody::random(float randMax, float randMin)
{
    float result;
    result =(float)rand() / (float)RAND_MAX;

    return ((1.0f - result) * randMin + result *randMax);
}

int
NBody::setupNBody()
{
    // make sure numParticles is multiple of group size
    numParticles = (cl_int)(((size_t)numParticles 
        < groupSize) ? groupSize : numParticles);
    numParticles = (cl_int)((numParticles / groupSize) * groupSize);

    numBodies = numParticles;

    initPos = (cl_float*)malloc(numBodies * sizeof(cl_float4));
    CHECK_ALLOCATION(initPos, "Failed to allocate host memory. (initPos)");

    initVel = (cl_float*)malloc(numBodies * sizeof(cl_float4));
    CHECK_ALLOCATION(initVel, "Failed to allocate host memory. (initVel)");

#if defined (_WIN32)
    pos = (cl_float*)_aligned_malloc(numBodies * sizeof(cl_float4), 16);
#else
    pos = (cl_float*)memalign(16, numBodies * sizeof(cl_float4));
#endif
    CHECK_ALLOCATION(pos, "Failed to allocate host memory. (pos)");

#if defined (_WIN32)
    vel = (cl_float*)_aligned_malloc(numBodies * sizeof(cl_float4), 16);
#else
    vel = (cl_float*)memalign(16, numBodies * sizeof(cl_float4));
#endif
    CHECK_ALLOCATION(vel, "Failed to allocate host memory. (vel)");

    // initialization of inputs
    for(int i = 0; i < numBodies; ++i)
    {
        int index = 4 * i;

        // First 3 values are position in x,y and z direction
        for(int j = 0; j < 3; ++j)
        {
            initPos[index + j] = random(3, 50);
        }

        // Mass value
        initPos[index + 3] = random(1, 1000);

        // First 3 values are velocity in x,y and z direction
        for(int j = 0; j < 3; ++j)
        {
            initVel[index + j] = 0.0f;
        }

        // unused
        initVel[3] = 0.0f;
    }

    memcpy(pos, initPos, 4 * numBodies * sizeof(cl_float));
    memcpy(vel, initVel, 4 * numBodies * sizeof(cl_float));

    return SDK_SUCCESS;
}

int 
NBody::genBinaryImage()
{
    streamsdk::bifData binaryData;
    binaryData.kernelName = std::string("NBody_Kernels.cl");
    binaryData.flagsStr = std::string("");
    if(isComplierFlagsSpecified())
        binaryData.flagsFileName = std::string(flags.c_str());

    binaryData.binaryName = std::string(dumpBinary.c_str());
    int status = sampleCommon->generateBinaryImage(binaryData);
    return status;
}


int
NBody::setupCL()
{
    cl_int status = CL_SUCCESS;
    cl_event writeEvt1,writeEvt2;

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

    /*
    * Create and initialize memory objects
    */

    // Create memory objects for position
    currPos = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        numBodies * sizeof(cl_float4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (oldPos)");

    // Initialize position buffer
    status = clEnqueueWriteBuffer(commandQueue,
                                  currPos,
                                  CL_FALSE,
                                  0,
                                  numBodies * sizeof(cl_float4),
                                  pos,
                                  0,
                                  0,
                                  &writeEvt1);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. ");


    // Create memory objects for position
    newPos = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        numBodies * sizeof(cl_float4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (newPos)");

    // Create memory objects for velocity
    currVel = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        numBodies * sizeof(cl_float4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (oldVel)");

    // Initialize velocity buffer
    status = clEnqueueWriteBuffer(commandQueue,
                                  currVel,
                                  CL_FALSE,
                                  0,
                                  numBodies * sizeof(cl_float4),
                                  vel,
                                  0,
                                  0,
                                  &writeEvt2);
    CHECK_OPENCL_ERROR(status, "clEnqueueWriteBuffer failed. ");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed. ");

    // Create memory objects for velocity
    newVel = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        numBodies * sizeof(cl_float4),
        0,
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateBuffer failed. (newVel)");

    // create a CL program using the kernel source 
    streamsdk::buildProgramData buildData;
    buildData.kernelName = std::string("NBody_Kernels.cl");
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
    kernel = clCreateKernel(
        program,
        "nbody_sim",
        &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel failed.");

    status = sampleCommon->waitForEventAndRelease(&writeEvt1);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt1) Failed");

    status = sampleCommon->waitForEventAndRelease(&writeEvt2);
    CHECK_ERROR(status, SDK_SUCCESS, "WaitForEventAndRelease(writeEvt2) Failed");

    return SDK_SUCCESS;
}


int 
NBody::setupCLKernels()
{
    cl_int status;

    // Set appropriate arguments to the kernel

    // Particle positions
    status = clSetKernelArg(
        kernel,
        0,
        sizeof(cl_mem),
        (void*)&currPos);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (updatedPos)");

    // Particle velocity
    status = clSetKernelArg(
        kernel,
        1,
        sizeof(cl_mem),
        (void *)&currVel);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (updatedVel)");

    // numBodies
    status = clSetKernelArg(
        kernel,
        2,
        sizeof(cl_int),
        (void *)&numBodies);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (numBodies)");

    // time step
    status = clSetKernelArg(
        kernel,
        3,
        sizeof(cl_float),
        (void *)&delT);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (delT)");

    // upward Pseudoprobability
    status = clSetKernelArg(
        kernel,
        4,
        sizeof(cl_float),
        (void *)&espSqr);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (espSqr)");

    // local memory
    status = clSetKernelArg(
        kernel,
        5,
        GROUP_SIZE * 4 * sizeof(float),
        NULL);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (localPos)");

    // Particle positions
    status = clSetKernelArg(
        kernel,
        6,
        sizeof(cl_mem),
        (void*)&newPos);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (unewPos)");

    // Particle velocity
    status = clSetKernelArg(
        kernel,
        7,
        sizeof(cl_mem),
        (void *)&newVel);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (newVel)");

    status = kernelInfo.setKernelWorkGroupInfo(kernel, devices[deviceId]);
    CHECK_ERROR(status, SDK_SUCCESS, "kernelInfo.setKernelWorkGroupInfo() failed");

    if(kernelInfo.localMemoryUsed > deviceInfo.localMemSize)
    {
        std::cout << "Unsupported: Insufficient local memory on device" <<
            std::endl;
        return SDK_FAILURE;
    }

    if(groupSize > kernelInfo.kernelWorkGroupSize)
    {
        if(!quiet)
        {
            std::cout << "Out of Resources!" << std::endl;
            std::cout << "Group Size specified : " << groupSize << std::endl;
            std::cout << "Max Group Size supported on the kernel : "
                      << kernelInfo.kernelWorkGroupSize << std::endl;
            std::cout << "Falling back to " << kernelInfo.kernelWorkGroupSize << std::endl;
        }
        groupSize = kernelInfo.kernelWorkGroupSize;
    }

    return SDK_SUCCESS;
}

int 
NBody::runCLKernels()
{
    cl_int status;

    /* 
    * Enqueue a kernel run call.
    */
    size_t globalThreads[] = {numBodies};
    size_t localThreads[] = {groupSize};

    if(localThreads[0] > deviceInfo.maxWorkItemSizes[0] ||
       localThreads[0] > deviceInfo.maxWorkGroupSize)
    {
        std::cout << "Unsupported: Device"
            "does not support requested number of work items.";
        return SDK_FAILURE;
    }

    status = clEnqueueNDRangeKernel(
        commandQueue,
        kernel,
        1,
        NULL,
        globalThreads,
        localThreads,
        0,
        NULL,
        NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel failed.");

    // Read data for verification or display
    status = clEnqueueReadBuffer(
        commandQueue,
        exchange ? currPos : newPos,
        CL_FALSE,
        0,
        numBodies* sizeof(cl_float4),
        pos,
        0,
        NULL,
        &this->glEvent);
    CHECK_OPENCL_ERROR(status, "clEnqueueReadBuffer failed.");

    status = clFlush(commandQueue);
    CHECK_OPENCL_ERROR(status, "clFlush failed.");

    /* Set inputs as output and outputs as inputs 
     to avoid explicit copy*/

    // Particle positions
    status = clSetKernelArg(
        kernel,
        exchange ? 6 : 0,
        sizeof(cl_mem),
        (void*)&currPos);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (updatedPos)");

    // Particle velocity 
    status = clSetKernelArg(
        kernel,
        exchange ? 7 : 1,
        sizeof(cl_mem),
        (void *)&currVel);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (updatedVel)");

    // Particle positions
    status = clSetKernelArg(
        kernel,
        exchange ? 0 : 6,
        sizeof(cl_mem),
        (void*)&newPos);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (updatedPos)");

    // Particle velocity
    status = clSetKernelArg(
        kernel,
        exchange ? 1 : 7,
        sizeof(cl_mem),
        (void *)&newVel);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg failed. (updatedVel)");

    exchange = exchange ? false : true;

    return SDK_SUCCESS;
}

/*
* n-body simulation on cpu
*/
void 
NBody::nBodyCPUReference()
{
    //Iterate for all samples
    for(int i = 0; i < numBodies; ++i)
    {
        int myIndex = 4 * i;
        float acc[3] = {0.0f, 0.0f, 0.0f};
        for(int j = 0; j < numBodies; ++j)
        {
            float r[3];
            int index = 4 * j;

            float distSqr = 0.0f;
            for(int k = 0; k < 3; ++k)
            {
                r[k] = refPos[index + k] - refPos[myIndex + k];

                distSqr += r[k] * r[k];
            }

            float invDist = 1.0f / sqrt(distSqr + espSqr);
            float invDistCube =  invDist * invDist * invDist;
            float s = refPos[index + 3] * invDistCube;

            for(int k = 0; k < 3; ++k)
            {
                acc[k] += s * r[k];
            }
        }

        for(int k = 0; k < 3; ++k)
        {
            refPos[myIndex + k] += refVel[myIndex + k] * delT + 0.5f * acc[k] * delT * delT;
            refVel[myIndex + k] += acc[k] * delT;
        }
    }
}

int
NBody::initialize()
{
    // Call base class Initialize to get default configuration
    int status = 0;
    if (this->SDKSample::initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    streamsdk::Option *num_particles = new streamsdk::Option;
    CHECK_ALLOCATION(num_particles, "error. Failed to allocate memory (num_particles)\n");

    num_particles->_sVersion = "x";
    num_particles->_lVersion = "particles";
    num_particles->_description = "Number of particles";
    num_particles->_type = streamsdk::CA_ARG_INT;
    num_particles->_value = &numParticles;

    sampleArgs->AddOption(num_particles);
    delete num_particles;

    streamsdk::Option *num_iterations = new streamsdk::Option;
    CHECK_ALLOCATION(num_iterations, "error. Failed to allocate memory (num_iterations)\n");

    num_iterations->_sVersion = "i";
    num_iterations->_lVersion = "iterations";
    num_iterations->_description = "Number of iterations";
    num_iterations->_type = streamsdk::CA_ARG_INT;
    num_iterations->_value = &iterations;

    sampleArgs->AddOption(num_iterations);
    delete num_iterations;

    return SDK_SUCCESS;
}

int
NBody::setup()
{
    int status = 0;
    if(setupNBody() != SDK_SUCCESS)
        return SDK_FAILURE;

    int timer = sampleCommon->createTimer();
    sampleCommon->resetTimer(timer);
    sampleCommon->startTimer(timer);

    status = setupCL();
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    sampleCommon->stopTimer(timer);
    // Compute setup time
    setupTime = (double)(sampleCommon->readTimer(timer));

    display = !quiet && !verify;

    return SDK_SUCCESS;
}

/** 
* @brief Initialize GL 
*/
void 
GLInit()
{
    glClearColor(0.0 ,0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);	
    glLoadIdentity();
}

/** 
* @brief Glut Idle function
*/
void 
idle()
{
    glutPostRedisplay();
}

/** 
* @brief Glut reshape func
* 
* @param w numParticles of OpenGL window
* @param h height of OpenGL window 
*/
void 
reShape(int w,int h)
{
    glViewport(0, 0, w, h);

    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(45.0f, w/h, 1.0f, 1000.0f);
    gluLookAt (0.0, 0.0, -2.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
}

/** 
* @brief OpenGL display function
*/
void displayfunc()
{
    glClearColor(0.0 ,0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glPointSize(1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    glColor3f(1.0f,0.6f,0.0f);

	NBody *nb = (NBody *)me;
	if (nb->isFirstLuanch)
	{
		//Calling kernel for calculatig subsequent positions
		nb->runCLKernels();
		nb->isFirstLuanch = false;
		return;
	}
	clWaitForEvents(1, &nb->glEvent);
    glBegin(GL_POINTS);
    for(int i = 0; i < numBodies; ++i)
    {
        //divided by 300 just for scaling
        glVertex3d(pos[i * 4 + 0] / 300, pos[i * 4 + 1] / 300, pos[i * 4 + 2] / 300);
    }
    glEnd();
	//Calling kernel for calculatig subsequent positions
	nb->runCLKernels();

    glFlush();
    glutSwapBuffers();
	
}

// keyboard function 
void
keyboardFunc(unsigned char key, int mouseX, int mouseY)
{
    switch(key)
    {
        // If the user hits escape or Q, then exit 

        // ESCAPE_KEY = 27
    case 27:
    case 'q':
    case 'Q':
        {
            if(((NBody*)me)->cleanup() != SDK_SUCCESS)
                exit(1);
            else
                exit(0);
        }
    default:
        break;
    }
}


int 
NBody::run()
{
    int status = 0;
    // Arguments are set and execution call is enqueued on command buffer
    if(setupCLKernels() != SDK_SUCCESS)
        return SDK_FAILURE;

    if(verify || timing)
    {
        int timer = sampleCommon->createTimer();
        sampleCommon->resetTimer(timer);
        sampleCommon->startTimer(timer);

        for(int i = 0; i < iterations; ++i)
        {
            if(runCLKernels() != SDK_SUCCESS)
                return SDK_FAILURE;
        }

		status = clFinish(this->commandQueue);
        sampleCommon->stopTimer(timer);
        // Compute kernel time
        kernelTime = (double)(sampleCommon->readTimer(timer)) / iterations;
    }

    if(!quiet)
    {
        sampleCommon->printArray<cl_float>("Output", pos, numBodies, 1);
    }

    return SDK_SUCCESS;
}

int
NBody::verifyResults()
{
    if(verify)
    {
        /* reference implementation
         * it overwrites the input array with the output
         */

        refPos = (cl_float*)malloc(numBodies * sizeof(cl_float4));
        CHECK_ALLOCATION(refPos, "Failed to allocate host memory. (refPos)");

        refVel = (cl_float*)malloc(numBodies * sizeof(cl_float4));
        CHECK_ALLOCATION(refVel, "Failed to allocate host memory. (refVel)");

        memcpy(refPos, initPos, 4 * numBodies * sizeof(cl_float));
        memcpy(refVel, initVel, 4 * numBodies * sizeof(cl_float));

        for(int i = 0; i < iterations; ++i)
        {
            nBodyCPUReference();
        }

        // compare the results and see if they match
        if(sampleCommon->compare(pos, refPos, 4 * numBodies, 0.00001))
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

void 
NBody::printStats()
{
    std::string strArray[4] = 
    {
        "Particles", 
        "Iterations", 
        "Time(sec)", 
        "kernelTime(sec)"
    };

    std::string stats[4];
    totalTime = setupTime + kernelTime;

    stats[0] = sampleCommon->toString(numParticles, std::dec);
    stats[1] = sampleCommon->toString(iterations, std::dec);
    stats[2] = sampleCommon->toString(totalTime, std::dec);
    stats[3] = sampleCommon->toString(kernelTime, std::dec);

    this->SDKSample::printStats(strArray, stats, 4);
}

int
NBody::cleanup()
{
    // Releases OpenCL resources (Context, Memory etc.)
    cl_int status;

    status = clReleaseKernel(kernel);
    CHECK_OPENCL_ERROR(status, "clReleaseKernel failed.(kernel)");

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.(program)");

    status = clReleaseMemObject(currPos);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(currPos)");

    status = clReleaseMemObject(currVel);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(currVel)");

    status = clReleaseMemObject(newPos);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(newPos)");

    status = clReleaseMemObject(newVel);
    CHECK_OPENCL_ERROR(status, "clReleaseMemObject failed.(newVel)");

    status = clReleaseCommandQueue(commandQueue);
    CHECK_OPENCL_ERROR(status, "clReleaseCommandQueue failed.(commandQueue)");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.(context)");

    return SDK_SUCCESS;
}

NBody::~NBody()
{
	if (this->glEvent)
		clReleaseEvent(this->glEvent);
    // release program resources 
    FREE(initPos);

    FREE(initVel);

#if defined (_WIN32)
        ALIGNED_FREE(pos);
#else
        FREE(pos);
#endif

#if defined (_WIN32)
        ALIGNED_FREE(vel);
#else
        FREE(vel);
#endif

    FREE(devices);
    FREE(refPos);
    FREE(refVel);
}


int 
main(int argc, char * argv[])
{
    int status = 0;
    NBody clNBody("OpenCL NBody");
    me = &clNBody;

    if(clNBody.initialize() != SDK_SUCCESS)
        return SDK_FAILURE;

    if (clNBody.parseCommandLine(argc, argv) != SDK_SUCCESS)
        return SDK_FAILURE;

    if(clNBody.isDumpBinaryEnabled())
    {
        return clNBody.genBinaryImage();
    }

    status = clNBody.setup();
    if(status != SDK_SUCCESS)
        return SDK_FAILURE;

    status = clNBody.run();
    CHECK_ERROR(status, SDK_SUCCESS, "Sample Run Program Failed");

    status = clNBody.verifyResults();
    CHECK_ERROR(status, SDK_SUCCESS, "Sample Verify Results Failed");

    clNBody.printStats();

    if(display)
    {
        // Run in  graphical window if requested 
        glutInit(&argc, argv);
        glutInitWindowPosition(100,10);
        glutInitWindowSize(600,600); 
        glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
        glutCreateWindow("nbody simulation"); 
        GLInit(); 
        glutDisplayFunc(displayfunc); 
        glutReshapeFunc(reShape);
        glutIdleFunc(idle); 
        glutKeyboardFunc(keyboardFunc);
        glutMainLoop();
    }

    status = clNBody.cleanup();
    CHECK_ERROR(status, SDK_SUCCESS, "Sample CleanUP Failed");

    return SDK_SUCCESS;
}
