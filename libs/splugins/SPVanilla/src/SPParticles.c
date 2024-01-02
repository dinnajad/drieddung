

#include "SPParticles.h"

//define local identifiers for this mod to use, it will be given back as the localTypeID in update functions, and used by the engine to determine particle type when adding particles

enum {
	sp_vanillaEmitterTypeCampfireLarge = 0,
	sp_vanillaEmitterTypeCampfireMedium,
	sp_vanillaEmitterTypeCampfireSmall,
	sp_vanillaEmitterTypeTorchLarge,
	sp_vanillaEmitterTypeTorchSmall,
	sp_vanillaEmitterTypeWoodChop,
	sp_vanillaEmitterTypeFeathers,
	sp_vanillaEmitterTypeClouds,
	sp_vanillaEmitterTypeWaterRipples,
	sp_vanillaEmitterTypeDig,
	sp_vanillaEmitterTypePullWeeds,
	sp_vanillaEmitterTypeDestroy,
	sp_vanillaEmitterTypeDustParticles,
	sp_vanillaEmitterTypeSnow,
	sp_vanillaEmitterTypeRain,
	sp_vanillaEmitterTypeDestroyLarge,
	sp_vanillaEmitterTypeKilnFire,
	sp_vanillaEmitterTypeKilnFireMedium,
	sp_vanillaEmitterTypeKilnFireSmall,
	sp_vanillaEmitterTypeBlacksmithSparks,
};

enum {
	sp_vanillaRenderGroupSmoke = 0,
	sp_vanillaRenderGroupFire,
	sp_vanillaRenderGroupStandard,
	sp_vanillaRenderGroupSpark,
	sp_vanillaRenderGroupCloud,
	sp_vanillaRenderGroupCloudWindDirBlown,
	sp_vanillaRenderGroupCloudBlended,
	sp_vanillaRenderGroupCloudBlendedWindDirBlown,
	sp_vanillaRenderGroupWaterRipples,
	sp_vanillaRenderGroupDust,
	sp_vanillaRenderGroupDustParticles,
	sp_vanillaRenderGroupSnow,
	sp_vanillaRenderGroupRain,
	sp_vanillaRenderGroupRainBounce,
	sp_vanillaRenderGroupSlowReleaseDust,
};


//define emitter types that we wish to override or add. Vanilla functions and functions for mods with earlier order indexes than this one that override the same type, will not get called.
//Mods with later order indexes than this mod will win, so it's possible that even though you define behavior in the functions here, those functions may not actually get called..

#define EMITTER_TYPES_COUNT 20
static SPParticleEmitterTypeInfo particleEmitterTypeInfos[EMITTER_TYPES_COUNT] = {
	{
		"campfireLarge",
		sp_vanillaEmitterTypeCampfireLarge
	},
	{
		"campfireMedium",
		sp_vanillaEmitterTypeCampfireMedium
	},
	{
		"campfireSmall",
		sp_vanillaEmitterTypeCampfireSmall
	},
	{
		"torchLarge",
		sp_vanillaEmitterTypeTorchLarge
	},
	{
		"torchSmall",
		sp_vanillaEmitterTypeTorchSmall
	},
	{
		"woodChop",
		sp_vanillaEmitterTypeWoodChop
	},
	{
		"feathers",
		sp_vanillaEmitterTypeFeathers
	},
	{
		"clouds",
		sp_vanillaEmitterTypeClouds
	},
	{
		"waterRipples",
		sp_vanillaEmitterTypeWaterRipples
	},
	{
		"dig",
		sp_vanillaEmitterTypeDig
	},
	{
		"pullWeeds",
		sp_vanillaEmitterTypePullWeeds
	},
	{
		"destroy",
		sp_vanillaEmitterTypeDestroy
	},
	{
		"dustParticles",
		sp_vanillaEmitterTypeDustParticles
	},
	{
		"snow",
		sp_vanillaEmitterTypeSnow
	},
	{
		"rain",
		sp_vanillaEmitterTypeRain
	},
	{
		"destroyLarge",
		sp_vanillaEmitterTypeDestroyLarge
	},
	{
		"kilnFire",
		sp_vanillaEmitterTypeKilnFire
	},
	{
		"kilnFireMedium",
		sp_vanillaEmitterTypeKilnFireMedium
	},
	{
		"kilnFireSmall",
		sp_vanillaEmitterTypeKilnFireSmall
	},
	{
		"blacksmithSparks",
		sp_vanillaEmitterTypeBlacksmithSparks
	},
};

//define the vertex attributes that the shader will use. In the vanilla mod, all currently take the same, but this could be different for more complex shaders
#define GENERIC_VERTEX_ATTRIBUTE_COUNT 3
static int genericVertexDescriptionTypes[GENERIC_VERTEX_ATTRIBUTE_COUNT] = {
	SPRenderGroupVertexDescriptionType_vec3, 
	SPRenderGroupVertexDescriptionType_vec2,
	SPRenderGroupVertexDescriptionType_vec4
};

#define EXTRA_VEC3_VERTEX_ATTRIBUTE_COUNT 4
static int extraVec3VertexDescriptionTypes[EXTRA_VEC3_VERTEX_ATTRIBUTE_COUNT] = {
	SPRenderGroupVertexDescriptionType_vec3, 
	SPRenderGroupVertexDescriptionType_vec2,
	SPRenderGroupVertexDescriptionType_vec4,
	SPRenderGroupVertexDescriptionType_vec3
};


//define render groups that we wish to use, override or add. To use an existing/predefined render group, either define again or set vertexDescriptionTypeCount to 0
#define RENDER_GROUP_TYPES_COUNT 15
static SPParticleRenderGroupInfo renderGroupInfos[RENDER_GROUP_TYPES_COUNT] = {
	{ 
		"cloud",
		sp_vanillaRenderGroupCloud,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/cloudsN.png",
		"img/cloudsP.png",
		true,
		true,
	},
	{ 
		"cloud",
		sp_vanillaRenderGroupCloudWindDirBlown,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/cloudsN.png",
		"img/cloudsP.png",
		true,
		true,
	},
	{ 
		"cloudBlended",
		sp_vanillaRenderGroupCloudBlended,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/cloudsN.png",
		"img/cloudsP.png",
		true,
		true,
	},
	{ 
		"cloudBlended",
		sp_vanillaRenderGroupCloudBlendedWindDirBlown,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/cloudsN.png",
		"img/cloudsP.png",
		true,
		true,
	},
	{
		"smokeParticle",
		sp_vanillaRenderGroupSmoke,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"fireParticle",
		sp_vanillaRenderGroupFire,
		EXTRA_VEC3_VERTEX_ATTRIBUTE_COUNT,
		extraVec3VertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"particle",
		sp_vanillaRenderGroupStandard,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{ 
		"spark",
		sp_vanillaRenderGroupSpark,
		EXTRA_VEC3_VERTEX_ATTRIBUTE_COUNT,
		extraVec3VertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"waterRipples",
		sp_vanillaRenderGroupWaterRipples,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"particle",
		sp_vanillaRenderGroupDust,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"particleWeather",
		sp_vanillaRenderGroupDustParticles,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"particleWeather",
		sp_vanillaRenderGroupSnow,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"particleRain",
		sp_vanillaRenderGroupRain,
		EXTRA_VEC3_VERTEX_ATTRIBUTE_COUNT,
		extraVec3VertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"particleRainBounce",
		sp_vanillaRenderGroupRainBounce,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
	{
		"particle",
		sp_vanillaRenderGroupSlowReleaseDust,
		GENERIC_VERTEX_ATTRIBUTE_COUNT,
		genericVertexDescriptionTypes,
		"img/particles.png",
		NULL,
		false,
		false,
	},
};

int spGetEmitterTypesCount()
{
	return EMITTER_TYPES_COUNT;
}

SPParticleEmitterTypeInfo* spGetEmitterTypes()
{
	return particleEmitterTypeInfos;
}


int spGetRenderGroupTypesCount()
{
	return RENDER_GROUP_TYPES_COUNT;
}

static const double cloudVelocity = 0.0001;
static const double cloudWorldTimeInfluence = 0.0001;

static const double cloudFieldSize = SP_METERS_TO_PRERENDER(400000.0);
static const double cloudFieldHalfSize = SP_METERS_TO_PRERENDER(200000.0);

static const int cumulusLargeGridCount = 16;
static const double cumulusLargeAltitude = SP_METERS_TO_PRERENDER(2400.0);
static const double cumulusLargeScale = 4.0;

static const int cumulusSmallGridCount = 64;
static const double cumulusSmallAltitude = SP_METERS_TO_PRERENDER(600.0);
static const double cumulusSmallScale = 1.2;

static const int altoCloudGridCount = 48;
static const double altoCloudAltitude = SP_METERS_TO_PRERENDER(6800.0);
static const double altoCloudScale = 40.0;

static const int cirrusCloudGridCount = 8;
static const double cirrusCloudAltitude = SP_METERS_TO_PRERENDER(8000.0);
static const double cirrusCloudScale = 60.0;


SPParticleRenderGroupInfo* spGetRenderGroupTypes()
{
	return renderGroupInfos;
}

void addClouds(SPParticleThreadState* threadState,
	SPParticleEmitterState* emitterState)
{

	SPRand* spRand = threadState->spRand;

	double posLength = spVec3Length(emitterState->p);
	SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
	SPVec3 north = {0.0, 1.0, 0.0};
	SPVec3 right = spVec3Normalize(spVec3Cross(north, normalizedPos));
	SPVec3 negZ = spVec3Normalize(spVec3Cross(normalizedPos, right));

	SPVec3 zeroVec = {0,0,0};


	SPParticleState state;


	int counter = 0;
	for(int y = 0; y < cumulusLargeGridCount; y++)
	{
		for(int x = 0; x < cumulusLargeGridCount; x++)
		{
			//SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(120000.0));
			SPVec3 randVec = spRandomVec3ForUniqueID(spRand, ((uint64_t)(threadState->worldTime * 0.1)) + 34 + y * cumulusLargeGridCount + x);
			if(randVec.z > 0.2)
			{

				state.lifeLeft = ((((double)x) + 0.5 * randVec.x) / cumulusLargeGridCount);

				double xPos = -cloudFieldHalfSize + cloudFieldSize * state.lifeLeft;
				double zPos = -cloudFieldHalfSize + cloudFieldSize * ((((double)y) + 0.5 * randVec.y) / cumulusLargeGridCount);

				SPVec3 pos = spVec3Add(spVec3Mul(right, xPos), spVec3Mul(negZ, zPos));

				SPVec3 offsetVec = spVec3Add(normalizedPos, pos);
				SPVec3 posNormal = spVec3Normalize(offsetVec);

				SPVec3 lookup = {(posNormal.x + 1.2) * 100.9, (posNormal.y + 1.3) * 100.9, (posNormal.z + 2.4)  * 100.9 + threadState->worldTime * cloudWorldTimeInfluence};
				double noiseValue = spNoiseGet(threadState->spNoise, lookup, 2);

				if(noiseValue > 0.0)
				{
					double altitude = 1.0 + cumulusLargeAltitude + 0.0000002 * counter;
					state.p = spVec3Mul(posNormal,altitude);
					state.particleTextureType = (counter % 3);
					state.randomValueA = spRandGetValue(spRand);
					state.scale = (randVec.z + 0.5) * cumulusLargeScale * (noiseValue + 1.0);
					state.userData.x = altitude;

					state.v = spVec3Mul(right,cloudFieldSize);

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupCloud,
						&state);

					state.particleTextureType = (counter % 3) + 4;

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupCloud,
						&state);

					counter++;
				}
			}
		}
	}

	SPVec3 cirrusNorth = {0.9, -0.5, 0.0};
	cirrusNorth = spVec3Normalize(cirrusNorth);
	SPVec3 cirrusRight = spVec3Normalize(spVec3Cross(cirrusNorth, normalizedPos));
	SPVec3 cirrusNegZ = spVec3Normalize(spVec3Cross(normalizedPos, cirrusRight));

	//cirrus
	counter = 0;
	for(int y = 0; y < cirrusCloudGridCount; y++)
	{
		for(int x = 0; x < cirrusCloudGridCount; x++)
		{

			SPVec3 randVec = spRandomVec3ForUniqueID(spRand, ((uint64_t)(threadState->worldTime * 0.1)) + 79157 + y * cirrusCloudGridCount + x);
			if(randVec.z > 0.2 && randVec.z < 0.4)
			{

				double xPosBase = ((((double)x) + 0.5 * randVec.x) / cirrusCloudGridCount);
				double zPosBase = ((((double)y) + 0.5 * randVec.y) / cirrusCloudGridCount);

				state.lifeLeft = xPosBase;

				double xPos = -cloudFieldHalfSize + cloudFieldSize * xPosBase;
				double zPos = -cloudFieldHalfSize + cloudFieldSize * zPosBase;

				SPVec3 pos = spVec3Add(spVec3Mul(cirrusRight, xPos), spVec3Mul(cirrusNegZ, zPos));

				SPVec3 offsetVec = spVec3Add(normalizedPos, pos);
				SPVec3 posNormal = spVec3Normalize(offsetVec);

				double altitude = 1.0 + cirrusCloudAltitude - 0.0000000001 * ((double)counter);
				state.p = spVec3Mul(posNormal, altitude);
				state.randomValueA = spRandGetValue(spRand);
				state.scale = cirrusCloudScale * (1.0 + randVec.z);

				state.v = spVec3Mul(cirrusRight,cloudFieldSize);
				state.userData.x = altitude;

				state.particleTextureType = 9;

				(*threadState->addParticle)(threadState->particleManager,
					emitterState,
					sp_vanillaRenderGroupCloudBlended,
					&state);

				counter++;
			}
		}
	}

	//altocumulus

	SPVec3 altocumulusNorth = {0.5, 1.0, 0.0};
	altocumulusNorth = spVec3Normalize(altocumulusNorth);
	SPVec3 altocumulusRight = spVec3Normalize(spVec3Cross(altocumulusNorth, normalizedPos));
	SPVec3 altocumulusNegZ = spVec3Normalize(spVec3Cross(normalizedPos, altocumulusRight));

	counter = 0;
	for(int y = 0; y < altoCloudGridCount; y++)
	{
		for(int x = 0; x < altoCloudGridCount; x++)
		{
			//SPVec3 randVec = spRandomVec3ForUniqueID(spRand, ((uint64_t)(threadState->worldTime * 0.1)) + 91711  + y * altoCloudGridCount + x);
			SPVec3 randVec = spRandomVec3ForUniqueID(spRand, ((uint64_t)(threadState->worldTime * 0.1)) + 79157 + y * altoCloudGridCount + x);
			if(randVec.z > 0.0 && randVec.z < 0.4)
			{
				double xPosBase = ((((double)x) + 0.5 * randVec.x) / altoCloudGridCount);
				double zPosBase = ((((double)y) + 0.5 * randVec.y) / altoCloudGridCount);

				state.lifeLeft = xPosBase;

				double xPos = -cloudFieldHalfSize + cloudFieldSize * xPosBase;
				double zPos = -cloudFieldHalfSize + cloudFieldSize * zPosBase;

				SPVec3 pos = spVec3Add(spVec3Mul(altocumulusRight, xPos), spVec3Mul(altocumulusNegZ, zPos));

				SPVec3 offsetVec = spVec3Add(normalizedPos, pos);
				SPVec3 posNormal = spVec3Normalize(offsetVec);

				SPVec3 lookup = {(posNormal.x + 1.7) * 100.9, (posNormal.y + 1.6) * 100.9, (posNormal.z + 1.2)  * 100.9 + threadState->worldTime * cloudWorldTimeInfluence};
				double noiseValue = spNoiseGet(threadState->spNoise, lookup, 1);

				if(noiseValue > 0.2)
				{

					int altoType = counter % 3;

					double altitude = 1.0 + altoCloudAltitude - 0.0000001 * ((double)counter) * 0.01;
					state.p = spVec3Mul(posNormal, altitude);
					state.randomValueA = (altoType == 2 ?  2.5 : spRandGetValue(spRand));
					state.scale = (randVec.z + 0.4 - 0.3) * altoCloudScale;

					state.v = spVec3Mul(altocumulusRight,cloudFieldSize);
					state.userData.x = altitude;

					state.particleTextureType = (altoType == 0 ? 8 : (altoType == 1 ? 10 : 11));

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupCloudBlended,
						&state);

					counter++;
				}
			}
		}
	}

	SPVec3 cumulusSmallNorth = {-0.3, 1.0, 0.0};
	cumulusSmallNorth = spVec3Normalize(cumulusSmallNorth);
	SPVec3 cumulusSmallRight = spVec3Normalize(spVec3Cross(cumulusSmallNorth, normalizedPos));
	SPVec3 cumulusSmallNegZ = spVec3Normalize(spVec3Cross(normalizedPos, cumulusSmallRight));


	//counter = 0;
	for(int y = 0; y < cumulusSmallGridCount; y++)
	{
		for(int x = 0; x < cumulusSmallGridCount; x++)
		{
			//SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(120000.0));
			SPVec3 randVec = spRandomVec3ForUniqueID(spRand, ((uint64_t)(threadState->worldTime * 0.1)) + 1487 + y * cumulusSmallGridCount + x);
			//if(randVec.z > 0.2)
			{

				state.lifeLeft = 1.0;

				double xPos = -cloudFieldHalfSize + cloudFieldSize * ((((double)x) + 0.5 * randVec.x) / cumulusSmallGridCount);
				double zPos = -cloudFieldHalfSize + cloudFieldSize * ((((double)y) + 0.5 * randVec.y) / cumulusSmallGridCount);

				SPVec3 pos = spVec3Add(spVec3Mul(cumulusSmallRight, xPos), spVec3Mul(cumulusSmallNegZ, zPos));

				SPVec3 offsetVec = spVec3Add(normalizedPos, pos);
				SPVec3 posNormal = spVec3Normalize(offsetVec);

				SPVec3 lookup = {(posNormal.x + 1.2) * 100.9, (posNormal.y + 1.3) * 100.9, (posNormal.z + 2.4)  * 100.9 + threadState->worldTime * cloudWorldTimeInfluence};
				double noiseValue = spNoiseGet(threadState->spNoise, lookup, 2);

				if(noiseValue > 0.0)
				{

					double altitude = 1.0 + cumulusSmallAltitude;
					state.p = spVec3Mul(posNormal, 1.0 + cumulusSmallAltitude);
					state.particleTextureType = (counter % 16) + 12;
					state.randomValueA = spRandGetValue(spRand);
					state.scale = (randVec.z + 1.2) * cumulusSmallScale * (noiseValue + 0.7);

					state.v = threadState->windVelocity;
					state.userData.x = altitude;

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupCloudWindDirBlown,
						&state);

					state.particleTextureType = (counter % 16) + 12 + 16;

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupCloudBlendedWindDirBlown,
						&state);

					counter++;

					counter++;
				}
			}
		}
	}
}

bool spEmitterWasAdded(SPParticleThreadState* threadState,
	SPParticleEmitterState* emitterState,
	uint32_t localEmitterTypeID)
{
	bool removeImmediately = false;
	SPRand* spRand = threadState->spRand;


	switch(localEmitterTypeID)
	{
	case sp_vanillaEmitterTypeCampfireLarge:
	case sp_vanillaEmitterTypeCampfireMedium:
	case sp_vanillaEmitterTypeCampfireSmall:
	case sp_vanillaEmitterTypeTorchLarge:
	case sp_vanillaEmitterTypeTorchSmall:
	case sp_vanillaEmitterTypeWaterRipples:
	case sp_vanillaEmitterTypeRain:
	case sp_vanillaEmitterTypeKilnFire:
	case sp_vanillaEmitterTypeKilnFireMedium:
	case sp_vanillaEmitterTypeKilnFireSmall:
	{
	
	}

		break;
	case sp_vanillaEmitterTypeWoodChop:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-10.0));


		for(int i = 0; i < 16; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(2.0));
			state.particleTextureType = 6;
			state.lifeLeft = 1.0;
			state.randomValueA = spRandGetValue(spRand);
			state.gravity = gravity;
			state.scale = 0.05 + spRandGetValue(spRand) * 0.2;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupStandard,
				&state);
		}

		for(int i = 0; i < 8; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.1));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(0.4));
			state.particleTextureType = 7;
			state.lifeLeft = 1.0;
			state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
			state.gravity = spVec3Mul(gravity, 0.2);
			state.scale = 0.5 + spRandGetValue(spRand) * 0.3;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupDust,
				&state);
		}
	}
		break;
	case sp_vanillaEmitterTypeDig:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-10.0));


		for(int i = 0; i < 8; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(2.6));
			state.particleTextureType = 6;
			state.lifeLeft = 1.0;
			state.randomValueA = spRandGetValue(spRand);
			state.gravity = gravity;
			state.scale = 0.02 + spRandGetValue(spRand) * 0.1;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupStandard,
				&state);
		}

		for(int i = 0; i < 16; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.1));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(0.8));
			state.particleTextureType = 7;
			state.lifeLeft = 1.0;
			state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
			state.gravity = spVec3Mul(gravity, 0.2);
			state.scale = 1.0 + spRandGetValue(spRand) * 0.3;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupDust,
				&state);
		}
	}
		break;
	case sp_vanillaEmitterTypePullWeeds:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-10.0));


		//SPVec3 pullDirectionLocal = {-0.2,2.0,-1.0};
		SPVec3 pullDirection = spVec3Mul(spMat3GetRow(emitterState->rot, 2), -1.0);
		pullDirection = spVec3Add(pullDirection, spVec3Mul(spMat3GetRow(emitterState->rot, 0), -1.0)); 
		pullDirection = spVec3Add(pullDirection, normalizedPos); 
		pullDirection = spVec3Normalize(pullDirection);
		//SPVec3 pullDirection = spVec3xMat3(pullDirectionLocal, emitterState->rot);
		//SPVec3 pullDirection = spMat3GetRow(emitterState->rot, 1);

		for(int i = 0; i < 8; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spVec3Mul(spRandGetVec3(spRand), 0.5);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(pullDirection, randVelVec), SP_METERS_TO_PRERENDER(2.0));
			state.particleTextureType = 6;
			state.lifeLeft = 1.0;
			state.randomValueA = spRandGetValue(spRand);
			state.gravity = gravity;
			state.scale = 0.02 + spRandGetValue(spRand) * 0.1;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupStandard,
				&state);
		}


		for(int i = 0; i < 8; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spRandGetVec3(spRand);

			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(pullDirection, randVelVec), SP_METERS_TO_PRERENDER(2.0));
			state.particleTextureType = 10;
			state.lifeLeft = 1.0;
			state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.2;
			state.gravity = gravity;
			state.scale = 0.2 + spRandGetValue(spRand) * 0.4;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupStandard,
				&state);
		}

		for(int i = 0; i < 4; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.1));
			SPVec3 randVelVec = spVec3Mul(spRandGetVec3(spRand), 0.5);

			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(pullDirection, randVelVec), SP_METERS_TO_PRERENDER(1.0));
			state.particleTextureType = 7;
			state.lifeLeft = 1.0;
			state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
			state.gravity = spVec3Mul(gravity, 0.2);
			state.scale = 1.0 + spRandGetValue(spRand) * 0.3;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupDust,
				&state);
		}
	}
	break;
	case sp_vanillaEmitterTypeDestroy:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-10.0) * 1.0);

		SPVec3 direction = spMat3GetRow(emitterState->rot, 2);//spVec3Mul(spMat3GetRow(emitterState->rot, 2), 0.5);
		direction = spVec3Add(direction, normalizedPos); 
		direction = spVec3Normalize(direction);

		for(int i = 0; i < 20; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.3));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(direction, randVelVec), SP_METERS_TO_PRERENDER(1.6));
			state.particleTextureType = 7;
			state.lifeLeft = 1.0;
			state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
			state.gravity = gravity;
			state.scale = 0.5 + spRandGetValue(spRand) * 0.5;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupDust,
				&state);
		}
	}
	break;
	case sp_vanillaEmitterTypeDestroyLarge:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		//SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-10.0) * 1.0);

		SPVec3 direction = spMat3GetRow(emitterState->rot, 2);//spVec3Mul(spMat3GetRow(emitterState->rot, 2), 0.5);
		direction = spVec3Add(direction, normalizedPos); 
		direction = spVec3Normalize(direction);

		for(int i = 0; i < 100; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.5));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(direction, randVelVec), SP_METERS_TO_PRERENDER(0.5));
			state.particleTextureType = 7;
			double lifeLeftAddition = spRandGetValue(spRand);
			state.lifeLeft = 1.0 + lifeLeftAddition * lifeLeftAddition * 0.5;
			state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
			SPVec3 zeroVec = {0,0,0};
			state.gravity = zeroVec;
			state.scale = 0.001;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupSlowReleaseDust,
				&state);
		}

		for(int i = 0; i < 20; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.5));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(direction, randVelVec), SP_METERS_TO_PRERENDER(1.0));
			state.particleTextureType = 6;
			double lifeLeftAddition = spRandGetValue(spRand);
			state.lifeLeft = 1.0 + lifeLeftAddition * lifeLeftAddition * 0.5;
			state.randomValueA = (0.5 + (spRandGetValue(spRand) - 0.5)) * 5.0;
			SPVec3 zeroVec = {0,0,0};
			state.gravity = zeroVec;
			state.scale = 0.001;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupSlowReleaseDust,
				&state);
		}

		/*for(int i = 0; i < 20; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(2.6));
			state.particleTextureType = 6;
			state.lifeLeft = 4.0;
			state.randomValueA = spRandGetValue(spRand);
			state.gravity = gravity;
			state.scale = 0.02 + spRandGetValue(spRand) * 0.1;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupStandard,
				&state);
		}*/
	}
	break;
	case sp_vanillaEmitterTypeFeathers:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-2.0));

		for(int i = 0; i < 8; i++)
		{
			SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.2));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.25)), randPosVec);
			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(1.0));
			state.particleTextureType = 4;
			state.lifeLeft = 1.0;
			state.randomValueA = spRandGetValue(spRand);
			state.gravity = gravity;

			(*threadState->addParticle)(threadState->particleManager,
		emitterState,
				sp_vanillaRenderGroupStandard,
				&state);
		}
	}
	case sp_vanillaEmitterTypeClouds:
	{
		addClouds(threadState, emitterState);
	}
	break;
	case sp_vanillaEmitterTypeBlacksmithSparks:
	{
		removeImmediately = true;
		double posLength = spVec3Length(emitterState->p);
		SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
		SPVec3 gravity = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(-10.0) * 0.2);

		SPVec3 direction = spMat3GetRow(emitterState->rot, 2);//spVec3Mul(spMat3GetRow(emitterState->rot, 2), 0.5);
		direction = spVec3Add(direction, normalizedPos); 
		direction = spVec3Normalize(direction);

		for(int i = 0; i < 20; i++)
		{
			SPVec3 randVec = spRandGetVec3(spRand);
			SPVec3 randPosVec = spVec3Mul(randVec, SP_METERS_TO_PRERENDER(0.05));
			SPVec3 randVelVec = spVec3Mul(randVec, SP_METERS_TO_PRERENDER(1.0));

			SPParticleState state;

			double heightOffsetMeters = -0.2;

			state.p = spVec3Add(emitterState->p, randPosVec);


			state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(2.0 + spRandGetValue(spRand) * 0.5) * 0.5);
			state.particleTextureType = 3;
			state.lifeLeft = 0.9;
			state.scale = 0.01 + spRandGetValue(spRand) * 0.01;
			state.randomValueA = spRandGetValue(spRand);
			state.randomValueB = spRandGetValue(spRand);
			state.gravity = spVec3Add(gravity, spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(1.0)));

			/*if(!emitterState->covered)
			{
				state.v = spVec3Add(state.v, spVec3Mul(threadState->windVelocity, 0.1));
			}*/

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupSpark,
				&state);

			/*SPParticleState state;
			SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.3));
			SPVec3 randVelVec = spRandGetVec3(spRand);
			state.p = spVec3Add(spVec3Mul(normalizedPos, posLength), randPosVec);
			state.v = spVec3Mul(spVec3Add(direction, randVelVec), SP_METERS_TO_PRERENDER(1.6));
			state.particleTextureType = 7;
			state.lifeLeft = 1.0;
			state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
			state.gravity = gravity;
			state.scale = 0.5 + spRandGetValue(spRand) * 0.5;

			(*threadState->addParticle)(threadState->particleManager,
				emitterState,
				sp_vanillaRenderGroupSpark,
				&state);*/
		}
	}
	break;
	}

	if(!removeImmediately)
	{
		for(int i = 0; i < 4; i++)
		{
			emitterState->counters[i] = 0;
		}
		emitterState->timeAccumulatorA = 0.0;
		emitterState->timeAccumulatorB = 0.0;
	}

	return removeImmediately;
}

void emitFireParticle(SPParticleThreadState* threadState,
	SPParticleEmitterState* emitterState,
	double scaleAverage,
	SPVec3 randPosVec,
	bool addWind)
{
	SPRand* spRand = threadState->spRand;
	SPParticleState state;

	double posLength = spVec3Length(emitterState->p);
	SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);

	state.p = spVec3Add(spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.1)), randPosVec);
	state.v = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(0.2 + spRandGetValue(spRand) * scaleAverage));

	if(addWind)
	{
		state.v = spVec3Mix(state.v, spVec3Mul(threadState->windVelocity, 0.5 * scaleAverage), spClamp(threadState->windStrength * 0.25, 0.0, 0.67));
	}
	state.particleTextureType = ((spRandGetValue(spRand) > 0.5) ? 1 : 4);
	state.lifeLeft = 1.0;
	state.scale = scaleAverage + spRandGetValue(spRand) * 0.2;
	state.randomValueA = spRandGetValue(spRand);
	state.randomValueB = spRandGetValue(spRand);

	(*threadState->addParticle)(threadState->particleManager,
		emitterState,
		sp_vanillaRenderGroupFire,
		&state);
}

static const double fixedTimeStep = 1.0/60.0;


void spEmitterWasUpdated(SPParticleThreadState* threadState,
	SPParticleEmitterStateUpdate* updatedState,
	SPParticleEmitterState* emitterState,
	uint32_t localEmitterTypeID)
{
	emitterState->p = updatedState->p;
	emitterState->rot = updatedState->rot;
	emitterState->userData = updatedState->userData;
}

void spUpdateEmitter(SPParticleThreadState* threadState,
	SPParticleEmitterState* emitterState,
	uint32_t localEmitterTypeID,
	double dt)
{
	// An emitter is marked as complete when it is removed, either by returning removeImmediately from emitterWasAdded, or by being removed manually later. 
	// It may still have active particles though, and this function will still be called in that case.
	if(emitterState->complete) 
	{
		return;
	}
	SPRand* spRand = threadState->spRand;

	emitterState->timeAccumulatorA += dt;

	while(emitterState->timeAccumulatorA > 0.0)
	{
		emitterState->timeAccumulatorA -= fixedTimeStep;
		emitterState->timeAccumulatorB += fixedTimeStep;
		switch(localEmitterTypeID)
		{
		case sp_vanillaEmitterTypeWaterRipples:
		{
			if(emitterState->counters[0] == 0)
			{
				double posLength = spVec3Length(emitterState->p);
				SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
				SPVec3 randVec = spRandGetVec3(spRand);
				SPVec3 randPosVec = spVec3Mul(randVec, SP_METERS_TO_PRERENDER(0.2));

				SPParticleState state;

				SPVec3 zeroVec = {0,0,0};

				SPVec3 posVec = spVec3Add(normalizedPos, randPosVec);

				state.p = spVec3Normalize(posVec);
				state.v = zeroVec;
				state.particleTextureType = 3;
				state.lifeLeft = 1.0;
				state.scale = 0.2 + spRandGetValue(spRand) * 0.2;
				state.randomValueA = spRandGetValue(spRand);
				state.randomValueB = spRandGetValue(spRand);
				state.gravity = zeroVec;

				(*threadState->addParticle)(threadState->particleManager,
					emitterState,
					sp_vanillaRenderGroupWaterRipples,
					&state);

				SPVec3 lookup = {(normalizedPos.x + 1.2) * 99999.9, ((normalizedPos.y + 1.2) * 4.5 + normalizedPos.z + 2.4) * 99999.9, emitterState->timeAccumulatorB * 0.1};
				double noiseValue = spNoiseGet(threadState->spNoise, lookup, 2);
				emitterState->counters[0] = 1 + (uint8_t)(8 * (1.0 - noiseValue));
			}
			emitterState->counters[0]--;
		}
		break;


		case sp_vanillaEmitterTypeDustParticles:
		{
			if(emitterState->userData.x > 0.1)
			{
				SPVec3 right = spMat3GetRow(emitterState->rot, 0);
				SPVec3 up = spMat3GetRow(emitterState->rot, 1);
				SPVec3 forward = spMat3GetRow(emitterState->rot, 2);

				SPVec3 zeroVec = {0,0,0};
				for(int i = 0; i < 1; i++)
				{
					SPParticleState state;
					SPVec3 pos = spVec3Add(emitterState->p, spVec3Mul(right, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.5) * 40.0)));
					double randY = spRandGetValue(spRand);
					pos = spVec3Add(pos, spVec3Mul(up, SP_METERS_TO_PRERENDER((randY * randY) * 40.0)));
					pos = spVec3Add(pos, spVec3Mul(forward, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.5) * 40.0)));
					state.p = pos;

					state.v = zeroVec;
					state.particleTextureType = 20;
					state.lifeLeft = 1.0;
					state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
					state.scale = 0.01;

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupDustParticles,
						&state);
				}
			}
		}
		break;

		case sp_vanillaEmitterTypeSnow:
		{
			if(emitterState->userData.x > 0.0)
			{
				SPVec3 right = spMat3GetRow(emitterState->rot, 0);
				SPVec3 up = spMat3GetRow(emitterState->rot, 1);
				SPVec3 forward = spMat3GetRow(emitterState->rot, 2);

				SPVec3 zeroVec = {0,0,0};
				for(int i = 0; i < 100 * emitterState->userData.x; i++)
				{
					SPParticleState state;
					SPVec3 pos = spVec3Add(emitterState->p, spVec3Mul(right, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.5) * 40.0)));
					pos = spVec3Add(pos, spVec3Mul(up, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.1) * 30.0)));
					pos = spVec3Add(pos, spVec3Mul(forward, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.5) * 40.0)));
					state.p = pos;

					state.v = zeroVec;
					for(int frameAxisIndex = 0; frameAxisIndex < 3; frameAxisIndex++)
					{
						double windStrength = 8.0 + threadState->windStrength * 20.0;
						SPVec3 lookup = {(pos.x + 1.2 + 0.1 * frameAxisIndex) * 1999999.9, (pos.y + 1.1 + 0.1 * frameAxisIndex) * 1999999.9, emitterState->timeAccumulatorB * 0.1 + (pos.z + 1.1 + 0.1 * frameAxisIndex) * 1999999.9};
						double noiseValue = spNoiseGet(threadState->spNoise, lookup, 1);

						if(frameAxisIndex == 1) //up/down
						{
							noiseValue -= 0.5;
						}
						state.v = spVec3Add(state.v, spVec3Mul(spMat3GetRow(emitterState->rot, frameAxisIndex), SP_METERS_TO_PRERENDER(noiseValue * windStrength) * dt));
						state.v = spVec3Add(state.v, spVec3Mul(threadState->windVelocity, dt * 6.0));
					}

					state.particleTextureType = 21;
					state.lifeLeft = 1.0;
					state.randomValueA = 0.5 + (spRandGetValue(spRand) - 0.5) * 0.3;
					state.scale = 0.05;

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupSnow,
						&state);
				}
			}
		}
		break;

		case sp_vanillaEmitterTypeRain:
		{
			if(emitterState->userData.x > 0.0)
			{
				SPVec3 right = spMat3GetRow(emitterState->rot, 0);
				SPVec3 up = spMat3GetRow(emitterState->rot, 1);
				SPVec3 forward = spMat3GetRow(emitterState->rot, 2);

				for(int i = 0; i < 100 * emitterState->userData.x; i++)
				{
					SPParticleState state;
					SPVec3 pos = spVec3Add(emitterState->p, spVec3Mul(right, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.5) * 40.0)));
					pos = spVec3Add(pos, spVec3Mul(up, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.05) * 40.0)));
					pos = spVec3Add(pos, spVec3Mul(forward, SP_METERS_TO_PRERENDER((spRandGetValue(spRand) - 0.5) * 40.0)));
					state.p = pos;

					state.v = spVec3Mul(up, SP_METERS_TO_PRERENDER(-10.0));
					state.v = spVec3Add(state.v, spVec3Mul(threadState->windVelocity, 2.0));

					double randValue = spRandGetValue(spRand);
					state.lifeLeft = 1.0;
					state.randomValueA = 0.5 + (randValue - 0.5) * 0.3;
					state.scale = 0.01;

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupRain,
						&state);


					state.particleTextureType = 22;
					state.scale = 0.02 + 0.02 * randValue;

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupRainBounce,
						&state);
				}
			}
		}
		break;

		case sp_vanillaEmitterTypeCampfireLarge:
		case sp_vanillaEmitterTypeCampfireMedium:
		case sp_vanillaEmitterTypeCampfireSmall:
		case sp_vanillaEmitterTypeTorchLarge:
		case sp_vanillaEmitterTypeTorchSmall:
		case sp_vanillaEmitterTypeKilnFire:
		case sp_vanillaEmitterTypeKilnFireMedium:
		case sp_vanillaEmitterTypeKilnFireSmall:
		{
			if(emitterState->counters[0] == 0) // SMOKE
			{
				double posLength = spVec3Length(emitterState->p);
				SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
				SPParticleState state;


				SPVec3 randPosVec;
				if(localEmitterTypeID == sp_vanillaEmitterTypeTorchLarge || localEmitterTypeID == sp_vanillaEmitterTypeTorchSmall)
				{
					randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.02));
					randPosVec = spVec3Add(randPosVec, spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.1)));
				}
				else
				{
					randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.1));
					randPosVec = spVec3Add(randPosVec, spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(0.2)));

					if(localEmitterTypeID == sp_vanillaEmitterTypeKilnFire || localEmitterTypeID == sp_vanillaEmitterTypeKilnFireMedium || localEmitterTypeID == sp_vanillaEmitterTypeKilnFireSmall)
					{
						randPosVec = spVec3Add(randPosVec, spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(1.5)));
					}
				}

				state.p = randPosVec;

				state.particleTextureType = ((spRandGetValue(spRand) > 0.5) ? 2 : 5);
				state.lifeLeft = 1.0;
				state.scale = 0.2 + spRandGetValue(spRand) * 0.2;
				state.randomValueA = spRandGetValue(spRand);
				state.randomValueB = spRandGetValue(spRand);
				if(localEmitterTypeID == sp_vanillaEmitterTypeTorchLarge || localEmitterTypeID == sp_vanillaEmitterTypeTorchSmall)
				{
					//state.lifeLeft = 0.2;
					state.randomValueB += 8.0;
					//state.scale = state.scale * 0.5;
				}
				SPVec3 lookup = {(normalizedPos.x + 1.2) * 99999.9, ((normalizedPos.y + 1.2) * 4.5 + (normalizedPos.z + 1.2) + 2.4) * 99999.9, emitterState->timeAccumulatorB * 0.1};
				SPVec3 lookupB = {(normalizedPos.x + 1.4) * 99999.9, ((normalizedPos.y + 1.2) * 4.6 + (normalizedPos.z + 1.2) + 2.8) * 99999.9, emitterState->timeAccumulatorB * 0.1};
				SPVec3 lookupC = {(normalizedPos.x + 1.8) * 99999.9, ((normalizedPos.y + 1.2) * 4.8 + (normalizedPos.z + 1.2) + 2.9) * 99999.9, emitterState->timeAccumulatorB * 0.5};
				double noiseValue = spNoiseGet(threadState->spNoise, lookup, 2);
				double noiseValueB = spNoiseGet(threadState->spNoise, lookupB, 2);
				double noiseValueC = spNoiseGet(threadState->spNoise, lookupC, 2);

				state.v = spVec3Mul(normalizedPos, SP_METERS_TO_PRERENDER(0.9 + noiseValueC) * 0.7);
				if(!emitterState->covered)
				{
					state.v = spVec3Mix(state.v, spVec3Mul(threadState->windVelocity, 0.1), spClamp(threadState->windStrength * 0.25, 0.0, 0.67));
				}

				state.gravity = spVec3Mul(spMat3GetRow(emitterState->rot, 0), SP_METERS_TO_PRERENDER(noiseValue) * 2.5);
				state.gravity = spVec3Add(state.gravity, spVec3Mul(spMat3GetRow(emitterState->rot, 2), SP_METERS_TO_PRERENDER(noiseValueB) * 2.5));

				(*threadState->addParticle)(threadState->particleManager,
					emitterState,
					sp_vanillaRenderGroupSmoke,
					&state);

				emitterState->counters[0] = 1 + (uint8_t)(4 * (1.0 - noiseValueC));
			}
			else
			{
				emitterState->counters[0]--;
			}

			float quantityMultiplier = 1.0f;
			float scaleMultiplier = 1.0f;
			bool windAffected = (localEmitterTypeID != sp_vanillaEmitterTypeKilnFire && localEmitterTypeID != sp_vanillaEmitterTypeKilnFireSmall && localEmitterTypeID != sp_vanillaEmitterTypeKilnFireMedium) && !emitterState->covered;
			if(localEmitterTypeID == sp_vanillaEmitterTypeCampfireLarge || localEmitterTypeID == sp_vanillaEmitterTypeKilnFire)
			{
				quantityMultiplier = 3.0f;
				scaleMultiplier = 1.5f;
			}
			else if(localEmitterTypeID == sp_vanillaEmitterTypeCampfireSmall || localEmitterTypeID == sp_vanillaEmitterTypeKilnFireSmall)
			{
				quantityMultiplier = 0.5f;
				scaleMultiplier = 0.5f;
			}
			else if(localEmitterTypeID == sp_vanillaEmitterTypeTorchLarge)
			{
				quantityMultiplier = 0.5f;
				scaleMultiplier = 0.3f;
			}
			else if(localEmitterTypeID == sp_vanillaEmitterTypeTorchSmall)
			{
				quantityMultiplier = 0.25f;
				scaleMultiplier = 0.2f;
			}

			if(emitterState->counters[1] == 0) //FLAME 1
			{
				SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.04) * scaleMultiplier);
				double scaleAverage = 0.3 * scaleMultiplier;

				emitFireParticle(threadState,
					emitterState,
					scaleAverage,
					randPosVec,
					windAffected);

				emitterState->counters[1] = 2 + (uint8_t)(10 * spRandGetValue(spRand) / quantityMultiplier);

			}
			else
			{
				emitterState->counters[1]--;
			}

			if(emitterState->counters[2] == 0) //FLAME 2
			{
				SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.04) * scaleMultiplier);
				randPosVec = spVec3Add(randPosVec, spVec3Mul(spMat3GetRow(emitterState->rot, 0), SP_METERS_TO_PRERENDER(0.2) * scaleMultiplier));
				double scaleAverage = 0.15 * scaleMultiplier;

				emitFireParticle(threadState, 
					emitterState,
					scaleAverage,
					randPosVec,
					windAffected);

				emitterState->counters[2] = 2 + (uint8_t)(10 * spRandGetValue(spRand) / quantityMultiplier);

			}
			else
			{
				emitterState->counters[2]--;
			}

			if(emitterState->counters[3] == 0) //FLAME 3
			{
				SPVec3 randPosVec = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(0.04) * scaleMultiplier);
				randPosVec = spVec3Add(randPosVec, spVec3Mul(spMat3GetRow(emitterState->rot, 2), SP_METERS_TO_PRERENDER(0.1) * scaleMultiplier));
				double scaleAverage = 0.1 * scaleMultiplier;

				emitFireParticle(threadState, 
					emitterState,
					scaleAverage,
					randPosVec,
					windAffected);

				emitterState->counters[3] = 3 + (uint8_t)(10 * spRandGetValue(spRand) / quantityMultiplier);
			}
			else 
			{
				if(emitterState->counters[3] == 6 / quantityMultiplier) //spark
				{

					double posLength = spVec3Length(emitterState->p);
					SPVec3 normalizedPos = spVec3Div(emitterState->p, posLength);
					SPVec3 randVec = spRandGetVec3(spRand);
					SPVec3 randPosVec = spVec3Mul(randVec, SP_METERS_TO_PRERENDER(0.1));
					SPVec3 randVelVec = spVec3Mul(randVec, SP_METERS_TO_PRERENDER(0.4));

					SPParticleState state;

					double heightOffsetMeters = 0.1;
					if(localEmitterTypeID == sp_vanillaEmitterTypeKilnFire || localEmitterTypeID == sp_vanillaEmitterTypeKilnFireSmall || localEmitterTypeID == sp_vanillaEmitterTypeKilnFireMedium)
					{
						heightOffsetMeters = 1.7;
					}

					state.p = spVec3Add(spVec3Mul(normalizedPos, posLength + SP_METERS_TO_PRERENDER(heightOffsetMeters)), randPosVec);


					state.v = spVec3Mul(spVec3Add(normalizedPos, randVelVec), SP_METERS_TO_PRERENDER(2.0 + spRandGetValue(spRand) * 0.5) * 0.5);
					state.particleTextureType = 3;
					state.lifeLeft = 1.0;
					state.scale = 0.01 + spRandGetValue(spRand) * 0.02;
					state.randomValueA = spRandGetValue(spRand);
					state.randomValueB = spRandGetValue(spRand);
					state.gravity = spVec3Mul(spRandGetVec3(spRand), SP_METERS_TO_PRERENDER(1.0));

					if(!emitterState->covered)
					{
						state.v = spVec3Add(state.v, spVec3Mul(threadState->windVelocity, 0.5));
					}

					(*threadState->addParticle)(threadState->particleManager,
						emitterState,
						sp_vanillaRenderGroupSpark,
						&state);
				}
				emitterState->counters[3]--;
			}

		}
		break;
		}
	}
}

static const SPVec2 texCoords[4] = {
	{0.0,0.0},
	{0.0,1.0},
	{1.0,1.0},
	{1.0,0.0},
};

void constrainToCamera(SPParticleEmitterState* emitterState, SPParticleState* particleState)
{
	static const double maxDistance = SP_METERS_TO_PRERENDER(16.0);

	double xDistance = particleState->p.x - emitterState->p.x;
	double yDistance = particleState->p.y - emitterState->p.y;
	double zDistance = particleState->p.z - emitterState->p.z;

	if(xDistance > maxDistance)
	{
		double difference = fmod(xDistance, maxDistance);
		particleState->p.x = emitterState->p.x - maxDistance + difference;
	}
	else if(xDistance < -maxDistance)
	{
		double difference = fmod(-xDistance, maxDistance);
		particleState->p.x = emitterState->p.x + maxDistance - difference;
	}
	if(yDistance > maxDistance)
	{
		double difference = fmod(yDistance, maxDistance);
		particleState->p.y = emitterState->p.y - maxDistance + difference;
	}
	else if(yDistance < -maxDistance)
	{
		double difference = fmod(-yDistance, maxDistance);
		particleState->p.y = emitterState->p.y + maxDistance - difference;
	}
	if(zDistance > maxDistance)
	{
		double difference = fmod(zDistance, maxDistance);
		particleState->p.z = emitterState->p.z - maxDistance + difference;
	}
	else if(zDistance < -maxDistance)
	{
		double difference = fmod(-zDistance, maxDistance);
		particleState->p.z = emitterState->p.z + maxDistance - difference;
	}
}


bool spUpdateParticle(SPParticleThreadState* threadState, 
	SPParticleEmitterState* emitterState,
	SPParticleState* particleState, 
	uint32_t localRenderGroupTypeID,
	double dt, 
	SPVec3 origin, 
	float* renderBuffer)
{
	if(localRenderGroupTypeID == sp_vanillaRenderGroupCloud || localRenderGroupTypeID == sp_vanillaRenderGroupCloudBlended || 
		localRenderGroupTypeID == sp_vanillaRenderGroupCloudWindDirBlown || localRenderGroupTypeID == sp_vanillaRenderGroupCloudBlendedWindDirBlown)
	{

		if(localRenderGroupTypeID == sp_vanillaRenderGroupCloudWindDirBlown || localRenderGroupTypeID == sp_vanillaRenderGroupCloudBlendedWindDirBlown)
		{
			particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.05);
			particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt));
			particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v,  dt * 0.5));

		}
		else
		{
			particleState->lifeLeft -= dt * cloudVelocity;
			if(particleState->lifeLeft < 0.0)
			{
				particleState->lifeLeft += 1.0;
				particleState->p = spVec3Add(particleState->p, particleState->v);
			}
			else
			{
				particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v,  -dt * cloudVelocity));
			}
		}

		//particleState->p = spVec3Mul(spVec3Normalize(particleState->p), particleState->userData.x);

		SPVec3 posLocal = spVec3Mul(spVec3Sub(particleState->p, origin), SP_RENDER_SCALE);
		for(int v = 0; v < 4; v++)
		{
			renderBuffer[v * 9 + 0] = posLocal.x;
			renderBuffer[v * 9 + 1] = posLocal.y;
			renderBuffer[v * 9 + 2] = posLocal.z;
			renderBuffer[v * 9 + 3] = texCoords[v].x;
			renderBuffer[v * 9 + 4] = texCoords[v].y;
			renderBuffer[v * 9 + 5] = particleState->particleTextureType;
			renderBuffer[v * 9 + 6] = particleState->lifeLeft;
			renderBuffer[v * 9 + 7] = particleState->randomValueA;
			renderBuffer[v * 9 + 8] = particleState->scale;
		}

		return true;
	}


	double lifeLeftMultiplier = 1.0;

	if(localRenderGroupTypeID == sp_vanillaRenderGroupSmoke)
	{
		lifeLeftMultiplier = 0.05 + particleState->randomValueB * 0.1;
		if(!emitterState->covered)
		{
			lifeLeftMultiplier += threadState->windStrength * 0.1;
		}
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupFire)
	{
		lifeLeftMultiplier = (2.0 - particleState->randomValueB * 0.5);
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupSpark)
	{
		lifeLeftMultiplier = (1.5 - particleState->randomValueB * 1.0);
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupWaterRipples)
	{
		lifeLeftMultiplier = 0.5;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupDust)
	{
		lifeLeftMultiplier = 1.0;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupDustParticles)
	{
		lifeLeftMultiplier = 0.05;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupSnow)
	{
		lifeLeftMultiplier = 0.2;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupRainBounce)
	{
		lifeLeftMultiplier = 5.0;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupSlowReleaseDust)
	{
		lifeLeftMultiplier = 0.5;
	}

	

	double lifeLeft = particleState->lifeLeft - dt * lifeLeftMultiplier;


	particleState->lifeLeft = lifeLeft;
	if(lifeLeft < 0.0)
	{
		return false;
	}

	if(localRenderGroupTypeID == sp_vanillaRenderGroupFire)
	{
		if(!emitterState->covered)
		{
			particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt * 0.1));
		}
		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, (2.0 - lifeLeft) * dt * 1.5));
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupSmoke)
	{
		particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.05);
		if(!emitterState->covered)
		{
			particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt));
		}

		SPVec3 vel = spVec3Add(particleState->v, spVec3Mul(particleState->gravity, (1.0 - particleState->lifeLeft))); //gravity is random wind

		particleState->p = spVec3Add(particleState->p, spVec3Mul(vel, dt));

		double windStrengthToUse = 0.0;
		if(!emitterState->covered)
		{
			windStrengthToUse = threadState->windStrength;
		}

		particleState->scale = particleState->scale + dt * (particleState->lifeLeft * particleState->lifeLeft) * (1.0 + particleState->randomValueA) * 0.15 * (1.0 + windStrengthToUse);
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupDust)
	{
		particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.05);
		if(!emitterState->covered)
		{
			particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt));
		}
		particleState->v = spVec3Add(particleState->v, spVec3Mul(particleState->gravity, dt));

		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, dt));

		particleState->scale = particleState->scale + dt * (particleState->lifeLeft * particleState->lifeLeft) * (1.0 + particleState->randomValueA) * 0.5;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupSlowReleaseDust)
	{
		if(particleState->lifeLeft < 1.0)
		{
			particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.05);
			if(!emitterState->covered)
			{
				particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt));
			}

			particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, dt));

			if(particleState->particleTextureType == 6)
			{
				particleState->scale = spMax(particleState->scale, 0.2) + dt * (particleState->lifeLeft * particleState->lifeLeft) * (1.0 + particleState->randomValueA) * 0.1;
			}
			else
			{
				particleState->scale = spMax(particleState->scale, 0.2) + dt * (particleState->lifeLeft * particleState->lifeLeft) * (1.0 + particleState->randomValueA) * 2.0;
			}

		}
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupDustParticles)
	{

		SPVec3 pos = particleState->p;

		uint8_t frameAxisIndex = threadState->frameCounter % 3; // add velocity to each axis in a round robbin, so only 1 (expensive) noise lookup is required per particle, per frame
		double windStrength = 1.0;
		if(frameAxisIndex == 1) //up/down
		{
			windStrength = 0.3;
		}

		SPVec3 lookup = {(pos.x + 1.2 + 0.1 * frameAxisIndex) * 999999.9, (pos.y + 1.1 + 0.1 * frameAxisIndex) * 999999.9, emitterState->timeAccumulatorB * 0.2 + (pos.z + 1.1 + 0.1 * frameAxisIndex) * 999999.9};
		double noiseValue = spNoiseGet(threadState->spNoise, lookup, 1);

		particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.2);
		particleState->v = spVec3Add(particleState->v, spVec3Mul(spMat3GetRow(emitterState->rot, frameAxisIndex), SP_METERS_TO_PRERENDER(noiseValue * windStrength) * dt));
		particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt * 0.1));

		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, dt));

		constrainToCamera(emitterState, particleState);

		double rawScale = (1.0 - fabs(particleState->lifeLeft - 0.5) / 0.5);
		particleState->scale = spMin(rawScale * 2.0, 1.0) * 0.1 * particleState->randomValueA + 0.01;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupSnow)
	{
		uint8_t frameAxisIndex = ((int)(particleState->randomValueA * 100 + threadState->frameCounter)) % 27; // add velocity to each axis in a round robbin, so only 1 (expensive) noise lookup is required per particle, per frame
		if(frameAxisIndex < 3)
		{
			SPVec3 pos = particleState->p;
			double windStrength = 8.0 + threadState->windStrength * 20.0;

			SPVec3 lookup = {(pos.x + 1.2 + 0.1 * frameAxisIndex) * 1999999.9, (pos.y + 1.1 + 0.1 * frameAxisIndex) * 1999999.9, emitterState->timeAccumulatorB * 0.1 + (pos.z + 1.1 + 0.1 * frameAxisIndex) * 1999999.9};
			double noiseValue = spNoiseGet(threadState->spNoise, lookup, 1);

			if(frameAxisIndex == 1) //up/down
			{
				noiseValue -= 0.5;
			}

			particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.2);
			particleState->v = spVec3Add(particleState->v, spVec3Mul(spMat3GetRow(emitterState->rot, frameAxisIndex), SP_METERS_TO_PRERENDER(noiseValue * windStrength) * dt));
			particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt * 6.0));
		}

		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, dt));

		constrainToCamera(emitterState, particleState);

		double rawScale = (1.0 - fabs(particleState->lifeLeft - 0.5) / 0.5);
		particleState->scale = spMin(rawScale * 2.0, 1.0) * 0.05 * particleState->randomValueA + 0.05;
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupRain)
	{
		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, dt));
		constrainToCamera(emitterState, particleState);
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupRainBounce)
	{
	}
	else
	{
		particleState->v = spVec3Mul(particleState->v, 1.0 - dt * 0.1);
		if(!emitterState->covered)
		{
			particleState->v = spVec3Add(particleState->v, spVec3Mul(threadState->windVelocity, dt * 0.1));
		}
		particleState->v = spVec3Add(particleState->v, spVec3Mul(particleState->gravity, dt));
		particleState->p = spVec3Add(particleState->p, spVec3Mul(particleState->v, dt));
	}


	SPVec3 posLocal = spVec3Mul(spVec3Sub(particleState->p, origin), SP_RENDER_SCALE);

	int attributeFloatCount = 9;
	SPVec3 upVector;

	if(localRenderGroupTypeID == sp_vanillaRenderGroupRain)
	{
		attributeFloatCount = 12;
		SPVec3 normalVec = spVec3Normalize(particleState->p);
		upVector = spVec3Normalize(spVec3Mix(normalVec, spVec3Normalize(spVec3Neg(threadState->windVelocity)), spClamp(threadState->windStrength * 0.125, 0.0, 0.67)));
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupFire)
	{
		attributeFloatCount = 12;
		SPVec3 normalVec = spVec3Normalize(particleState->p);
		if(!emitterState->covered)
		{
			upVector = spVec3Normalize(spVec3Mix(normalVec, spVec3Normalize(threadState->windVelocity), spClamp(threadState->windStrength * 0.25, 0.0, 0.67)));
		}
		else
		{
			upVector = normalVec;
		}
	}
	else if(localRenderGroupTypeID == sp_vanillaRenderGroupSpark)
	{
		attributeFloatCount = 12;
		upVector = spVec3Normalize(particleState->v);
	}

	for(int v = 0; v < 4; v++)
	{
		renderBuffer[v * attributeFloatCount + 0] = posLocal.x;
		renderBuffer[v * attributeFloatCount + 1] = posLocal.y;
		renderBuffer[v * attributeFloatCount + 2] = posLocal.z;
		renderBuffer[v * attributeFloatCount + 3] = texCoords[v].x;
		renderBuffer[v * attributeFloatCount + 4] = texCoords[v].y;
		renderBuffer[v * attributeFloatCount + 5] = particleState->particleTextureType;
		renderBuffer[v * attributeFloatCount + 6] = particleState->lifeLeft;
		renderBuffer[v * attributeFloatCount + 7] = particleState->randomValueA;
		renderBuffer[v * attributeFloatCount + 8] = particleState->scale;

		if(localRenderGroupTypeID == sp_vanillaRenderGroupRain || localRenderGroupTypeID == sp_vanillaRenderGroupFire || localRenderGroupTypeID == sp_vanillaRenderGroupSpark)
		{
			renderBuffer[v * attributeFloatCount + 9] = upVector.x;
			renderBuffer[v * attributeFloatCount + 10] = upVector.y;
			renderBuffer[v * attributeFloatCount + 11] = upVector.z;
		}
	}
		
	return true;
}