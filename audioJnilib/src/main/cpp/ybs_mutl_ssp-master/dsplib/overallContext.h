
#ifndef OVERALLCONTEXT_H
#define OVERALLCONTEXT_H

#define SAMPLES_PER_FRAME 160
#define NB_MICROPHONES 6
#define RAW_BUFFER_SIZE (SAMPLES_PER_FRAME * NB_MICROPHONES)

// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif

#include "hardware.h"
#include "parameters.h"
#include "Geometry/microphones.h"
#include "Preprocessing/preprocessor.h"
#include "Localisation/beamformer.h"
#include "Localisation/potentialSources.h"
#include "Tracking/mixture.h"
#include "Tracking/trackedSources.h"
#include "Separation/gss.h"
#include "Separation/separatedSources.h"
#include "Separation/postfilter.h"
#include "Separation/postfilteredSources.h"
#include "Postprocessing/postprocessor.h"
#include "Output/output.h"
#include "Output/outputChunk.h"


/*******************************************************************************
 * Structures                                                                  *
 ******************************************************************************/

    struct objOverall
    {

        // +-------------------------------------------------------------------+
        // | Objects                                                           |
        // +-------------------------------------------------------------------+

        struct ParametersStruct* myParameters;

        struct objMicrophones* myMicrophones;
        struct objPreprocessor* myPreprocessor;
        struct objBeamformer* myBeamformer;
        struct objMixture* myMixture;
        struct objGSS* myGSS;
        struct objPostfilter* myPostfilter;
        struct objPostprocessor* myPostprocessorSeparated;
        struct objPostprocessor* myPostprocessorPostfiltered;

        struct objPotentialSources* myPotentialSources;
        struct objTrackedSources* myTrackedSources;
        struct objSeparatedSources* mySeparatedSources;
        struct objPostfilteredSources* myPostfilteredSources;

        struct objOutput* myOutputSeparated;
        struct objOutput* myOutputPostfiltered;

        struct objOutputChunk* myOutputChunkSeparated;
        struct objOutputChunk* myOutputChunkPostfiltered;

    };

/*******************************************************************************
 * Prototypes                                                                  *
 ******************************************************************************/

    struct objOverall createEmptyOverallContext();

    void deleteOverallContext(struct objOverall myContext);

// =============================================================================


#ifdef __cplusplus
}
#endif

#endif


void init_fun(struct objOverall* workspace);
//void process_fun(struct objOverall* workspace, short* audio_raw_data);
void process_fun(struct objOverall* workspace, short* audio_raw_data, signed short* currentSample);

void output_fun(int flag, int cache, int16_t* buffer, int16_t* buffer_out, int16_t(buffer_cache)[][160]);