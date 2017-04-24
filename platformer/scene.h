#ifndef VEGICIDE_SCENE_H
#define VEGICIDE_SCENE_H

#include <glm/glm.hpp>

#include "audio/audio-frame.h"
// TODO: Move ECS out of graphics
#include "graphics/ecs.h"

enum class ETexture {
	Beet,
	BeetDead,
	Blood,
	Carrot,
	CarrotDead,
	CrabApple,
	CrabAppleDead,
	Egg,
	Farm,
	Pumpking,
	PumpkingDead,
	Shadow,
	Tiles,
	Title,
	Venus,
	VenusDead,
	Vignette,
	White,
};

enum class EMesh {
	DangerZone,
	Level,
	Square,
	Venus,
};

enum class EShader {
	Opaque,
	Particle,
	Shadow,
};

enum class EPowerup {
	NoPowerup,
	Pounce_4,
	Pounce_7,
	Pounce_10,
	Health_10,
};

struct Venus {
	float pounce_anim;
	float pounce_range;
};

struct PumpKing {
	bool spotted_player;
};

struct SceneEcs : Ecs {
	Components <glm::vec3> positions;
	// True if activated
	Components <bool> eggs;
	Components <EPowerup> powerups;
	Components <glm::vec3> velocities;
	Components <float> radii;
	Components <bool> targeted;
	Components <float> anim_t;
	Components <bool> pouncables;
	
	Components <EcsTrue> pounce_target;
	// Different meaning before and during pounce
	Components <glm::vec2> pounce_vec;
	Components <glm::vec3> pounce_vel;
	// Doubles as invuln frames on enemies with big health
	Components <long> damage_flash;
	Components <bool> dead;
	Components <int> hp;
	Components <ESound> pain_sound;
	Components <ESound> death_sound;
	
	// Enemies
	Components <EcsTrue> beetniks;
	Components <EcsTrue> carrots;
	Components <EcsTrue> crabapples;
	Components <PumpKing> pumpkings;
	
	Components <Venus> venuses;
	Components <EcsTrue> player_input;
	Components <bool> ai_active;
	
	glm::vec2 camera;
	glm::vec2 last_walk;
	float screenshake_t;
	
	AudioFrame audio_frame;
	
	// wew
	long respawn_time;
	
	void play_sound (ESound);
};

#endif
