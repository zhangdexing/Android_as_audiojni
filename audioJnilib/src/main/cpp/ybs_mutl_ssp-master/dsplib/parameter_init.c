#include "ybs_mutl_ssp.h"

Workspace_single_ssp ybs_createEmptyOverallContext()
{
	Workspace_single_ssp tmp;

	tmp.data_buffer = (struct Workspace_data_buffer*)malloc(sizeof(struct Workspace_data_buffer));
	tmp.aec_ssp = (struct Workspace_aec_ssp*)malloc(sizeof(struct Workspace_aec_ssp));
	tmp.ans_ssp = (struct Workspace_ans_ssp*)malloc(sizeof(struct Workspace_ans_ssp));
	tmp.vad_ssp = (struct Workspace_vad_ssp*)malloc(sizeof(struct Workspace_vad_ssp));
	return tmp;
}


void data_buffer_int(struct Workspace_data_buffer* o)
{
	memset(o->audio_raw_data, 0x00, sizeof(o->audio_raw_data));
	memset(o->audio_float_data, 0x00, sizeof(o->audio_float_data));
	memset(o->buffer_out, 0x00, sizeof(o->buffer_out));
	memset(o->buffer_near, 0x00, sizeof(o->buffer_near));
	memset(o->buffer_far, 0x00, sizeof(o->buffer_far));
	memset(o->buffer_output, 0x00, sizeof(o->buffer_output));
	memset(o->buffer, 0x00, sizeof(o->buffer));
	memset(o->buffer_final_out, 0x00, sizeof(o->buffer_final_out));
	memset(o->buffer_cache, 0x00, sizeof(o->buffer_cache));
}


void aec_int(struct Workspace_aec_ssp* o)
{
	AecmConfig config;
	config.cngMode = AecmTrue;
	config.echoMode = 3;
	size_t samples = 160;
	
	o->aecmInst0 = WebRtcAecm_Create();
	o->aecmInst1 = WebRtcAecm_Create();
	o->aecmInst2 = WebRtcAecm_Create();
	o->aecmInst3 = WebRtcAecm_Create();
	o->aecmInst4 = WebRtcAecm_Create();
	o->aecmInst5 = WebRtcAecm_Create();

	o->status0 = WebRtcAecm_Init(o->aecmInst0, 16000);
	o->status1 = WebRtcAecm_Init(o->aecmInst1, 16000);
	o->status2 = WebRtcAecm_Init(o->aecmInst2, 16000);
	o->status3 = WebRtcAecm_Init(o->aecmInst3, 16000);
	o->status4 = WebRtcAecm_Init(o->aecmInst4, 16000);
	o->status5 = WebRtcAecm_Init(o->aecmInst5, 16000);

	o->msInSndCardBuf0 = 2;
	o->msInSndCardBuf1 = 1;
	o->msInSndCardBuf2 = 1;
	o->msInSndCardBuf3 = 1;
	o->msInSndCardBuf4 = 1;
	o->msInSndCardBuf5 = 1;

}

void ans_int(struct Workspace_ans_ssp* o)
{
	
	o->nsHandle = WebRtcNs_Create();
	o->status_denoise = WebRtcNs_Init(o->nsHandle, 16000);
	o->status_denoise = WebRtcNs_set_policy(o->nsHandle, 3);

}



void vad_int(struct Workspace_vad_ssp* o)
{

	o->flag = 0;
	o->vad_smooth_lowtop = 0;
	o->vad_smooth_toplow = 0;
	o->nVadRet;
	o->cache = 70;
	o->vad_ernag = 0;
	o->energy = 0;
	o->tot_rshifts = 0;
	o->u = 0;
	o->v = 0;
	o->vadInst = WebRtcVad_Create();
	o->status_vad = WebRtcVad_Init(o->vadInst);
	o->status_vad = WebRtcVad_set_mode(o->vadInst, 2);

}


void parameter_int(Workspace_single_ssp* ybs_ssp)
{
	data_buffer_int(ybs_ssp->data_buffer);
	aec_int(ybs_ssp->aec_ssp);
	ans_int(ybs_ssp->ans_ssp);
	vad_int(ybs_ssp->vad_ssp);

}


void function_free(Workspace_single_ssp *ybs_ssp, struct objOverall *workspace)
{

	preprocessorTerminate(workspace->myPreprocessor);
	beamformerTerminate(workspace->myBeamformer);
	mixtureTerminate(workspace->myMixture);
	gssTerminate(workspace->myGSS);
	postfilterTerminate(workspace->myPostfilter);
	postprocessorTerminate(workspace->myPostprocessorSeparated);
	potentialSourcesTerminate(workspace->myPotentialSources);
	trackedSourcesTerminate(workspace->myTrackedSources);
	separatedSourcesTerminate(workspace->mySeparatedSources);
	// outputTerminate(workspace.myOutputSeparated);
	microphonesTerminate(workspace->myMicrophones);

	WebRtcAecm_Free(ybs_ssp->aec_ssp->aecmInst0);
	WebRtcAecm_Free(ybs_ssp->aec_ssp->aecmInst1);
	WebRtcAecm_Free(ybs_ssp->aec_ssp->aecmInst2);
	WebRtcAecm_Free(ybs_ssp->aec_ssp->aecmInst3);
	WebRtcAecm_Free(ybs_ssp->aec_ssp->aecmInst4);
	WebRtcAecm_Free(ybs_ssp->aec_ssp->aecmInst5);

	WebRtcNs_Free(ybs_ssp->ans_ssp->nsHandle);
	WebRtcVad_Free(ybs_ssp->vad_ssp->vadInst);



}