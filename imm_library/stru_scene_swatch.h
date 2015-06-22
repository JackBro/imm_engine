////////////////
// stru_scene_swatch.h
// This file is a portion of the immature engine.
// It is distributed under the BSD license.
// Copyright 2015 Huang Yiting (http://endrollex.com)
////////////////
////////////////
#ifndef STRU_SCENE_SWATCH_H
#define STRU_SCENE_SWATCH_H
#include "imm_core.h"
namespace imm
{
////////////////
// model_material_common
// common 3D materials parameters
////////////////
////////////////
void model_material_common(material &mat, const float &mat_refraction, const int &mat_property)
{
	// http://www.nicoptere.net/dump/materials.html
	mat.reflect = XMFLOAT4(mat_refraction, mat_refraction, mat_refraction, 1.0f);
	switch(mat_property) {
		case 1: //Brass
			mat.ambient     = XMFLOAT4(0.329412f, 0.223529f, 0.027451f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.780392f, 0.568627f, 0.113725f, 1.0f);
			mat.specular    = XMFLOAT4(0.992157f, 0.941176f, 0.807843f, 27.8974f);
			break;
		case 2: //Bronze
			mat.ambient     = XMFLOAT4(0.2125f, 0.1275f, 0.054f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.714f, 0.4284f, 0.18144f, 1.0f);
			mat.specular    = XMFLOAT4(0.393548f, 0.271906f, 0.166721f, 25.6f);
			break;
		case 3: //Polished Bronze
			mat.ambient     = XMFLOAT4(0.25f, 0.148f, 0.06475f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.4f, 0.2368f, 0.1036f, 1.0f);
			mat.specular    = XMFLOAT4(0.774597f, 0.458561f, 0.200621f, 76.8f);
			break;
		case 4: //Chrome
			mat.ambient     = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
			mat.specular    = XMFLOAT4(0.774597f, 0.774597f, 0.774597f, 76.8f);
			break;
		case 5: //Copper
			mat.ambient     = XMFLOAT4(0.19125f, 0.0735f, 0.0225f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.7038f, 0.27048f, 0.0828f, 1.0f);
			mat.specular    = XMFLOAT4(0.256777f, 0.137622f, 0.086014f, 12.8f);
			break;
		case 6: //Polished Copper
			mat.ambient     = XMFLOAT4(0.2295f, 0.08825f, 0.0275f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.5508f, 0.2118f, 0.066f, 1.0f);
			mat.specular    = XMFLOAT4(0.580594f, 0.223257f, 0.0695701f, 51.2f);
			break;
		case 7: //Gold
			mat.ambient     = XMFLOAT4(0.24725f, 0.1995f, 0.0745f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.75164f, 0.60648f, 0.22648f, 1.0f);
			mat.specular    = XMFLOAT4(0.628281f, 0.555802f, 0.366065f, 51.2f);
			break;
		case 8: //Polished Gold
			mat.ambient     = XMFLOAT4(0.24725f, 0.2245f, 0.0645f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.34615f, 0.3143f, 0.0903f, 1.0f);
			mat.specular    = XMFLOAT4(0.797357f, 0.723991f, 0.208006f, 83.2f);
			break;
		case 9: //Pewter
			mat.ambient     = XMFLOAT4(0.105882f, 0.058824f, 0.113725f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.427451f, 0.470588f, 0.541176f, 1.0f);
			mat.specular    = XMFLOAT4(0.333333f, 0.333333f, 0.521569f, 9.84615f);
			break;
		case 10: //Silver
			mat.ambient     = XMFLOAT4(0.19225f, 0.19225f, 0.19225f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.50754f, 0.50754f, 0.50754f, 1.0f);
			mat.specular    = XMFLOAT4(0.508273f, 0.508273f, 0.508273f, 51.2f);
			break;
		case 11: //Polished Silver
			mat.ambient     = XMFLOAT4(0.23125f, 0.23125f, 0.23125f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.2775f, 0.2775f, 0.2775f, 1.0f);
			mat.specular    = XMFLOAT4(0.773911f, 0.773911f, 0.773911f, 89.6f);
			break;
		case 12: //Emerald
			mat.ambient     = XMFLOAT4(0.0215f, 0.1745f, 0.0215f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.07568f, 0.61424f, 0.07568f, 1.0f);
			mat.specular    = XMFLOAT4(0.633f, 0.727811f, 0.633f, 76.8f);
			break;
		case 13: //Jade
			mat.ambient     = XMFLOAT4(0.135f, 0.2225f, 0.1575f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.54f, 0.89f, 0.63f, 1.0f);
			mat.specular    = XMFLOAT4(0.316228f, 0.316228f, 0.316228f, 12.8f);
			break;
		case 14: //Obsidian
			mat.ambient     = XMFLOAT4(0.05375f, 0.05f, 0.06625f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.18275f, 0.17f, 0.22525f, 1.0f);
			mat.specular    = XMFLOAT4(0.332741f, 0.328634f, 0.346435f, 38.4f);
			break;
		case 15: //Pearl
			mat.ambient     = XMFLOAT4(0.25f, 0.20725f, 0.20725f, 1.0f);
			mat.diffuse     = XMFLOAT4(1.0f, 0.829f, 0.829f, 1.0f);
			mat.specular    = XMFLOAT4(0.296648f, 0.296648f, 0.296648f, 51.2f);
			break;
		case 16: //Ruby
			mat.ambient     = XMFLOAT4(0.1745f, 0.01175f, 0.01175f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.61424f, 0.04136f, 0.04136f, 1.0f);
			mat.specular    = XMFLOAT4(0.727811f, 0.626959f, 0.626959f, 76.8f);
			break;
		case 17: //Turquoise
			mat.ambient     = XMFLOAT4(0.1f, 0.18725f, 0.1745f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.396f, 0.74151f, 0.69102f, 1.0f);
			mat.specular    = XMFLOAT4(0.297254f, 0.30829f, 0.306678f, 12.8f);
			break;
		case 18: //Black Plastic
			mat.ambient     = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
			mat.specular    = XMFLOAT4(0.5f, 0.5f, 0.5f, 32.0f);
			break;
		case 19: //Black Rubber
			mat.ambient     = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
			mat.specular    = XMFLOAT4(0.4f, 0.4f, 0.4f, 10.0f);
			break;
		case 20: //White
			mat.ambient     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.diffuse     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.specular    = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);
			break;
		case 21: //Gray
			mat.ambient     = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
			mat.diffuse     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.specular    = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
			break;
		case 22: //Deepblue
			mat.ambient     = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
			mat.diffuse     = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
			mat.specular    = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
			break;
		default:
			mat.ambient     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.diffuse     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			mat.specular    = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);
	}
}
////////////////
// scene_dir_lights_common
////////////////
////////////////
void scene_dir_lights_common(lit_dir dir_lights[]){
	dir_lights[0].ambient   = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dir_lights[0].diffuse   = XMFLOAT4(1.0f, 0.9f, 0.9f, 1.0f);
	dir_lights[0].specular  = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	dir_lights[0].direction = XMFLOAT3(-0.457496f, -0.762493f, 0.457496f);
	dir_lights[1].ambient   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dir_lights[1].diffuse   = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	dir_lights[1].specular  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dir_lights[1].direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
	dir_lights[2].ambient   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dir_lights[2].diffuse   = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	dir_lights[2].specular  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dir_lights[2].direction = XMFLOAT3(0.0f, 0.0, -1.0f);
}
}
#endif