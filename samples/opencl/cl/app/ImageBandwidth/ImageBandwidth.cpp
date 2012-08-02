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

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

int nLoops;               // overall number of timing loops
int nRepeats;             // # of repeats for each transfer step
int nSkip;                // to discount lazy allocation effects, etc.
int nKLoops;              // repeat inside kernel to show peak mem B/W
int nKLaunches;           // launch many async kernels to verify absence
                          // of performance gains due to intra-kernel cache hits
int nLocalThreadsX;
int nLocalThreadsY;
int nThreadsX;
int nThreadsY;
int nStrips;

int nGroups;

int imageWidth;
int imageHeight;

size_t  imageOrigin[3];
size_t  imageRegion[3];

int pixFormat;
int nChannels;
int nBytesPerChannel;
int nBytes;               // input and output image/buffer size
int nBytesRegion;         // size of image region
int nPixels;              // number of pixels in image
int nPixelsPerThread;

int nAlign;
int nBytesResult;

bool printLog;
bool doHost;
int  whichTest;
bool mapRW;

TestLog *tlog;
bool vFailure=false;

void *memIn,
     *memOut,
     *memResult,
     *memRW;

cl_image_format pixelFormat;

cl_mem inputImage,
       outputImage,
       inputCopyImage,
       outputCopyImage;

cl_mem inputCopyBuffer,
       outputCopyBuffer,
       resultBuffer;

struct _formats { cl_image_format f;
                  cl_int          nChannels;
                  cl_int          nBytesPerChannel;
                  const char  *s;} formats[] = {

             { { CL_RGBA, CL_UNSIGNED_INT32 },  4, 4, "CL_RGBA / CL_UNSIGNED_INT32" },
             { { CL_RGBA, CL_UNSIGNED_INT8 },   4, 1, "CL_RGBA / CL_UNSIGNED_INT8" } };

int nFormats = sizeof(formats) / sizeof(formats[0]);

void usage()
{
            std::cout << "\nOptions:\n\n";
            std::cout << "   -t <n>             type of test:\n\n" ;
            std::cout << "                      0 clEnqueue[Map,Unmap]Image\n" ;
            std::cout << "                      1 clEnqueue[Read,Write]Image\n" ;
            std::cout << "                      2 clEnqueueCopyImage\n" ;
            std::cout << "                      3 clEnqueueCopyBufferToImage and\n" ;
            std::cout << "                        clEnqueueCopyImageToBuffer\n" ;
            std::cout << "                      4 clEnqueue[Read,Write]Image, prepinned\n\n" ;
            std::cout << "   -d <n>             number of GPU device\n" ;
            std::cout << "   -x <n>             image width in pixels\n" ;
            std::cout << "   -y <n>             image height in pixels\n";
            std::cout << "   -ox <n>            sub image origin x in pixels\n" ;
            std::cout << "   -oy <n>            sub image origin y pixels\n";
            std::cout << "   -rx <n>            sub image region x in pixels\n" ;
            std::cout << "   -ry <n>            sub image region y pixels\n";
            std::cout << "   -nl <n>            number of timing loops\n";
            std::cout << "   -nr <n>            time each CL command <n> times\n";
            std::cout << "   -nk <n>            number of loops in kernel\n";
            std::cout << "   -nkl <n>           number of kernel launches\n";
            std::cout << "   -l                 print complete timing log\n";
            std::cout << "   -s <n>             skip first <n> timings for average\n";
            std::cout << "                      (default: 1)\n";
            std::cout << "   -[if,of,cf] <n>    input, output, copy mem object flags\n";
            std::cout << "                      (ok to use multiple):\n\n";

            for( int i=0; i < nFlags ; i++ )
                std::cout << "                      " << i << "  " << flags[i].s << std::endl;
            std::cout << "\n" ;

            std::cout << "   -p <n>             pixel format\n\n";

            for( int i=0; i < nFormats ; i++ )
                std::cout << "                      " << i << "  " << formats[i].s << std::endl;
            std::cout << "\n";

            std::cout << "   -m                 always map as MAP_READ | MAP_WRITE\n";
            std::cout << "   -db                disable host mem B/W baseline\n";
            std::cout << "   -h                 print this message\n\n";

            exit(0);
}

void parseOptions(int argc, char * argv[])
{
    while(--argc) 
    {
        if( strcmp(argv[argc], "-nl") == 0 )
            nLoops = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-x") == 0 )
            imageWidth = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-y") == 0 )
            imageHeight = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-ox") == 0 )
            imageOrigin[0] = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-oy") == 0 )
            imageOrigin[1] = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-rx") == 0 )
            imageRegion[0] = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-ry") == 0 )
            imageRegion[1] = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-nr") == 0 )
            nRepeats = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-nk") == 0 )
            nKLoops = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-nkl") == 0 )
            nKLaunches = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-t") == 0 )
            whichTest = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-s") == 0 )
            nSkip = atoi( argv[ argc+1 ] );

        if( strcmp(argv[argc], "-l") == 0 )
            printLog = true;

        if( strcmp(argv[argc], "-if") == 0 )
        {
            int f = atoi( argv[ argc+1 ] );
            if( f < nFlags )
                inFlags |= flags[ f ].f;
        }

        if( strcmp(argv[argc], "-of") == 0 )
        {
            int f = atoi( argv[ argc+1 ] );
            if( f < nFlags )
                outFlags |= flags[ f ].f;
        }

        if( strcmp(argv[argc], "-cf") == 0 )
        {
            int f = atoi( argv[ argc+1 ] );
            if( f < nFlags )
                copyFlags |= flags[ f ].f;
        }

        if( strcmp(argv[argc], "-p") == 0 )
        {
            int f = atoi( argv[ argc+1 ] );
            if( f < nFormats )
                pixFormat = f;
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
            devnum = atoi( argv[ argc+1 ] );
        }

        if( strcmp(argv[argc], "-h") == 0 )
           usage();
    }

    if( whichTest > 4 )
        usage();
}

void timedImageMappedRead( cl_command_queue queue,
                           cl_mem image,
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

     if( !mapRW )
        mapFlag = CL_MAP_READ;

    size_t  rowPitch;

    t1.Start();

    ptr = clEnqueueMapImage( queue,
                             image, 
                             CL_FALSE, 
                             mapFlag,
                             imageOrigin, imageRegion,
                             &rowPitch, NULL,
                             0, NULL,
                             &ev,
                             &ret );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();
    t2.Start();

    bool verify = readmem2DPitch( ptr, v, rowPitch , (int) imageRegion[1] );

    t2.Stop();
    t3.Start();

    ret = clEnqueueUnmapMemObject( queue,
                                   image,
                                   (void *) ptr,
                                   0, NULL, &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t3.Stop();

    const char *msg;

    if( mapRW )
        msg = "clEnqueueMapImage(READ|WRITE):";
    else
        msg = "clEnqueueMapImage(READ):";

    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytesRegion, 1 );
    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "CPU read:", t2.GetElapsedTime(), nBytesRegion, 1 ); 

    if( verify )
        tlog->Msg( "%32s\n", "verification ok" );
    else
    {
        tlog->Error( "%32s\n", "verification FAILED" );
        vFailure = true;
    }

    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", "clEnqueueUnmapMemObject():", t3.GetElapsedTime(), nBytesRegion, 1 );
}

void timedImageMappedWrite( cl_command_queue queue,
                            cl_mem image,
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

    if( !mapRW )
        mapFlag = CL_MAP_WRITE;

    size_t  rowPitch;

    t1.Start();

    ptr = clEnqueueMapImage( queue,
                             image, 
                             CL_FALSE, 
                             mapFlag,
                             imageOrigin, imageRegion,
                             &rowPitch, NULL,
                             0, NULL,
                             &ev,
                             &ret );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();
    t2.Start();

    memset2DPitch( ptr, v, 
                 imageRegion[0] * nChannels * nBytesPerChannel,
                 imageRegion[1], rowPitch );

    t2.Stop();
    t3.Start();

    ret = clEnqueueUnmapMemObject( queue,
                                   image,
                                   (void *) ptr,
                                   0, NULL, &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t3.Stop();

    const char *msg;

    if( mapRW )
        msg = "clEnqueueMapImage(READ|WRITE):";
    else
        msg = "clEnqueueMapImage(WRITE):";

    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytesRegion, 1 );
    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "memset():", t2.GetElapsedTime(), nBytesRegion, 1 );    
    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", "clEnqueueUnmapMemObject():", t3.GetElapsedTime(), nBytesRegion, 1 );
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
                                         nBytesRegion,
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
        tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytesRegion, 1 );
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

void timedImageCLRead( cl_command_queue queue,
                       cl_mem image,
                       void *ptr,
                       unsigned char v )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Start();

    ret = clEnqueueReadImage( queue,
                              image,
                              CL_FALSE,
                              imageOrigin, imageRegion,
                              0, 0,
                              ptr,
                              0, NULL,
                              &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    bool verify = readmem2DPitch( ptr, v, nBytesRegion, 1 );

    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueReadImage():", t1.GetElapsedTime(), nBytesRegion, 1 );

    if( verify )
        tlog->Msg( "%32s\n", "verification ok" );
    else
    {
        tlog->Error( "%32s\n", "verification FAILED" );
        vFailure = true;
    }
}

void timedImageCLWrite( cl_command_queue queue,
                      cl_mem image,
                      void *ptr )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Start();

    ret = clEnqueueWriteImage( queue,
                               image,
                               CL_FALSE,
                               imageOrigin, imageRegion,
                               0,0,
                               ptr,
                               0, NULL,
                               &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();
    
    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueWriteImage():", t1.GetElapsedTime(), nBytesRegion, 1 );
}

void timedImageCLCopy( cl_command_queue queue,
                       cl_mem srcImg,
                       cl_mem dstImg )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Start();

    ret = clEnqueueCopyImage( queue,
                              srcImg,
                              dstImg,
                              imageOrigin,
                              imageOrigin,
                              imageRegion,
                              0, NULL,
                              &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueCopyImage():", t1.GetElapsedTime(), nBytesRegion, 1 );
}

void timedImageBufferCLCopy( cl_command_queue queue,
                             cl_mem srcImg,
                             cl_mem dstBuf )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Start();

    ret = clEnqueueCopyImageToBuffer( queue,
                                      srcImg,
                                      dstBuf,
                                      imageOrigin, 
                                      imageRegion,
                                      0,
                                      0, NULL,
                                      &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueCopyImageToBuffer():", t1.GetElapsedTime(), nBytesRegion, 1 );
}

void timedBufferImageCLCopy( cl_command_queue queue,
                             cl_mem srcBuf,
                             cl_mem dstImg )
{
    CPerfCounter t1;
    cl_int ret;
    cl_event ev;

    t1.Start();

    ret = clEnqueueCopyBufferToImage( queue,
                                      srcBuf,
                                      dstImg,
                                      0,
                                      imageOrigin,
                                      imageRegion,
                                      0, NULL,
                                      &ev );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();

    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueCopyBufferToImage():", t1.GetElapsedTime(), nBytesRegion, 1 );
}

void timedBufMappedRead( cl_command_queue queue,
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

    if( !mapRW )
        mapFlag = CL_MAP_READ;

    ptr = (void * ) clEnqueueMapBuffer( queue,
                                        buf,
                                        CL_FALSE,
                                        mapFlag,
                                        0,
                                        nBytesRegion,
                                        0, NULL,
                                        &ev,
                                        &ret );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();
    t2.Start();

    bool verify = readmem2DPitch( ptr, v, nBytesRegion, 1 );

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

    const char *msg;

    if( mapRW )
        msg = "clEnqueueMapBuffer(READ|WRITE):";
    else
        msg = "clEnqueueMapBuffer(READ):";

    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytesRegion, 1 );
    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "CPU read:", t2.GetElapsedTime(), nBytesRegion, 1 ); 

    if( verify )
        tlog->Msg( "%32s\n", "verification ok" );
    else
    {
        tlog->Error( "%32s\n", "verification FAILED" );
        vFailure = true;
    }

    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", "clEnqueueUnmapMemObject():", t3.GetElapsedTime(), nBytesRegion, 1 );
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

    if( !mapRW )
        mapFlag = CL_MAP_WRITE;

    ptr = (void * ) clEnqueueMapBuffer( queue,
                                        buf,
                                        CL_FALSE,
                                        mapFlag,
                                        0,
                                        nBytesRegion,
                                        0, NULL, 
                                        &ev,
                                        &ret );
    ASSERT_CL_RETURN( ret );

    clFlush( queue );
    spinForEventsComplete( 1, &ev );

    t1.Stop();
    t2.Start();

    memset( ptr, v, nBytesRegion );

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

    const char *msg;

    if( mapRW )
        msg = "clEnqueueMapBuffer(READ|WRITE):";
    else
        msg = "clEnqueueMapBuffer(WRITE):";

    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", msg, t1.GetElapsedTime(), nBytesRegion, 1 );
    tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "memset():", t2.GetElapsedTime(), nBytesRegion, 1 );    
    tlog->Timer( "%32s  %lf s [ %8.2lf GB/s ]\n", "clEnqueueUnmapMemObject():", t3.GetElapsedTime(), nBytesRegion, 1 );
}

void timedKernel( cl_command_queue queue,
                  cl_kernel        kernel,
                  cl_mem           bufSrc,
                  cl_mem           bufDst,
                  unsigned char    v )
{
     cl_int       ret;
     cl_event     ev = 0;
     CPerfCounter t;

     size_t global_work_offset[2] = { imageOrigin[0], imageOrigin[1] };
     size_t global_work_size[2] =   { nThreadsX, nThreadsY };
     size_t local_work_size[2] =    { nLocalThreadsX, nLocalThreadsY };

     cl_uint val=0;

     for(int i=0; i < nBytesPerChannel; i++)
        val |= v << (i*8);

     ret  = clSetKernelArg( kernel, 0, sizeof(void *),  (void *) &bufSrc );
     ret |= clSetKernelArg( kernel, 1, sizeof(void *),  (void *) &bufDst );
     ret |= clSetKernelArg( kernel, 2, sizeof(int),     (void *) &nPixelsPerThread );
     ret |= clSetKernelArg( kernel, 3, sizeof(cl_uint), (void *) &val );
     ret |= clSetKernelArg( kernel, 4, sizeof(cl_uint), (void *) &nKLoops );

     ASSERT_CL_RETURN( ret );

     t.Reset();
     t.Start();

     for(int i=0; i < nKLaunches; i++)
     {
        ret = clEnqueueNDRangeKernel( queue,
                                      kernel,
                                      2,
                                      global_work_offset,
                                      global_work_size,
                                      local_work_size,
                                      0, NULL, &ev );
        ASSERT_CL_RETURN( ret );
     }

     clFlush( queue );
     spinForEventsComplete( 1, &ev );

     t.Stop();


     tlog->Timer( "%32s  %lf s   %8.2lf GB/s\n", "clEnqueueNDRangeKernel():", 
                  t.GetElapsedTime() / nKLaunches, nBytesRegion, nKLoops );
}

void timedReadKernelVerify( cl_command_queue queue,
                            cl_kernel        kernel,
                            cl_mem           bufSrc,
                            cl_mem           bufRes,
                            unsigned char    v,
                            bool quiet )
{
    cl_int ret;
    cl_event ev;

    timedKernel( queue, kernel, bufSrc, bufRes, v );

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

     for(int i=0; i < nGroups; i++)
         sum += ((cl_uint *) memResult)[i];

     bool verify;

     if( sum == nBytesRegion / nBytesPerChannel )
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

void createHostBuffers()
{
   // host memory buffers

#ifdef _WIN32
   memIn =     (void *) _aligned_malloc( nBytes, nAlign );
   memOut =    (void *) _aligned_malloc( nBytes, nAlign );
   memResult = (void *) _aligned_malloc( nBytesResult, nAlign );
   memRW     = (void *) _aligned_malloc( nBytes, nAlign );
#else
   memIn =     (void *) memalign( nAlign, nBytes );
   memOut =    (void *) memalign( nAlign, nBytes );
   memResult = (void *) memalign( nBytesResult, nBytesResult );
   memRW     = (void *) memalign( nAlign, nBytes );
#endif

   if( memIn == NULL ||
       memOut == NULL ||
       memResult == NULL ||
       memRW == NULL ) 
   {
       fprintf( stderr, "%s:%d: error: %s\n", \
                __FILE__, __LINE__, "could not allocate host buffers\n" );
       exit(-1);
   }
}

void createCLBuffers()
{
   // CL buffers

   cl_int ret;
   void *hostPtr = NULL;

   if( copyFlags & CL_MEM_USE_HOST_PTR ||
       copyFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memIn;

   if( whichTest == 3 ||
       whichTest == 4 )
   {
      inputCopyBuffer = clCreateBuffer( context,
                                        copyFlags,
                                        nBytes,
                                        hostPtr, &ret );
      ASSERT_CL_RETURN( ret );
   }

   hostPtr = NULL;

   if( copyFlags & CL_MEM_USE_HOST_PTR ||
       copyFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memOut;

   if( whichTest == 3 ||
       whichTest == 4 )
   {
      outputCopyBuffer = clCreateBuffer( context,
                                         copyFlags,
                                         nBytes,
                                         hostPtr, &ret );
      ASSERT_CL_RETURN( ret );
   }

   resultBuffer = clCreateBuffer( context,
                                  CL_MEM_READ_WRITE,
                                  nBytesResult,
                                  NULL, &ret );
   ASSERT_CL_RETURN( ret );
}

void createCLImages()
{
   // CL images
   cl_image_desc pixelDesc;
   memset(&pixelDesc, '\0', sizeof(cl_image_desc));
   pixelDesc.image_type = CL_MEM_OBJECT_IMAGE2D;
   pixelDesc.image_width = imageWidth;
   pixelDesc.image_height = imageHeight;

   cl_int ret;
   void *hostPtr = NULL;

   if( inFlags & CL_MEM_USE_HOST_PTR ||
       inFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memIn;

   inputImage = clCreateImage ( context,
                                inFlags,
                                &pixelFormat,
                                &pixelDesc,
                                hostPtr,
                                &ret );

   ASSERT_CL_RETURN( ret );

   hostPtr = NULL;

   if( outFlags & CL_MEM_USE_HOST_PTR ||
       outFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memOut;

   outputImage = clCreateImage ( context,
                                 outFlags,
                                 &pixelFormat,
                                 &pixelDesc,
                                 hostPtr,
                                 &ret );
   ASSERT_CL_RETURN( ret );

   hostPtr = NULL;

   if( copyFlags & CL_MEM_USE_HOST_PTR ||
       copyFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memIn;

   if( whichTest == 2 )
   {
      inputCopyImage = clCreateImage ( context,
                                       copyFlags,
                                       &pixelFormat,
                                       &pixelDesc,
                                       hostPtr,
                                       &ret );

      ASSERT_CL_RETURN( ret );
   }

   hostPtr = NULL;

   if( copyFlags & CL_MEM_USE_HOST_PTR ||
       copyFlags & CL_MEM_COPY_HOST_PTR )
       hostPtr = memOut;

   if( whichTest == 2 )
   {
      outputCopyImage = clCreateImage ( context,
                                        copyFlags,
                                        &pixelFormat,
                                        &pixelDesc,
                                        hostPtr,
                                        &ret );

      ASSERT_CL_RETURN( ret );
   }
}

void printHeader()
{
    std::cout <<"\nDevice " << devnum <<":             " << devname << std::endl;

#ifdef _WIN32
   std::cout << "Build:                _WINxx" ; 
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

    std::cout << "Pixel format:         " << formats[pixFormat].s << std::endl;
   std::cout << "Image width:           " << imageWidth << std::endl;
   std::cout << "Image height:          " << imageHeight << std::endl;
   std::cout << "Image origin:          " << (int) imageOrigin[0] << "  " << (int) imageOrigin[1] << std::endl;
   std::cout << "Image region:          " << (int) imageRegion[0] << "  " << (int) imageRegion[1] << std::endl;
   std::cout << "Image size in bytes:   " << nBytes << std::endl;
   std::cout << "Region size in bytes:  " << nBytesRegion << std::endl;
   std::cout << "Global work items X:   " << nThreadsX << std::endl;
   std::cout << "Global work items Y:   " << nThreadsY << std::endl;
   std::cout << "Total work items:      " << nThreadsX * nThreadsY << std::endl;
   std::cout << "Pixels per thread:     " << nPixelsPerThread << std::endl;
   std::cout << "Local work items X:    " << nLocalThreadsX << std::endl;
   std::cout << "Local work items Y:    " << nLocalThreadsY << std::endl;
   std::cout << "Number of groups:      " << nGroups << std::endl;

   std::cout << "Timing loops:          " << nLoops << std::endl;
   std::cout << "Repeats:               " << nRepeats << std::endl;
   std::cout << "Kernel loops:          " << nKLoops << std::endl;
   std::cout << "Kernel launches:       " << nKLaunches << std::endl;

   std::cout <<"\n";

   std::cout << "inputImage:           " ;

   for( int i=0; i < nFlags; i++ )
      if( inFlags & flags[i].f )
         std::cout << flags[i].s ;

   std::cout << "\noutputImage:          " ;

   for( int i=0; i < nFlags; i++ )
      if( outFlags & flags[i].f )
         std::cout << flags[i].s;

   if( whichTest == 2 ||
       whichTest == 3 ||
       whichTest == 4 )
   {
      std::cout << "\ncopyImage/copyBuffer: " ;

      for( int i=0; i < nFlags; i++ )
      if( copyFlags & flags[i].f )
         std::cout << "%s ", flags[i].s;
   }

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

void runImageMapTest()
{
   int  nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Host mapped write to inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedImageMappedWrite( queue, inputImage, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "2. GPU kernel read of inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputImage, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel write to outputImage" );

      for(int i=0; i<nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputImage, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "4. Host mapped read of outputImage" );

      for(int i=0; i<nRepeats; i++)
          timedImageMappedRead( queue, outputImage, nl & 0xff );

      tlog->Msg( "%s\n", "" );
   }
}

void runImageReadWriteTest()
{
   int nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Host CL write to inputImage" );

      memset( memRW, nl & 0xff, nBytesRegion );

      for(int i=0; i<nRepeats; i++)
          timedImageCLWrite( queue, inputImage, memRW );

      tlog->Msg( "\n%s\n\n", "2. GPU kernel read of inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputImage, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel write to outputImage" );

      for(int i=0; i<nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputImage, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "4. Host CL read of outputImage" );

      for(int i=0; i<nRepeats; i++)
          timedImageCLRead( queue, outputImage, memRW, nl & 0xff );

      tlog->Msg( "%s\n", "" );
   }
}

void runImageMappedReadWriteTest()
{
   int nl = nLoops;

   void *mappedPtr;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Mapping inputCopyBuffer as mappedPtr" );

      timedBufMap( queue, inputCopyBuffer, &mappedPtr, false );

      tlog->Msg( "\n%s\n\n", "2. Host CL write of mappedPtr to inputImage" );

      memset( mappedPtr, nl & 0xff, nBytesRegion );

      for(int i=0; i<nRepeats; i++)
          timedImageCLWrite( queue, inputImage, mappedPtr );

      tlog->Msg( "\n%s\n\n", "3. Unmapping inputCopyBuffer" );

      timedBufUnmap( queue, inputCopyBuffer, &mappedPtr, false );

      tlog->Msg( "\n%s\n\n", "4. GPU kernel read of inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputImage, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "5. GPU kernel write to outputImage" );

      for(int i=0; i<nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputImage, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "6. Mapping outputCopyBuffer as mappedPtr" );

      timedBufMap( queue, outputCopyBuffer, &mappedPtr, false );

      tlog->Msg( "\n%s\n\n", "7. Host CL read of outputImage to mappedPtr" );

      for(int i=0; i<nRepeats; i++)
          timedImageCLRead( queue, outputImage, mappedPtr, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "8. Unmapping outputCopyBuffer" );

      timedBufUnmap( queue, outputCopyBuffer, &mappedPtr, false );

      tlog->Msg( "%s\n", "" );
   }
}

void runImageCopyTest()
{
   int nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Host mapped write to copyImage" );

      for(int i=0; i<nRepeats; i++)
          timedImageMappedWrite( queue, inputCopyImage, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "2. CL copy of copyImage to inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedImageCLCopy( queue, inputCopyImage, inputImage );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel read of inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputImage, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "4. GPU kernel write to outputImage" );

      for(int i=0; i<nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputImage, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "5. CL copy of outputImage to copyImage" );

      for(int i=0; i<nRepeats; i++)
          timedImageCLCopy( queue, outputImage, outputCopyImage );

      tlog->Msg( "\n%s\n\n", "6. Host mapped read of copyImage" );

      for(int i=0; i<nRepeats; i++)
          timedImageMappedRead( queue, outputCopyImage, nl & 0xff );

      tlog->Msg( "%s\n", "" );
   }
}

void runImageBufferCopyTest()
{
   int nl = nLoops;

   while( nl-- )
   {
      tlog->loopMarker();

      tlog->Msg( "\n%s\n\n", "1. Host mapped write to copyBuffer" );

      for(int i=0; i<nRepeats; i++)
          timedBufMappedWrite( queue, inputCopyBuffer, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "2. CL copy of copyBuffer to inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedBufferImageCLCopy( queue, inputCopyBuffer, inputImage );

      tlog->Msg( "\n%s\n\n", "3. GPU kernel read of inputImage" );

      for(int i=0; i<nRepeats; i++)
          timedReadKernelVerify( queue, read_kernel, inputImage, resultBuffer, nl & 0xff, false );

      tlog->Msg( "\n%s\n\n", "4. GPU kernel write to outputImage" );

      for(int i=0; i<nRepeats; i++)
          timedKernel( queue, write_kernel, resultBuffer, outputImage, nl & 0xff );

      tlog->Msg( "\n%s\n\n", "5. CL copy of outputImage to copyBuffer" );

      for(int i=0; i<nRepeats; i++)
          timedImageBufferCLCopy( queue, outputImage, outputCopyBuffer );

      tlog->Msg( "\n%s\n\n", "6. Host mapped read of copyBuffer" );

      for(int i=0; i<nRepeats; i++)
          timedBufMappedRead( queue, outputCopyBuffer, nl & 0xff );

      tlog->Msg( "%s\n", "" );
   }
}

void initDefaults()
{
    nWorkers =   1;
    nLoops =     10;
    nRepeats =   1;
    nSkip =      2;
    nKLoops =    100;
    nKLaunches = 1;

    nAlign = 4096;    // decent bet for many platforms

    imageWidth =  1024;
    imageHeight = 1024;

    imageOrigin[0] = 0;
    imageOrigin[1] = 0;
    imageOrigin[2] = 0;

    imageRegion[0] = imageWidth;
    imageRegion[1] = imageHeight;
    imageRegion[2] = 1;

    nLocalThreadsX = 16;
    nLocalThreadsY = 8;

    nStrips = 1;

    pixFormat = 0;

    printLog =  false;
    doHost =    true;
    whichTest = 0;
    mapRW =     false;
    devnum =    0;
}

void computeGlobals()
{
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

    if( whichTest == 4 && (copyFlags & f) == 0 )
             copyFlags |= CL_MEM_ALLOC_HOST_PTR;

    if( (copyFlags & f) == 0 )
             copyFlags = CL_MEM_READ_WRITE;

    f = CL_MEM_READ_ONLY |
        CL_MEM_WRITE_ONLY |
        CL_MEM_READ_WRITE;

    if( (copyFlags & f) == 0 )
             copyFlags |= CL_MEM_READ_WRITE;

    if( whichTest == 4 )
        mapRW = true;

    nSkip = nLoops > nSkip ? nSkip : 0;

    pixelFormat.image_channel_order =     formats[pixFormat].f.image_channel_order;
    pixelFormat.image_channel_data_type = formats[pixFormat].f.image_channel_data_type;
    nChannels =                           formats[pixFormat].nChannels;
    nBytesPerChannel =                    formats[pixFormat].nBytesPerChannel;

    imageOrigin[0] = ( imageOrigin[0] / nLocalThreadsX ) * nLocalThreadsX;
    imageOrigin[1] = ( imageOrigin[1] / nLocalThreadsY ) * nLocalThreadsY;

    imageRegion[0] = min( imageRegion[0], imageWidth - imageOrigin[0] );
    imageRegion[0] = max( imageRegion[0], (size_t) nLocalThreadsX );
    imageRegion[1] = min( imageRegion[1], imageHeight - imageOrigin[1] );
    imageRegion[1] = max( imageRegion[1], (size_t) nLocalThreadsY );

    imageRegion[0] = ( imageRegion[0] / nLocalThreadsX ) * nLocalThreadsX;
    imageRegion[1] = ( imageRegion[1] / nLocalThreadsY ) * nLocalThreadsY;

    nThreadsX =        (int) imageRegion[0];
    nThreadsY =        nLocalThreadsY * nStrips;
    nThreadsY =        min( nThreadsY, (int) imageRegion[1] );
    nGroups =          ( nThreadsX * nThreadsY ) /
                       ( nLocalThreadsX * nLocalThreadsY );

    nPixels =          (int) (imageRegion[0] * imageRegion[1]);
    nPixelsPerThread = (int) imageRegion[1] / nThreadsY;
    nBytesRegion =     nPixels * nChannels * nBytesPerChannel;
    nBytes =           imageWidth * imageHeight * nChannels * nBytesPerChannel;
    nBytesResult =     nGroups * sizeof( cl_uint );
}

int main(int argc, char **argv)
{
    initDefaults();

    parseOptions( argc, argv );

    tlog = new TestLog( nLoops * nRepeats * 50 );

    initCL( (char *) "ImageBandwidth_Kernels.cl" );

    computeGlobals();
    printHeader();

    createHostBuffers();
    createCLBuffers();
    createCLImages();

    if( doHost )
       assessHostMemPerf( memIn, memOut, nBytes );

    switch( whichTest )
    {
       case 0: runImageMapTest(); break;
       case 1: runImageReadWriteTest(); break;
       case 2: runImageCopyTest(); break;
       case 3: runImageBufferCopyTest(); break;
       case 4: runImageMappedReadWriteTest(); break;
    }

    printResults();

    if( vFailure )
    {
        fprintf( stderr, "Failed!\n");
        return -1;
    }
    else
    {
        fprintf( stdout, "Passed!\n");
        return 0;
    }
}
