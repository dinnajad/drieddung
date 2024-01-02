
#ifndef SPCommon_h
#define SPCommon_h

#include "SPMath.h"


typedef struct SPWorldGenOptions {
	double heightOffset;
	double rainfallMultiplier;
	double temperatureOffset;
	SPVec3 scales;
	SPVec3 influences;

	SPVec4 userDataA; //these can all be used for mods that want to add more options. They are read from the table passed through from Lua.
	SPVec4 userDataB;
	SPVec4 userDataC;
	SPVec4 userDataD;
	void* userDataP; //any data can be stored in here by mods. On the Lua side, it needs to be passed in the customOptions table as a lua string for the key 'userDataP'. A good way to do this might be to serialize your own struct using LuaJIT's FFI

} SPWorldGenOptions;



#endif