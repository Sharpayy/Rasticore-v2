#pragma once
#include <gl/glew.h>

enum VertexArrayParameterType
{
	TypeVertex3Only,
	TypeVertex2Only,
	TypeVertex2Texture,
	TypeVertex3Texture,
	TypeVertex2TextureColor3
};

enum TextureQuality
{
	Quality128,
	Quality256,
	Quality512,
	Quality1024,
	Quality4096,
	QualityAlwaysEnd
};

// SHADER INPUT LOCATIONS

constexpr uint64_t SHADER_INPUT_LOCATION_VERTEX_POSITION = 0;
// layout(location = 0) in vec2 pos;

constexpr uint64_t SHADER_INPUT_LOCATION_TEXTURE_COORDS = 1;
// layout(location = 1) in vec2 uv;

constexpr uint64_t SHADER_INPUT_LOCATION_COLOR = 2;
// layout(location = 2) in vec3 color;



// UNIFORM BINDINGS

constexpr uint64_t UNIFORM_BUFFER_BINDING_MAIN_VIEW = 2;
// layout (std140, binding = 2) uniform MainView
// {
//	mat4 view;
//	mat4 projection
//  mat4 projview;
// };


// OBEJCTS FLAGS

constexpr uint32_t OBEJCT_FLAG_ENABLED = 0x00000001;

constexpr uint32_t OBJECT_FLAG_CREATION_STD = OBEJCT_FLAG_ENABLED;