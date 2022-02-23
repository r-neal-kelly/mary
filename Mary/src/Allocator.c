#include <stdlib.h>
#include <Mary/Allocator.h>
#include <Mary/Pool.h>
#include <Mary/Arena.h>

MARY_Primitives;

extern Mary_Pool_t MARY_POOLS[];

void *Mary_Allocator_Alloc(Mary_Allocator_t allocator, Mary_Size_t bytes)
{
    if (allocator.type == MARY_ALLOCATOR_FRAME) {
        return Mary_Arena_Alloc_Frame(allocator.var, bytes);
    } else if (allocator.type == MARY_ALLOCATOR_CHAIN) {
        return Mary_Arena_Alloc_Chain(allocator.var, bytes);
    } else if (allocator.type == MARY_ALLOCATOR_ERROR) {
        return Mary_Arena_Alloc_Error(allocator.var, bytes);
    } else if (allocator.type == MARY_ALLOCATOR_POOL) {
        return Mary_Pool_Alloc(&MARY_POOLS[allocator.var], bytes);
    } else if (allocator.type == MARY_ALLOCATOR_HEAP) {
        return Mary_Alloc(bytes);
    } else if (allocator.type == MARY_ALLOCATOR_STACK) {
        MARY_Assert(MARY_FALSE, "Stack must be allocated in place.");
        return 0;
    } else if (allocator.type == MARY_ALLOCATOR_STATIC) {
        MARY_Assert(MARY_FALSE, "Static must be allocated in place.");
        return 0;
    } else {
        MARY_Assert(MARY_FALSE, "Invalid allocator.");
        return 0;
    }
}

void *Mary_Allocator_Calloc(Mary_Allocator_t allocator, Mary_Size_t unit, Mary_Size_t units)
{
    if (allocator.type == MARY_ALLOCATOR_FRAME) {
        //return Mary_Arena_Calloc_Frame(allocator.var, unit, units);
        return 0;
    } else if (allocator.type == MARY_ALLOCATOR_CHAIN) {
        //return Mary_Arena_Calloc_Chain(allocator.var, unit, units);
        return 0;
    } else if (allocator.type == MARY_ALLOCATOR_ERROR) {
        //return Mary_Arena_Calloc_Error(allocator.var, unit, units);
        return 0;
    } else if (allocator.type == MARY_ALLOCATOR_POOL) {
        //return Mary_Pool_Calloc(&MARY_POOLS[allocator.var], unit, units);
        return 0;
    } else if (allocator.type == MARY_ALLOCATOR_HEAP) {
        return Mary_Calloc(unit, units);
    } else if (allocator.type == MARY_ALLOCATOR_STACK) {
        MARY_Assert(MARY_FALSE, "Stack must be allocated in place.");
        return 0;
    } else if (allocator.type == MARY_ALLOCATOR_STATIC) {
        MARY_Assert(MARY_FALSE, "Static must be allocated in place.");
        return 0;
    } else {
        MARY_Assert(MARY_FALSE, "Invalid allocator.");
        return 0;
    }
}

void *Mary_Allocator_Realloc(Mary_Allocator_t allocator, void *data, Mary_Size_t bytes)
{
    /* eventually what I would like is some way to tell the user when the realloc failed
    other than returning 0. Because I do not want to lose the original pointer, and
    it will give the caller the chance to free some memory. so, if it does fail,
    it simply returns the same pointer and an error by other means.

    in vector Reserve, Fit, and Grow, we are not even checking the data ptr. we want to check
    for an error separately! We can use the arena system in this function in particular
    because it is not meant to be used by anyone but the library.

    also, instead of the exception like system, we could also send a union instead,
    which would be more like the option system, it either is something or isn't. either
    would probably be okay...*/

    if (allocator.type == MARY_ALLOCATOR_FRAME) {
        return Mary_Arena_Realloc_Frame(allocator.var, data, bytes);
    } else if (allocator.type == MARY_ALLOCATOR_CHAIN) {
        return Mary_Arena_Realloc_Chain(allocator.var, data, bytes);
    } else if (allocator.type == MARY_ALLOCATOR_POOL) {
        return Mary_Pool_Realloc(&MARY_POOLS[allocator.var], data, bytes);
    } else if (allocator.type == MARY_ALLOCATOR_HEAP) {
        return Mary_Realloc(data, bytes);
    } else if (allocator.type == MARY_ALLOCATOR_STACK) {
        MARY_Assert(MARY_FALSE, "Stack cannot be reallocated.");
        return 0;
    } else if (allocator.type == MARY_ALLOCATOR_STATIC) {
        MARY_Assert(MARY_FALSE, "Static cannot be reallocated.");
        return 0;
    } else {
        MARY_Assert(MARY_FALSE, "Invalid allocator.");
        return 0;
    }
}

void Mary_Allocator_Dealloc(Mary_Allocator_t allocator, void *data)
{
    if (allocator.type == MARY_ALLOCATOR_FRAME) {
        Mary_Arena_Dealloc_Frame(allocator.var, data);
    } else if (allocator.type == MARY_ALLOCATOR_CHAIN) {
        Mary_Arena_Dealloc_Chain(allocator.var, data);
    } else if (allocator.type == MARY_ALLOCATOR_POOL) {
        Mary_Pool_Dealloc(&MARY_POOLS[allocator.var], data);
    } else if (allocator.type == MARY_ALLOCATOR_HEAP) {
        Mary_Dealloc(data);
    } else if (allocator.type == MARY_ALLOCATOR_STACK) {
        MARY_Assert(MARY_FALSE, "Stack cannot be deallocated.");
    } else if (allocator.type == MARY_ALLOCATOR_STATIC) {
        MARY_Assert(MARY_FALSE, "Static cannot be deallocated.");
    } else {
        MARY_Assert(MARY_FALSE, "Invalid allocator.");
    }
}
