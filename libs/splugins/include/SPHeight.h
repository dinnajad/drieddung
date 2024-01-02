
#ifndef SPHeight_h
#define SPHeight_h

#include "SPCommon.h"
#include "SPNoise.h"

typedef void (* SPHeightInitFunc) (SPNoise* noise1, 
	SPNoise* noise2, 
	SPWorldGenOptions worldGenOptions);
typedef bool (* SPReplacesPreviousHeightFunc) ();
typedef SPVec4 (* SPHeightGetFunc) (SPVec4 previousHeight,
	SPNoise* noise1, 
	SPNoise* noise2,
	SPVec3 pointNormal, 
	SPVec3 noiseLoc, 
	SPWorldGenOptions worldGenOptions, 
	double riverValue,
	double riverDistance);

MJ_EXPORT void spHeightInit(SPNoise* noise1, 
	SPNoise* noise2, 
	SPWorldGenOptions worldGenOptions);
MJ_EXPORT bool spReplacesPreviousHeight();
MJ_EXPORT SPVec4 spHeightGet(SPVec4 previousHeight,
	SPNoise* noise1, 
	SPNoise* noise2,
	SPVec3 pointNormal, 
	SPVec3 noiseLoc, 
	SPWorldGenOptions worldGenOptions, 
	double riverValue,
	double riverDistance);

#endif