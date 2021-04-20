
SET(ManyEarsLib_SRCS
    Geometry/microphones.c
    Preprocessing/mcra.c
    Preprocessing/micst.c
    Preprocessing/preprocessor.c
    Localisation/beamformer.c
    Localisation/delays.c
    Localisation/potentialSources.c
    Localisation/rij.c
    Localisation/sphere.c
    Tracking/filter.c
    Tracking/mixture.c
    Tracking/trackedSources.c
    Separation/gss.c
    Separation/postfilter.c
    Separation/postfilteredSources.c
    Separation/separatedSources.c
    Output/output.c
    Output/outputChunk.c
    Output/outputSingleStream.c
    Postprocessing/postprocessor.c
    Utilities/linearCorrelation.c
    Utilities/dynamicMemory.c
    Utilities/fft.c
    Utilities/idList.c
    Utilities/idManager.c
    Utilities/matrix.c
    Utilities/randGen.c
    Utilities/transcendental.c
    Utilities/window.c
    Utilities/mathadvanced.c
    parameters.c
    overallContext.c
	aecm.c
	parameter_init.c
	output.c
	noise_suppression.c
	vad.c
)

SET(ManyEarsLib_HEADERS
    Geometry/microphones.h
    Preprocessing/mcra.h
    Preprocessing/micst.h
    Preprocessing/preprocessor.h
    Localisation/beamformer.h
    Localisation/delays.h
    Localisation/potentialSources.h
    Localisation/rij.h
    Localisation/sphere.h
    Tracking/filter.h
    Tracking/mixture.h
    Tracking/trackedSources.h
    Separation/gss.h
    Separation/postfilter.h
    Separation/postfilteredSources.h
    Separation/separatedSources.h
    Output/output.h
    Output/outputChunk.h
    Output/outputSingleStream.h
    Postprocessing/postprocessor.h
    Utilities/linearCorrelation.h
    Utilities/dynamicMemory.h
    Utilities/fft.h
    Utilities/idList.h
    Utilities/idManager.h
    Utilities/mathadvanced.h
    Utilities/matrix.h
    Utilities/randGen.h  
    Utilities/transcendental.h
    Utilities/window.h
    parameters.h
    overallContext.h
    hardware.h
	ybs_mutl_ssp.h
	aecm.h
	noise_suppression.h
	vad.h

)

#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/dsplib )
INCLUDE_DIRECTORIES(.)


add_library(ManyEarsLib STATIC ${ManyEarsLib_SRCS} ${ManyEarsLib_HEADERS} )
target_link_libraries(ManyEarsLib )

INSTALL(TARGETS ManyEarsLib DESTINATION lib)



