////////////////
// stru_sprite.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_SPRITE_H
#define STRU_SPRITE_H
#include "cast_particle.h"
namespace imm
{
////////////////
// sprite
////////////////
////////////////
struct sprite
{
	sprite();
	~sprite();
	void init();
	particle element;
	int instance_ix;
};
//
sprite::sprite():
	instance_ix(-1)
{
	;
}
//
void sprite::init()
{
	;
}
//
}
#endif