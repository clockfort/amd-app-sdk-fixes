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

#include "Host.h"
#include "Timer.h"

#include <stdio.h>
#if !defined(__MINGW64__)
#include <emmintrin.h>
#include <xmmintrin.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <string.h>
#endif

#define MTYPE unsigned long

// Suppress the warning #810 if intel compiler is used.
#if defined(__INTEL_COMPILER) || defined(__MINGW64__)
#pragma warning(disable : 810)
#endif

int nWorkers;

typedef struct {

    void (*work_func)( int );

    void *ptr;
    void *ptr2;
    unsigned char v;
    size_t len;

    bool ret;
    bool noop;
    bool exit;

} _threadWork;

static _threadWork work[MAXWORKERS];

#ifdef MEM_MULTICORE

static HANDLE      tn[MAXWORKERS];
static unsigned    tid[MAXWORKERS];

// pad to something > cacheline to avoid false sharing
static int volatile myBarrier[MAXWORKERS][64]={0};

unsigned int __stdcall myThreadFunc( void * arg )
{
    int* idPtr = (int*)(arg);
	int id = *idPtr;
    bool exit = false;

    while( !exit )
    {
        // counterpart to releaseThreads()
        while(myBarrier[id][0] != 1)
            Sleep(0);
        myBarrier[id][0] = 0;

        if( work[id].work_func != NULL )
            work[id].work_func( id );

        if( work[id].exit == true )
            exit = true;

        // counterpart to waitForThreads()
        myBarrier[id][1] = 1;
        Sleep(0);
    }

    return 0;
}
#endif

void releaseThreads()
{
#ifdef MEM_MULTICORE
    int n;

    for( n=1; n<nWorkers; n++ )
    {
        myBarrier[n][0] = 1;
    }
    Sleep(0);
#endif
}

void waitForThreads()
{
#ifdef MEM_MULTICORE
    int n;

    for( n=1; n<nWorkers; n++ )
    {
        while( myBarrier[n][1] != 1 )
            Sleep(0);

        myBarrier[n][1] = 0;
    }
#endif
}

void launchThreads()
{
#ifdef MEM_MULTICORE
    int *nArg = (int*)malloc(sizeof(int) *(nWorkers + 1));
    int n;
    for( n=1; n<nWorkers; n++ )
    {
        nArg[n] = n;
        work[n].exit = false;
        work[n].work_func = NULL;

        tn[n] = (HANDLE) _beginthreadex( NULL, 0, myThreadFunc, (void *) (&nArg[n]), 0, &tid[n] );
    }

    // XXX if this isn't here subsequent barrier roundtrips
    // are more expensive. Why?
    releaseThreads();
    waitForThreads();
	if(nArg)
		free(nArg);
#endif
}

void shutdownThreads()
{
#ifdef MEM_MULTICORE
    int n;

    for( n=1; n<nWorkers; n++ )
    {
        work[n].exit = true;
        work[n].work_func = NULL;
    }

    releaseThreads();
    waitForThreads();

    for(int n=1; n<nWorkers; n++)
    {
        WaitForSingleObject( tn[n], INFINITE );
    }
#endif
}

bool myMasterFunc()
{
    releaseThreads();

    // do my chunk of work
    work[0].work_func( 0 );

    waitForThreads();

    bool ret = true;

    for(int n=0; n<nWorkers; n++)
    {
        if( work[n].ret == false ) ret = false;
    }

    return( ret );
}

void readVerifyMemCPU_WF( int id )
{
    work[id].ret = readVerifyMemCPU( work[id].ptr, work[id].v, work[id].len );
}

bool readVerifyMemCPU_MT( void *ptr, unsigned char v, size_t len )
{
    size_t blksz = len / nWorkers;

    // distribute work
    for(int n=0; n<nWorkers; n++)
    {
        work[n].work_func = readVerifyMemCPU_WF;
        work[n].ptr = (void *) ((char *) ptr + n * blksz);
        work[n].v = v;
        work[n].len = blksz;
    }

    return myMasterFunc();
}

void memcpy_WF( int id )
{
    memcpy( work[id].ptr, work[id].ptr2, work[id].len );
}

void memcpy_MT( void *ptr, void *ptr2, size_t len )
{
    size_t blksz = len / nWorkers;

    // distribute work
    for(int n=0; n<nWorkers; n++)
    {
        work[n].work_func = memcpy_WF;
        work[n].ptr = (void *) ((char *) ptr + n * blksz);
        work[n].ptr2 = (void *) ((char *) ptr2 + n * blksz);
        work[n].len = blksz;
    }

    (void) myMasterFunc();
}

void memset_WF( int id )
{
    memset( work[id].ptr, work[id].v, work[id].len );
}

void memset_MT( void *ptr, unsigned char v, size_t len )
{
    size_t blksz = len / nWorkers;

    // distribute work
    for(int n=0; n<nWorkers; n++)
    {
        work[n].work_func = memset_WF;
        work[n].ptr = (void *) ((char *) ptr + n * blksz);
        work[n].v = v;
        work[n].len = blksz;
    }

    (void) myMasterFunc();
}

void empty_WF( int id )
{
    volatile int i=0;
}

void empty_MT()
{
    // distribute work
    for(int n=0; n<nWorkers; n++) {
        work[n].work_func = empty_WF;
    }

    (void) myMasterFunc();
}

void benchLaunch()
{
   CPerfCounter t; t.Reset();

   int nl=100;

   t.Start();

   for(int n=0; n<nl; n++)
   {
      launchThreads();
      shutdownThreads();
   }

   t.Stop();

   printf("%32s  %5.lf ns\n", "Launch speed", t.GetElapsedTime() / nl * 1e9, nWorkers );
}

void benchBarrier()
{
   CPerfCounter t; t.Reset();

   int nl=100000;

   t.Start();

   for(int n=0; n<nl; n++)
   {
      empty_MT();
   }

   t.Stop();

   printf("%32s  %5.lf ns\n", "Barrier speed", t.GetElapsedTime() / nl * 1e9, nWorkers );
}

bool readVerifyMemCPU( void *ptr, unsigned char v, size_t len )
{
   register MTYPE r = (MTYPE) 0;
   register MTYPE *p = (MTYPE *) ptr;
   register size_t i = len/sizeof(MTYPE);
   register unsigned int idx = 0;

#define UNROLL 32

   while( i >= UNROLL )
   {
      _mm_prefetch( (char *) &p[idx+8], _MM_HINT_NTA );

      r |= p[idx];
      r |= p[idx+1];
      r |= p[idx+2];
      r |= p[idx+3];
      r |= p[idx+4];
      r |= p[idx+5];
      r |= p[idx+6];
      r |= p[idx+7];

      _mm_prefetch( (char *) &p[idx+16], _MM_HINT_NTA );

      r |= p[idx+8];
      r |= p[idx+9];
      r |= p[idx+10];
      r |= p[idx+11];
      r |= p[idx+12];
      r |= p[idx+13];
      r |= p[idx+14];
      r |= p[idx+15];

      _mm_prefetch( (char *) &p[idx+24], _MM_HINT_NTA );

      r |= p[idx+16];
      r |= p[idx+17];
      r |= p[idx+18];
      r |= p[idx+19];
      r |= p[idx+20];
      r |= p[idx+21];
      r |= p[idx+22];
      r |= p[idx+23];

      _mm_prefetch( (char *) &p[idx+32], _MM_HINT_NTA );

      r |= p[idx+24];
      r |= p[idx+25];
      r |= p[idx+26];
      r |= p[idx+27];
      r |= p[idx+28];
      r |= p[idx+29];
      r |= p[idx+30];
      r |= p[idx+31];

      i -= UNROLL;
      idx += UNROLL;
   }

   // make sure compiler can't optimize
   static __volatile MTYPE always = r;

   MTYPE val=0;

   for(int i=0; i < sizeof(MTYPE); i++)
       val |= (MTYPE) v << (i*8);

   if( r == val )
       return true;
   else
       return false;
}

bool readmem2DPitch( void *ptr, unsigned char v, size_t pitch, int rows )
{
   register MTYPE r = (MTYPE) 0;
   register MTYPE *p;
   register size_t i;
   register unsigned int idx;

   for( int row=0; row < rows; row++ )
   {
       p = (MTYPE *) ((unsigned char *) ptr + (size_t) row * pitch );
       idx = 0;
       i = pitch / sizeof( MTYPE );

#define UNROLL 32

       while( i >= UNROLL )
       {
          _mm_prefetch( (char *) &p[idx+UNROLL*sizeof(MTYPE)], _MM_HINT_NTA );

          r |= p[idx];
          r |= p[idx+1];
          r |= p[idx+2];
          r |= p[idx+3];
          r |= p[idx+4];
          r |= p[idx+5];
          r |= p[idx+6];
          r |= p[idx+7];
          r |= p[idx+8];
          r |= p[idx+9];
          r |= p[idx+10];
          r |= p[idx+11];
          r |= p[idx+12];
          r |= p[idx+13];
          r |= p[idx+14];
          r |= p[idx+15];
          r |= p[idx+16];
          r |= p[idx+17];
          r |= p[idx+18];
          r |= p[idx+19];
          r |= p[idx+20];
          r |= p[idx+21];
          r |= p[idx+22];
          r |= p[idx+23];
          r |= p[idx+24];
          r |= p[idx+25];
          r |= p[idx+26];
          r |= p[idx+27];
          r |= p[idx+28];
          r |= p[idx+29];
          r |= p[idx+30];
          r |= p[idx+31];

          i -= UNROLL;
          idx += UNROLL;
       }
   }

   // make sure compiler can't optimize
   static __volatile MTYPE always = r;

   MTYPE val=0;

   for(int i=0; i < sizeof(MTYPE); i++)
       val |= (MTYPE) v << (i*8);

   if( r == val )
       return true;
   else
       return false;
}

void writeMemCPU( void *ptr, unsigned char v, size_t len )
{
   register MTYPE r=0;
   register MTYPE *p = (MTYPE *) ptr;
   register size_t i = len/sizeof(MTYPE);
   register size_t idx = 0;

   for(int i=0; i < sizeof(MTYPE); i++)
       r |= (MTYPE) v << (i*8);

   while( idx < (const size_t) ( len/sizeof(MTYPE) ) )
   {
      p[idx] = r;
      idx++;
   }
}

bool readVerifyMemSSE( void *ptr, unsigned char v, size_t len )
{
    register __m128i r1 = _mm_setzero_si128();
    register __m128i r2 = _mm_setzero_si128();
    register __m128i *p = (__m128i *) ptr;
    register unsigned int idx = 0;

    while(idx < (const size_t) (len/sizeof(__m128i)) ) 
    {
        if( idx < (const size_t) (len/sizeof(__m128i)) - 64 )
           _mm_prefetch( (char *) &p[idx+64], _MM_HINT_NTA );

        r1 = _mm_load_si128( &p[idx] );
        r2 = _mm_or_si128( r1, r2 );

        idx++;
    }

    // make sure compiler can't optimize
    static __volatile __m128i always = r2;

    unsigned char res[16];
    _mm_storeu_si128( (__m128i *) res, r2 );

    bool ret = true;

    unsigned char val;
    val = v;

    for(int i=0; i < sizeof( __m128i ); i++)
        if( res[i] != val ) {
            ret = false;
        }

    return ret;
}

void writeMemSSE ( void *ptr, unsigned char val, size_t len )
{
    register const __m128i r1 = _mm_set1_epi8( val );
    register __m128i *p = (__m128i *) ptr;
    register size_t idx = 0;

    while( idx < (const size_t) (len / sizeof(__m128i)) )
    { 
        _mm_store_si128( &p[idx], r1 );
        idx++;
    }
}

void memset2DPitch( void *ptr, unsigned char val, size_t columns, size_t rows, size_t pitch )
{
    for( size_t r=0; r<rows; r++ )
        memset( ( unsigned char * ) ptr + r * pitch, val, columns );
}

void stridePagesCPU( void *ptr, size_t stride, size_t nbytes )
{
    register unsigned int *p = ( unsigned int * ) ptr;
    register size_t i;

    CPerfCounter t;
    double kTime;

    t.Reset();
    t.Start();

    for(i=0; i < nbytes/sizeof(unsigned int); i += stride/sizeof(unsigned int))
        p[i] = 0;

    t.Stop();
    kTime = t.GetElapsedTime();

    printf("%32s  %5.lf ns\n", "Page fault", (kTime*1e9) / ((double) nbytes/stride));
}

#define TIMED_LOOP( STRING, EXPR, NBYTES ) \
{\
   t.Reset();\
   t.Start();\
\
   int nl=20;\
   for( int i=0; i<nl; i++ )\
      EXPR;\
\
   t.Stop();\
\
   printf("%32s  %5.2lf GB/s\n", STRING, (((double) nl*(NBYTES)) / t.GetElapsedTime()) / 1e9 );\
}

void assessHostMemPerf( void *ptr, void *ptr2, size_t nbytes )
{
    CPerfCounter t;

    printf("Host baseline (single thread, naive):\n\n");

    double sum = 0.;
    int ctr=0;

    for(int i=0; i<1e6; i++)
    {
        t.Reset();
        t.Start();
        t.Stop();

        double e = t.GetElapsedTime();

        if( e > 0. ) {
            sum += e;
             ctr++;
        }
    }

    printf("%32s  %5.lf ns\n", "Timer resolution", ( sum / (double) ctr ) * 1e9 );

#ifdef _WIN32
    //Sleep( 1000 );
#else
    usleep( 1000 * 1e3 );
#endif
    size_t pagesize;

#ifdef _WIN32
    SYSTEM_INFO system_info;

    GetSystemInfo (&system_info);
    pagesize = (size_t) system_info.dwPageSize;
#else
    pagesize = getpagesize();
#endif

    stridePagesCPU( ptr, pagesize, nbytes );

#ifdef MEM_MULTICORE
    benchBarrier();

    printf("\n");

#endif

#if 0
    TIMED_LOOP( "SSE read", readVerifyMemSSE( ptr, 0, nbytes ), nbytes )
    TIMED_LOOP( "SSE write", writeMemSSE( ptr, 0, nbytes ), nbytes )
    TIMED_LOOP( "CPU write", writeMemCPU( ptr, 0, nbytes ), nbytes )
#endif

    TIMED_LOOP( "CPU read", readVerifyMemCPU_MT( ptr, 0, nbytes ), nbytes )

    TIMED_LOOP( "memcpy()", memcpy_MT( ptr, ptr2, nbytes ), nbytes )

    TIMED_LOOP( "memset(,1,)", memset_MT( ptr, 1, nbytes ), nbytes )
    TIMED_LOOP( "memset(,0,)", memset_MT( ptr, 0, nbytes ), nbytes )

    printf("\n");
}
