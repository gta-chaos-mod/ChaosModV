#pragma once

namespace Hooks
{
	void SetAudioPitchFromSpeedMult(float speedMult);
	void SetAudioPitch(float pitch);
	float GetTargetAudioPitch();
	void ResetAudioPitch();

	void SetAudioLPFCutoff(short value);
	void ResetAudioLPFCutoff();

	void SetAudioHPFCutoff(short value);
	void ResetAudioHPFCutoff();

	void SetAudioVolume(float value);
	void ResetAudioVolume();
}