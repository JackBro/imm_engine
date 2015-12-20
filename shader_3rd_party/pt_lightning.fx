////////////////
// pt_lightning.fx
// method: (Stormy Night By: Brandon Fogerty)
// from: http://glslsandbox.com/e#25418.10
//       http://xdpixel.com/stormy-night/
////////////////
////////////////
//***********************************************
// GLOBALS                                      *
//***********************************************
cbuffer cbPerFrame
{
	float3 gEyePosW;
	// for when the emit position/direction is varying
	float3 gEmitPosW;
	float3 gEmitDirW;
	float gGameTime;
	float gTimeStep;
	float4x4 gViewProj;
};
cbuffer cbFixed
{
	// Texture coordinates used to stretch texture over quad
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] =
	{
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f)
	};
};
DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};
DepthStencilState NoDepthWrites
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
};
BlendState AdditiveBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};
//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float Hash(float2 p)
{
	float3 p2 = float3(p.xy, 1.0f);
	return frac(sin(dot(p2, float3(37.1f, 61.7f, 12.4f)))*3758.54f);
}
float Noise(float2 p)
{
	float2 i = floor(p);
	float2 f = frac(p);
	f *= f * (3.0f-2.0f*f);
	return lerp(lerp(Hash(i + float2(0.0f, 0.0f)), Hash(i + float2(1.0f, 0.0f)), f.x),
	            lerp(Hash(i + float2(0.0f, 1.0f)), Hash(i + float2(1.0f, 1.0f)), f.x),
	            f.y);
}
float FBM(float2 p)
{
	float v = 0.0f;
	v += Noise(p*1.0f)*0.5f;
	v += Noise(p*2.0f)*0.25f;
	v += Noise(p*4.0f)*0.125f;
	return v;
}
float3 Lightning(float2 uv)
{
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	for(int i=0; i < 3; ++i) {
		float indexAsFloat = float(i);
		float amp = 40.0f + (indexAsFloat*1.0f);
		float period = 2.0f + (indexAsFloat+2.0f);
		float thickness = lerp(1.1f, 1.7f, uv.y * 0.5f + 0.5f);
		float intensity = lerp(0.5f, 1.5f, Noise(uv*10.0f));
		float t = abs(thickness / (sin(uv.x + FBM(uv + gGameTime * period)) * amp) * intensity);
		float show = frac(abs(sin(gGameTime))) >= 0.3 ? 1.0 : 0.0;
		show *= step(abs(FBM(float2(sin(gGameTime * 50.0f), 0.0f))), 0.4f);
		finalColor +=  t * float3(0.3f, 0.5f, 2.0f) * show;
	}
	return finalColor;
}
//***********************************************
// STREAM-OUT TECH                              *
//***********************************************
#define PT_EMITTER 0
#define PT_FLARE 1
struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age          : AGE;
	uint Type          : TYPE;
};
Particle StreamOutVS(Particle vin)
{
	return vin;
}
// The stream-out GS is just responsible for emitting
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be
// different.
[maxvertexcount(1)]
void StreamOutGS(
	point Particle gin[1],
	inout PointStream<Particle> ptStream)
{
	if (gin[0].Type == PT_EMITTER) {
		// only use one particle
		Particle p;
		p.InitialPosW = gEmitPosW.xyz;
		p.InitialVelW = float3(0.0f, 0.0f, 0.0f);
		p.SizeW       = float2(50.0f, 50.0f);
		p.Age         = 0.0f;
		p.Type        = PT_FLARE;
		ptStream.Append(p);
	}
	else {
		ptStream.Append(gin[0]);
	}
}
GeometryShader gsStreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");
technique11 StreamOutTech
{
	pass P0 {
		SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
		SetGeometryShader(gsStreamOut);
		// disable pixel shader for stream-out only
		SetPixelShader(NULL);
		// we must also disable the depth buffer for stream-out only
		SetDepthStencilState(DisableDepth, 0);
	}
}
//***********************************************
// DRAW TECH                                    *
//***********************************************
struct VertexOut
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
};
VertexOut DrawVS(Particle vin)
{
	VertexOut vout;
	// constant acceleration equation
	vout.PosW = vin.InitialPosW;
	vout.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	vout.SizeW = vin.SizeW;
	vout.Type  = vin.Type;
	return vout;
}
struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	float2 Tex   : TEXCOORD;
};
// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void DrawGS(
	point VertexOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
	// Compute world matrix so that billboard faces the camera.
	float3 look  = normalize(gEyePosW.xyz - gin[0].PosW);
	look.xz = gEyePosW.xz;
	look = normalize(look);
	float3 right = normalize(cross(float3(0,1,0), look));
	float3 up    = cross(look, right);
	// Compute triangle strip vertices (quad) in world space.
	float halfWidth  = 0.5f*gin[0].SizeW.x;
	float halfHeight = 0.5f*gin[0].SizeW.y;
	float4 v[4];
	v[0] = float4(gin[0].PosW + halfWidth*right - halfHeight*up, 1.0f);
	v[1] = float4(gin[0].PosW + halfWidth*right + halfHeight*up, 1.0f);
	v[2] = float4(gin[0].PosW - halfWidth*right - halfHeight*up, 1.0f);
	v[3] = float4(gin[0].PosW - halfWidth*right + halfHeight*up, 1.0f);
	// Transform quad vertices to world space and output
	// them as a triangle strip.
	GeoOut gout;
	[unroll]
	for(int i = 0; i < 4; ++i) {
		gout.PosH  = mul(v[i], gViewProj);
		gout.Tex   = gQuadTexC[i];
		gout.Color = gin[0].Color;
		triStream.Append(gout);
	}
}
float4 DrawPS(GeoOut pin) : SV_TARGET
{
	float2 resolution = float2(1.0f, 1.0f);
	float2 uv = (pin.Tex.xy / resolution.xy) * 2.0f - 1.0f;
	uv.x *= resolution.x/resolution.y;
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	float2 uvOffset = float2(-0.5f, 0.0f);
	float2 lightningUV = uv + uvOffset;
	float theta = 0.0f;
	lightningUV.x = uv.x * cos(theta) - uv.y*sin(theta);
	lightningUV.y = uv.x * sin(theta) + uv.y*cos(theta);
	finalColor += Lightning(lightningUV + uvOffset);
	finalColor *= float3(0.93f, 0.21f, 0.34f);
	finalColor -= float3(0.93f, 0.21f, 0.34f);
	return float4(finalColor, 1.0f);
}
technique11 DrawTech
{
	pass P0 {
		SetVertexShader(CompileShader(vs_5_0, DrawVS()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPS()));
		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState(NoDepthWrites, 0);
	}
}