
#include "fft.h"

/*******************************************************************************
 * Compatibility issues                                                        *
 ******************************************************************************/

// In some environment, the constant PI is not define with the math.h library
#ifndef M_PI
	#define M_PI	3.1415926535897932384626433832795f
#endif

/*******************************************************************************
 * fftInit                                                                     *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object to be initialized           *
 *                                                                             *
 * Outputs:     myFFT               The initialized FFT object                 *
 *                                                                             *
 * Description: This function initializes the FFT object.                      *
 *                                                                             *
 ******************************************************************************/

void fftInit(struct objFFT* myFFT, struct ParametersStruct* myParameters, unsigned int size)
{

    // Temporary variable
    unsigned int tmpFrameSize;

    // Temporary variable
    unsigned int tmpNumberLevels;

    // Define the index to generate Wn(r)
    unsigned int r;

    // Define the index to generate the reverse bit order array
    unsigned int indexRevBitOrder;

    // Define the index to generate the empty array
    unsigned int emptyIndex;

    // Define the INDEX of the input parameter a
    unsigned int a;
    // Define the INDEX of the input parameter b
    unsigned int b;

    // Define accumulator to compute the index of parameters a and b
    unsigned int accumulatorA;
    // Define accumulator to compute the index of parameter r
    unsigned int accumulatorR;

    // Define the nubmer of groups in the current level
    unsigned int numberGroups;
    // Define the number of points per group
    unsigned int numberSubGroups;

    // Define the index of the level
    unsigned int indexLevel;
    // Define the index of the group
    unsigned int indexGroup;
    // Define the index of the point inside the group
    unsigned int indexSubGroup;

    // Define the index of the twiddle-factor in memory
    unsigned int indexTwiddle;

    // Define the index of the simd array for a with groups
    unsigned int simdAIndexGroup;
    // Define the index of the simd array for b with groups
    unsigned int simdBIndexGroup;
    // Define the index of the simd array for r with groups
    unsigned int simdRIndexGroup;
    // Define the index of the simd array for a with individual elements
    unsigned int simdAIndexIndividual;
    // Define the index of the simd array for b with individual elements
    unsigned int simdBIndexIndividual;

    // *************************************************************************
    // * STEP 1: Load parameters                                               *
    // *************************************************************************

    myFFT->FFT_SIZE = size;

    tmpFrameSize = myFFT->FFT_SIZE;
    tmpNumberLevels = 0;

    while(tmpFrameSize > 1)
    {
        tmpNumberLevels++;
        tmpFrameSize /= 2;
    }

    myFFT->FFT_NBLEVELS = tmpNumberLevels;
    myFFT->FFT_HALFSIZE = myFFT->FFT_SIZE / 2;
    myFFT->FFT_SIZE_INV = (1.0f / myFFT->FFT_SIZE);
    myFFT->FFT_SIMD_GROUP = ((myFFT->FFT_SIZE/2) * (myFFT->FFT_NBLEVELS-2) / 4);
    myFFT->FFT_SIMD_INDIVIDUAL = ((myFFT->FFT_SIZE/2) * 2);

    // *************************************************************************
    // * STEP 2: Initialize context                                            *
    // *************************************************************************

        // +-------------------------------------------------------------------+
        // | Step A: Create arrays                                             |
        // +-------------------------------------------------------------------+

        myFFT->WnReal = (float*) newTable1D(myFFT->FFT_HALFSIZE, sizeof(float));
        myFFT->WnImag = (float*) newTable1D(myFFT->FFT_HALFSIZE, sizeof(float));
        myFFT->simdWnReal = (float*) newTable1D(myFFT->FFT_SIMD_GROUP*4, sizeof(float));
        myFFT->simdWnImag = (float*) newTable1D(myFFT->FFT_SIMD_GROUP*4, sizeof(float));
        myFFT->workingArrayReal = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->workingArrayImag = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->fftTwiceReal = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->fftTwiceRealFlipped = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->fftTwiceImag = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->fftTwiceImagFlipped = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->emptyArray = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->trashArray = (float*) newTable1D(myFFT->FFT_SIZE, sizeof(float));
        myFFT->revBitOrderArray = (unsigned int*) newTable1D(myFFT->FFT_SIZE, sizeof(unsigned int));
        myFFT->simdARealGroups = (float**) newTable1D(myFFT->FFT_SIMD_GROUP, sizeof(float*));
        myFFT->simdAImagGroups = (float**) newTable1D(myFFT->FFT_SIMD_GROUP, sizeof(float*));
        myFFT->simdBRealGroups = (float**) newTable1D(myFFT->FFT_SIMD_GROUP, sizeof(float*));
        myFFT->simdBImagGroups = (float**) newTable1D(myFFT->FFT_SIMD_GROUP, sizeof(float*));
        myFFT->simdRRealGroups = (float**) newTable1D(myFFT->FFT_SIMD_GROUP, sizeof(float*));
        myFFT->simdRImagGroups = (float**) newTable1D(myFFT->FFT_SIMD_GROUP, sizeof(float*));
        myFFT->simdAIndividual = (float**) newTable1D(myFFT->FFT_SIMD_INDIVIDUAL, sizeof(float*));
        myFFT->simdBIndividual = (float**) newTable1D(myFFT->FFT_SIMD_INDIVIDUAL, sizeof(float*));

        // +-------------------------------------------------------------------+
        // | Step B: Generate the FFT factors Wn(r)                            |
        // +-------------------------------------------------------------------+

        // Generate Wn(r) = exp(-j*2*pi*r/N) for r = 0 ... (N/2 - 1)
        for (r = 0; r < myFFT->FFT_HALFSIZE; r++)
        {

            myFFT->WnReal[r] = cosf(2.0f * M_PI * r / myFFT->FFT_SIZE);
            myFFT->WnImag[r] = -1.0f * sinf(2.0f * M_PI * r / myFFT->FFT_SIZE);

        }

        // +-------------------------------------------------------------------+
        // | Step C: Generate an array with reverse-bit indexes                |
        // +-------------------------------------------------------------------+

        // Generate an array of reverse bit order
        for (indexRevBitOrder = 0; indexRevBitOrder < myFFT->FFT_SIZE; indexRevBitOrder++)
        {

            myFFT->revBitOrderArray[indexRevBitOrder] = (indexRevBitOrder & 0x0001) << 15;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0002) << 13;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0004) << 11;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0008) << 9;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0010) << 7;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0020) << 5;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0040) << 3;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0080) << 1;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0100) >> 1;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0200) >> 3;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0400) >> 5;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x0800) >> 7;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x1000) >> 9;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x2000) >> 11;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x4000) >> 13;
            myFFT->revBitOrderArray[indexRevBitOrder] += (indexRevBitOrder & 0x8000) >> 15;

            myFFT->revBitOrderArray[indexRevBitOrder] >>= (16 - myFFT->FFT_NBLEVELS);

        }

        // +-------------------------------------------------------------------+
        // | Step D: Generate an empty array                                   |
        // +-------------------------------------------------------------------+

        // Generate an empty array (will be used as a dummy array for the imaginary
        // part when the FFT of a single real signal is computed)
        for (emptyIndex = 0; emptyIndex < myFFT->FFT_SIZE; emptyIndex++)
        {
            myFFT->emptyArray[emptyIndex] = 0;
        }

        // +-------------------------------------------------------------------+
        // | Step E: SIMD: Compute the indexes used for memory accesses        |
        // +-------------------------------------------------------------------+

        // Load parameters
        numberGroups = 1;
        numberSubGroups = myFFT->FFT_HALFSIZE;

        // Initialize pointers
        simdAIndexGroup = 0;
        simdBIndexGroup = 0;
        simdRIndexGroup = 0;
        simdAIndexIndividual = 0;
        simdBIndexIndividual = 0;
        indexTwiddle = 0;

        // Loop for each level
        for (indexLevel = 0; indexLevel < myFFT->FFT_NBLEVELS; indexLevel++)
        {

            accumulatorA = 0;
            accumulatorR = 0;

            // Loop for each group in the current level
            for (indexGroup = 0; indexGroup < numberGroups; indexGroup++)
            {

                // Loop for each element of the group
                for (indexSubGroup = 0; indexSubGroup < numberSubGroups; indexSubGroup++)
                {

                    // Calculate the indexes
                    a = accumulatorA;
                    b = accumulatorA + numberSubGroups;
                    r = accumulatorR;
                    accumulatorA++;
                    accumulatorR += numberGroups;

                    // Check if there are groups of at least 4 elements
                    if (numberSubGroups >= 4)
                    {

                        // Copy corresponding twiddle factor
                        myFFT->simdWnReal[indexTwiddle] = myFFT->WnReal[r];
                        myFFT->simdWnImag[indexTwiddle] = myFFT->WnImag[r];
                        indexTwiddle++;

                        // Check if a is a multiple of 4
                        if ((a / 4.0f) == floorf(a/4.0f))
                        {

                            myFFT->simdARealGroups[simdAIndexGroup] = &myFFT->workingArrayReal[a];
                            myFFT->simdAImagGroups[simdAIndexGroup] = &myFFT->workingArrayImag[a];
                            myFFT->simdBRealGroups[simdBIndexGroup] = &myFFT->workingArrayReal[b];
                            myFFT->simdBImagGroups[simdBIndexGroup] = &myFFT->workingArrayImag[b];
                            myFFT->simdRRealGroups[simdRIndexGroup] = &myFFT->simdWnReal[indexTwiddle - 1];
                            myFFT->simdRImagGroups[simdRIndexGroup] = &myFFT->simdWnImag[indexTwiddle - 1];

                            simdAIndexGroup++;
                            simdBIndexGroup++;
                            simdRIndexGroup++;

                        }

                    }
                    else
                    {

                        myFFT->simdAIndividual[simdAIndexIndividual++] = &myFFT->workingArrayReal[a];
                        myFFT->simdBIndividual[simdBIndexIndividual++] = &myFFT->workingArrayReal[b];

                    }

                }

                // Update accumulators
                accumulatorA += numberSubGroups;
                accumulatorR = 0;

            }

            // Divide the number of points by group by 2 for the next level
            numberSubGroups >>= 1;
            // Multiply the number of groups by 2 for the next level
            numberGroups <<= 1;

        }

}

/*******************************************************************************
 * fftTerminate                                                                *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object to be terminated            *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function terminates the FFT object.                       *
 *                                                                             *
 ******************************************************************************/

void fftTerminate(struct objFFT* myFFT)
{

    // Free memory
    deleteTable1D((void*) myFFT->WnReal);
    deleteTable1D((void*) myFFT->WnImag);
    deleteTable1D((void*) myFFT->simdWnReal);
    deleteTable1D((void*) myFFT->simdWnImag);
    deleteTable1D((void*) myFFT->workingArrayReal);
    deleteTable1D((void*) myFFT->workingArrayImag);
    deleteTable1D((void*) myFFT->fftTwiceReal);
    deleteTable1D((void*) myFFT->fftTwiceRealFlipped);
    deleteTable1D((void*) myFFT->fftTwiceImag);
    deleteTable1D((void*) myFFT->fftTwiceImagFlipped);
    deleteTable1D((void*) myFFT->emptyArray);
    deleteTable1D((void*) myFFT->trashArray);
    deleteTable1D((void*) myFFT->revBitOrderArray);
    deleteTable1D((void*) myFFT->simdARealGroups);
    deleteTable1D((void*) myFFT->simdAImagGroups);
    deleteTable1D((void*) myFFT->simdBRealGroups);
    deleteTable1D((void*) myFFT->simdBImagGroups);
    deleteTable1D((void*) myFFT->simdRRealGroups);
    deleteTable1D((void*) myFFT->simdRImagGroups);
    deleteTable1D((void*) myFFT->simdAIndividual);
    deleteTable1D((void*) myFFT->simdBIndividual);

}

/*******************************************************************************
 * fftCompute                                                                  *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object used for this computation   *
 *              sourceArrayReal     Pointer to the first element of the array  *
 *                                  that contains the real part of the signal  *
 *                                  to be transformed with the FFT             *
 *              sourceArrayImag     Pointer to the first element of the array  *
 *                                  that contains the imaginary part of the    *
 *                                  signal to be transformed with the FFT      *
 *                                                                             *
 * Outputs:     destArrayReal       Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the FFT                          *
 *              destArrayImag       Pointer to the first element of the array  *
 *                                  that will receive the imaginary part of    *
 *                                  the result of the FFT                      *
 *                                                                             *
 *                                                                             *
 * Description: This function performs the FFT computation using the           *
 *              radix-2 decimation-in-frequency FFT algorithm                  *
 *                                                                             *
 ******************************************************************************/

void fftCompute(struct objFFT* myFFT, float* sourceArrayReal, float* sourceArrayImag, float* destArrayReal, float* destArrayImag)
{


    unsigned int indexArray;
/*
    // The basic structure in this algorithm is a butterfly as follows:
    //
    // a o---o-->--o--->---o A = a + b
    //        \   /
    //         \ /
    //          X
    //         / \
    //        /   \  Wn(r)
    // b o---o-->--o--->---o B = (a - b) Wn(r)
    //         -1
*/
    // Define the INDEX of the input parameter a
    unsigned int a;
    // Define the INDEX of the input parameter b
    unsigned int b;
    // Define the parameter r
    unsigned int r;

    // Define the real part of the input parameter a
    float valueAReal;
    // Define the imaginary part of the input parameter a
    float valueAImag;
    // Define the real part of the input parameter b
    float valueBReal;
    // Define the imaginary part of the input parameter b
    float valueBImag;
    // Define the real part of the output parameter A
    float newValueAReal;
    // Define the imaginary part of the output parameter A
    float newValueAImag;
    // Define the real part of the output parameter B
    float newValueBReal;
    // Define the imaginary part of the output parameter B
    float newValueBImag;

    // Define accumulator to compute the index of parameters a and b
    unsigned int accumulatorA;
    // Define accumulator to compute the index of parameter r
    unsigned int accumulatorR;

    // Define the nubmer of groups in the current level
    unsigned int numberGroups;
    // Define the number of points per group
    unsigned int numberSubGroups;

    // Define the index of the level
    unsigned int indexLevel;
    // Define the index of the group
    unsigned int indexGroup;
    // Define the index of the point inside the group
    unsigned int indexSubGroup;

    // Define the index to generate the reverse bit order array
    unsigned int indexRevBitOrder;

    // Temporary variables to speed up butterfly computation
    float diffABReal;
    float diffABImag;
    float curWnReal;
    float curWnImag;

    // Load parameters
    numberGroups = 1;
    numberSubGroups = myFFT->FFT_HALFSIZE;

    // Copy all elements from the source array in the working array
    for (indexArray = 0; indexArray < myFFT->FFT_SIZE; indexArray++)
    {
        myFFT->workingArrayReal[indexArray] = sourceArrayReal[indexArray];
        myFFT->workingArrayImag[indexArray] = sourceArrayImag[indexArray];
    }

    // Loop for each level
    for (indexLevel = 0; indexLevel < myFFT->FFT_NBLEVELS; indexLevel++)
    {

        accumulatorA = 0;
        accumulatorR = 0;

        // Loop for each group in the current level
        for (indexGroup = 0; indexGroup < numberGroups; indexGroup++)
        {

            // Loop for each element of the group
            for (indexSubGroup = 0; indexSubGroup < numberSubGroups; indexSubGroup++)
            {

                // Calculate the indexes
                a = accumulatorA;
                b = accumulatorA + numberSubGroups;
                r = accumulatorR;
                accumulatorA++;
                accumulatorR += numberGroups;

                // Load the values a and b (these are complex values)
                valueAReal = myFFT->workingArrayReal[a];
                valueAImag = myFFT->workingArrayImag[a];
                valueBReal = myFFT->workingArrayReal[b];
                valueBImag = myFFT->workingArrayImag[b];

                // Apply A = a + b from the butterfly
                newValueAReal = valueAReal + valueBReal;
                newValueAImag = valueAImag + valueBImag;

                // Apply B = (a - b) * Wn(r) from the butterfly
                diffABReal = valueAReal - valueBReal;
                diffABImag = valueAImag - valueBImag;
                curWnReal = myFFT->WnReal[r];
                curWnImag = myFFT->WnImag[r];
                newValueBReal = diffABReal * curWnReal - diffABImag * curWnImag;
                newValueBImag = diffABReal * curWnImag + diffABImag * curWnReal;

                // Save results at the same place as the initial values
                myFFT->workingArrayReal[a] = newValueAReal;
                myFFT->workingArrayImag[a] = newValueAImag;
                myFFT->workingArrayReal[b] = newValueBReal;
                myFFT->workingArrayImag[b] = newValueBImag;

            }

            // Update accumulators
            accumulatorA += numberSubGroups;
            accumulatorR = 0;

        }

        // Divide the number of points by group by 2 for the next level
        numberSubGroups >>= 1;
        // Multiply the number of groups by 2 for the next level
        numberGroups <<= 1;

    }

    // Reorder result (it is actually in reverse bit order) and copy to destination array
    for (indexRevBitOrder = 0; indexRevBitOrder < myFFT->FFT_SIZE; indexRevBitOrder++)
    {

        destArrayReal[indexRevBitOrder] = myFFT->workingArrayReal[myFFT->revBitOrderArray[indexRevBitOrder]];
        destArrayImag[indexRevBitOrder] = myFFT->workingArrayImag[myFFT->revBitOrderArray[indexRevBitOrder]];

    }

}

/*******************************************************************************
 * fftComputeOnce                                                              *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object used for this computation   *
 *              sourceArray         Pointer to the first element of the array  *
 *                                  that contains the signal to be transformed *
 *                                  with the FFT (signal is real)              *
 *                                                                             *
 * Outputs:     destArrayReal       Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the FFT of the signal            *
 *              destArrayImag       Pointer to the first element of the array  *
 *                                  that will receive the imaginary part of    *
 *                                  the result of the FFT of the signal        *
 *                                                                             *
 * Description: This function computes the FFT for a real signal               *
 *                                                                             *
 ******************************************************************************/

void fftComputeOnce(struct objFFT* myFFT, float *sourceArray, float *destArrayReal, float *destArrayImag)
{
    fftCompute(myFFT, sourceArray, myFFT->emptyArray, destArrayReal, destArrayImag);
}

/*******************************************************************************
 * fftComputeTwice                                                             *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object used for this computation   *
 *              sourceArray1        Pointer to the first element of the array  *
 *                                  that contains the signal to be transformed *
 *                                  with the FFT (signal is real)              *
 *              sourceArray2        Pointer to the first element of the array  *
 *                                  that contains the signal to be transformed *
 *                                  with the FFT (signal is real)              *
 *                                                                             *
 * Outputs:     destArray1Real      Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the FFT of the first signal      *
 *              destArray1Imag      Pointer to the first element of the array  *
 *                                  that will receive the imaginary part of    *
 *                                  the result of the FFT of the first signal  *
 *              destArray2Real      Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the FFT of the second signal     *
 *              destArray2Imag      Pointer to the first element of the array  *
 *                                  that will receive the imaginary part of    *
 *                                  the result of the FFT of the second signal *
 *                                                                             *
 * Description: This function performs two FFT computations using the          *
 *              radix-2 decimation-in-frequency FFT algorithm                  *
 *                                                                             *
 ******************************************************************************/

void fftComputeTwice(struct objFFT* myFFT, float *sourceArray1, float *sourceArray2, float *destArray1Real, float *destArray1Imag, float *destArray2Real, float *destArray2Imag)
{

    // Index to loop throught the array
    unsigned int k;

    // Compute the FFT
    fftCompute(myFFT, sourceArray1, sourceArray2, myFFT->fftTwiceReal, myFFT->fftTwiceImag);

    // Real part of the first FFT = (twiceReal + twiceReal flipped) / 2
    // Imag part of the first FFT = (twiceImag - twiceImag flipped) / 2
    // Real part of the second FFT = (twiceImag + twiceImag flipped) / 2
    // Imag part of the second FFT = (twiceReal flipped - twiceReal) / 2

     // Except for DC that stays the same
    destArray1Real[0] = myFFT->fftTwiceReal[0];
    destArray1Imag[0] = 0;
    destArray2Real[0] = myFFT->fftTwiceImag[0];
    destArray2Imag[0] = 0;

    for (k = 1; k < myFFT->FFT_SIZE; k++)
    {
        destArray1Real[k] = (myFFT->fftTwiceReal[k] + myFFT->fftTwiceReal[myFFT->FFT_SIZE - k]) / 2;
        destArray1Imag[k] = (myFFT->fftTwiceImag[k] - myFFT->fftTwiceImag[myFFT->FFT_SIZE - k]) / 2;
        destArray2Real[k] = (myFFT->fftTwiceImag[k] + myFFT->fftTwiceImag[myFFT->FFT_SIZE - k]) / 2;
        destArray2Imag[k] = (myFFT->fftTwiceReal[myFFT->FFT_SIZE - k] - myFFT->fftTwiceReal[k]) / 2;
    }

}

/*******************************************************************************
 * ifftCompute                                                                 *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object used for this computation   *
 *              sourceArrayReal     Pointer to the first element of the array  *
 *                                  that contains the real part of the signal  *
 *                                  to be transformed with the inverse FFT     *
 *              sourceArrayImag     Pointer to the first element of the array  *
 *                                  that contains the imaginary part of the    *
 *                                  signal to be transformed with the inverse  *
 *                                  FFT                                        *
 *                                                                             *
 * Outputs:     destArrayReal       Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the inverse FFT                  *
 *              destArrayImag       Pointer to the first element of the array  *
 *                                  that will receive the imaginary part of    *
 *                                  the result of the inverse FFT              *
 *                                                                             *
 *                                                                             *
 * Description: This function performs the inverse FFT computation using the   *
 *              radix-2 decimation-in-frequency FFT algorithm with some        *
 *		preprocessing and postprocessing:                              *
 *                                                                             *
 *              X1[k] for k = 0 , 1 , ... , N is the signal in freq domain     *
 *                                                                             *
 *              Preprocessing:	X2[k] = X1[k]* for k = 0 , 1 , ... , N         *
 *              Processing:     X3[k] = FFT(X1[k]) for k = 0 , 1 , ... , N     *
 *              Postprocessing:	X4[k] = (1/N) * X3[k]* for k = 0 , 1 , ... , N *
 *                                                                             *
 *              X4[k] for k = 0 , 1 , ... , N is the result of the inverse     *
 *              Fourier transform                                              *
 *                                                                             *
 ******************************************************************************/

void ifftCompute(struct objFFT* myFFT, float *sourceArrayReal, float *sourceArrayImag, float *destArrayReal, float *destArrayImag)
{



    // Index to go through each element
    unsigned int k;

    // Apply prefiltering
    for (k = 0; k < myFFT->FFT_SIZE; k++)
    {
        destArrayImag[k] = -1.0f * sourceArrayImag[k];
    }

    // Compute FFT
    fftCompute(myFFT, sourceArrayReal, destArrayImag, destArrayReal, destArrayImag);

    // Apply post filtering
    for (k = 0; k < myFFT->FFT_SIZE; k++)
    {
        destArrayReal[k] = myFFT->FFT_SIZE_INV * destArrayReal[k];
        destArrayImag[k] = -1.0f * myFFT->FFT_SIZE_INV * destArrayImag[k];
    }


}

/*******************************************************************************
 * ifftComputeOnce                                                             *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object used for this computation   *
 *              sourceArray1Real    Pointer to the first element of the array  *
 *                                  that contains the real part of the signal  *
 *                                  to be transformed with the inverse FFT     *
 *                                  (signal is even)                           *
 *              sourceArray1Imag    Pointer to the first element of the array  *
 *                                  that contains the imaginary part of the    *
 *                                  signal to be transformed with the inverse  *
 *                                  FFT (signal is even)                       *
 *                                                                             *
 * Outputs:     destArray1Real      Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the FFT of the signal            *
 *                                                                             *
 * Description: This function computes the inverse FFT for a real signal       *
 *                                                                             *
 ******************************************************************************/

void ifftComputeOnce(struct objFFT* myFFT, float *sourceArray1Real, float *sourceArray1Imag, float *destArray1)
{
    ifftCompute(myFFT, sourceArray1Real, sourceArray1Imag, destArray1, myFFT->trashArray);
}

/*******************************************************************************
 * ifftComputeTwice                                                            *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myFFT               The FFT object used for this computation   *
 *              sourceArray1Real    Pointer to the first element of the array  *
 *                                  that contains the real part of the signal  *
 *                                  to be transformed with the inverse FFT     *
 *                                  (signal is even)                           *
 *              sourceArray1Imag    Pointer to the first element of the array  *
 *                                  that contains the imaginary part of the    *
 *                                  signal to be transformed with the inverse  *
 *                                  FFT (signal is even)                       *
 *              sourceArray2Real    Pointer to the first element of the array  *
 *                                  that contains the real part of the signal  *
 *                                  to be transformed with the inverse FFT     *
 *                                  (signal is even)                           *
 *              sourceArray2Imag    Pointer to the first element of the array  *
 *                                  that contains the imaginary part of the    *
 *                                  signal to be transformed with the inverse  *
 *                                  FFT (signal is even)                       *
 *                                                                             *
 * Outputs:     destArray1Real      Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the FFT of the first signal      *
 *              destArray2Real      Pointer to the first element of the array  *
 *                                  that will receive the real part of the     *
 *                                  result of the FFT of the second signal     *
 *                                                                             *
 * Description: This function performs two inverse FFT computations using the  *
 *              radix-2 decimation-in-frequency FFT algorithm.                 *
 *                                                                             *
 ******************************************************************************/

void ifftComputeTwice(struct objFFT* myFFT, float *sourceArray1Real, float *sourceArray1Imag, float *sourceArray2Real, float *sourceArray2Imag, float *destArray1, float *destArray2)
{

    // Index to loop throught the array
    unsigned int k;

    // Real part of the FFT = firstReal - secondImag
    // Real part of the flipped FFT = firstReal + secondImag
    // Imaginary part of the FFT = firstImag + secondReal
    // Imaginary part of the FFT = secondReal - firstImag

    // Use destArray1 to store the real part of the FFT to be inversed
    // Use destArray2 to store the imaginary part of the FFT to be inversed

    // Re {Y_0} = Re {X1_0}
    // Im {Y_0} = Re {X2_0}
    destArray1[0] = sourceArray1Real[0];
    destArray2[0] = sourceArray2Real[0];

    // Re {Y_(N/2)} = Re{X1_(N/2)}
    // Im {Y_(N/2)} = Re{X2_(N/2)}
    destArray1[myFFT->FFT_HALFSIZE] = sourceArray1Real[myFFT->FFT_HALFSIZE];
    destArray2[myFFT->FFT_HALFSIZE] = sourceArray2Real[myFFT->FFT_HALFSIZE];

    // Re {Y_k} = Re{X1_k} - Im{X2_k} for k = 1 ... (N/2) - 1
    // Im {Y_k} = Im{X1_k} + Re{X2_k} for k = 1 ... (N/2) - 1
    // Re (Y_-k} = Re{X1_k} + Im{X2_k} for k = 1 ... (N/2) - 1
    // Im {Y_-k} = Re{X2_k} - Im{X1_k} for k = 1 ... (N/2) - 1
    for (k = 1; k < myFFT->FFT_HALFSIZE; k++)
    {
        destArray1[k] = sourceArray1Real[k] - sourceArray2Imag[k];
        destArray2[k] = sourceArray1Imag[k] + sourceArray2Real[k];
        destArray1[myFFT->FFT_SIZE - k] = sourceArray1Real[k] + sourceArray2Imag[k];
        destArray2[myFFT->FFT_SIZE - k] = sourceArray2Real[k] - sourceArray1Imag[k];
    }

    // Compute IFFT
    ifftCompute(myFFT, &destArray1[0], &destArray2[0], &destArray1[0], &destArray2[0]);

}

