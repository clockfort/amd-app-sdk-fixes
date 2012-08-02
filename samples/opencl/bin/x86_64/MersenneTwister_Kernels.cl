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

/*
 * Implemented Gaussian Random Number Generator. SIMD-oriented Fast Mersenne
 * Twister(SFMT) used to generate random numbers and Box mullar transformation used
 * to convert them to Gaussian random numbers.
 * 
 * The SFMT algorithm details could be found at 
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html
 *
 * Box-Muller Transformation 
 * http://mathworld.wolfram.com/Box-MullerTransformation.html
 *
 * One invocation of this kernel(gaussianRand), i.e one work thread writes
 * mulFactor output values.
 */




class gaussianRandom
{
protected:
	const __global uint4 *seedArray;
    uint width;
    uint mulFactor;
    __global float4 *gaussianRand;
	
    uint4 temp[8];
    
    size_t xPid;
    size_t yPid;
    
    uint4 state1;
    uint4 state2; 
    uint4 state3; 
    uint4 state4; 
    uint4 state5;
    
    uint4 mask4;
    uint4 thirty4; 
    uint4 one4;
    uint4 two4;
    uint4 three4;
    uint4 four4;

    uint4 r1;
    uint4 r2;

    uint4 a;
    uint4 b; 

    uint4 e; 
    uint4 f; 
    
    unsigned int thirteen;
    unsigned int fifteen;
    unsigned int shift;

    unsigned int mask11;
    unsigned int mask12;
    unsigned int mask13;
    unsigned int mask14;
    
    size_t actualPos;
    
    float one;
    float intMax;
    float PI;
    float two;

    float4 r; 
    float4 phi;

    float4 temp1;
    float4 temp2;
	
	unsigned int invshift;
	
	uint4 lshift128(uint4);
	uint4 rshift128(uint4);
	void evaluate(uint4 num_1, 
                  uint4 num_2, 
                  uint4 num_3, 
                  uint4 num_4);

public:
	void initial(const __global uint4*, uint, uint, __global float4*);
	void run();
	

};

void
gaussianRandom::initial(const __global uint4 *seedArray,
            uint width,
            uint mulFactor,
            __global float4 *gaussianRand)
{
	this->seedArray = seedArray;
    this->width = width;
    this->mulFactor = mulFactor ; 
    this->gaussianRand = gaussianRand ;

	xPid = get_global_id(0);
    yPid = get_global_id(1);
	
	state1 = seedArray[yPid *width + xPid];
    
    mask4 = (uint4)(1812433253u);
    thirty4 = (uint4)(30u); 
    one4 = (uint4)(1u);
    two4 = (uint4)(2u);
    three4 = (uint4)(3u);
    four4 = (uint4)(4u);
    
    thirteen  = 13u;
    fifteen = 15u;
    shift = 8u * 3u;

    mask11 = 0xfdff37ffu;
    mask12 = 0xef7f3f7du;
    mask13 = 0xff777b7du;
    mask14 = 0x7ff7fb2fu;

    one = 1.0f;
    intMax = 4294967296.0f;
    PI = 3.14159265358979f;
    two = 2.0f;
	
	invshift =  32u - shift;
	
	state2 = mask4 * (state1 ^ (state1 >> thirty4)) + one4;
    state3 = mask4 * (state2 ^ (state2 >> thirty4)) + two4;
    state4 = mask4 * (state3 ^ (state3 >> thirty4)) + three4;
    state5 = mask4 * (state4 ^ (state4 >> thirty4)) + four4;
}

void
gaussianRandom::run()
{
	uint i = 0;
    for(i = 0; i < mulFactor; ++i)
    {
        switch(i)
        {
            case 0:
				evaluate(state4, state5, state1, state3);
                break;
            case 1:
				evaluate(r2, temp[0], state2, state4);
                break;
            case 2:
				evaluate(r2, temp[1], state3, state5);
                break;
            case 3:
				evaluate(r2, temp[2], state4, state1);
                break;
            case 4:
				evaluate(r2, temp[3], state5, state2);
                break;
            case 5:
				evaluate(r2, temp[4], temp[0], temp[2]);
                break;
            case 6:
				evaluate(r2, temp[5], temp[1], temp[3]);
                break;
            case 7:
				evaluate(r2, temp[6], temp[2], temp[4]);
                break;
            default:
                break;            
                
        }
        
        e = lshift128(a);
        f = rshift128(r1);

        temp[i].x = a.x ^ e.x ^ ((b.x >> thirteen) & mask11) ^ f.x ^ (r2.x << fifteen);
        temp[i].y = a.y ^ e.y ^ ((b.y >> thirteen) & mask12) ^ f.y ^ (r2.y << fifteen);
        temp[i].z = a.z ^ e.z ^ ((b.z >> thirteen) & mask13) ^ f.z ^ (r2.z << fifteen);
        temp[i].w = a.w ^ e.w ^ ((b.w >> thirteen) & mask14) ^ f.w ^ (r2.w << fifteen);
    }        

    actualPos = (yPid * width + xPid) * mulFactor;    

    for(i = 0; i < mulFactor / 2; ++i)
    {
        temp1 = convert_float4(temp[i]) * one / intMax;
        temp2 = convert_float4(temp[i + 1]) * one / intMax;
        
        // Applying Box Mullar Transformations.
        r = sqrt((-two) * log(temp1));
        phi  = two * PI * temp2;
        gaussianRand[actualPos + i * 2 + 0] = r * cos(phi);
        gaussianRand[actualPos + i * 2 + 1] = r * sin(phi);
    }         

}

uint4 
gaussianRandom::lshift128(uint4 input)
{
    uint4 temp;
	
    temp.x = input.x << shift;
    temp.y = (input.y << shift) | (input.x >> invshift);
    temp.z = (input.z << shift) | (input.y >> invshift);
    temp.w = (input.w << shift) | (input.z >> invshift);
    
    return temp;
}

uint4
gaussianRandom::rshift128(uint4 input)
{
    uint4 temp;

    temp.w = input.w >> shift;
    temp.z = (input.z >> shift) | (input.w << invshift);
    temp.y = (input.y >> shift) | (input.z << invshift);
    temp.x = (input.x >> shift) | (input.y << invshift);

    return temp;
}

void
gaussianRandom::evaluate(uint4 num_1, 
                         uint4 num_2, 
                         uint4 num_3, 
                         uint4 num_4)
{
	this->r1 = num_1;
	this->r2 = num_2;
	this->a = num_3;
	this->b = num_4;
}

__kernel
void gaussianRand(const __global uint4 *seedArray,
            uint width,
            uint mulFactor,
            __global float4 *gaussianRand)
{

    gaussianRandom GaussianRand;
	GaussianRand.initial(seedArray, width, mulFactor, gaussianRand);
	GaussianRand.run();
	   
}
