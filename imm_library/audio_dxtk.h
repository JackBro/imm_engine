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
#include <dbt.h>
////////////////
// audio_dxtk
////////////////
////////////////
namespace imm
{
struct audio_dxtk
{
	audio_dxtk();
	void play_loop();
	std::unique_ptr<AudioEngine> aud_engine;
	std::unique_ptr<SoundEffect> sound_effect;
};
//
audio_dxtk::audio_dxtk():
	aud_engine(nullptr),
	sound_effect(nullptr)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	aud_engine = std::unique_ptr<AudioEngine>(new AudioEngine(eflags));
	play_loop();
}
//
void audio_dxtk::play_loop()
{
	std::wstring path_med(GLOBAL["path_med"].begin(), GLOBAL["path_med"].end());
	std::wstring path_play = path_med+L"windows_notify.wav";
	//sound_effect = std::unique_ptr<SoundEffect>(new SoundEffect(aud_engine.get(), path_play.c_str()));
	//auto effect = sound_effect->CreateInstance();
	//effect->Play(true);
	//sound_effect->Play();
}
//
}
#endif