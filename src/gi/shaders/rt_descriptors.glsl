#include "interface/rp_main.h"
#include "common.glsl"

layout(binding = BINDING_INDEX_OUT_PIXELS, std430) buffer PixelsBuffer { vec4 pixels[]; };

layout(binding = BINDING_INDEX_FACES, std430) readonly buffer FacesBuffer { Face faces[]; };

#ifdef NEXT_EVENT_ESTIMATION
//layout(binding = BINDING_INDEX_EMISSIVE_FACES, std430) readonly buffer EmissiveFacesBuffer { uint emissive_face_indices[]; };
#endif

layout(binding = BINDING_INDEX_VERTICES, std430) readonly buffer VerticesBuffer { FVertex vertices[]; };

#if (TEXTURE_COUNT_2D > 0) || (TEXTURE_COUNT_3D > 0)
layout(binding = BINDING_INDEX_SAMPLER) uniform sampler tex_sampler;
#endif

#if TEXTURE_COUNT_2D > 0
layout(binding = BINDING_INDEX_TEXTURES_2D) uniform texture2D textures_2d[TEXTURE_COUNT_2D];
#endif

#if TEXTURE_COUNT_3D > 0
layout(binding = BINDING_INDEX_TEXTURES_3D) uniform texture3D textures_3d[TEXTURE_COUNT_3D];
#endif

layout(binding = BINDING_INDEX_SCENE_AS) uniform accelerationStructureEXT sceneAS;

layout(push_constant) uniform PushConstantBlock { PushConstants PC; };
