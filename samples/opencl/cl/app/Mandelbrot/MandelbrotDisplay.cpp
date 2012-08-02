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


#include "MandelbrotDisplay.hpp"
#include "Mandelbrot.hpp"
#ifdef linux
# define GL_GLEXT_PROTOTYPES
#endif // !linux
#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>

// An instance of the Mandelbrot Class 
Mandelbrot clMandelbrot("OpenCL Mandelbrot fractal");

// Window height, Window Width and the pixels to be displayed 
int width;
int height;
unsigned char * output;

int mouseX = 0;
int mouseY = 0;
bool panning = false;

bool zoomIn = false;
bool zoomOut = false;

// display function 
void
displayFunc()
{
    if (!clMandelbrot.getBenched())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, output);
        glFlush();
        glutSwapBuffers();
    }
}

// idle function 
void
idleFunc(void)
{
    if (panning)
    {
        if (mouseX < (width / 4))
        {
            clMandelbrot.setXPos(clMandelbrot.getXPos() - clMandelbrot.getXStep());
        }
        else if (mouseX > (3 * width / 4))
        {
            clMandelbrot.setXPos(clMandelbrot.getXPos() + clMandelbrot.getXStep());
        }
        if (mouseY < (height / 4))
        {
            clMandelbrot.setYPos(clMandelbrot.getYPos() + clMandelbrot.getYStep());
        }
        else if (mouseY > (3 * height / 4))
        {
            clMandelbrot.setYPos(clMandelbrot.getYPos() - clMandelbrot.getYStep());
        }
        if (zoomIn)
        {
            clMandelbrot.setXSize(clMandelbrot.getXSize() * 0.99);
        }
        else if (zoomOut)
        {
            clMandelbrot.setXSize(clMandelbrot.getXSize() / 0.99);
        }
    }
    clMandelbrot.run();
    //clMandelbrot.verifyResults();

    glutPostRedisplay();
}

// keyboard function 
void
keyboardFunc(unsigned char key, int mouseX, int mouseY)
{
    switch(key)
    {
        // If the user hits escape or Q, then exit 
        case GLUT_ESCAPE_KEY:
        case 'q':
        case 'Q':
            {
                cleanup();
                exit(0);
                break;
            }
        case 'c':
            {
                printf("center (%.13f, %.13f), window width %.13f\n", clMandelbrot.getXPos(), clMandelbrot.getYPos(),
                                                                      clMandelbrot.getXStep());
                break;
            }
        case 'i':
            {
                cl_uint maxIterations = clMandelbrot.getMaxIterations();
                maxIterations = ((maxIterations * 2) < MAX_ITER) ? maxIterations * 2 : MAX_ITER;
                printf("Setting maxIterations to %d\n", maxIterations);
                clMandelbrot.setMaxIterations(maxIterations);
                break;
            }
        case 'I':
            {
                cl_uint maxIterations = clMandelbrot.getMaxIterations();
                maxIterations = ((maxIterations / 2) > MIN_ITER) ? maxIterations / 2 : MIN_ITER;
                printf("Setting maxIterations to %d\n", maxIterations);
                clMandelbrot.setMaxIterations(maxIterations);
                break;
            }
        case 'b':
            {
                if (clMandelbrot.getTiming())
                {
                    clMandelbrot.setBench(1);
                }
            }
        case 'p':
            {
                panning = (panning == false)? true : false;
                break;
            }
        default:
            break;
    }
}

void mouseEntry(int state)
{
    if (state == GLUT_LEFT)
    {
        panning = false;
    }
    else
    {
        panning = true;
    }
}

void
mouseFunc(int button, int state, int x, int y)
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            {
                if ((state == GLUT_DOWN) && !zoomOut)
                {
                    zoomIn = true;
                }
                else
                {
                    zoomIn = false;
                }
                break;
            }
        case GLUT_RIGHT_BUTTON:
            {
                if ((state == GLUT_DOWN) && !zoomIn)
                {
                    zoomOut = true;
                }
                else
                {
                    zoomOut = false;
                }
                break;
            }
        default:
            break;
    }
    mouseX = x;
    mouseY = y;
}

void
motionFunc(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

void
passiveMotionFunc(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

// initalise display 
void
initDisplay(int argc, char *argv[])
{
    initGlut(argc, argv);
    initGL();
}

// initalise glut 
void
initGlut(int argc, char *argv[])
{
    /* initialising the window */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    
    // Print available keyboard and mouse options.
    printf("\n\tKeyboard Options :\n"
           "\t  'i'  double the number of iterations\n"
           "\t  'I'  halve the number of iterations\n"
           "\t  'b'  benchmark the current frame (when -t is enabled)\n"
           "\t  'c'  print the current center position\n"
           "\t  'p'  toggle panning\n" 
           "\n\tMouse Options :\n"
           "\t  Move mouse to pan image\n"
           "\t  Left click to zoom in\n"
           "\t  Right click to zoom out\n\n");

    printf("width %d, height %d\n", width, height);
    glutInitWindowSize(width, height);
    mouseX = width / 2;
    mouseY = height / 2;
    glutInitWindowPosition(0,0);
    glutCreateWindow("Mandelbrot Fractal Generator");

    // the various glut callbacks 
    glutDisplayFunc(displayFunc);
    glutIdleFunc(idleFunc);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutPassiveMotionFunc(passiveMotionFunc);
    glutEntryFunc(mouseEntry);
}

// initalise OpenGL 
void
initGL(void)
{
    glewInit();
}

void
mainLoopGL(void)
{
    glutMainLoop();
}

// free any allocated resources 
void
cleanup(void)
{
    clMandelbrot.cleanup();
    clMandelbrot.printStats();

}

int 
main(int argc, char * argv[])
{
    // initalise and run the mandelbrot kernel 
    clMandelbrot.initialize();
    if(clMandelbrot.parseCommandLine(argc, argv))
        return SDK_FAILURE;

    if(clMandelbrot.isDumpBinaryEnabled())
    {
        return clMandelbrot.genBinaryImage();
    }
    else
    {
        int returnVal = clMandelbrot.setup();
        if(returnVal != SDK_SUCCESS)
            return (returnVal == SDK_EXPECTED_FAILURE)? SDK_SUCCESS : SDK_FAILURE;

        if(clMandelbrot.run()!=SDK_SUCCESS)
            return SDK_FAILURE;
        if(clMandelbrot.verifyResults()!=SDK_SUCCESS)
            return SDK_FAILURE;

        // show window if it is not running in quiet mode 
        if(clMandelbrot.showWindow())
        {
            width = clMandelbrot.getWidth();
            height = clMandelbrot.getHeight();
            output = (unsigned char *)clMandelbrot.getPixels();

            initDisplay(argc, argv);
            mainLoopGL();
        }
        cleanup();
    }
    return SDK_SUCCESS;
}
