#ifndef GI_BVH_COLLAPSE_H
#define GI_BVH_COLLAPSE_H

#include <stddef.h>

#include "bvh.h"

struct gi_bvhc_node
{
  gml_aabb aabbs[8];
  uint32_t offsets[8];
  uint32_t counts[8];
  uint32_t child_index;
  uint32_t face_index;
};

struct gi_bvhc
{
  gml_aabb             aabb;
  uint32_t             node_count;
  struct gi_bvhc_node* nodes;
  uint32_t             face_count;
  struct gi_face*      faces;
};

struct gi_bvhc_params
{
  const struct gi_bvh* bvh;
  float                face_intersection_cost;
  uint32_t             max_leaf_size;
  float                node_traversal_cost;
};

void gi_bvh_collapse(const struct gi_bvhc_params* params,
                     struct gi_bvhc* bvhc);

void gi_free_bvhc(struct gi_bvhc* bvhcc);

#endif
