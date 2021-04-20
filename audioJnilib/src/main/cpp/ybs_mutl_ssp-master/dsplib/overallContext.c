
#include "overallContext.h"

/*******************************************************************************
 * createEmptyOverallContext                                                   *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      (none)                                                         *
 *                                                                             *
 * Outputs:     (objOverall)    Structure with all the allocated objects for   *
 *                              processing                                     *
 *                                                                             *
 * Description: This function creates a structure with all the objects that    *
 *              need to be used to perform operations in the library.          *
 *                                                                             *
 ******************************************************************************/

struct objOverall createEmptyOverallContext()
{

    struct objOverall tmp;

    tmp.myMicrophones = (struct objMicrophones*) malloc(sizeof(struct objMicrophones));
    tmp.myPreprocessor = (struct objPreprocessor*) malloc(sizeof(struct objPreprocessor));
    tmp.myBeamformer = (struct objBeamformer*) malloc(sizeof(struct objBeamformer));
    tmp.myMixture = (struct objMixture*) malloc(sizeof(struct objMixture));
    tmp.myGSS = (struct objGSS*) malloc(sizeof(struct objGSS));
    tmp.myPostfilter = (struct objPostfilter*) malloc(sizeof(struct objPostfilter));
    tmp.myPostprocessorSeparated = (struct objPostprocessor*) malloc(sizeof(struct objPostprocessor));
    tmp.myPostprocessorPostfiltered = (struct objPostprocessor*) malloc(sizeof(struct objPostprocessor));

    tmp.myPotentialSources = (struct objPotentialSources*) malloc(sizeof(struct objPotentialSources));
    tmp.myTrackedSources = (struct objTrackedSources*) malloc(sizeof(struct objTrackedSources));
    tmp.mySeparatedSources = (struct objSeparatedSources*) malloc(sizeof(struct objSeparatedSources));
    tmp.myPostfilteredSources = (struct objPostfilteredSources*) malloc(sizeof(struct objPostfilteredSources));

    tmp.myOutputSeparated = (struct objOutput*) malloc(sizeof(struct objOutput));
    tmp.myOutputPostfiltered = (struct objOutput*) malloc(sizeof(struct objOutput));

    tmp.myOutputChunkSeparated = (struct objOutputChunk*) malloc(sizeof(struct objOutputChunk));
    tmp.myOutputChunkPostfiltered = (struct objOutputChunk*) malloc(sizeof(struct objOutputChunk));

    tmp.myParameters = (struct ParametersStruct*) malloc(sizeof(struct ParametersStruct));

    return tmp;

}

/*******************************************************************************
 * deleteOverallContext                                                        *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      myContext       The context to be deleted                      *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function frees the memory used by the objects.            *
 *                                                                             *
 ******************************************************************************/

void deleteOverallContext(struct objOverall myContext)
{

    free((void*) myContext.myMicrophones);
    free((void*) myContext.myPreprocessor);
    free((void*) myContext.myBeamformer);
    free((void*) myContext.myMixture);
    free((void*) myContext.myGSS);
    free((void*) myContext.myPostfilter);
    free((void*) myContext.myPostprocessorSeparated);
    free((void*) myContext.myPostprocessorPostfiltered);

    free((void*) myContext.myPotentialSources);
    free((void*) myContext.myTrackedSources);
    free((void*) myContext.mySeparatedSources);
    free((void*) myContext.myPostfilteredSources);

    free((void*) myContext.myOutputSeparated);
    free((void*) myContext.myOutputPostfiltered);

    free((void*) myContext.myOutputChunkSeparated);
    free((void*) myContext.myOutputChunkPostfiltered);

    free((void*) myContext.myParameters);

}



void setup_microphone_positions_and_gains(struct ParametersStruct* parametersStruct, struct objMicrophones* myMicrophones)
{

    // Set the number of microphones
    microphonesInit(myMicrophones, 6);

    // Add microphone 1...
    microphonesAdd(myMicrophones,
        0,
        parametersStruct->P_GEO_MICS_MIC1_X,
        parametersStruct->P_GEO_MICS_MIC1_Y,
        parametersStruct->P_GEO_MICS_MIC1_Z,
        parametersStruct->P_GEO_MICS_MIC1_GAIN
    );

    // Add microphone 2...
    microphonesAdd(myMicrophones,
        1,
        parametersStruct->P_GEO_MICS_MIC2_X,
        parametersStruct->P_GEO_MICS_MIC2_Y,
        parametersStruct->P_GEO_MICS_MIC2_Z,
        parametersStruct->P_GEO_MICS_MIC2_GAIN
    );

    // Add microphone 3...
    microphonesAdd(myMicrophones,
        2,
        parametersStruct->P_GEO_MICS_MIC3_X,
        parametersStruct->P_GEO_MICS_MIC3_Y,
        parametersStruct->P_GEO_MICS_MIC3_Z,
        parametersStruct->P_GEO_MICS_MIC3_GAIN
    );

    // Add microphone 4...
    microphonesAdd(myMicrophones,
        3,
        parametersStruct->P_GEO_MICS_MIC4_X,
        parametersStruct->P_GEO_MICS_MIC4_Y,
        parametersStruct->P_GEO_MICS_MIC4_Z,
        parametersStruct->P_GEO_MICS_MIC4_GAIN
    );

    // Add microphone 5...
    microphonesAdd(myMicrophones,
        4,
        parametersStruct->P_GEO_MICS_MIC5_X,
        parametersStruct->P_GEO_MICS_MIC5_Y,
        parametersStruct->P_GEO_MICS_MIC5_Z,
        parametersStruct->P_GEO_MICS_MIC5_GAIN
    );

    // Add microphone 6...
    microphonesAdd(myMicrophones,
        5,
        parametersStruct->P_GEO_MICS_MIC6_X,
        parametersStruct->P_GEO_MICS_MIC6_Y,
        parametersStruct->P_GEO_MICS_MIC6_Z,
        parametersStruct->P_GEO_MICS_MIC6_GAIN
    );


}





void init_fun(struct objOverall* workspace)
{
    ParametersLoadDefault(workspace->myParameters);

    microphonesInit(workspace->myMicrophones, NB_MICROPHONES);

    setup_microphone_positions_and_gains(workspace->myParameters, workspace->myMicrophones);

    // Initialize the preprocessor
    preprocessorInit(workspace->myPreprocessor, workspace->myParameters, workspace->myMicrophones);

    // Initialize the beamformer
    beamformerInit(workspace->myBeamformer, workspace->myParameters, workspace->myMicrophones);

    // Initialize the mixture
    mixtureInit(workspace->myMixture, workspace->myParameters);

    // Initialize the gss
    gssInit(workspace->myGSS, workspace->myParameters, workspace->myMicrophones);

    // Initialize the postfilter
    postfilterInit(workspace->myPostfilter, workspace->myParameters);

    // Initialize the postprocessor
    postprocessorInit(workspace->myPostprocessorSeparated, workspace->myParameters);

    // Initialize the potential sources
    potentialSourcesInit(workspace->myPotentialSources, workspace->myParameters);

    // Initialize the tracked sources
    trackedSourcesInit(workspace->myTrackedSources, workspace->myParameters);

    // Initialize the separated sources
    separatedSourcesInit(workspace->mySeparatedSources, workspace->myParameters);


    outputInit(workspace->myOutputSeparated, workspace->myParameters, NULL, NULL, NULL, '*');

}




void process_fun(struct objOverall* workspace , short* audio_raw_data , signed short* currentSample)
{

    float audio_float_data[NB_MICROPHONES][SAMPLES_PER_FRAME];
    unsigned char channel = 0;
    int frame_index = 0;


    for (channel = 0; channel < NB_MICROPHONES; channel++)
    {
        for (frame_index = 0; frame_index < SAMPLES_PER_FRAME; frame_index++)
        {
            audio_float_data[channel][frame_index] = ((float)audio_raw_data[channel + (NB_MICROPHONES * frame_index)]) / 32768.0;
        }

        // Copy frames to the beamformer frames, will do 50% overlap internally
        preprocessorPushFrames(workspace->myPreprocessor, SAMPLES_PER_FRAME, channel);
        preprocessorAddFrame(workspace->myPreprocessor, &audio_float_data[channel][0], channel, SAMPLES_PER_FRAME);
    }



    //#2 Preprocess
    preprocessorProcessFrame(workspace->myPreprocessor);

    //#3 Find potential sources from the beamformer
    beamformerFindMaxima(workspace->myBeamformer, workspace->myPreprocessor, workspace->myPotentialSources);

    //#4 Track Sources
    mixtureUpdate(workspace->myMixture, workspace->myTrackedSources, workspace->myPotentialSources);

    //#5 Separate sources
    gssProcess(workspace->myGSS, workspace->myPreprocessor, workspace->myTrackedSources, workspace->mySeparatedSources);


    //#6 Postprocess
    postprocessorProcessFrameSeparated(workspace->myPostprocessorSeparated, workspace->myTrackedSources, workspace->mySeparatedSources);


    outputProcess(workspace->myOutputSeparated, workspace->myPostprocessorSeparated, currentSample);


}

