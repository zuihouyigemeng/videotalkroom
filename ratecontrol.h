#ifndef _RATE_CONRTROL_
#define _RATE_CONRTROL_
typedef struct
{
	int rtn_quant;
	long long frames;
	long long total_size;
	double framerate;
	int target_rate;
	short max_quant;
	short min_quant;
	long long last_change;
	long long quant_sum;
	double quant_error[32];
	double avg_framesize;
	double target_framesize;
	double sequence_quality;
	int averaging_period;
	int reaction_delay_factor;
	int buffer;
}
RateControl;

#define RC_AVERAGE_PERIOD   100
#define DELAY_FACTOR        16
#define ARG_RC_BUFFER       100

int get_framerate(float fFrameRate);
void
RateControlInit(RateControl * rate_control,
				unsigned int target_rate,
				unsigned int reaction_delay_factor,
				unsigned int averaging_period,
				unsigned int buffer,
				int framerate,
				int max_quant,
				int min_quant,
				unsigned int initq);
				
void RateControlUpdate(RateControl *rate_control,
				  short quant,
				  int frame_size,
				  int keyframe);
#endif