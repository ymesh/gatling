//
// Copyright (C) 2023 Pablo Delgado Krämer
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef SI_RP_MAIN
#define SI_RP_MAIN

#include "interface/gtl.h"

SI_NAMESPACE_BEGIN(rp_main)

struct FVertex
{
  /* f32 pos[3], f32 bsign */
  SI_VEC4 field1;
  /* u32 norm, u32 tan, f32 texcoords[2] */
  SI_VEC4 field2;
};

struct Face
{
  SI_UINT v_0;
  SI_UINT v_1;
  SI_UINT v_2;
};

struct PushConstants
{
  SI_VEC3  cameraPosition;
  SI_UINT  imageDims;
  SI_VEC3  cameraForward;
  SI_FLOAT focusDistance;
  SI_VEC3  cameraUp;
  SI_FLOAT cameraVFoV;
  SI_VEC4  backgroundColor;
  SI_UINT  sampleOffset;
  SI_FLOAT lensRadius;
  SI_UINT  sampleCount;
  SI_FLOAT maxSampleValue;
  SI_VEC3  domeLightTransformCol0;
  SI_UINT  maxBouncesAndRrBounceOffset;
  SI_VEC3  domeLightTransformCol1;
  SI_FLOAT rrInvMinTermProb;
  SI_VEC3  domeLightTransformCol2;
  // 1 float remaining
};

SI_BINDING_INDEX(OUT_PIXELS,     0)
SI_BINDING_INDEX(FACES,          1)
SI_BINDING_INDEX(EMISSIVE_FACES, 2)
SI_BINDING_INDEX(VERTICES,       3)
SI_BINDING_INDEX(SAMPLER,        4)
SI_BINDING_INDEX(TEXTURES_2D,    5)
SI_BINDING_INDEX(TEXTURES_3D,    6)
SI_BINDING_INDEX(SCENE_AS,       7)

SI_NAMESPACE_END()

#endif
