/* ============================================================

Copyright (c) 2009-2010 Advanced Micro Devices, Inc.  All rights reserved.
 
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

//
// Data required
// Global array : 9 f input values (rho and u are computed from these values)
// Global array : 9 f output values
// Constant array : Boundary or Fluid (1 bit : 0 for fluid and 1 for boundary)
// Private variables : 9 f input values, rho, u[2], 
// Constant arrays : 9 directions, 9 weights, omega, 


#ifdef KHR_DP_EXTENSION
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#else
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#endif

// Calculates equivalent distribution 
double computefEq(double rho, double weight, double2 dir, double2 u)
{
    double u2 = (u.x * u.x) + (u.y * u.y);		//x^2 + y^2
    double eu = (dir.x * u.x) + (dir.y * u.y);	//
    return rho * weight * (1.0f + (3.0f * eu) + (4.5f * eu * eu) - (1.5f * u2));
}

__kernel void lbm(__global double *if0, __global double *of0, 
                  __global double4 *if1234, __global double4 *of1234,
                  __global double4 *if5678, __global double4 *of5678,
                  __global bool *type,	// This will only work for sizes <= 512 x 512 as constant buffer is only 64KB
                  double8 dirX,  double8 dirY,	//Directions is (0, 0) for 0
                  __constant double weight[9],	//Directions : 0, 1, 2, 3, 4, 5, 6, 7, 8
                  double omega,
                  __global double2 *velocityBuffer)
{
    uint2 id = (uint2)(get_global_id(0), get_global_id(1));
    uint width = get_global_size(0);
    uint pos = id.x + width * id.y;

    // Read input distributions
    double f0 = if0[pos];
    double4 f1234 = if1234[pos];
    double4 f5678 = if5678[pos];


    double rho;	//Density
    double2 u;	//Velocity

    // Collide
    //boundary
    if(type[pos])
    {
        // Swap directions by swizzling
        f1234.xyzw = f1234.zwxy;
        f5678.xyzw = f5678.zwxy;

        rho = 0;
        u = (double2)(0, 0);
    }
    //fluid
    else
    {
        // Compute rho and u
        // Rho is computed by doing a reduction on f
        double4 temp = f1234 + f5678;
        temp.lo += temp.hi;
        rho = temp.x + temp.y;
        rho += f0;

        // Compute velocity
        u.x = (dot(f1234, dirX.lo) + dot(f5678, dirX.hi)) / rho;
        u.y = (dot(f1234, dirY.lo) + dot(f5678, dirY.hi)) / rho;

        double4 fEq1234;	// Stores feq 
        double4 fEq5678;
        double fEq0;

        // Compute fEq
        fEq0 = computefEq(rho, weight[0], (double2)(0, 0), u);
        fEq1234.x = computefEq(rho, weight[1], (double2)(dirX.s0, dirY.s0), u);
        fEq1234.y = computefEq(rho, weight[2], (double2)(dirX.s1, dirY.s1), u);
        fEq1234.z = computefEq(rho, weight[3], (double2)(dirX.s2, dirY.s2), u);
        fEq1234.w = computefEq(rho, weight[4], (double2)(dirX.s3, dirY.s3), u);
        fEq5678.x = computefEq(rho, weight[5], (double2)(dirX.s4, dirY.s4), u);
        fEq5678.y = computefEq(rho, weight[6], (double2)(dirX.s5, dirY.s5), u);
        fEq5678.z = computefEq(rho, weight[7], (double2)(dirX.s6, dirY.s6), u);
        fEq5678.w = computefEq(rho, weight[8], (double2)(dirX.s7, dirY.s7), u);

        f0 = (1 - omega) * f0 + omega * fEq0;
        f1234 = (1 - omega) * f1234 + omega * fEq1234;
        f5678 = (1 - omega) * f5678 + omega * fEq5678;
    }

    velocityBuffer[pos] = u;

    // Propagate
    // New positions to write (Each thread will write 8 values)
    int8 nX = id.x + convert_int8(dirX);
    int8 nY = id.y + convert_int8(dirY);
    int8 nPos = nX + width * nY;

    // Write center distrivution to thread's location
    of0[pos] = f0;

    int t1 = id.x < get_global_size(0) - 1; // Not on Right boundary
    int t4 = id.y > 0;                      // Not on Upper boundary
    int t3 = id.x > 0;                      // Not on Left boundary
    int t2 = id.y < get_global_size(1) - 1; // Not on lower boundary

    // Propagate to right cell
    if(t1)
        of1234[nPos.s0].x = f1234.x;

    // Propagate to Lower cell
    if(t2)
        of1234[nPos.s1].y = f1234.y;

    // Propagate to left cell
    if(t3)
        of1234[nPos.s2].z = f1234.z;

    // Propagate to Upper cell
    if(t4)
        of1234[nPos.s3].w = f1234.w;

    // Propagate to Lower-Right cell
    if(t1 && t2)
        of5678[nPos.s4].x = f5678.x;

    // Propogate to Lower-Left cell
    if(t2 && t3)
        of5678[nPos.s5].y = f5678.y;

    // Propagate to Upper-Left cell
    if(t3 && t4)
        of5678[nPos.s6].z = f5678.z;

    // Propagate to Upper-Right cell
    if(t4 && t1)
        of5678[nPos.s7].w = f5678.w;
}