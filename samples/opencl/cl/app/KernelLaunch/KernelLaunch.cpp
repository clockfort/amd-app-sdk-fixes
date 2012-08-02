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
#include "Log.h"
#include "Timer.h"

#define  WS 64      // work group size
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define SDK_EXPECTED_FAILURE 2


int nLoops;         // overall number of timing loops
int nRepeats;       // # of repeats for each transfer step
int nKernelsBatch;  // time batch of kernels
int nSkip;          // to discount lazy allocation effects, etc.
int nKLoops;        // repeat inside kernel to show peak mem B/W
int status = 0;

int nBytes;         // input and output buffer size
int nThreads;       // number of GPU work items
int nItems;         // number of 32-bit 4-vectors for GPU kernel
int nAlign;         // safe bet for most PCs

int nBytesResult;

bool printLog;
bool doHost;
int  whichTest;
int  nWF;

TestLog *tlog;
bool vFailure=false;

void *memIn,
     *memOut,
     *memResult;

cl_mem inputBuffer,
       resultBuffer;

void usage()
{
            std::cout << "\nOptions:\n\n";
            std::cout << "   -d <n>             number of GPU device\n" ;
            std::cout << "   -nl <n>            number of timing loops\n";
            std::cout << "   -nr <n>            repeat each timing <n> times\n";
            std::cout << "   -nk <n>            number of loops in kernel\n";
            std::cout << "   -nkb <n>           number of kernel launches per batch\n";
            std::cout << "   -nb <n>            buffer size in bytes\n";
            std::cout << "   -nw <n>            # of wave fronts per SIMD\n";
            std::cout << "                      (default: 7)\n";
            std::cout << "   -l                 print complete timing log\n";
            std::cout << "   -s <n>             skip first <n> timings for average\n";
            std::cout << "                      (default: 1)\n";
            std::cout << "   -if <n>            input flags\n";
            std::cout << "                      (ok to use multiple):\n\n";

            for( int i = 0; i < nFlags; i++ )
               std::cout << "                      " << i << "  " << flags[i].s;
            std::cout << "\n";

            std::cout << "   -h                 print this message\n\n";

            exit(0);
}

void parseOptions(int argc, char * argv[])
{
    while(--argc) 
    {
        if( strcmp(argv[argc], "-nl") == 0 )
            nLoops = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nb") == 0 )
            nBytes = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nr") == 0 )
            nRepeats = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nk") == 0 )
           nKLoops = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nkb") == 0 )
           nKernelsBatch = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nw") == 0 )
           nWF = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-s") == 0 )
            nSkip = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-l") == 0 )
            printLog = true;

        if( strcmp(argv[argc], "-if") == 0 )
        {
            int f = atoi( argv[ argc + 1 ] );
            if( f < nFlags )
                inFlags |= flags[ f ].f;
        }

        if( strcmp(argv[argc], "-d") == 0 )
        {
            devnum = atoi( argv[ argc + 1 ] );
        }

        if( strcmp(argv[argc], "-h") == 0 )
           usage();
    }

    cl_mem_flags f = CL_MEM_READ_ONLY |
                     CL_MEM_WRITE_ONLY |
                     CL_MEM_READ_WRITE;

    if( (inFlags & f) == 0 )
             inFlags |= CL_MEM_READ_ONLY;

    nSkip = nLoops > nSkip ? nSkip : 0;
}

void timedBufMappedWrite( cl_command_queue queue,
                          cl_mem buf,
                          unsigned char v )
{
    CPerfCounter t1, t2, t3;
    cl_int ret;
    cl_event ev;
    void *ptr;
    cl_map_flags mapFlag = CL_MAP_READ | CL_MAP_WRITE;

    t1.Reset();
    t2.Reset();
    t3.Reset();

    t1.Start();

    ptr = (void * ) clEnqueueMapBuffer( queue,
                                        buf,
                                        CL_FALSE,
                                        mapFlag,
                                        0,
                                        nBytes,
                                        0, NULL, 
                                        &ev,
                                        &ret );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    t2.Start();

    memset( ptr, v, nBytes );

    t2.Stop();

    t3.Start();

    ret = clEnqueueUnmapMemObject( queue,
                                   buf,
                                   (void *) ptr,
                                   0, NULL, &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t3.Stop();
}

void timedKernel( cl_command_queue queue,
                  cl_kernel        kernel,
                  cl_mem           bufSrc,
                  cl_mem           bufDst,
                  unsigned char    v,
                  bool quiet )
{
     cl_int ret;
     cl_event ev = 0;
     CPerfCounter t; 

     cl_uint nItemsPerThread = nItems / nThreads;

     size_t global_work_size[2] = { nThreads, 0 };
     size_t local_work_size[2] = { WS, 0 };

     cl_uint val = 0;

     for(int i = 0; i < sizeof( cl_uint ); i++)
        val |= v << (i * 8);

     clSetKernelArg( kernel, 0, sizeof(void *),  (void *) &bufSrc );
     clSetKernelArg( kernel, 1, sizeof(void *),  (void *) &bufDst );
     clSetKernelArg( kernel, 2, sizeof(cl_uint), (void *) &nItemsPerThread);
     clSetKernelArg( kernel, 3, sizeof(cl_uint), (void *) &val);
     clSetKernelArg( kernel, 4, sizeof(cl_uint), (void *) &nKLoops);

     t.Reset();
     t.Start();

     for( int r = 0; r < nKernelsBatch; r++ )
     {
        ret = clEnqueueNDRangeKernel( queue,
                                      kernel,
                                      1,
                                      NULL,
                                      global_work_size,
                                      local_work_size,
                                      0, NULL, &ev );
         ASSERT_CL_RETURN( ret );
     }

     clFlush( queue );
     spinForEventsComplete( 1, &ev );

     t.Stop();

     if( !quiet )
         tlog->Timer( "%32s  %lf s\n", "clEnqueueNDRangeKernel():", 
                      t.GetElapsedTime() / nKernelsBatch, nBytes, nKLoops );
}

void timedReadKernelVerify( cl_command_queue queue,
                            cl_kernel        kernel,
                            cl_mem           bufSrc,
                            cl_mem           bufRes,
                            unsigned char    v,
                            bool             quiet )
{
    cl_int ret;
    cl_event ev;

    timedKernel( queue, kernel, bufSrc, bufRes, v, quiet );

    ret = clEnqueueReadBuffer( queue,
                               bufRes,
                               CL_FALSE,
                               0,
                               nBytesResult,
                               memResult,
                               0, NULL,
                               &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

     cl_uint sum = 0;

     for(int i = 0; i < nThreads / WS; i++)
         sum += ((cl_uint *) memResult)[i];

     bool verify;

     if( sum == nBytes / sizeof(cl_uint) )
         verify = true;
     else
     {
         verify = false;
         vFailure = true;
     }
 
    if( !quiet )
    {
      if( verify )
        tlog->Msg( "%s\n", "\nPassed" );
      else
        tlog->Error( "%s\n", "\nFailed" );
    }
}

void createBuffers()
{
   // host memory buffers

    // return if a error or expected error has occured	
    if(status != SDK_SUCCESS) 
    return;
    

#ifdef _WIN32
   memIn =      (void *) _aligned_malloc( nBytes, nAlign );
   memOut =     (void *) _aligned_malloc( nBytes, nAlign );
   memResult =  (void *) _aligned_malloc( nBytesResult, nAlign );
#else
   memIn =      (void *) memalign( nAlign, nBytes );
   memOut =     (void *) memalign( nAlign, nBytes );
   memResult =  (void *) memalign( nBytesResult, nBytesResult );
#endif

   if( memIn == NULL ||
       memOut == NULL ||
       memResult == NULL ) 
   {
       fprintf( stderr, "%s:%d: error: %s\n", \
                __FILE__, __LINE__, "could not allocate host buffers\n" );
       exit(-1);
   }

   // CL buffers

   cl_int ret;
   void *hostPtr = NULL;

   if( inFlags & CL_MEM_USE_HOST_PTR ||
       inFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memIn;

   inputBuffer = clCreateBuffer( context,
                                 inFlags,
                                 nBytes,
                                 hostPtr, &ret );

   ASSERT_CL_RETURN( ret );

   resultBuffer = clCreateBuffer( context,
                                  CL_MEM_READ_WRITE,
                                  nBytesResult,
                                  NULL, &ret );
   ASSERT_CL_RETURN( ret );
}

void printHeader()
{
// return if a error or expected error has occured	
    if(status != SDK_SUCCESS) 
    return;   

    std::cout <<"\nDevice " << devnum << ":            " << devname << std::endl;

#ifdef _WIN32
   std::cout << "Build:               _WINxx"; 
#ifdef _DEBUG
   std::cout << " DEBUG";
#else
   std::cout << " release";
#endif
   std::cout << "\n" ;
#else
#ifdef NDEBUG
    std::cout <<"Build:               release\n";
#else
    std::cout <<"Build:               DEBUG\n";
#endif
#endif

   std::cout << "GPU work items:      " << nThreads << std::endl;
   std::cout << "Buffer size:         " << nBytes << std::endl;
   std::cout << "Timing loops:        " << nLoops << std::endl; 
   std::cout << "Repeats:             " << nRepeats << std::endl;
   std::cout << "Kernel loops:        " << nKLoops << std::endl;
   std::cout << "Batch size:          " << nKernelsBatch << std::endl;

   std::cout << "inputBuffer:         " << std::endl;

   for( int i = 0; i < nFlags; i++ )
      if( inFlags & flags[i].f )
          std::cout << flags[i].s << std::endl;

   std::cout <<"\n\n";
}

void printResults()
{
   if( printLog ) 
      tlog->printLog();

   tlog->printSummary( nSkip );

   std::cout << "\n" ;
   fflush( stdout );
}

void runKernelLaunchTest()
{
   CPerfCounter t;
   int  nl = nLoops;
   
    // return if an error or expected error has already occured
    if(status)
    return;

   t.Reset(); t.Start();

   while( nl-- )
   {
      tlog->loopMarker();

      for(int i = 0; i < nRepeats; i++)
          timedBufMappedWrite( queue, inputBuffer, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "GPU kernel read of inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputBuffer, resultBuffer, nl & 0xff, false );

      tlog->Msg( "%s\n", "" );
   }
}

void initDefaults()
{
   devnum =         0;
   nLoops =         1000;
   nKernelsBatch =  1;
   nRepeats =       1;
   nSkip =          2; 
   nKLoops =        1;  

   nBytes =         1024;
   nAlign =         4096;
   printLog =       false;
   doHost =         true;
   whichTest =      0;
   nWF =            7;
}

void computeGlobals()
{
    // educated guess of optimal work size

    int minBytes = WS * sizeof( cl_uint ) * 4;

    nBytes = ( nBytes / minBytes ) * minBytes;
    nBytes = nBytes < minBytes ? minBytes : nBytes;
    nItems = nBytes / ( 4 * sizeof(cl_uint));
    
    int maxThreads = nBytes / ( 4 * sizeof( cl_uint ) );

    nThreads = deviceMaxComputeUnits * nWF * WS;
 
    if( nThreads > maxThreads )
        nThreads = maxThreads;
    else
    {
        while( nItems % nThreads != 0 )
            nThreads += WS;
    }

    nBytesResult = ( nThreads / WS ) * sizeof(cl_uint);
}

int main(int argc, char **argv)
{
    initDefaults();

    parseOptions( argc, argv );

    tlog = new TestLog( nLoops * nRepeats * 50 );

    initCL( (char *) "KernelLaunch_Kernels.cl" );

    computeGlobals();
    printHeader();

    createBuffers();

    runKernelLaunchTest();

    printResults();

    if(vFailure || (status == SDK_FAILURE))
    {
    std::cout << "Failed!";
        return -1;
    }
// Passed or Expected Failure
    else
    {
    if(SDK_SUCCESS)
        std::cout << "Passed!/n";
        return SDK_SUCCESS;
    }
}
