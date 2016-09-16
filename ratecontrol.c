/* rate control */
#include "ratecontrol.h"

#define FRAMERATE_INCR		1001
#define SMALL_EPS 1e-10


int get_framerate(float fFrameRate)
{
    int fincr,fbase,i;

	if ((fFrameRate - (int)fFrameRate) < SMALL_EPS) {
		fincr = 1;
		fbase = (int)fFrameRate;
	}
	else {
		fincr = FRAMERATE_INCR;
		fbase = (int)(FRAMERATE_INCR * fFrameRate);
	}
	if (fincr <= 0 || fbase <= 0) {
		fincr = 1;
		fbase = 25;
	}
	i = fincr; //i = pParam->fincr;
	while (i > 1) {
		if (fincr % i == 0 && fbase % i == 0) {
			fincr /= i;
			fbase /= i;
			i = fincr;
			continue;
		}
		i--;
	}
	if (fbase > 65535) {
		float div = (float) fbase / 65535;
		fbase = (int) (fbase / div);
		fincr = (int) (fincr / div);
	}
	if (fbase > 65535) {
		float div = (float) fbase / 65535;
		fbase = (int) (fbase / div);
		fincr = (int) (fincr / div);
	}   
	return (fbase*1000/fincr);
}


void RateControlInit(RateControl * rate_control,
				unsigned int target_rate,
				unsigned int reaction_delay_factor,
				unsigned int averaging_period,
				unsigned int buffer,
				int framerate,
				int max_quant,
				int min_quant,
				unsigned int initq)
{
	int i;
	rate_control->frames = 0;
	rate_control->total_size = 0;
	rate_control->framerate = (double) framerate / 1000.0;
	rate_control->target_rate = (int) target_rate;
//	rate_control->rtn_quant = get_initial_quant(target_rate);
	rate_control->rtn_quant = initq;
	rate_control->max_quant = (short) max_quant;
	rate_control->min_quant = (short) min_quant;
	for (i = 0; i < 32; ++i) {
		rate_control->quant_error[i] = 0.0;
	}
	rate_control->target_framesize =
		(double) target_rate / 8.0 / rate_control->framerate;
	rate_control->sequence_quality = 2.0 / (double) rate_control->rtn_quant;
	rate_control->avg_framesize = rate_control->target_framesize;
	rate_control->reaction_delay_factor = (int) reaction_delay_factor;
	rate_control->averaging_period = (int) averaging_period;
	rate_control->buffer = (int) buffer;
}


void RateControlUpdate(RateControl *rate_control,
				  short quant,
				  int frame_size,
				  int keyframe)
{
	long long deviation;
	double overflow, averaging_period, reaction_delay_factor;
	double quality_scale, base_quality, target_quality;
	int rtn_quant;

	rate_control->frames++;
	rate_control->total_size += frame_size;

	deviation =
		(long long) ((double) rate_control->total_size -
				   ((double) ((double) rate_control->target_rate / 8.0 /
					 (double) rate_control->framerate) * (double) rate_control->frames));
	if ((rate_control->rtn_quant <=2) && (deviation < (-rate_control->target_framesize * 80)))
		rate_control->total_size += (rate_control->target_framesize - frame_size);
	if (rate_control->rtn_quant >= 2) {
		averaging_period = (double) rate_control->averaging_period;
		rate_control->sequence_quality -=
			rate_control->sequence_quality / averaging_period;
		rate_control->sequence_quality +=
			2.0 / (double) rate_control->rtn_quant / averaging_period;
		if (rate_control->sequence_quality < 0.1)
			rate_control->sequence_quality = 0.1;

		if (!keyframe) {
			reaction_delay_factor =
				(double) rate_control->reaction_delay_factor;
			rate_control->avg_framesize -=
				rate_control->avg_framesize / reaction_delay_factor;
			rate_control->avg_framesize += frame_size / reaction_delay_factor;
		}
	}

	quality_scale =
		rate_control->target_framesize / rate_control->avg_framesize *
		rate_control->target_framesize / rate_control->avg_framesize;

	base_quality = rate_control->sequence_quality;
	if (quality_scale >= 1.0) {
		base_quality = 1.0 - (1.0 - base_quality) / quality_scale;
	} else {
		base_quality = 0.06452 + (base_quality - 0.06452) * quality_scale;
	}

	overflow = -((double) deviation / (double) rate_control->buffer);

	target_quality =
		base_quality + (base_quality -
						0.06452) * overflow / rate_control->target_framesize;

	if (target_quality > 2.0)
		target_quality = 2.0;
	else if (target_quality < 0.06452)
		target_quality = 0.06452;

	rtn_quant = (int) (2.0 / target_quality);

	if (rtn_quant < 31) {
		rate_control->quant_error[rtn_quant] +=
			2.0 / target_quality - rtn_quant;
		if (rate_control->quant_error[rtn_quant] >= 1.0) {
			rate_control->quant_error[rtn_quant] -= 1.0;
			rtn_quant++;
		}
	}

	if (rtn_quant > rate_control->rtn_quant + 1)
		rtn_quant = rate_control->rtn_quant + 1;
	else if (rtn_quant < rate_control->rtn_quant - 1)
		rtn_quant = rate_control->rtn_quant - 1;

	if (rtn_quant > rate_control->max_quant)
		rtn_quant = rate_control->max_quant;
	else if (rtn_quant < rate_control->min_quant)
		rtn_quant = rate_control->min_quant;
	rate_control->rtn_quant = rtn_quant;
}
