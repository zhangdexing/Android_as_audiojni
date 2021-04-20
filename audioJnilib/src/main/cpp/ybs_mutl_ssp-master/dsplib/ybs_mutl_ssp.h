#include "aecm.h"
#include "vad.h"
#include "overallContext.h"
#include "noise_suppression.h"
#include "aecm.h"


#define SAMPLES_PER_FRAME 160
#define NB_MICROPHONES_pro1 8
#define NB_MICROPHONES_pro2 6
#define RAW_BUFFER_SIZE1 (SAMPLES_PER_FRAME * NB_MICROPHONES_pro1)
#define RAW_BUFFER_SIZE2 (SAMPLES_PER_FRAME * NB_MICROPHONES_pro2)
#define data_accept1 1280
#define data_accept2 960

struct  Workspace_data_buffer
{
    int16_t audio_raw_data[1280];
    int16_t audio_float_data[8][160];
    int16_t buffer_out[960];
    int16_t buffer_near[160];
    int16_t buffer_far[160];
    int16_t buffer_output[160];
    int16_t buffer[160];
    int16_t buffer_final_out[160];
    int16_t buffer_cache[70][160];

};

struct  Workspace_aec_ssp
{
    int16_t echoMode;
    int16_t sampleRate;
    int16_t msInSndCardBuf0;
    int16_t msInSndCardBuf1;
    int16_t msInSndCardBuf2;
    int16_t msInSndCardBuf3;
    int16_t msInSndCardBuf4;
    int16_t msInSndCardBuf5;
    void* aecmInst0;
    void* aecmInst1;
    void* aecmInst2;
    void* aecmInst3;
    void* aecmInst4;
    void* aecmInst5;
    int status0;
    int status1;
    int status2;
    int status3;
    int status4;
    int status5;

};




struct  Workspace_ans_ssp
{
    NsHandle* nsHandle;
    int status_denoise;

};

struct  Workspace_vad_ssp
{
    int flag;
    int vad_smooth_lowtop;
    int vad_smooth_toplow;
    int nVadRet;
    int cache;
    int vad_ernag;
    uint32_t energy;
    int tot_rshifts;
    int u;
    int v;
    void* vadInst;
    int status_vad;
};

typedef struct
{
    struct Workspace_data_buffer* data_buffer;
	struct Workspace_aec_ssp* aec_ssp;
	struct Workspace_ans_ssp* ans_ssp;
	struct Workspace_vad_ssp* vad_ssp;

}Workspace_single_ssp;

Workspace_single_ssp ybs_createEmptyOverallContext();
void parameter_int(Workspace_single_ssp* ybs__ssp);
void data_buffer_int(struct Workspace_data_buffer* o);
void aec_int(struct Workspace_aec_ssp* o);
void ans_int(struct Workspace_ans_ssp* o);
void vad_int(struct Workspace_vad_ssp* o);
void ybs_aec_process(struct Workspace_aec_ssp* o, struct  Workspace_data_buffer* p);
void function_free(Workspace_single_ssp* ybs_ssp, struct objOverall* workspace);