#include "components/ParticleComponent.h"
#include "components/CompReflectionMacro.h"


RTTR_REGISTRATION
{
	using namespace TDS;

	rttr::registration::class_<Particle_Component>("Particle Component")
		.property("MinLife", &Particle_Component::GetMinLife, &Particle_Component::SetMinLife)
		.property("MaxLife", &Particle_Component::GetMaxLife, &Particle_Component::SetMaxLife)
		.property("Spawn Offset", &Particle_Component::GetMinSpawnoffset, &Particle_Component::SetMinSpawnOffset)
		.property("Spawn Offset", &Particle_Component::GetMaxSpawnoffset, &Particle_Component::SetMaxSpawnOffset)
		.property("Velocity", &Particle_Component::GetMinVelocity, &Particle_Component::SetMinVelocity)
		.property("Velocity", &Particle_Component::GetMaxVelocity, &Particle_Component::SetMaxVelocity)
		.property("Acceleration", &Particle_Component::GetMinAcceleration, &Particle_Component::SetMinAcceleration)
		.property("Acceleration", &Particle_Component::GetMaxAcceleration, &Particle_Component::SetMaxAcceleration)
		.property("Size", &Particle_Component::GetMinSize, &Particle_Component::SetMinSize)
		.property("Size", &Particle_Component::GetMaxSize, &Particle_Component::SetMaxSize)
		.property("Color", &Particle_Component::GetColor, &Particle_Component::SetColor)
		.property("ParticleMesh", &Particle_Component::type)
		.property("SpawnInterval", &Particle_Component::spawninterval)
		.property("SpawnTimer", &Particle_Component::spawntimer)
		.property("MaxParticles", &Particle_Component::maxparticles);

	rttr::registration::enumeration<ParticleMesh>("ParticleMesh")
		(
			rttr::value("CUBE", ParticleMesh::CUBE),
			rttr::value("SPHERE", ParticleMesh::SPHERE),
			rttr::value("CAPSULE", ParticleMesh::CAPSULE)
		);
}


namespace TDS {
	Particle_Component* GetParticle_Component(EntityID entityID) {
		return ecs.getComponent<Particle_Component>(entityID);
	}

	Particle_Component::Particle_Component() : Emitter(), type(CUBE), spawninterval(1.f), spawntimer(128.f), maxparticles(50) {}
}