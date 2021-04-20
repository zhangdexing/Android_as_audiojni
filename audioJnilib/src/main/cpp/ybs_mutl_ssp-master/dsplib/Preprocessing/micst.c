

#include "Preprocessing/micst.h"

/*******************************************************************************
 * micstInitNormal                                                             *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myMicST         The micST object to be initialized             *
 *              myParameters    List of parameters that is used to configure   *
 *                              the object                                     *
 *              position        The microphone position on the cube            *
 *                              (must be an array made of 3 elements, where    *
 *                              1st element = x, 2nd = y and 3rd = z)          *
 *              gain            Gain of the microphone                         *
 *                                                                             *
 * Outputs:     myMicST         The micST initialized object                   *
 *                                                                             *
 * Description: This function initializes the micST object                     *
 *                                                                             *
 ******************************************************************************/

void micstInit(struct objMicST *myMicST, struct ParametersStruct *myParameters, float *position, float gain)
{

    // Index to fill the arrays
    unsigned int k;

    // *************************************************************************
    // * STEP 1: Load parameters                                               *
    // *************************************************************************

    myMicST->MICST_ALPHAD = myParameters->P_MICST_ALPHAD;
    myMicST->MICST_C = GLOBAL_C;
    myMicST->MICST_DELTA = myParameters->P_MICST_DELTA;
    myMicST->MICST_FRAMESIZE = GLOBAL_FRAMESIZE;
    myMicST->MICST_HALFFRAMESIZE = myMicST->MICST_FRAMESIZE / 2;
    myMicST->MICST_FS = GLOBAL_FS;
    myMicST->MICST_GAMMA = myParameters->P_MICST_GAMMA;
    myMicST->MICST_OVERLAP = GLOBAL_OVERLAP;

    // *************************************************************************
    // * STEP 2: Initialize context                                            *
    // *************************************************************************

    myMicST->gain = gain;
    myMicST->position[0] = position[0];
    myMicST->position[1] = position[1];
    myMicST->position[2] = position[2];

    myMicST->ksi = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->lambda = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->lambda_prev = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->sigma = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->weightedResultImag = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->weightedResultReal = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->xfreqImag = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->xfreqReal = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->xpower = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->xpower_prev = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->xtime = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->xtime_windowed = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->zeta = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));
    myMicST->zeta_prev = (float*) newTable1D(myMicST->MICST_FRAMESIZE,sizeof(float));

    myMicST->noiseEstimator = (struct objMcra*) malloc(sizeof(struct objMcra));

    // *************************************************************************
    // * STEP 3: Initialize the arrays                                         *
    // *************************************************************************

    for (k = 0; k < myMicST->MICST_FRAMESIZE; k++)
    {

        myMicST->ksi[k] = 0;
        myMicST->lambda[k] = 0;
        myMicST->lambda_prev[k] = 0;
        myMicST->sigma[k] = 0;
        myMicST->xfreqImag[k] = 0;
        myMicST->xfreqReal[k] = 0;
        myMicST->xpower[k] = 0;
        myMicST->xpower_prev[k] = 0;
        myMicST->xtime[k] = 0;
        myMicST->xtime_windowed[k] = 0;
        myMicST->zeta[k] = 0;
        myMicST->zeta_prev[k] = 0;
        myMicST->weightedResultImag[k] = 0;
        myMicST->weightedResultReal[k] = 0;

    }

    mcraInit(myMicST->noiseEstimator, myParameters, myMicST->MICST_FRAMESIZE);

}

/*******************************************************************************
 * micstTerminate                                                              *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myMicST         The micst object to be terminated              *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function terminates the micst object                      *
 *                                                                             *
 ******************************************************************************/

void micstTerminate(struct objMicST *myMicST)
{

    // Free memory
    deleteTable1D((void*) myMicST->ksi);
    deleteTable1D((void*) myMicST->lambda);
    deleteTable1D((void*) myMicST->lambda_prev);
    deleteTable1D((void*) myMicST->sigma);
    deleteTable1D((void*) myMicST->weightedResultImag);
    deleteTable1D((void*) myMicST->weightedResultReal);
    deleteTable1D((void*) myMicST->xfreqImag);
    deleteTable1D((void*) myMicST->xfreqReal);
    deleteTable1D((void*) myMicST->xpower);
    deleteTable1D((void*) myMicST->xpower_prev);
    deleteTable1D((void*) myMicST->xtime);
    deleteTable1D((void*) myMicST->xtime_windowed);
    deleteTable1D((void*) myMicST->zeta);
    deleteTable1D((void*) myMicST->zeta_prev);

    mcraTerminate(myMicST->noiseEstimator);
    free(myMicST->noiseEstimator);

}

/*******************************************************************************
 * micstProcessFrame		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myMicST         The micST object to use to process the frame   *
 *                                                                             *
 * Outputs:     myMicST         The updatd micST object                        *
 *                                                                             *
 * Description: This function processes the new frame and returns the          *
 *              cross correlation terms to be used by the beamformer           *
 *                                                                             *
 ******************************************************************************/

void micstProcessFrame(struct objMicST *myMicST)
{

    // Index to go through each array
    unsigned int k;

    // Temporary variable to store the square root of Xpower[k] for a given k
    float tmpSqrt;

    // Temporary variable to store the posteriori information
    float post;

    /***************************************************************************
     * Step 0: Set DC to zero                                                  *
     **************************************************************************/

    myMicST->xfreqReal[0] = 0.0f;
    myMicST->xfreqImag[0] = 0.0f;

    /***************************************************************************
     * Step 1: Compute the power                                               *
     **************************************************************************/


    // +-----------------------------------------------------------------------+
    // | No hardware acceleration                                              |
    // +-----------------------------------------------------------------------+

    myMicST->xpower[0] = myMicST->xfreqReal[0] * myMicST->xfreqReal[0] + myMicST->xfreqImag[0] * myMicST->xfreqImag[0];
    myMicST->xpower[myMicST->MICST_HALFFRAMESIZE] = myMicST->xfreqReal[myMicST->MICST_HALFFRAMESIZE] * myMicST->xfreqReal[myMicST->MICST_HALFFRAMESIZE] + myMicST->xfreqImag[myMicST->MICST_HALFFRAMESIZE] * myMicST->xfreqImag[myMicST->MICST_HALFFRAMESIZE];

    for (k = 1; k < myMicST->MICST_HALFFRAMESIZE; k++)
    {

        myMicST->xpower[k] = myMicST->xfreqReal[k] * myMicST->xfreqReal[k] + myMicST->xfreqImag[k] * myMicST->xfreqImag[k];
        myMicST->xpower[myMicST->MICST_FRAMESIZE - k] = myMicST->xpower[k];

    }



    /***************************************************************************
     * Step 2: Compute noise with MCRA                                         *
     **************************************************************************/

    // sigma_2_i[k]
    mcraProcessFrame(myMicST->noiseEstimator, myMicST->xpower, myMicST->sigma);

    /***************************************************************************
     * Step 3: Compute the simple reverberation model                          *
     **************************************************************************/


    // +-----------------------------------------------------------------------+
    // | No hardware acceleration                                              |
    // +-----------------------------------------------------------------------+

    // lambda_rev_n,i[k] = gamma * lambda_rev_n-1,i[k] + (1 - gamma) * delta * | zeta_n-1_i[k] * X_n-1_i[k] |^2

    myMicST->lambda[0] = myMicST->MICST_GAMMA * myMicST->lambda_prev[0] + (1.0f - myMicST->MICST_GAMMA) * myMicST->MICST_DELTA * fabsf(myMicST->zeta_prev[0]) * fabsf(myMicST->zeta_prev[0]) * myMicST->xpower_prev[0];
    myMicST->lambda[myMicST->MICST_HALFFRAMESIZE] = myMicST->MICST_GAMMA * myMicST->lambda_prev[myMicST->MICST_HALFFRAMESIZE] + (1.0f - myMicST->MICST_GAMMA) * myMicST->MICST_DELTA * fabsf(myMicST->zeta_prev[myMicST->MICST_HALFFRAMESIZE]) * fabsf(myMicST->zeta_prev[myMicST->MICST_HALFFRAMESIZE]) * myMicST->xpower_prev[myMicST->MICST_HALFFRAMESIZE];

    for (k = 1; k < myMicST->MICST_HALFFRAMESIZE; k++)
    {

        myMicST->lambda[k] = myMicST->MICST_GAMMA * myMicST->lambda_prev[k] + (1.0f - myMicST->MICST_GAMMA) * myMicST->MICST_DELTA * fabsf(myMicST->zeta_prev[k]) * fabsf(myMicST->zeta_prev[k]) * myMicST->xpower_prev[k];
        myMicST->lambda[myMicST->MICST_FRAMESIZE - k] = myMicST->lambda[k];

    }

    /***************************************************************************
     * Step 4: Compute the estimate of the a priori SNR                        *
     **************************************************************************/



    // +-----------------------------------------------------------------------+
    // | No hardware acceleration                                              |
    // +-----------------------------------------------------------------------+

    // post[k] = | X_n_i[k] |^2 - sigma_2_i[k] - lambda_rev_n,i[k]
    // ksi_n_i[k] = {(1 - alphaD) * [ zeta_n-1_i[k] ]^2 * | X_n-1_i[k] |^2 + alphaD * post[k]} / (sigma_2_i[k] + lambda_rev_n,i[k] + 1E-20)

    post = myMicST->xpower[0] - myMicST->sigma[0] - myMicST->lambda[0];
    if (post < 0.0f)
    {
        post = 0.0f;
    }
    myMicST->ksi[0] = ((1 - myMicST->MICST_ALPHAD) * fabsf(myMicST->zeta_prev[0]) * fabsf(myMicST->zeta_prev[0]) * myMicST->xpower_prev[0] + myMicST->MICST_ALPHAD * post) / (myMicST->sigma[0] + myMicST->lambda[0] + 1E-20f);

    post = myMicST->xpower[myMicST->MICST_HALFFRAMESIZE] - myMicST->sigma[myMicST->MICST_HALFFRAMESIZE] - myMicST->lambda[myMicST->MICST_HALFFRAMESIZE];
    if (post < 0.0f)
    {
        post = 0.0f;
    }
    myMicST->ksi[myMicST->MICST_HALFFRAMESIZE] = ((1 - myMicST->MICST_ALPHAD) * fabsf(myMicST->zeta_prev[myMicST->MICST_HALFFRAMESIZE]) * fabsf(myMicST->zeta_prev[myMicST->MICST_HALFFRAMESIZE]) * myMicST->xpower_prev[myMicST->MICST_HALFFRAMESIZE] + myMicST->MICST_ALPHAD * post) / (myMicST->sigma[myMicST->MICST_HALFFRAMESIZE] + myMicST->lambda[myMicST->MICST_HALFFRAMESIZE] + 1E-20f);

    for (k = 1; k < myMicST->MICST_HALFFRAMESIZE; k++)
    {
        post = myMicST->xpower[k] - myMicST->sigma[k] - myMicST->lambda[k];
        if (post < 0.0f)
        {
            post = 0.0f;
        }
        myMicST->ksi[k] = ((1 - myMicST->MICST_ALPHAD) * fabsf(myMicST->zeta_prev[k]) * fabsf(myMicST->zeta_prev[k]) * myMicST->xpower_prev[k] + myMicST->MICST_ALPHAD * post) / (myMicST->sigma[k] + myMicST->lambda[k] + 1E-20f);

        myMicST->ksi[myMicST->MICST_FRAMESIZE - k] = myMicST->ksi[k];
    }


    /***************************************************************************
     * Step 5: Compute the weighting function                                  *
     **************************************************************************/


    // +-----------------------------------------------------------------------+
    // | No hardware acceleration                                              |
    // +-----------------------------------------------------------------------+

    // zeta_n_i[k] = ksi_n_i[k] / (ksi_n_i[k] + 1)

    myMicST->zeta[0] = myMicST->ksi[0] / (myMicST->ksi[0] + 1.0f);
    myMicST->zeta[myMicST->MICST_HALFFRAMESIZE] = myMicST->ksi[myMicST->MICST_HALFFRAMESIZE] / (myMicST->ksi[myMicST->MICST_HALFFRAMESIZE] + 1.0f);

    for (k = 1; k < myMicST->MICST_HALFFRAMESIZE; k++)
    {

        myMicST->zeta[k] = myMicST->ksi[k] / (myMicST->ksi[k] + 1.0f);
        myMicST->zeta[myMicST->MICST_FRAMESIZE - k] = myMicST->zeta[k];

    }


    /***************************************************************************
     * Step 6: Compute the weighted spectrum                                   *
     **************************************************************************/


    // +-----------------------------------------------------------------------+
    // | No hardware acceleration                                              |
    // +-----------------------------------------------------------------------+

    // zeta_i[k] * X_i[k] / | X_i[k] |

    tmpSqrt = sqrtf(myMicST->xpower[0]);

    myMicST->weightedResultReal[0] = myMicST->zeta[0] * myMicST->xfreqReal[0] / (tmpSqrt + 1E-20f);
    myMicST->weightedResultImag[0] = myMicST->zeta[0] * myMicST->xfreqImag[0] / (tmpSqrt + 1E-20f);


    tmpSqrt = sqrtf(myMicST->xpower[myMicST->MICST_HALFFRAMESIZE]);

    myMicST->weightedResultReal[myMicST->MICST_HALFFRAMESIZE] = myMicST->zeta[myMicST->MICST_HALFFRAMESIZE] * myMicST->xfreqReal[myMicST->MICST_HALFFRAMESIZE] / (tmpSqrt + 1E-20f);
    myMicST->weightedResultImag[myMicST->MICST_HALFFRAMESIZE] = myMicST->zeta[myMicST->MICST_HALFFRAMESIZE] * myMicST->xfreqImag[myMicST->MICST_HALFFRAMESIZE] / (tmpSqrt + 1E-20f);

    for (k = 1; k < myMicST->MICST_HALFFRAMESIZE; k++)
    {
        tmpSqrt = sqrtf(myMicST->xpower[k]);

        myMicST->weightedResultReal[k] = myMicST->zeta[k] * myMicST->xfreqReal[k] / (tmpSqrt + 1E-20f);
        myMicST->weightedResultImag[k] = myMicST->zeta[k] * myMicST->xfreqImag[k] / (tmpSqrt + 1E-20f);

        myMicST->weightedResultReal[myMicST->MICST_FRAMESIZE - k] = myMicST->weightedResultReal[k];
        myMicST->weightedResultImag[myMicST->MICST_FRAMESIZE - k] = -1.0f * myMicST->weightedResultImag[k];

    }


    /***************************************************************************
     * Step 7: Update parameters                                               *
     **************************************************************************/

    for (k = 0; k < myMicST->MICST_FRAMESIZE; k++)
    {
        myMicST->lambda_prev[k] = myMicST->lambda[k];
        myMicST->xpower_prev[k] = myMicST->xpower[k];
        myMicST->zeta_prev[k] = myMicST->zeta[k];
    }

}
