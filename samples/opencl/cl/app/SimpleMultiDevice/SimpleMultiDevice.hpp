#ifndef MULTI_DEVICE_H_
#define MULTI_DEVICE_H_

#include <string.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <SDKCommon.hpp>
#include <SDKThread.hpp>

#define KERNEL_ITERATIONS 100
#define GROUP_SIZE 64
#define NUM_THREADS 1024 * 64

class Device
{
public:

    //CL Objects and memory buffers
    int status;
    cl_device_type dType;       //device type
    cl_device_id deviceId;      //device ID
    cl_context context;         //context
    cl_command_queue queue;     //command-queue
    cl_mem inputBuffer;         //input buffer
    cl_mem outputBuffer;        //output buffer
    cl_program program;         //program object
    cl_kernel kernel;           //kernel object
    cl_event eventObject;       //event object
    cl_ulong kernelStartTime;   //kernel start time
    cl_ulong kernelEndTime;     //kernel end time
    double elapsedTime;         //elapsed time in ms
    cl_float *output;           //output host buffer for verification

    Device()
    {
        output = NULL;
    }

    ~Device();

    // Create Context
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int createContext();

    // Create Command-queue
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int createQueue();

    // Create input and output buffers and Enqueue write data
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int createBuffers();

    // Initialize Input buffers
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int enqueueWriteBuffer();

    // Create Program object
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int createProgram(const char **source, const size_t *sourceSize);

    // Build Program source
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int buildProgram();

    // Create Kernel object
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int createKernel();

    // Set Kernel arguments
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int setKernelArgs();

    // Enqueue NDRAnge kernel
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int enqueueKernel(size_t *globalThreads, size_t *localThreads);

    // Wait for kernel execution to finish
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int waitForKernel();

    // Get kernel execution time
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int getProfilingData();

    // Get output data from device
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int enqueueReadData();

    // Verify results against host computation
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int verifyResults();

    // Cleanup allocated resources
    // @ return SDK_SUCCESS on success and SDK_FAILURE on failure
    int cleanupResources();

};


/*** GLOBALS ***/

//Separator
std::string sep = "----------------------------------------------------------";
bool verify = false;

// Pointer to list of devices
Device *cpu;
Device *gpu;

// Number of CPU and GPU devices 
int numDevices;
int numCPUDevices;
int numGPUDevices;

// Size of input data
int width;

// Input data is same for all devices
cl_float *input;

// Host Output data for verification
cl_float *verificationOutput;

// Kernel source string
std::string sourceStr;
const char *source;

// SDK object
streamsdk::SDKCommon sdkObject;

// Context properties
const cl_context_properties* cprops;
cl_context_properties cps[3];
cl_platform_id platform = NULL;

// Count for verification
cl_uint verificationCount = 0;
cl_uint requiredCount = 0;

/*** FUNCTION DECLARATIONS ***/

// Read a file into a string
std::string convertToString(const char * filename);

// Host kernel computation

int CPUkernel();

// Runs the kernel on all GPU devices and verify results with host output

int runMultiGPU();

// Runs the kernel on a CPU and a GPU and verifies their results with host output

int runMultiDevice();

// Calls runMultiGPU and runMultiDevice function

int run(void);

// Releases program's resources
void cleanupHost(void);

/*
 * Prints no more than 256 elements of the given array.
 * Prints full array if length is less than 256.
 *
 * Prints Array name followed by elements.
 */
void print1DArray(const std::string arrayName, 
                  const unsigned int *arrayData, 
                  const unsigned int length);


#endif  /* #ifndef MULTI_DEVICE_H_ */
