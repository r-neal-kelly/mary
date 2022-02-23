#include <stdio.h>
#include <Mary/Vector.h>
#include <Mary/Hashmap.h>
#include <Mary/GL_Texture.h>

MARY_Primitives;

typedef struct
{
  GLuint slot;
  GLenum target;
}
Location;

typedef struct
{
  Mary_Vector_t *free, *used;
}
Slots;

Mary_Vector_t
  free_1d, free_2d, free_3d, free_1d_a, free_2d_a, free_rect, free_cube, free_buffer, free_2d_m, free_2d_ma,
  used_1d, used_2d, used_3d, used_1d_a, used_2d_a, used_rect, used_cube, used_buffer, used_2d_m, used_2d_ma;
Mary_Hashmap_t
  targets_to_slots, locations_to_textures;

static void Mary_GL_Texture_Slot(Mary_GL_Texture_t *texture);
static void Mary_GL_Texture_Unslot(Mary_GL_Texture_t *texture);

void Mary_GL_Texture_Start()
{
  // should prob. put all these vectors on a pool to make sure they are near each other.
  GLuint max_slots = MARY_GL_MAX_TEXTURES;
  Mary_Vector_Create(&free_1d, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_2d, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_3d, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_1d_a, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_2d_a, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_rect, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_cube, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_buffer, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_2d_m, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&free_2d_ma, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  for (GLuint i = 0; i < max_slots; ++i)
  {
    MARY_Vector_Push(&free_1d, GLuint, i);
    MARY_Vector_Push(&free_2d, GLuint, i);
    MARY_Vector_Push(&free_3d, GLuint, i);
    MARY_Vector_Push(&free_1d_a, GLuint, i);
    MARY_Vector_Push(&free_2d_a, GLuint, i);
    MARY_Vector_Push(&free_rect, GLuint, i);
    MARY_Vector_Push(&free_cube, GLuint, i);
    MARY_Vector_Push(&free_buffer, GLuint, i);
    MARY_Vector_Push(&free_2d_m, GLuint, i);
    MARY_Vector_Push(&free_2d_ma, GLuint, i);
  }

  Mary_Vector_Create(&used_1d, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_2d, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_3d, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_1d_a, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_2d_a, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_rect, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_cube, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_buffer, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_2d_m, MARY_Allocator_Heap, sizeof(GLuint), max_slots);
  Mary_Vector_Create(&used_2d_ma, MARY_Allocator_Heap, sizeof(GLuint), max_slots);

  GLenum target; Slots slots;
  Mary_Hashmap_Create(&targets_to_slots, sizeof(GLenum), sizeof(Slots));
  target = GL_TEXTURE_1D, slots = (Slots) { &free_1d, &used_1d }, 
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_2D, slots = (Slots) { &free_2d, &used_2d },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_3D, slots = (Slots) { &free_3d, &used_3d },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_1D_ARRAY, slots = (Slots) { &free_1d_a, &used_1d_a },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_2D_ARRAY, slots = (Slots) { &free_2d_a, &used_2d_a },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_RECTANGLE, slots = (Slots) { &free_rect, &used_rect },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_CUBE_MAP, slots = (Slots) { &free_cube, &used_cube },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_BUFFER, slots = (Slots) { &free_buffer, &used_buffer },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_2D_MULTISAMPLE, slots = (Slots) { &free_2d_m, &used_2d_m },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);
  target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY, slots = (Slots) { &free_2d_ma, &used_2d_ma },
    Mary_Hashmap_Assign(&targets_to_slots, &target, &slots);

  Mary_Hashmap_Create(&locations_to_textures, sizeof(Location), sizeof(Mary_GL_Texture_t *));
}

void Mary_GL_Texture_Finish()
{
  Mary_Vector_Destroy(&free_1d);
  Mary_Vector_Destroy(&free_2d);
  Mary_Vector_Destroy(&free_3d);
  Mary_Vector_Destroy(&used_1d);
  Mary_Vector_Destroy(&used_2d);
  Mary_Vector_Destroy(&used_3d);
  Mary_Hashmap_Destroy(&locations_to_textures);
  Mary_Hashmap_Destroy(&targets_to_slots);
}

void Mary_GL_Texture_Create(Mary_GL_Texture_t *texture, GLenum target)
{
  texture->target = target;
  texture->slot = -1;
  glGenTextures(1, &texture->id);
  Mary_GL_Texture_Slot(texture);
  glActiveTexture(GL_TEXTURE0 + texture->slot);
  glBindTexture(texture->target, texture->id);
}

void Mary_GL_Texture_Destroy(Mary_GL_Texture_t *texture)
{
  if (texture->slot != -1)
  {
    Mary_GL_Texture_Unslot(texture);
  }
  glDeleteTextures(1, &texture->id);
  texture->id = 0;
}

void Mary_GL_Texture_Activate(Mary_GL_Texture_t *texture)
{
  if (texture->slot == -1)
  {
    Mary_GL_Texture_Slot(texture);
    glActiveTexture(GL_TEXTURE0 + texture->slot);
    glBindTexture(texture->target, texture->id);
  }
  else
  {
    glActiveTexture(GL_TEXTURE0 + texture->slot);
  }
}

static void Mary_GL_Texture_Slot(Mary_GL_Texture_t *texture)
{
  Slots slots; Mary_Hashmap_At(&targets_to_slots, &texture->target, &slots);
  if (slots.free->units == 0)
  {
    GLuint used_slot; Mary_Vector_Pop_At(slots.used, 0, &used_slot);
    Location loc = { used_slot, texture->target };
    Mary_GL_Texture_t *old_texture;
    Mary_Hashmap_At(&locations_to_textures, &loc, &old_texture);
    old_texture->slot = -1;
    MARY_Vector_Push(slots.used, GLuint, used_slot);
    Mary_Hashmap_Assign(&locations_to_textures, &loc, &texture);
    texture->slot = used_slot;
  }
  else
  {
    GLuint free_slot = MARY_Vector_Pop(slots.free, GLuint);
    Location loc = { free_slot, texture->target };
    MARY_Vector_Push(slots.used, GLuint, free_slot);
    Mary_Hashmap_Assign(&locations_to_textures, &loc, &texture);
    texture->slot = free_slot;
  }
}

static void Mary_GL_Texture_Unslot(Mary_GL_Texture_t *texture)
{
  Slots slots; Mary_Hashmap_At(&targets_to_slots, &texture->target, &slots);
  GLuint slot = texture->slot;
  Mary_Vector_Erase(slots.used, &slot);
  MARY_Vector_Push(slots.free, GLuint, slot);
  Location loc = { slot, texture->target };
  Mary_Hashmap_Erase(&locations_to_textures, &loc);
  texture->slot = -1;
}
