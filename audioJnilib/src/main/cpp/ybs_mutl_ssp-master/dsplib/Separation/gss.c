
#include "Separation/gss.h"

/*******************************************************************************
 * Compatibility issues                                                        *
 ******************************************************************************/

/*******************************************************************************
 * gssInit                                                                     *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:  myGSS           The gss object used to be initialized              *
 *          myParameters    The parameters used for initialization             *
 *                                                                             *
 * Outputs: myGSS           The initialized gss object                         *
 *                                                                             *
 * Description: This function initializes the gss object.                      *
 *                                                                             *
 ******************************************************************************/

void gssInit(struct objGSS* myGSS, struct ParametersStruct* myParameters, struct objMicrophones* myMicrophones)
{

    unsigned int indexMicrophone;

    unsigned int indexPoint;

    float x, y, z;

    float delay;
    float delayMin;
    float delayMax;

    float distanceX;
    float distanceY;
    float distanceZ;
    float distance;

    struct objSphere* mySphere;

    unsigned int k;
    unsigned int indexDelay;

    unsigned int nSources;

    // *************************************************************************
    // * STEP 1: Load parameters                                               *
    // *************************************************************************

    myGSS->GSS_SOURCEDISTANCE = (float) myParameters->P_GSS_SOURCEDISTANCE;
    myGSS->GSS_FS = (unsigned int) GLOBAL_FS;
    myGSS->GSS_C = (float) GLOBAL_C;
    myGSS->GSS_NFRAMES = (unsigned int) GLOBAL_FRAMESIZE;
    myGSS->GSS_HALFNFRAMES = myGSS->GSS_NFRAMES / 2;
    myGSS->GSS_HALFNFRAMESPLUSONE = myGSS->GSS_HALFNFRAMES + 1;

    myGSS->GSS_LAMBDA = myParameters->P_GSS_LAMBDA;
    myGSS->GSS_MU = myParameters->P_GSS_MU;

    myGSS->GSS_NBSOURCES = (unsigned int) myParameters->P_GEN_DYNSOURCES;

    // *************************************************************************
    // * STEP 2: Initialize microphones variables                              *
    // *************************************************************************

    myGSS->myMicrophones = (struct objMicrophones*) malloc(sizeof(struct objMicrophones));
    microphonesClone(myMicrophones, myGSS->myMicrophones);

    // *************************************************************************
    // * STEP 3: Initialize general variables                                  *
    // *************************************************************************

    myGSS->delayOffset = 0;

    // *************************************************************************
    // * STEP 4: Initialize source variables                                   *
    // *************************************************************************

    idListInit(&myGSS->sourcesIDNow, myGSS->GSS_NBSOURCES);
    idListInit(&myGSS->sourcesIDAdded, myGSS->GSS_NBSOURCES);
    idListInit(&myGSS->sourcesIDDeleted, myGSS->GSS_NBSOURCES);
    myGSS->sourcesPosition = (float**) newTable2D(myGSS->GSS_NBSOURCES, 3, sizeof(float));
    nSources = 0;

    // *************************************************************************
    // * STEP 5: Initialize matrices                                           *
    // *************************************************************************

    // Initialize the matrix x(k): nMics x 1
    myGSS->x = matrixCreate(myGSS->myMicrophones->nMics,1,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix xH(k): 1 x nMics
    myGSS->xH = matrixCreate(1,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix y(k): nSources x 1
    myGSS->y = matrixCreate(nSources,1,myGSS->GSS_HALFNFRAMESPLUSONE);
    myGSS->yFull = matrixCreate(nSources,1,myGSS->GSS_NFRAMES);

    // Initialize the matrix yH(k): 1 x nSources
    myGSS->yH = matrixCreate(1,nSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix Ryy_E(k): nSources x nSources
    myGSS->Ryy_E = matrixCreate(nSources,nSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix A(k): nMics x nSources
    myGSS->A = matrixCreate(myGSS->myMicrophones->nMics,nSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix AH(k): nMics x nSources
    myGSS->AH = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix Alpha(k): 1x1
    myGSS->alpha = matrixCreate(1,1,myGSS->GSS_HALFNFRAMESPLUSONE);
    myGSS->alphatmp = matrixCreate(1,1,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix W^n(k): nSources x nMics
    myGSS->Wn = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp1: nSources x nMics
    myGSS->Wntmp1 = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix dJ1(k): nSources x nMics
    myGSS->dJ1 = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp1: nSources x nSources
    myGSS->dJ1tmp1 = matrixCreate(nSources,nSources,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp2: nSources x nMics
    myGSS->dJ1tmp2 = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp3: nMics x 1
    myGSS->dJ1tmp3 = matrixCreate(myGSS->myMicrophones->nMics,1,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp4: nSources x nMics
    myGSS->dJ1tmp4 = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix dJ2(k): nSources x nMics
    myGSS->dJ2 = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp1: nSources x nSources
    myGSS->dJ2tmp1 = matrixCreate(nSources,nSources,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp2: nSources x nMics
    myGSS->dJ2tmp2 = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Initialize the matrix dJR'(k); nSources x nMics
    myGSS->dJR = matrixCreate(nSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // *************************************************************************
    // * STEP 6: Find the range of delays                                      *
    // *************************************************************************

    // +-----------------------------------------------------------------------+
    // | Step A: Create a unit sphere                                          |
    // +-----------------------------------------------------------------------+

    mySphere = (struct objSphere*) malloc(sizeof(struct objSphere));
    sphereInit(mySphere,4);

    // +-----------------------------------------------------------------------+
    // | Step B: Test each point                                               |
    // +-----------------------------------------------------------------------+

    delayMin = INFINITY;
    delayMax = 0.0f;

    for (indexPoint = 0; indexPoint < mySphere->SPHERE_NUMBERPOINTS; indexPoint++)
    {

        x = myGSS->GSS_SOURCEDISTANCE * mySphere->spherePoints[indexPoint][0];
        y = myGSS->GSS_SOURCEDISTANCE * mySphere->spherePoints[indexPoint][1];
        z = myGSS->GSS_SOURCEDISTANCE * mySphere->spherePoints[indexPoint][2];

        for (indexMicrophone = 0; indexMicrophone < myGSS->myMicrophones->nMics; indexMicrophone++)
        {

            distanceX = microphonesGetPositionX(myGSS->myMicrophones, indexMicrophone) - x;
            distanceY = microphonesGetPositionY(myGSS->myMicrophones, indexMicrophone) - y;
            distanceZ = microphonesGetPositionZ(myGSS->myMicrophones, indexMicrophone) - z;

            distance = sqrtf(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);

            delay = (myGSS->GSS_FS / myGSS->GSS_C) * distance;

            if (delay < delayMin)
            {
                delayMin = delay;
            }
            if (delay > delayMax)
            {
                delayMax = delay;
            }

        }

    }

    // Save the delay
    myGSS->delayOffset = (unsigned int) (floor(delayMin));

    // Save the range
    myGSS->delayMinimum = (unsigned int) 0;
    myGSS->delayMaximum = (unsigned int) (floor(delayMax + 1.0));

    // Clear sphere
    sphereTerminate(mySphere);
    free((void*) mySphere);

    // +---------------------------------------------------------------+
    // | Step C: Create arrays for math acceleration                   |
    // +---------------------------------------------------------------+

    myGSS->cosTable = (float**) newTable2D(myGSS->delayMaximum, myGSS->GSS_HALFNFRAMESPLUSONE, sizeof(float));
    myGSS->sinTable = (float**) newTable2D(myGSS->delayMaximum, myGSS->GSS_HALFNFRAMESPLUSONE, sizeof(float));

    for (indexDelay = 0; indexDelay < myGSS->delayMaximum; indexDelay++)
    {
        for (k = 0; k < myGSS->GSS_HALFNFRAMESPLUSONE; k++)
        {

            // Compute aij(k)   = exp(-j*2*pi*k*delay/N)
            //                  = cos(2*pi*k*delay/N) - j * sin(2*pi*k*delay/N)
            myGSS->cosTable[indexDelay][k] = cosf(2.0f * M_PI * k * indexDelay / myGSS->GSS_NFRAMES);
            myGSS->sinTable[indexDelay][k] = -1.0f * sinf(2.0f * M_PI * k * indexDelay / myGSS->GSS_NFRAMES);

        }
    }

}

/*******************************************************************************
 * gssTerminate                                                                *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:  myGSS           The gss object used to be initialized              *
 *                                                                             *
 * Outputs: (none)                                                             *
 *                                                                             *
 * Description: This function terminates the gss object.                       *
 *                                                                             *
 ******************************************************************************/

void gssTerminate(struct objGSS* myGSS)
{

    // *************************************************************************
    // * STEP 1: Terminate microphones variables                               *
    // *************************************************************************

    microphonesTerminate(myGSS->myMicrophones);
    free((void*) myGSS->myMicrophones);

    // *************************************************************************
    // * STEP 2: Terminate sources variables                                   *
    // *************************************************************************

    idListTerminate(&myGSS->sourcesIDAdded);
    idListTerminate(&myGSS->sourcesIDDeleted);
    idListTerminate(&myGSS->sourcesIDNow);
    deleteTable2D((void**) myGSS->sourcesPosition);

    // *************************************************************************
    // * STEP 3: Terminate matrices                                            *
    // *************************************************************************

    // Delete the matrix x(k)
    matrixDelete(myGSS->x);

    // Delete the matrix xH(k)
    matrixDelete(myGSS->xH);

    // Delete the matrix y(k)
    matrixDelete(myGSS->y);

    // Delete the matrix yH(k)
    matrixDelete(myGSS->yH);

    // Delete the matrix Ryy_E(k)
    matrixDelete(myGSS->Ryy_E);

    // Delete the matrix A(k)
    matrixDelete(myGSS->A);

    // Delete the matrix AH(k)
    matrixDelete(myGSS->AH);

    // Delete the matrix Alpha(k)
    matrixDelete(myGSS->alpha);
    matrixDelete(myGSS->alphatmp);

    // Delete the matrix W^n(k)
    matrixDelete(myGSS->Wn);
    // Tmp1: nSources x nMics
    matrixDelete(myGSS->Wntmp1);

    // Delete the matrix dJ1(k)
    matrixDelete(myGSS->dJ1);
    // Tmp1: nSources x nSources
    matrixDelete(myGSS->dJ1tmp1);
    // Tmp2: nSources x nMics
    matrixDelete(myGSS->dJ1tmp2);
    // Tmp3: nMics x 1
    matrixDelete(myGSS->dJ1tmp3);
    // Tmp4: nSources x nMics
    matrixDelete(myGSS->dJ1tmp4);

    // Delete the matrix dJ2(k)
    matrixDelete(myGSS->dJ2);
    // Tmp1: nSources x nSources
    matrixDelete(myGSS->dJ2tmp1);
    // Tmp2: nSources x nMics
    matrixDelete(myGSS->dJ2tmp2);

    // Delete the matrix dJR'(k)
    matrixDelete(myGSS->dJR);

    // *************************************************************************
    // * STEP 4: Terminate math acceleration tables                            *
    // *************************************************************************

    deleteTable2D((void**) myGSS->cosTable);
    deleteTable2D((void**) myGSS->sinTable);

}

/*******************************************************************************
 * gssAddSource                                                                *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:  myGSS           The gss object used in which a source is added     *
 *                                                                             *
 * Outputs: myGSS           The gss object with the source added               *
 *                                                                             *
 * Description: This function adds a new source to the gss object.             *
 *                                                                             *
 ******************************************************************************/

void gssAddSource(struct objGSS* myGSS, ID_TYPE newID, float positionX, float positionY, float positionZ)
{

    signed int indexID;

    unsigned int newNSources;
    unsigned int oldNSources;

    float x,y,z;
    float r;
    float xNotNorm,yNotNorm,zNotNorm;
    float xNorm,yNorm,zNorm;
    float distanceX,distanceY,distanceZ;
    float distance;

    unsigned int delay;
    unsigned int delayNorm;

    float aijReal;
    float aijImag;

    float wjiReal;
    float wjiImag;

    unsigned int indexMicrophone;
    unsigned int k;

    // *************************************************************************
    // * STEP 1: Get new number of sources                                     *
    // *************************************************************************

    oldNSources = idListGetNElements(&myGSS->sourcesIDNow);
    newNSources = oldNSources + 1;

    // *************************************************************************
    // * STEP 2: Change dimensions of matrices                                 *
    // *************************************************************************

    // +-----------------------------------------------------------------------+
    // | Step A: Insert row/column in recursive matrices                       |
    // +-----------------------------------------------------------------------+

    // Since these matrices are recursive, the old content needs to be
    // preserved. This is the reason why insert operations are performed.

    // Insert a new row in y(k)
    matrixInsertRow(myGSS->y,oldNSources);
    matrixInsertRow(myGSS->yFull,oldNSources);

    // Insert a new row in W^n(k)
    matrixInsertRow(myGSS->Wn,oldNSources);

    // +-----------------------------------------------------------------------+
    // | Step B: Resize non-recursive matrices                                 |
    // +-----------------------------------------------------------------------+

    // Since these matrices are NOT recursive, the old content can be discarded.
    // For this reason, and simple resize operation is performed. This operation
    // is faster than an insert operation.

    // Resize the matrix yH(k): 1 x nSources
    matrixResize(myGSS->yH,1,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix Ryy_E(k): nSources x nSources
    matrixResize(myGSS->Ryy_E,newNSources,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix A(k): nMics x nSources
    matrixResize(myGSS->A,myGSS->myMicrophones->nMics,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix AH(k): nMics x nSources
    matrixResize(myGSS->AH,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix W^n(k): nSources x nMics
    // Tmp1: nSources x nMics
    matrixResize(myGSS->Wntmp1,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix dJ1(k): nSources x nMics
    matrixResize(myGSS->dJ1,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp1: nSources x nSources
    matrixResize(myGSS->dJ1tmp1,newNSources,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp2: nSources x nMics
    matrixResize(myGSS->dJ1tmp2,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp3: nMics x 1
    matrixResize(myGSS->dJ1tmp3,myGSS->myMicrophones->nMics,1,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp4: nSources x nMics
    matrixResize(myGSS->dJ1tmp4,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix dJ2(k): nSources x nMics
    matrixResize(myGSS->dJ2,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp1: nSources x nSources
    matrixResize(myGSS->dJ2tmp1,newNSources,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp2: nSources x nMics
    matrixResize(myGSS->dJ2tmp2,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix dJR'(k); nSources x nMics
    matrixResize(myGSS->dJR,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // *************************************************************************
    // * STEP 3: Update list of IDs and positions                              *
    // *************************************************************************

    // Add IDs to list
    indexID = idListAdd(&myGSS->sourcesIDNow, newID);
    if (indexID != -1)
    {
        myGSS->sourcesPosition[indexID][0] = positionX;
        myGSS->sourcesPosition[indexID][1] = positionY;
        myGSS->sourcesPosition[indexID][2] = positionZ;
    }

    // *************************************************************************
    // * STEP 4: Update the matrix Wn(k)                                       *
    // *************************************************************************

    // Normalize the position of the source

    xNotNorm = myGSS->sourcesPosition[indexID][0];
    yNotNorm = myGSS->sourcesPosition[indexID][1];
    zNotNorm = myGSS->sourcesPosition[indexID][2];

    r = sqrtf(xNotNorm * xNotNorm + yNotNorm * yNotNorm + zNotNorm * zNotNorm);

    xNorm = xNotNorm / (r + 1E-10f);
    yNorm = yNotNorm / (r + 1E-10f);
    zNorm = zNotNorm / (r + 1E-10f);

    // Set at the defined distance

    x = myGSS->GSS_SOURCEDISTANCE * xNorm;
    y = myGSS->GSS_SOURCEDISTANCE * yNorm;
    z = myGSS->GSS_SOURCEDISTANCE * zNorm;

    for (indexMicrophone = 0; indexMicrophone < myGSS->myMicrophones->nMics; indexMicrophone++)
    {

        // Compute the distance between the source and the microphone
        distanceX = microphonesGetPositionX(myGSS->myMicrophones,indexMicrophone) - x;
        distanceY = microphonesGetPositionY(myGSS->myMicrophones,indexMicrophone) - y;
        distanceZ = microphonesGetPositionZ(myGSS->myMicrophones,indexMicrophone) - z;

        distance = sqrtf(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);

        // Compute the delay for sound to reach the microphone from the source
        delay = (unsigned int) floor(((myGSS->GSS_FS / myGSS->GSS_C) * distance) + 0.5);

        // Remove the offset to avoid delaying the samples by too much if not needed
        delayNorm = delay - myGSS->delayOffset;

        for (k = 0; k < myGSS->GSS_HALFNFRAMESPLUSONE; k++)
        {

            // Compute aij(k)   = exp(-j * 2 * pi * k * delay / nFrames)
            //                  = cos(2 * pi * k * delay / nFrames) - j * sin(2 * pi * k * delay / nFrames)
            aijReal = cosf(2.0f * M_PI * k * delayNorm / myGSS->GSS_NFRAMES);
            aijImag = -1.0f * sinf(2.0f * M_PI * k * delayNorm / myGSS->GSS_NFRAMES);

            // Compute wji(k) = aij*(k) / nMics
            wjiReal = aijReal / myGSS->myMicrophones->nMics;
            wjiImag = -1.0f * aijImag / myGSS->myMicrophones->nMics;

            // Save in the matrix Wn(k)
            matrixSetReal(myGSS->Wn,oldNSources,indexMicrophone,k,wjiReal);
            matrixSetImag(myGSS->Wn,oldNSources,indexMicrophone,k,wjiImag);

        }

    }

}

/*******************************************************************************
 * gssDeleteSource                                                             *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:  myGSS           The gss object used in which a source is deleted   *
 *                                                                             *
 * Outputs: myGSS           The gss object with the source deleted             *
 *                                                                             *
 * Description: This function removes a new source from the gss object.        *
 *                                                                             *
 ******************************************************************************/

void gssDeleteSource(struct objGSS* myGSS, ID_TYPE deleteID)
{

    signed int indexID;

    unsigned int indexElement;
    unsigned int newNSources;

    // *************************************************************************
    // * STEP 1: Delete the ID                                                 *
    // *************************************************************************

    indexID = idListDelete(&myGSS->sourcesIDNow, deleteID);
    newNSources = idListGetMaxNElements(&myGSS->sourcesIDNow);

    // *************************************************************************
    // * STEP 2: Update positions                                              *
    // *************************************************************************

    for (indexElement = (unsigned int) indexID; indexElement < (newNSources - 1); indexElement++)
    {
        myGSS->sourcesPosition[indexElement][0] = myGSS->sourcesPosition[indexElement + 1][0];
        myGSS->sourcesPosition[indexElement][1] = myGSS->sourcesPosition[indexElement + 1][1];
        myGSS->sourcesPosition[indexElement][2] = myGSS->sourcesPosition[indexElement + 1][2];
    }

    // *************************************************************************
    // * STEP 3: Change dimensions of matrices                                 *
    // *************************************************************************

    // +-----------------------------------------------------------------------+
    // | Step A: Delete row/column in recursive matrices                       |
    // +-----------------------------------------------------------------------+

    // Since these matrices are recursive, the old content needs to be
    // preserved. This is the reason why delete operations are performed.

    // Remove a row from y(k)
    matrixDeleteRow(myGSS->y,indexID);
    matrixDeleteRow(myGSS->yFull,indexID);

    // Remove a row from W^n(k)
    matrixDeleteRow(myGSS->Wn,indexID);

    // +-----------------------------------------------------------------------+
    // | Step B: Resize non-recursive matrices                                 |
    // +-----------------------------------------------------------------------+

    // Since these matrices are NOT recursive, the old content can be discarded.
    // For this reason, and simple resize operation is performed. This operation
    // is faster than a delete operation.

    // Resize the matrix yH(k): nSources x 1
    matrixResize(myGSS->yH,1,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix Ryy(k): nSources x nSources
    matrixResize(myGSS->Ryy_E,newNSources,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix A(k): nMics x nSources
    matrixResize(myGSS->A,myGSS->myMicrophones->nMics,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix AH(k): nMics x nSources
    matrixResize(myGSS->AH,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix W^n(k): nSources x nMics
    // Tmp1: nSources x nMics
    matrixResize(myGSS->Wntmp1,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix dJ1(k): nSources x nMics
    matrixResize(myGSS->dJ1,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp1: nSources x nSources
    matrixResize(myGSS->dJ1tmp1,newNSources,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp2: nSources x nMics
    matrixResize(myGSS->dJ1tmp2,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp3: nMics x 1
    matrixResize(myGSS->dJ1tmp3,myGSS->myMicrophones->nMics,1,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp4: nSources x nMics
    matrixResize(myGSS->dJ1tmp4,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

    // Resize the matrix dJ2(k): nSources x nMics
    matrixResize(myGSS->dJ2,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp1: nSources x nSources
    matrixResize(myGSS->dJ2tmp1,newNSources,newNSources,myGSS->GSS_HALFNFRAMESPLUSONE);
    // Tmp2: nSources x nMics
    matrixResize(myGSS->dJ2tmp2,newNSources,myGSS->myMicrophones->nMics,myGSS->GSS_HALFNFRAMESPLUSONE);

}

/*******************************************************************************
 * gssUpdateSource                                                             *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:  myGSS           The gss object used in which a source position is  *
 *                          updated                                            *
 *                                                                             *
 * Outputs: myGSS           The gss object with the updated source position    *
 *                                                                             *
 * Description: This function updates the position of a source.                *
 *                                                                             *
 ******************************************************************************/

void gssUpdateSource(struct objGSS* myGSS, ID_TYPE sourceID, float positionX, float positionY, float positionZ)
{

    signed int indexElement;

    // *************************************************************************
    // * STEP 1: Find the position that matches the ID                         *
    // *************************************************************************

    indexElement = idListGetIndex(&myGSS->sourcesIDNow, sourceID);

    // *************************************************************************
    // * STEP 2: Update the position of the corresponding source               *
    // *************************************************************************

    if (indexElement != ID_NOTFOUND)
    {

        // Update the position values
        myGSS->sourcesPosition[indexElement][0] = positionX;
        myGSS->sourcesPosition[indexElement][1] = positionY;
        myGSS->sourcesPosition[indexElement][2] = positionZ;

    }

}

/*******************************************************************************
 * gssRefreshSources                                                           *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:  myGSS            The gss object which sources need to be refreshed *
 *          myTrackedSources The tracked sources                               *
 *                                                                             *
 * Outputs: myGSS            The gss object with the source deleted            *
 *                                                                             *
 * Description: This function removes a new source from the gss object.        *
 *                                                                             *
 ******************************************************************************/

void gssRefreshSources(struct objGSS* myGSS, struct objTrackedSources* myTrackedSources)
{

    unsigned int indexSource;

    ID_TYPE currentID;
    unsigned int currentIndex;

    // *************************************************************************
    // * STEP 1: Find sources to be deleted and added                          *
    // *************************************************************************

    idListCompare(&myGSS->sourcesIDNow, &myTrackedSources->sourcesID, &myGSS->sourcesIDAdded, &myGSS->sourcesIDDeleted);

    // *************************************************************************
    // * STEP 2: Delete sources                                                *
    // *************************************************************************

    for (indexSource = 0; indexSource < idListGetNElements(&myGSS->sourcesIDDeleted); indexSource++)
    {       
        currentID = idListGetID(&myGSS->sourcesIDDeleted, indexSource);
        gssDeleteSource(myGSS, currentID);
    }

    // *************************************************************************
    // * STEP 3: Add sources                                                   *
    // *************************************************************************

    for (indexSource = 0; indexSource < idListGetNElements(&myGSS->sourcesIDAdded); indexSource++)
    {
        currentID = idListGetID(&myGSS->sourcesIDAdded, indexSource);
        currentIndex = idListGetIndex(&myTrackedSources->sourcesID, currentID);

        gssAddSource(myGSS, currentID,
                     myTrackedSources->sourcesPosition[currentIndex][0],
                     myTrackedSources->sourcesPosition[currentIndex][1],
                     myTrackedSources->sourcesPosition[currentIndex][2]);
    }

    // *************************************************************************
    // * STEP 4: Update positions                                              *
    // *************************************************************************

    for (indexSource = 0; indexSource < idListGetNElements(&myTrackedSources->sourcesID); indexSource++)
    {
        currentID = idListGetID(&myTrackedSources->sourcesID, indexSource);
        currentIndex = idListGetIndex(&myTrackedSources->sourcesID, currentID);

        gssUpdateSource(myGSS, currentID,
                        myTrackedSources->sourcesPosition[currentIndex][0],
                        myTrackedSources->sourcesPosition[currentIndex][1],
                        myTrackedSources->sourcesPosition[currentIndex][2]);

    }
}

/*******************************************************************************
 * gssProcess                                                                  *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:  myGSS           The gss object used to perform separation          *
 *          myPreprocessor  The preprocessor object which contains frames that *
 *                          are used for this operation                        *
 *                                                                             *
 * Outputs: mySeparatedSources  The object that contains the separated sources *
 *                                                                             *
 * Description: This function performs the separation operation                *
 *                                                                             *
 ******************************************************************************/

void gssProcess(struct objGSS* myGSS, struct objPreprocessor* myPreprocessor, struct objTrackedSources* myTrackedSources, struct objSeparatedSources* mySeparatedSources)
{

    unsigned int indexSource;
    signed int indexID;
    unsigned int indexMicrophone;
    unsigned int k;

    float x,y,z;
    float r;
    float xNotNorm,yNotNorm,zNotNorm;
    float xNorm,yNorm,zNorm;
    float distanceX,distanceY,distanceZ;
    float distance;

    unsigned int delay;
    unsigned int delayNorm;

    float aijReal;
    float aijImag;

    float scalar;

    unsigned int nSources;

    // *************************************************************************
    // * STEP 0: Load tracked sources                                          *
    // *************************************************************************

    gssRefreshSources(myGSS, myTrackedSources);

    nSources = idListGetNElements(&myGSS->sourcesIDNow);

    // *************************************************************************
    // * STEP 1: Load frames in x(k)                                           *
    // *************************************************************************

    for (indexMicrophone = 0; indexMicrophone < myGSS->myMicrophones->nMics; indexMicrophone++)
    {



        for (k = 0; k < myGSS->GSS_HALFNFRAMESPLUSONE; k++)
        {
            // Load x(k)
            matrixSetReal(myGSS->x, indexMicrophone, 0, k, myPreprocessor->micArray[indexMicrophone]->xfreqReal[k]);
            matrixSetImag(myGSS->x, indexMicrophone, 0, k, myPreprocessor->micArray[indexMicrophone]->xfreqImag[k]);
        }


    }


    if (nSources > 0)
    {


        for (indexSource = 0; indexSource < nSources; indexSource++)
        {

            xNotNorm = myGSS->sourcesPosition[indexSource][0];
            yNotNorm = myGSS->sourcesPosition[indexSource][1];
            zNotNorm = myGSS->sourcesPosition[indexSource][2];

            r = sqrtf(xNotNorm * xNotNorm + yNotNorm * yNotNorm + zNotNorm * zNotNorm);

            xNorm = xNotNorm / (r + 1E-10f);
            yNorm = yNotNorm / (r + 1E-10f);
            zNorm = zNotNorm / (r + 1E-10f);

            x = myGSS->GSS_SOURCEDISTANCE * xNorm;
            y = myGSS->GSS_SOURCEDISTANCE * yNorm;
            z = myGSS->GSS_SOURCEDISTANCE * zNorm;

            for (indexMicrophone = 0; indexMicrophone < myGSS->myMicrophones->nMics; indexMicrophone++)
            {

                // Compute the distance between the source and the microphone
                distanceX = microphonesGetPositionX(myGSS->myMicrophones, indexMicrophone) - x;
                distanceY = microphonesGetPositionY(myGSS->myMicrophones, indexMicrophone) - y;
                distanceZ = microphonesGetPositionZ(myGSS->myMicrophones, indexMicrophone) - z;

                distance = sqrtf(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);

                // Compute the delay for sound to reach the microphone from the source
                delay = (unsigned int)floor(((myGSS->GSS_FS / myGSS->GSS_C) * distance) + 0.5);

                // Remove the offset to avoid delaying the samples by too much if not needed
                delayNorm = delay - myGSS->delayOffset;


                for (k = 0; k < myGSS->GSS_HALFNFRAMESPLUSONE; k++)
                {

                    // Compute aij(k)   = exp(-j*2*pi*k*delay/N)
                    //                  = cos(2*pi*k*delay/N) - j * sin(2*pi*k*delay/N)
                    aijReal = myGSS->cosTable[delayNorm][k];
                    aijImag = myGSS->sinTable[delayNorm][k];

                    // Save in the matrix A(k)
                    matrixSetReal(myGSS->A, indexMicrophone, indexSource, k, aijReal);
                    matrixSetImag(myGSS->A, indexMicrophone, indexSource, k, aijImag);

                }

            }

            /*******  A*X ***/
            matrixMultMatrixPerElement(myGSS->A, myGSS->x, myGSS->alphatmp);

            matrixAddRowMatrix(myGSS->alphatmp, myGSS->y);
        }
    }

    //matrixAddRowMatrix(myGSS->x, myGSS->y);

    // *************************************************************************
    // * STEP 3: Compute y(k) with all k's                                     *
    // *************************************************************************

    for (indexSource = 0; indexSource < nSources; indexSource++)
    {

        for (k = 1; k < myGSS->GSS_HALFNFRAMES; k++)
        {
            matrixSetReal(myGSS->yFull,indexSource, 0, k, matrixGetReal(myGSS->y, indexSource, 0, k));
            matrixSetImag(myGSS->yFull,indexSource, 0, k, matrixGetImag(myGSS->y, indexSource, 0, k));
            matrixSetReal(myGSS->yFull,indexSource, 0, (myGSS->GSS_NFRAMES - k), matrixGetReal(myGSS->y, indexSource, 0, k));
            matrixSetImag(myGSS->yFull,indexSource, 0, (myGSS->GSS_NFRAMES - k), -1.0f * matrixGetImag(myGSS->y, indexSource, 0, k));
        }

        matrixSetReal(myGSS->yFull, indexSource, 0, 0, matrixGetReal(myGSS->y, indexSource, 0, 0));
        matrixSetImag(myGSS->yFull, indexSource, 0, 0, matrixGetImag(myGSS->y, indexSource, 0, 0));

        matrixSetReal(myGSS->yFull, indexSource, 0, myGSS->GSS_HALFNFRAMES, matrixGetReal(myGSS->y, indexSource, 0, myGSS->GSS_HALFNFRAMES));
        matrixSetImag(myGSS->yFull, indexSource, 0, myGSS->GSS_HALFNFRAMES, matrixGetImag(myGSS->y, indexSource, 0, myGSS->GSS_HALFNFRAMES));


    }

    // *************************************************************************
    // * STEP 4: Export frames                                                 *
    // *************************************************************************

    // Reset list of IDs
    idListReset(&mySeparatedSources->sourcesID);

    // Then load sources
    for (indexSource = 0; indexSource < nSources; indexSource++)
    {

        indexID = idListAdd(&mySeparatedSources->sourcesID, idListGetID(&myGSS->sourcesIDNow, indexSource));


        for (k = 0; k < myGSS->GSS_NFRAMES; k++)
        {

            // Export y(k)
            mySeparatedSources->sourcesFramesReal[indexID][k] = matrixGetReal(myGSS->yFull, indexID, 0, k);
            mySeparatedSources->sourcesFramesImag[indexID][k] = matrixGetImag(myGSS->yFull, indexID, 0, k);

        }

		
     


    }

}

