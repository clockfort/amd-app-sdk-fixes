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

#include "Shared.h"

extern int status;
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define SDK_EXPECTED_FAILURE 2


cl_command_queue queue;
cl_context       context;
cl_kernel        read_kernel;
cl_kernel        write_kernel;
int              devnum;
char             devname[256];

cl_uint deviceMaxComputeUnits;
char extensions[1000];

cl_mem_flags inFlags = 0;
cl_mem_flags outFlags = 0;
cl_mem_flags copyFlags = 0;

struct _flags flags[] = {

             { CL_MEM_READ_ONLY,              "CL_MEM_READ_ONLY" },
             { CL_MEM_WRITE_ONLY,             "CL_MEM_WRITE_ONLY" },
             { CL_MEM_READ_WRITE,             "CL_MEM_READ_WRITE" },
             { CL_MEM_USE_HOST_PTR,           "CL_MEM_USE_HOST_PTR" },
             { CL_MEM_COPY_HOST_PTR,          "CL_MEM_COPY_HOST_PTR" },
             { CL_MEM_ALLOC_HOST_PTR,         "CL_MEM_ALLOC_HOST_PTR" },
             { CL_MEM_USE_PERSISTENT_MEM_AMD, "CL_MEM_USE_PERSISTENT_MEM_AMD"} };

int nFlags = sizeof(flags) / sizeof(flags[0]);

const char *cluErrorString(cl_int err) {

   switch(err) {

      case CL_SUCCESS: return "CL_SUCCESS";
      case CL_DEVICE_NOT_FOUND: return "CL_DEVICE_NOT_FOUND";
      case CL_DEVICE_NOT_AVAILABLE: return "CL_DEVICE_NOT_AVAILABLE";
      case CL_COMPILER_NOT_AVAILABLE: return
                                       "CL_COMPILER_NOT_AVAILABLE";
      case CL_MEM_OBJECT_ALLOCATION_FAILURE: return
                                       "CL_MEM_OBJECT_ALLOCATION_FAILURE";
      case CL_OUT_OF_RESOURCES: return "CL_OUT_OF_RESOURCES";
      case CL_OUT_OF_HOST_MEMORY: return "CL_OUT_OF_HOST_MEMORY";
      case CL_PROFILING_INFO_NOT_AVAILABLE: return
                                       "CL_PROFILING_INFO_NOT_AVAILABLE";
      case CL_MEM_COPY_OVERLAP: return "CL_MEM_COPY_OVERLAP";
      case CL_IMAGE_FORMAT_MISMATCH: return "CL_IMAGE_FORMAT_MISMATCH";
      case CL_IMAGE_FORMAT_NOT_SUPPORTED: return
                                       "CL_IMAGE_FORMAT_NOT_SUPPORTED";
      case CL_BUILD_PROGRAM_FAILURE: return "CL_BUILD_PROGRAM_FAILURE";
      case CL_MAP_FAILURE: return "CL_MAP_FAILURE";
      case CL_INVALID_VALUE: return "CL_INVALID_VALUE";
      case CL_INVALID_DEVICE_TYPE: return "CL_INVALID_DEVICE_TYPE";
      case CL_INVALID_PLATFORM: return "CL_INVALID_PLATFORM";
      case CL_INVALID_DEVICE: return "CL_INVALID_DEVICE";
      case CL_INVALID_CONTEXT: return "CL_INVALID_CONTEXT";
      case CL_INVALID_QUEUE_PROPERTIES: return "CL_INVALID_QUEUE_PROPERTIES";
      case CL_INVALID_COMMAND_QUEUE: return "CL_INVALID_COMMAND_QUEUE";
      case CL_INVALID_HOST_PTR: return "CL_INVALID_HOST_PTR";
      case CL_INVALID_MEM_OBJECT: return "CL_INVALID_MEM_OBJECT";
      case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return
                                       "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
      case CL_INVALID_IMAGE_SIZE: return "CL_INVALID_IMAGE_SIZE";
      case CL_INVALID_SAMPLER: return "CL_INVALID_SAMPLER";
      case CL_INVALID_BINARY: return "CL_INVALID_BINARY";
      case CL_INVALID_BUILD_OPTIONS: return "CL_INVALID_BUILD_OPTIONS";
      case CL_INVALID_PROGRAM: return "CL_INVALID_PROGRAM";
      case CL_INVALID_PROGRAM_EXECUTABLE: return
                                       "CL_INVALID_PROGRAM_EXECUTABLE";
      case CL_INVALID_KERNEL_NAME: return "CL_INVALID_KERNEL_NAME";
      case CL_INVALID_KERNEL_DEFINITION: return "CL_INVALID_KERNEL_DEFINITION";
      case CL_INVALID_KERNEL: return "CL_INVALID_KERNEL";
      case CL_INVALID_ARG_INDEX: return "CL_INVALID_ARG_INDEX";
      case CL_INVALID_ARG_VALUE: return "CL_INVALID_ARG_VALUE";
      case CL_INVALID_ARG_SIZE: return "CL_INVALID_ARG_SIZE";
      case CL_INVALID_KERNEL_ARGS: return "CL_INVALID_KERNEL_ARGS";
      case CL_INVALID_WORK_DIMENSION: return "CL_INVALID_WORK_DIMENSION";
      case CL_INVALID_WORK_GROUP_SIZE: return "CL_INVALID_WORK_GROUP_SIZE";
      case CL_INVALID_WORK_ITEM_SIZE: return "CL_INVALID_WORK_ITEM_SIZE";
      case CL_INVALID_GLOBAL_OFFSET: return "CL_INVALID_GLOBAL_OFFSET";
      case CL_INVALID_EVENT_WAIT_LIST: return "CL_INVALID_EVENT_WAIT_LIST";
      case CL_INVALID_EVENT: return "CL_INVALID_EVENT";
      case CL_INVALID_OPERATION: return "CL_INVALID_OPERATION";
      case CL_INVALID_GL_OBJECT: return "CL_INVALID_GL_OBJECT";
      case CL_INVALID_BUFFER_SIZE: return "CL_INVALID_BUFFER_SIZE";
      case CL_INVALID_MIP_LEVEL: return "CL_INVALID_MIP_LEVEL";
      case CL_INVALID_GLOBAL_WORK_SIZE: return "CL_INVALID_GLOBAL_WORK_SIZE";

      default: return "UNKNOWN CL ERROR CODE";
   }
}

cl_int spinForEventsComplete( cl_uint num_events, cl_event *event_list )
{
    cl_int ret = 0;
#if 0
    ret = clWaitForEvents( num_events, event_list );
#else
    cl_int param_value;
    size_t param_value_size_ret;

    for( cl_uint e=0; e < num_events; e++ )
    {
        while(1)
        {
            ret |= clGetEventInfo( event_list[ e ], 
                                   CL_EVENT_COMMAND_EXECUTION_STATUS,
                                   sizeof( cl_int ),
                                   &param_value,
                                   &param_value_size_ret );

            if( param_value == CL_COMPLETE )
                break;
        }
    }
#endif

    for( cl_uint e=0; e < num_events; e++ )
        clReleaseEvent( event_list[e] );

    return ret;
}

void initCL( char *kernel_file )
{
    // Get a platform, device, context and queue

    cl_platform_id   platform;
    cl_device_id     devices[128];
    cl_device_id     device;
    cl_uint          num_devices;
    cl_int           ret;

    cl_device_type   devs[] = { CL_DEVICE_TYPE_CPU,
                                CL_DEVICE_TYPE_GPU };

    clGetPlatformIDs( 1, &platform, NULL );

    ret = clGetDeviceIDs( platform,
                          devs[1],
                          128,
                          devices,
                          &num_devices );
    if(ret == CL_DEVICE_NOT_FOUND)
    {
        std::cout << "GPU not found. Exiting application" << std::endl;
        exit(0);
    }

    ASSERT_CL_RETURN( ret );

    device = devices[devnum];

    ret = clGetDeviceInfo( device,
                           CL_DEVICE_MAX_COMPUTE_UNITS,
                           sizeof(cl_uint),
                           &deviceMaxComputeUnits,
                           NULL);

    ASSERT_CL_RETURN( ret );

    ret = clGetDeviceInfo( device,
                           CL_DEVICE_NAME,
                           256,
                           devname,
                           NULL);

    ASSERT_CL_RETURN( ret );

    // Code Added to check if cl_khr_local_int32_base_atomics extension 
    // is supported by the selected device
    ret = clGetDeviceInfo( device,
                            CL_DEVICE_EXTENSIONS,
                            1000,
                            extensions,
                            NULL);

    ASSERT_CL_RETURN( ret );

    if(strstr(extensions,"cl_khr_local_int32_base_atomics") == NULL)
    {
        printf("Expected Error: cl_khr_local_int32_base_atomics is not supported by the device/n");
        status = SDK_EXPECTED_FAILURE;
	return;
    }

    context = clCreateContext( NULL,
                               1,
                               &device,
                               NULL, NULL, NULL );

    queue = clCreateCommandQueue( context,
                                 device,
                                 0,
                                 NULL );

    // Minimal error check.

    if( queue == NULL ) {
        fprintf( stderr, "Compute device setup failed\n");
        exit(-1);
    }

    // Perform runtime source compilation, and obtain kernel entry points.

    FILE *fp = fopen( kernel_file, "rb" );

    if( fp == NULL )
    {
        fprintf( stderr, "%s:%d: can't open kernel file: %s\n", \
             __FILE__, __LINE__, strerror( errno ));\
        exit( -1 );
    }

    fseek( fp, 0, SEEK_END );
    const size_t size = ftell( fp );
    const char *kernel_source = (const char *) malloc( size );

    rewind( fp );
    (void) fread( (void *) kernel_source, 1, size, fp );
    
    cl_program       program;

    program = clCreateProgramWithSource( context,
                                         1,
                                         &kernel_source,
                                         &size, 
                                         NULL );

    ret = clBuildProgram( program, 1, &device, NULL, NULL, NULL );

    static char buf[0x10000]={0};

    clGetProgramBuildInfo( program,
                           device,
                           CL_PROGRAM_BUILD_LOG,
                           0x10000,
                           buf,
                           NULL );

    printf( "%s\n", buf );

    ASSERT_CL_RETURN( ret );

    read_kernel = clCreateKernel( program, "read_kernel", &ret );

    ASSERT_CL_RETURN( ret );

    write_kernel = clCreateKernel( program, "write_kernel", &ret );

    ASSERT_CL_RETURN( ret );
}
