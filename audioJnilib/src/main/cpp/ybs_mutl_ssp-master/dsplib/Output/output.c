
#include "Output/output.h"

/*******************************************************************************
 * outputInit                                                                  *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myOutput        Object to be initialized                       *
 *              myParameters    List of parameters that is used to configure   *
 *                              the object.                                    *
 *              myPathTracking  The model path for creating tracking files. It *
 *                              is set to NULL if no file needs to be created. *
 *              myPathSeparation                                               *
 *                              The model path for creating separated files.   *
 *                              It is set to NULL if no file needs to be       *
 *                              created.                                       *
 *              myPathSeparationWave                                           *
 *                              The model path for created wave files. It is   *
 *                              set to NULL if no file needs to be created.    *
 *              wildcardChar    The wildcard character                         *
 *                                                                             *
 * Outputs:     myOutput        Initialized object                             *
 *                                                                             *
 * Description: This function initializes the output object and allocates      *
 *              memory that will be used later.                                *
 *                                                                             *
 ******************************************************************************/

void outputInit(struct objOutput* myOutput, struct ParametersStruct* myParameters, char* myPathTracking, char* myPathSeparation, char* myPathSeparationWave, char wildcardChar)
{

    unsigned int lenPathTracking;
    unsigned int lenPathSeparation;
    unsigned int lenPathSeparationWave;

    unsigned int indexSource;


    // *************************************************************************
    // * STEP 1: Load parameters                                               *
    // *************************************************************************

    myOutput->OUT_NBSOURCES = myParameters->P_GEN_DYNSOURCES;
    myOutput->OUT_HOPSIZE = (unsigned int) (GLOBAL_FRAMESIZE * GLOBAL_OVERLAP);
    myOutput->OUT_SAMPLERATE = GLOBAL_FS;
    myOutput->OUT_GAIN = myParameters->P_OUT_GAIN;
    myOutput->OUT_WAVEHEADERSIZE = myParameters->P_OUT_WAVEHEADERSIZE;

    // *************************************************************************
    // * STEP 2: Save strings                                                  *
    // *************************************************************************


        myOutput->deleteSeparated = 1;

    // *************************************************************************
    // * STEP 5: Allocate IDs                                                  *
    // *************************************************************************

    myOutput->listID = (signed int*) newTable1D(myOutput->OUT_NBSOURCES, sizeof(signed int));

    for (indexSource = 0; indexSource < myOutput->OUT_NBSOURCES; indexSource++)
    {
        myOutput->listID[indexSource] = OUTPUT_NOSOURCE;
    }

    // *************************************************************************
    // * STEP 6: Save wildcard character                                        *
    // *************************************************************************

    myOutput->wildcardChar = wildcardChar;

    // *************************************************************************
    // * STEP 7: Allocate temporary array                                      *
    // *************************************************************************

    myOutput->tmpArray = (float*) newTable1D(myOutput->OUT_HOPSIZE, sizeof(float));

    // *************************************************************************
    // * STEP 8: Allocate string for wave file header                          *
    // *************************************************************************

    myOutput->waveHeader = (char*) newTable1D(myOutput->OUT_WAVEHEADERSIZE, sizeof(char));

}

/*******************************************************************************
 * outputTerminate                                                             *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myOutput            The output object to be terminated         *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function frees the dynamic memory that was allocated in   *
 *              the initialization of this object                              *
 *                                                                             *
 ******************************************************************************/

void outputTerminate(struct objOutput* myOutput)
{

    unsigned int indexSource;

    // *************************************************************************
    // * STEP 1: Close files that correspond to remaining IDs                  *
    // *************************************************************************

    for (indexSource = 0; indexSource < myOutput->OUT_NBSOURCES; indexSource++)
    {
        if (myOutput->listID[indexSource] != OUTPUT_NOSOURCE)
        {

            if (myOutput->listPointersTracking[indexSource] != NULL)
            {
                fclose(myOutput->listPointersTracking[indexSource]);
            }
            if (myOutput->listPointersSeparation[indexSource] != NULL)
            {
                fclose(myOutput->listPointersSeparation[indexSource]);
            }
            if (myOutput->pathSeparationWave != NULL)
            {
                outputGenerateWave(myOutput, myOutput->listID[indexSource]);

                if (myOutput->deleteSeparated == 1)
                {
                    remove(myOutput->listFilenameSeparation[indexSource]);
                }

            }

            myOutput->listID[indexSource] = OUTPUT_NOSOURCE;

        }
    }

}

/*******************************************************************************
 * outputProcess                                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myOutput            The output object to be used               *
 *              myPostprocessor     The postprocessor object which contains    *
 *                                  the data to be written to files.           *
 *                                                                             *
 * Outputs:     (none but files will be written)                               *
 *                                                                             *
 * Description: This function writes the data in files.                        *
 *                                                                             *
 ******************************************************************************/

void outputProcess(struct objOutput* myOutput, struct objPostprocessor* myPostprocessor  , signed short* currentSample)
{

    unsigned int indexSource;
    unsigned int indexSource2;
    unsigned int indexSample;

   // signed short currentSample[myOutput->OUT_HOPSIZE];

    char exists = 0;

    // *************************************************************************
    // * STEP 1: Close files that correspond to IDs that no longer exist       *
    // *************************************************************************

    for (indexSource = 0; indexSource < myOutput->OUT_NBSOURCES; indexSource++)
    {
        if (myOutput->listID[indexSource] != OUTPUT_NOSOURCE)
        {

            exists = 0;

            for (indexSource2 = 0; indexSource2 < myPostprocessor->PP_NSOURCES; indexSource2++)
            {
                if (myOutput->listID[indexSource] == myPostprocessor->sourcesID[indexSource2])
                {
                    exists = 1;
                    break;
                }
            }

            if (exists == 0)
            {

                myOutput->listID[indexSource] = OUTPUT_NOSOURCE;
            }
        }
    }

    // *************************************************************************
    // * STEP 2: Open files that correspond to new IDs                         *
    // *************************************************************************

    for (indexSource = 0; indexSource < myPostprocessor->PP_NSOURCES; indexSource++)
    {
        if (myPostprocessor->sourcesID[indexSource] != GSS_NOSOURCE)
        {

            exists = 0;

            for (indexSource2 = 0; indexSource2 < myOutput->OUT_NBSOURCES; indexSource2++)
            {
                if (myPostprocessor->sourcesID[indexSource] == myOutput->listID[indexSource2])
                {
                    exists = 1;
                    break;
                }
            }

            if (exists == 0)
            {

                for (indexSource2 = 0; indexSource2 < myOutput->OUT_NBSOURCES; indexSource2++)
                {
                    if (myOutput->listID[indexSource2] == OUTPUT_NOSOURCE)
                    {

                        myOutput->listID[indexSource2] = myPostprocessor->sourcesID[indexSource];

                        break;

                    }
                }


            }
        }
    }

    // *************************************************************************
    // * STEP 3: Write in files that correspond to active IDs                  *
    // *************************************************************************

    for (indexSource = 0; indexSource < myOutput->OUT_NBSOURCES; indexSource++)
    {
        if (myOutput->listID[indexSource] != OUTPUT_NOSOURCE)
        {
            postprocessorExtractHop(myPostprocessor, myOutput->listID[indexSource], myOutput->tmpArray);
        
                for (indexSample = 0; indexSample < myOutput->OUT_HOPSIZE; indexSample++)
                {
                    currentSample[indexSample] = (signed short) floor((myOutput->tmpArray[indexSample] * myOutput->OUT_GAIN) * 32768.0 + 0.5);
                   
                }

        }
    }

}

/*******************************************************************************
 * outputGeneratePath                                                          *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myOutput            The output object to be used               *
 *              model               The string used as a model path            *
 *              id                  The id of the current source               *
 *                                                                             *
 * Outputs:     result              The string which contains the generated    *
 *                                  path                                       *
 *                                                                             *
 * Description: This generates a path related to the provided ID.              *
 *                                                                             *
 ******************************************************************************/

void outputGeneratePath(struct objOutput* myOutput, const char* model, char* result, ID_TYPE id)
{

    signed int indexChar;
    unsigned int lenModel;

    char modelChar;
    char newChar;
    signed int idAcc;
    signed int idRem;

    lenModel = strlen(model);
    idAcc = id;

    for (indexChar = lenModel - 1; indexChar >= 0; indexChar--)
    {
        modelChar = model[indexChar];

        // If this is a wildcar character, then remplace
        if (modelChar == myOutput->wildcardChar)
        {
            // Get last digit
            idRem = idAcc - ((idAcc / 10) * 10);
            idAcc /= 10;

            // In ASCII, 0 = 48, 1 = 49, 2 = 50, ...
            newChar = ((char) (idRem) + 48);            
        }
        else
        {
            newChar = modelChar;
        }

        // Remplace
        result[indexChar] = newChar;

    }

    // Set NULL character at the end of new string
    result[lenModel] = 0x0;

}

/*******************************************************************************
 * outputGenerateWave                                                          *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myOutput            The output object to be used               *
 *              id                  The id of the current source               *
 *                                                                             *
 * Outputs:     (none but wave file is created)                                *
 *                                                                             *
 * Description: This creates a WAV file based on the output of the id.         *
 *                                                                             *
 ******************************************************************************/

void outputGenerateWave(struct objOutput* myOutput, ID_TYPE id)
{

    FILE* ptrData;
    unsigned int indexData;
    char tmpChar;
    signed short tmpSample;

    FILE* ptrWave;

    unsigned int sizeSample;
    unsigned int chunkSize;
    unsigned int subChunk1Size;
    unsigned int subChunk2Size;
    unsigned short nChannels;
    unsigned int sampleRate;
    unsigned short audioFormat;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;    
    /*****************************************/


    // *************************************************************************
    // * STEP 1: Open data to check the length                                 *
    // *************************************************************************

    outputGeneratePath(myOutput, myOutput->pathSeparation, myOutput->filenameSeparation, id);
    ptrData = fopen(myOutput->filenameSeparation, "rb");

    indexData = 0;
    while(!feof(ptrData))
    {
        fread(&tmpChar,sizeof(char),1,ptrData);
        indexData++;
    }

    indexData--;

    fclose(ptrData);

    // *************************************************************************
    // * STEP 1: Compute parameters                                            *
    // *************************************************************************

    // Size of each sample in bytes
    sizeSample = sizeof(tmpSample);

    // Only one channel
    nChannels = 1;

    // Sample rate
    sampleRate = myOutput->OUT_SAMPLERATE;

    // subChunk1Size = 16 for PCM
    subChunk1Size = 16;

    // subChunk2Size = nSamples * nChannels * sizeof(signed int)
    subChunk2Size = indexData * nChannels * sizeSample;

    // chunkSize = 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
    chunkSize = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);

    // Audio format = 1 for PCM
    audioFormat = 1;

    // Byte rate = SampleRate * nChannels * sizeof(signed int)
    byteRate = sampleRate * nChannels * sizeSample;

    // Block align = nChannels * sizeof(signed int)
    blockAlign = nChannels * sizeSample;

    // Bits per sample = 8 * sizeof(signed int)
    bitsPerSample = 8 * sizeSample;

    // *************************************************************************
    // * STEP 2: Write header                                                  *
    // *************************************************************************

    outputGeneratePath(myOutput, myOutput->pathSeparationWave, myOutput->filenameSeparationWave, id);
    ptrWave = fopen(myOutput->filenameSeparationWave, "wb");
    ptrData = fopen(myOutput->filenameSeparation, "rb");

    // +-----------------------------------------------------------------------+
    // | Step A: Write the RIFF chunk descriptor                               |
    // +-----------------------------------------------------------------------+

        // +-------------------------------------------------------------------+
        // | Step i: ChunkID                                                   |
        // +-------------------------------------------------------------------+

        // Characters "RIFF"

        tmpChar = 'R';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'I';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'F';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'F';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step ii: ChunkSize                                                |
        // +-------------------------------------------------------------------+

        // Write chunk size with least significant bytes first

        fwrite(&chunkSize, 1, sizeof(chunkSize), ptrWave);


        // +-------------------------------------------------------------------+
        // | Step iii: Format                                                  |
        // +-------------------------------------------------------------------+

        // Characters WAVE

        tmpChar = 'W';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'A';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'V';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'E';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);

    // +-----------------------------------------------------------------------+
    // | Step B: Write the fmt sub-chunk                                       |
    // +-----------------------------------------------------------------------+

        // +-------------------------------------------------------------------+
        // | Step i: Subchunk1ID                                               |
        // +-------------------------------------------------------------------+

        // Characters "fmt "

        tmpChar = 'f';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'm';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 't';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = ' ';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step ii: Subchunk1Size                                            |
        // +-------------------------------------------------------------------+

        // Write sub chunk size with least significant bytes first
        fwrite(&subChunk1Size, 1, sizeof(subChunk1Size), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step iii: AudioFormat                                             |
        // +-------------------------------------------------------------------+

        // Write audioformat with least significant bytes first
        fwrite(&audioFormat, 1, sizeof(audioFormat), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step iv: NumberChannels                                           |
        // +-------------------------------------------------------------------+

        // Write numberchannels with least significant bytes first
        fwrite(&nChannels, 1, sizeof(nChannels), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step v: SampleRate                                                |
        // +-------------------------------------------------------------------+

        // Write sampleRate with least significant bytes first
        fwrite(&sampleRate, 1, sizeof(sampleRate), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step vi: ByteRate                                                 |
        // +-------------------------------------------------------------------+

        // Write byteRate with least significant bytes first
        fwrite(&byteRate, 1, sizeof(byteRate), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step vii: BlockAlign                                              |
        // +-------------------------------------------------------------------+

        // Write blockAlign with least significant bytes first
        fwrite(&blockAlign, 1, sizeof(blockAlign), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step viii: BitsPerSample                                          |
        // +-------------------------------------------------------------------+

        // Write bitsPerSample with least significant bytes first
        fwrite(&bitsPerSample, 1, sizeof(bitsPerSample), ptrWave);

    // +-----------------------------------------------------------------------+
    // | Step C: Write the data sub-chunk                                      |
    // +-----------------------------------------------------------------------+

        // +-------------------------------------------------------------------+
        // | Step i: Subchunk2ID                                               |
        // +-------------------------------------------------------------------+

        // Characters "data"

        tmpChar = 'd';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'a';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 't';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);
        tmpChar = 'a';
        fwrite(&tmpChar, 1, sizeof(tmpChar), ptrWave);

        // +-------------------------------------------------------------------+
        // | Step ii: Subchunk2Size                                            |
        // +-------------------------------------------------------------------+

        // Write sub chunk size with least significant bytes first
        fwrite(&subChunk2Size, 1, sizeof(subChunk2Size), ptrWave);

    // *************************************************************************
    // * STEP 3: Write data                                                    *
    // *************************************************************************

    while(!feof(ptrData))
    {
        fread(&tmpSample, 1, sizeof(tmpSample), ptrData);
        fwrite(&tmpSample, 1, sizeof(tmpSample), ptrWave);
    }

    fclose(ptrData);
    fclose(ptrWave);
}
