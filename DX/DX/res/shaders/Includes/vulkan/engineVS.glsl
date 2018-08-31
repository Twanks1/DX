#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Predefined vertex streams
#define VERTEX_POSITION POSITION
#define VERTEX_COLOR	COLOR
#define VERTEX_UV		TEXCOORD
#define VERTEX_NORMAL	NORMAL
#define VERTEX_TANGENT	TANGENT

// PER OBJECT PUSH CONSTANT
layout (std140, push_constant) uniform PushConstant 
{
	mat4 world;
} _Object;

//----------------------------------------------------------------------
// Descriptor-Sets
//----------------------------------------------------------------------

// Per frame global ubo
layout (set = 0, binding = 0) uniform GLOBAL
{
	float time;
	float ambient;
} _Global;

// Per camera ubo
layout (set = 0, binding = 1) uniform CAMERA
{
	mat4 view;
	mat4 proj;	
	vec3 pos;
	float PAD;
	float zNear;
	float zFar;
} _Camera;

//----------------------------------------------------------------------
vec4 TO_CLIP_SPACE( vec4 vert )
{
	mat4 mvp = _Camera.proj * _Camera.view * _Object.world;
    return mvp * vert;
}

//----------------------------------------------------------------------
vec4 TO_CLIP_SPACE( vec3 vert )
{
    return TO_CLIP_SPACE( vec4( vert, 1 ) );
}

//----------------------------------------------------------------------
vec3 TO_WORLD_SPACE( vec3 vert )
{
    return (_Object.world * vec4( vert, 1 )).xyz;
}

//----------------------------------------------------------------------
vec3 TRANSFORM_NORMAL( vec3 normal )
{
	return normalize( mat3(_Object.world) * normal );
}
