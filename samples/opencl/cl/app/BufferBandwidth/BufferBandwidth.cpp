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
#include "Host.h"
#include "Log.h"
#include "Timer.h"

#define  WS 128       // work group size

int nLoops;           // overall number of timing loops
int nRepeats;         // # of repeats for each transfer step
int nSkip;            // to discount lazy allocation effects, etc.
int nKLoops;          // repeat inside kernel to show peak mem B/W

int nBytes;           // input and output buffer size
int nThreads;         // number of GPU work items
int nItems;           // number of 32-bit 4-vectors for GPU kernel
int nAlign;           // safe bet for most PCs

int nBytesResult;

bool printLog;
bool doHost;
int  whichTest;
bool mapRW;
int  nWF;

TestLog *tlog;
bool vFailure=false;

void *memIn,
     *memOut,
     *memResult,
     *memScratch;

cl_mem inputBuffer,
       outputBuffer,
       resultBuffer,
       copyBuffer;

void usage()
{
            std::cout << "\nOptions:\n\n" <<
                 "   -t <n>             type of test:\n\n" <<
                 "                      0 clEnqueue[Map,Unmap]\n" <<
                 "                      1 clEnqueue[Read,Write]\n" <<
                 "                      2 clEnqueueCopy\n" <<
                 "                      3 clEnqueue[Read,Write], prepinned\n\n" <<
                 "   -pcie or -dma      measure PCIe/interconnect B/W\n" <<
				 "   -nwk <n>           number of CPU workers (max: 32 (Linux: 1))\n" <<
                 "   -nl <n>            number of timing loops\n" <<
                 "   -nr <n>            repeat each timing <n> times\n" <<
                 "   -nk <n>            number of loops in kernel\n" <<
                 "   -nb <n>            buffer size in bytes (min: 2048*CPU Workers)\n" <<
                 "   -nw <n>            # of wave fronts per SIMD\n" <<
                 "                      (default: 7)\n" <<
                 "   -l                 print complete timing log\n" <<
                 "   -s <n>             skip first <n> timings for average\n" <<
                 "                      (default: 1)\n" << 
                 "   -[if,of,cf] <n>    input, output, copy flags\n" <<
                 "                      (ok to use multiple):\n\n";

            for(int i = 0; i < nFlags; i++ )
                std::cout << "                      " << i << " " << flags[i].s << "\n";
            std::cout << "\n";

            std::cout << "   -m                 always map as MAP_READ | MAP_WRITE\n" <<
                         "   -db                disable host mem B/W baseline\n" <<
                         "   -d                 Number of GPU device\n" <<
                         "   -h                 print this message\n\n";
            exit(0);
}

void parseOptions(int argc, char * argv[])
{
    while(--argc) 
    {
#ifdef _WIN32
        if( strcmp(argv[argc], "-nwk") == 0 )
            nWorkers = atoi( argv[ argc + 1 ] );
#endif
        if( strcmp(argv[argc], "-nl") == 0 )
            nLoops = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nb") == 0 )
            nBytes = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nr") == 0 )
            nRepeats = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nk") == 0 )
           nKLoops = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-nw") == 0 )
           nWF = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-t") == 0 )
           whichTest = atoi( argv[ argc + 1 ] );

        if( strcmp(argv[argc], "-pcie") == 0 ||
            strcmp(argv[argc], "-dma") == 0 )
            whichTest = 5;

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

        if( strcmp(argv[argc], "-of") == 0 )
        {
            int f = atoi( argv[ argc + 1 ] );
            if( f < nFlags )
                outFlags |= flags[ f ].f;
        }

        if( strcmp(argv[argc], "-cf") == 0 )
        {
            int f = atoi( argv[ argc + 1 ] );
            if( f < nFlags )
                copyFlags |= flags[ f ].f;
        }

        if( strcmp(argv[argc], "-db") == 0 )
        {
            doHost = false;
        }

        if( strcmp(argv[argc], "-m") == 0 )
        {
            mapRW = true;
        }

        if( strcmp(argv[argc], "-d") == 0 )
        {
            devnum = atoi( argv[ argc + 1 ] );
        }

        if( strcmp(argv[argc], "-h") == 0 )
           usage();
    }

    if( whichTest > 5 )
        usage();
}

void timedBufMappedRead( cl_command_queue queue,
                         cl_mem buf,
                         unsigned char v,
                         bool pcie )
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

    if( !mapRW )
        mapFlag = CL_MAP_READ;

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

    bool verify = readVerifyMemCPU_MT( ptr, v, nBytes );

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

    if( pcie )
    {
        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "PCIe B/W host->device:", t1.GetElapsedTime(), nBytes, 1 );
    }
    else
    {
        const char *msg;

        if( mapRW )
            msg = "clEnqueueMapBuffer(READ|WRITE):";
        else
            msg = "clEnqueueMapBuffer(READ):";

        tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytes, 1 );
        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "CPU read:", t2.GetElapsedTime(), nBytes, 1 ); 

        if( verify )
            tlog->Msg( "%32s\n", "verification ok" );
        else
        {
            tlog->Error( "%32s\n", "verification FAILED" );
            vFailure = true;
        }

        tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", "clEnqueueUnmapMemObject():", t3.GetElapsedTime(), nBytes, 1 );
    }
}

void timedBufMappedWrite( cl_command_queue queue,
                          cl_mem buf,
                          unsigned char v,
                          bool pcie )
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

    if( !mapRW )
        mapFlag = CL_MAP_WRITE;

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

    memset_MT( ptr, v, nBytes );

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

    if( pcie )
    {
        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "PCIe B/W device->host:", t3.GetElapsedTime(), nBytes, 1 );
    }
    else
    {
        const char *msg;

        if( mapRW )
            msg = "clEnqueueMapBuffer(READ|WRITE):";
        else
            msg = "clEnqueueMapBuffer(WRITE):";

        tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytes, 1 );

        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "memset():", t2.GetElapsedTime(), nBytes, 1 );

        tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", "clEnqueueUnmapMemObject():", t3.GetElapsedTime(), nBytes, 1 );
    }
}

void timedBufMap( cl_command_queue queue,
                  cl_mem buf,
                  void **ptr,
                  bool quiet )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;
    cl_map_flags mapFlag = CL_MAP_READ | CL_MAP_WRITE;

    t1.Reset();
    t1.Start();

    *ptr = (void * ) clEnqueueMapBuffer( queue,
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

    const char *msg;

    if( mapRW )
        msg = "clEnqueueMapBuffer(READ|WRITE):";
    else
        msg = "clEnqueueMapBuffer(READ):";

    if( !quiet )
        tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytes, 1 );
}

void timedBufUnmap( cl_command_queue queue,
                    cl_mem buf,
                    void **ptr,
                    bool quiet )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Reset();
    t1.Start();

    ret = clEnqueueUnmapMemObject( queue,
                                   buf,
                                   (void *) *ptr,
                                   0, NULL, &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    if( !quiet )
        tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", "clEnqueueUnmapMemObject():", t1.GetElapsedTime(), nBytes, 1 );
}

void timedBufCLRead( cl_command_queue queue,
                     cl_mem buf,
                     void *ptr,
                     unsigned char v,
                     bool pcie )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Start();

    ret = clEnqueueReadBuffer( queue,
                               buf,
                               CL_FALSE,
                               0,
                               nBytes,
                               ptr,
                               0, NULL,
                               &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    bool verify = readVerifyMemCPU_MT( ptr, v, nBytes );

    if( pcie )
    {
        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "PCIe B/W device->host:", t1.GetElapsedTime(), nBytes, 1 );
    }
    else
    {
        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueReadBuffer:", t1.GetElapsedTime(), nBytes, 1 );

        if( verify )
            tlog->Msg( "%32s\n", "verification ok" );
        else
        {
            tlog->Error( "%32s\n", "verification FAILED" );
            vFailure = true;
        }
    }
}

void timedBufCLWrite( cl_command_queue queue,
                      cl_mem buf,
                      void *ptr,
                      unsigned char v,
                      bool pcie )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    memset( ptr, v, nBytes );

    t1.Start();

    ret = clEnqueueWriteBuffer( queue,
                                buf,
                                CL_FALSE,
                                0,
                                nBytes,
                                ptr,
                                0, NULL,
                                &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    if( pcie )
    {
        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "PCIe B/W host->device:", t1.GetElapsedTime(), nBytes, 1 );
    }
    else
    {
        tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueWriteBuffer:", t1.GetElapsedTime(), nBytes, 1 );
    }
}

void timedBufCLCopy( cl_command_queue queue,
                     cl_mem srcBuf,
                     cl_mem dstBuf )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Start();

    ret = clEnqueueCopyBuffer( queue,
                               srcBuf,
                               dstBuf,
                               0, 0,
                               nBytes,
                               0, NULL,
                               &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueCopyBuffer:", t1.GetElapsedTime(), nBytes, 1 );
}

void timedKernel( cl_command_queue queue,
                  cl_kernel        kernel,
                  cl_mem           bufSrc,
                  cl_mem           bufDst,
                  unsigned char    v,
                  bool             quiet )
{
     cl_int ret;
     cl_event ev=0;
     CPerfCounter t;

     cl_uint nItemsPerThread = nItems / nThreads;

     size_t global_work_size[2] = { nThreads, 0 };
     size_t local_work_size[2] = { WS, 0 };

     cl_uint val=0;

     for(int i = 0; i < sizeof( cl_uint ); i++)
        val |= v << (i * 8);

     clSetKernelArg( kernel, 0, sizeof(void *),  (void *) &bufSrc );
     clSetKernelArg( kernel, 1, sizeof(void *),  (void *) &bufDst );
     clSetKernelArg( kernel, 2, sizeof(cl_uint), (void *) &nItemsPerThread);
     clSetKernelArg( kernel, 3, sizeof(cl_uint), (void *) &val);
     clSetKernelArg( kernel, 4, sizeof(cl_uint), (void *) &nKLoops);

     t.Reset();
     t.Start();

     ret = clEnqueueNDRangeKernel( queue,
                                   kernel,
                                   1,
                                   NULL,
                                   global_work_size,
                                   local_work_size,
                                   0, NULL, &ev );
     ASSERT_CL_RETURN( ret );

     clFlush( queue );
     spinForEventsComplete( 1, &ev );

     t.Stop();

     if( !quiet )
         tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueNDRangeKernel():", 
                      t.GetElapsedTime(), nBytes, nKLoops );
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
            tlog->Msg( "%32s\n", "verification ok" );
        else
            tlog->Error( "%32s\n", "verification FAILED" );
    }
}

void createBuffers()
{
   // host memory buffers

#ifdef _WIN32
   memIn =      (void *) _aligned_malloc( nBytes, nAlign );
   memOut =     (void *) _aligned_malloc( nBytes, nAlign );
   memResult =  (void *) _aligned_malloc( nBytesResult, nAlign );
   memScratch = (void *) _aligned_malloc( nBytes, nAlign );
#else
   memIn =      (void *) memalign( nAlign, nBytes );
   memOut =     (void *) memalign( nAlign, nBytes );
   memResult =  (void *) memalign( nAlign, nBytesResult );
   memScratch = (void *) memalign( nAlign, nBytes );
#endif

   if( memIn == NULL ||
       memOut == NULL ||
       memResult == NULL ||
       memScratch == NULL ) 
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

   hostPtr = NULL;

   if( outFlags & CL_MEM_USE_HOST_PTR ||
       outFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memOut;

   outputBuffer = clCreateBuffer( context,
                                  outFlags,
                                  nBytes,
                                  hostPtr, &ret );

   ASSERT_CL_RETURN( ret );

   hostPtr = NULL;

   if( copyFlags & CL_MEM_USE_HOST_PTR ||
       copyFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memScratch;

   if( whichTest == 2 ||
       whichTest == 3 ||
       whichTest == 5 )
      copyBuffer = clCreateBuffer( context,
                                   copyFlags,
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
	std::cout << "\nDevice " << std::setw(2) << devnum << "            " << devname << "\n";

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
    std::cout << "Build:               release\n";
#else
    std::cout << "Build:               DEBUG\n";
#endif
#endif

    std::cout << "GPU work items:      " << nThreads << std::endl <<
                 "Buffer size:         " << nBytes << std::endl <<
                 "CPU workers:         " << nWorkers << std::endl <<
                 "Timing loops:        " << nLoops << std::endl <<
                 "Repeats:             " << nRepeats << std::endl <<
                 "Kernel loops:        " << nKLoops << std::endl;

    std::cout << "inputBuffer:         ";

   for( int i = 0; i < nFlags; i++ )
      if( inFlags & flags[i].f )
          std::cout << flags[i].s << " ";

   std::cout << "\noutputBuffer:        ";

   for( int i = 0; i < nFlags; i++ )
      if( outFlags & flags[i].f )
         std::cout << flags[i].s << " ";

   if( whichTest == 2 ||
       whichTest == 3 ||
       whichTest == 5 )
   {
       std::cout << "\ncopyBuffer:          " ;

      for( int i = 0; i < nFlags; i++ )
      if( copyFlags & flags[i].f )
          std::cout << flags[i].s << " ";
   }

   std::cout << "\n\n";
}

void printResults()
{
   if( printLog ) 
      tlog->printLog();

   tlog->printSummary( nSkip );

   std::cout << "\n" ;
   fflush(stdout);
}

void runMapTest()
{
   int  nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Host mapped write to inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufMappedWrite( queue, inputBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "2. GPU kernel read of inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputBuffer, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel write to outputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "4. Host mapped read of outputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufMappedRead( queue, outputBuffer, nl & 0xff, false );

      tlog->Msg( "%s\n", "" );
   }
}

void runPCIeTest()
{
   int  nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "%s\n", "" );

      for(int i = 0; i < nRepeats; i++)
          timedBufMappedWrite( queue, inputBuffer, nl & 0xff, true );

      for(int i = 0; i < nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputBuffer, resultBuffer, nl & 0xff, true );

      for(int i = 0; i < nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputBuffer, nl & 0xff, true );

      for(int i = 0; i < nRepeats; i++)
          timedBufMappedRead( queue, outputBuffer, nl & 0xff, true );
      
      tlog->Msg( "%s\n", "" );
   }
}

void runPCIeTest2()
{
   int nl = nLoops;

   void *mappedPtr;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "%s\n", "" );

      timedBufMap( queue, copyBuffer, &mappedPtr, true );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLWrite( queue, inputBuffer, mappedPtr, nl & 0xff, true );

      for(int i = 0; i < nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputBuffer, resultBuffer, nl & 0xff, true );

      for(int i = 0; i < nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputBuffer, nl & 0xff, true );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLRead( queue, outputBuffer, mappedPtr, nl & 0xff, true );

      timedBufUnmap( queue, copyBuffer, &mappedPtr, true );

      tlog->Msg( "%s\n", "" );
   }
}

void runReadWriteTest()
{
   int nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Host CL write to inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLWrite( queue, inputBuffer, memScratch, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "2. GPU kernel read of inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputBuffer, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel write to outputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "4. Host CL read of outputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLRead( queue, outputBuffer, memScratch, nl & 0xff, false );

      tlog->Msg( "%s\n", "" );
   }
}

void runMappedReadWriteTest()
{
   int nl = nLoops;

   void *mappedPtr;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Mapping copyBuffer as mappedPtr" );

      timedBufMap( queue, copyBuffer, &mappedPtr, false );

      tlog->Msg( "\n%s\n\n", "2. Host CL write from mappedPtr to inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLWrite( queue, inputBuffer, mappedPtr, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel read of inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputBuffer, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "4. GPU kernel write to outputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "5. Host CL read of outputBuffer to mappedPtr" );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLRead( queue, outputBuffer, mappedPtr, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "6. Unmapping copyBuffer" );

      timedBufUnmap( queue, copyBuffer, &mappedPtr, false );

      tlog->Msg( "%s\n", "" );
   }
}

void runCopyTest()
{
   int nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Host mapped write to copyBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufMappedWrite( queue, copyBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "2. CL copy of copyBuffer to inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLCopy( queue, copyBuffer, inputBuffer );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel read of inputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputBuffer, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "4. GPU kernel write to outputBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "5. CL copy of outputBuffer to copyBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufCLCopy( queue, outputBuffer, copyBuffer );

      tlog->Msg( "\n%s\n\n", "6. Host mapped read of copyBuffer" );

      for(int i = 0; i < nRepeats; i++)
          timedBufMappedRead( queue, copyBuffer, nl & 0xff, false );

      tlog->Msg( "%s\n", "" );
   }
}

void initDefaults()
{
    nWorkers = 1;
    nLoops =   20;
    nRepeats = 1;
    nSkip =    2;
    nKLoops =  20;

    nBytes =   32*1024*1024;
    nAlign =   4096;

    printLog =  false;
    doHost =    true;
    whichTest = 0;
    mapRW =     false;
    nWF =       7;
    devnum =    0;
}

void computeGlobals()
{
    if( nWorkers > MAXWORKERS ) nWorkers = MAXWORKERS;
    if( nWorkers <= 0 ) nWorkers = 1;

    cl_mem_flags f = CL_MEM_READ_ONLY |
                     CL_MEM_WRITE_ONLY |
                     CL_MEM_READ_WRITE;

    if( (inFlags & f) == 0 )
             inFlags |= CL_MEM_READ_ONLY;

    if( (outFlags & f) == 0 )
             outFlags |= CL_MEM_WRITE_ONLY;

    f |= CL_MEM_USE_HOST_PTR |
         CL_MEM_COPY_HOST_PTR |
         CL_MEM_ALLOC_HOST_PTR |
         CL_MEM_USE_PERSISTENT_MEM_AMD;

    if( (copyFlags & f) == 0 )
             copyFlags = CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_WRITE;

    f = CL_MEM_READ_ONLY |
        CL_MEM_WRITE_ONLY |
        CL_MEM_READ_WRITE;

    if( (copyFlags & f) == 0 )
             copyFlags |= CL_MEM_READ_WRITE;

    nSkip = nLoops > nSkip ? nSkip : 0;

    if( whichTest == 4 ||
        whichTest == 5 )
    {
        inFlags = CL_MEM_READ_ONLY;
        outFlags = CL_MEM_WRITE_ONLY;
        copyFlags = CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE;
        nKLoops = 1;
        doHost = false;
    }

    // educated guess of optimal work size
    int minBytes = WS * sizeof( cl_uint ) * 4 * nWorkers;

    nBytes = ( nBytes / minBytes ) * minBytes;
    nBytes = nBytes < minBytes ? minBytes : nBytes;
    nItems = nBytes / ( 4 * sizeof(cl_uint) );
    
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

    initCL( (char *) "BufferBandwidth_Kernels.cl" );

    computeGlobals();
    printHeader();

    createBuffers();

#ifdef MEM_MULTICORE
    launchThreads();
#endif

    if( doHost )
       assessHostMemPerf( memIn, memOut, nBytes );

    switch( whichTest )
    {
       case 0: runMapTest(); break;
       case 1: runReadWriteTest(); break;
       case 2: runCopyTest(); break;
       case 3: runMappedReadWriteTest(); break;
       case 4: runPCIeTest(); break;
       case 5: runPCIeTest2(); break;
    }

    printResults();

#ifdef MEM_MULTICORE
    shutdownThreads();
#endif

    if( vFailure )
    {
        fprintf( stderr, "Failed!\n");
        return -1;
    }
    else
    {
        fprintf( stderr, "Passed!\n");
        return 0;
    }
}
