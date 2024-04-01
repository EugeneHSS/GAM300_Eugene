#ifndef ParticleComponent
#define ParticleComponent

#include "ecs/ecs.h"

namespace TDS {


	enum ParticleMesh {
		CUBE = 0,
		SPHERE,
		CAPSULE,
		MAX_MESHES
	};

	struct alignas(16) Particle {
		Vec4 Size{ 10.f,10.f,10.f ,1.f};
		Vec4 Rotation{ 0.f,0.f,0.f,0.f };
		Vec4 Position{ 0.f,0.f,0.f ,1.f};
		Vec4 Velocity{ 0.f,10.f,0.f,0.f };
		Vec4 Acceleration{ 0.f, 10.f, 0.f,0.f };
		Vec4 Color{ 0.f,0.f,1.f ,1.f};
		float Age{ 0.f };
		std::uint32_t isActive{ 0 };
		std::uint32_t padding[2];
	};

	struct alignas(16) ParticleEmitter {
		Vec4 minSpawnoffset{ 0.f,0.f,0.f,0.f };
		Vec4 maxSpawnoffset{ 0.f,0.f,0.f,0.f };
		Vec4 minVelocity{ 0.f,10.f,0.f,0.f };
		Vec4 maxVelocity{ 0.f,15.f,0.f,0.f };
		Vec4 minAcceleration{ 0.f,0.f,0.f,0.f }; 
		Vec4 maxAcceleration{ 0.f,0.f,0.f,0.f };
		Vec4 minSize{ 1.f,1.f,1.f,0.f };
		Vec4 maxSize{ 1.5f,1.5f,1.5f,0.f };
		Vec4 Position{ 0.f,0.f,0.f,0.f };
		Vec4 Color{ 0.f,1.f,1.f,1.f };
		float minlife{ 0.1f };
		float maxlife{ 1.f };
		float padding[2];
	};

	struct alignas(16) Particle_Emitter_PushData {
		ParticleEmitter emitter;
		int particleamt;
		int padding[3];
	};

	class Particle_Component : public IComponent {
	public:
		DLL_API Particle_Component();
		DLL_API ~Particle_Component() = default;


		//getter
		float&				GetMinLife()			{ return Emitter.minlife; };
		float&				GetMaxLife()			{ return Emitter.maxlife; };
		Vec4&				GetMinSpawnoffset()		{ return Emitter.minSpawnoffset; };
		Vec4&				GetMaxSpawnoffset()		{ return Emitter.maxSpawnoffset; };
		Vec4&				GetMinVelocity()		{ return Emitter.minVelocity; };
		Vec4&				GetMaxVelocity()		{ return Emitter.maxVelocity; };
		Vec4&				GetMinAcceleration()	{ return Emitter.minAcceleration; };
		Vec4&				GetMaxAcceleration()	{ return Emitter.maxAcceleration; };
		Vec4&				GetMinSize()			{ return Emitter.minSize; };
		Vec4&				GetMaxSize()			{ return Emitter.maxSize; };
		Vec4&				GetPosition()			{ return Emitter.Position; };
		Vec4&				GetColor()				{ return Emitter.Color; };
		ParticleEmitter&	GetEmitter()			{ return Emitter; }
		ParticleMesh&		GetMeshType()			{ return type; }
		float&				GetSpawnInterval()		{ return spawninterval; }
		float&				GetSpawnTimer()			{ return spawntimer; }
		int&				GetMaxParticles()		{ return maxparticles; }

		//setters
		void SetMinLife(float& input) { Emitter.minlife = input; }
		void SetMaxLife(float& input) { Emitter.maxlife = input; }
		void SetMinSpawnOffset(Vec4& input) { Emitter.minSpawnoffset = input; }
		void SetMaxSpawnOffset(Vec4& input) { Emitter.maxSpawnoffset = input; }
		void SetMinVelocity(Vec4& input) { Emitter.minVelocity = input; }
		void SetMaxVelocity(Vec4& input) { Emitter.maxVelocity = input; }
		void SetMinAcceleration(Vec4& input) { Emitter.minAcceleration = input; }
		void SetMaxAcceleration(Vec4& input) { Emitter.maxAcceleration = input; }
		void SetMinSize(Vec4& input) { Emitter.minSize = input; }
		void SetMaxSize(Vec4& input) { Emitter.maxSize = input; }
		void SetColor(Vec4& input) { Emitter.Color = input; }
		void SetSpawnTimer(float& input) { spawntimer = input; }
		//void SetPosition(Vec3 input)							{ Emitter.Position = input; }

		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND;

	private:
		ParticleEmitter Emitter;
		ParticleMesh type{ CUBE };
		float spawninterval, spawntimer;
		int maxparticles;
	};
	DLL_API Particle_Component* GetParticle_Component(EntityID entityID);
}
#endif