////////////////
// audio_dxtk.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef AUDIO_DXTK_H
#define AUDIO_DXTK_H
#include <Audio.h>
////////////////
// audio_dxtk
////////////////
////////////////
namespace imm
{
struct audio_dxtk
{
	audio_dxtk();
	void init();
	void play_loop();
	void play_shot();
	void set_effect_inst_volume(float volume);
	void set_wave_bank_volume(float volume);
	float wave_bank_volume;
	std::unique_ptr<AudioEngine> aud_engine;
	std::unique_ptr<SoundEffect> sound_effect;
	std::unique_ptr<SoundEffectInstance> effect_inst;
	std::unique_ptr<WaveBank> wave_bank;
};
//
audio_dxtk::audio_dxtk():
	wave_bank_volume(1.0f),
	aud_engine(nullptr),
	sound_effect(nullptr),
	effect_inst(nullptr),
	wave_bank(nullptr)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	aud_engine = std::unique_ptr<AudioEngine>(new AudioEngine(eflags));
}
//
void audio_dxtk::init()
{
	std::wstring path_med(GLOBAL["path_med"].begin(), GLOBAL["path_med"].end());
	std::wstring path_play = path_med+L"dragon_fight.wav";
	sound_effect = std::unique_ptr<SoundEffect>(new SoundEffect(aud_engine.get(), path_play.c_str()));
	effect_inst = sound_effect->CreateInstance();
	std::wstring path_wb = path_med+L"punches.xwb";
	wave_bank = std::unique_ptr<WaveBank>(new WaveBank(aud_engine.get(), path_wb.c_str()));
}
//
void audio_dxtk::set_effect_inst_volume(float volume)
{
	effect_inst->SetVolume(volume);
}
//
void audio_dxtk::set_wave_bank_volume(float volume)
{
	wave_bank_volume = volume;
}
//
void audio_dxtk::play_loop()
{
	effect_inst->Play(true);
}
//
void audio_dxtk::play_shot()
{
	wave_bank->Play(0, wave_bank_volume, 0.0f, 0.0f);
}
//
}
#endif