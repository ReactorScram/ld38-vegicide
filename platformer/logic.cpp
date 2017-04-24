#include "logic.h"

#include <iostream>

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

EPowerup name_to_powerup (string name) {
	if (name == "Pounce_4") {
		return EPowerup::Pounce_4;
	}
	else if (name == "Pounce_7") {
		return EPowerup::Pounce_7;
	}
	else if (name == "Pounce_10") {
		return EPowerup::Pounce_10;
	}
	else if (name == "Health_10") {
		return EPowerup::Health_10;
	}
	else {
		return EPowerup::NoPowerup;
	}
}

void place_beetnik (SceneEcs & scene, const vec3 & pos) {
	auto e = scene.add_entity ();
	scene.positions [e] = pos;
	scene.anim_t [e] = 0.0f;
	scene.beetniks [e] = EcsTrue ();
	scene.pouncables [e] = true;
	scene.hp [e] = 1024;
	scene.death_sound [e] = ESound::Crunch;
	scene.pain_sound [e] = ESound::Whoa;
}

void place_carrot (SceneEcs & scene, const vec3 & pos) {
	auto e = scene.add_entity ();
	scene.positions [e] = pos;
	scene.anim_t [e] = 0.0f;
	scene.carrots [e] = EcsTrue ();
	scene.pouncables [e] = true;
	scene.hp [e] = 1;
	scene.death_sound [e] = ESound::Crunch;
	scene.pain_sound [e] = ESound::Crunch;
}

void place_egg (SceneEcs & scene, const vec3 & pos, string name) {
	auto e = scene.add_entity ();
	scene.positions [e] = pos;
	scene.eggs [e] = false;
	scene.radii [e] = 1.0f;
	scene.powerups [e] = name_to_powerup (name);
}

void place_crab_apple (SceneEcs & scene, const vec3 & pos) {
	auto e = scene.add_entity ();
	scene.positions [e] = pos;
	scene.anim_t [e] = 0.0f;
	scene.crabapples [e] = EcsTrue ();
	scene.pouncables [e] = true;
	scene.hp [e] = 2;
	scene.death_sound [e] = ESound::Crunch;
	scene.pain_sound [e] = ESound::Crunch;
}

void place_pumpking (SceneEcs & scene, const vec3 & pos) {
	auto e = scene.add_entity ();
	scene.positions [e] = pos;
	scene.anim_t [e] = 0.0f;
	scene.pumpkings [e] = PumpKing {false};
	scene.pouncables [e] = true;
	scene.hp [e] = 10;
	scene.radii [e] = 2.0f;
	scene.death_sound [e] = ESound::KingRoar;
	scene.pain_sound [e] = ESound::KingPain;
}

vec2 tmx_to_vg (vec2 v) {
	return v / vec2 (32.0f, 32.0f) + vec2 (0.0f, 0.0f);
}

vec2 obj_to_camera (vec2 v) {
	return 32.0f * (vec2 (v.x - 12.5f, v.y - 7.5f));
}

vec2 get_camera_target (const SceneEcs & scene, Entity e) {
	// Target camera
	auto pos = scene.positions.at (e);
	auto vel = scene.velocities.at (e);
	auto venus = scene.venuses.at (e);
	
	vec2 camera_target = scene.camera;
	
	if (pos.z > 0.0f) {
		// Pouncing
		camera_target = obj_to_camera (vec2 (pos) + vec2 (vel) * vec2 (20.0f));
		for (auto pair : scene.pounce_target) {
			camera_target = obj_to_camera (vec2 (scene.positions.at (pair.first)));
			break;
		}
	}
	else if (venus.pounce_anim > 0.0f) {
		// Tensing
		camera_target = obj_to_camera (vec2 (pos) + scene.pounce_vec.at (e) * vec2 (5.0f));
	}
	else {
		// Walking
		camera_target = obj_to_camera (vec2 (pos) + scene.last_walk * vec2 (4.0f, 2.4f));
	}
	
	camera_target.x = clamp (camera_target.x, 32.0f, 2048 - 800 - 32.0f);
	camera_target.y = clamp (camera_target.y, 32.0f, 1024 - 480 - 32.0f);
	
	return camera_target;
}

SceneEcs reset_scene (const Level & level) {
	SceneEcs scene;
	
	scene.camera = vec2 (32, 32);
	scene.screenshake_t = 0.0f;
	
	bool debug = false;
	
	for (LevelObj obj : level.objects) {
		vec3 center (tmx_to_vg (vec2 (obj.x + obj.width / 2, obj.y + obj.height / 2)), 0.0);
		
		const auto & t = obj.type;
		
		if (debug) {
			cerr << "Object at (" << center.x << ", " << center.y << ")" << endl;
		}
		
		if (t == "Carrot") {
			place_carrot (scene, center);
		}
		else if (t == "Pumpking") {
			place_pumpking (scene, center);
		}
		else if (t == "Beetnik") {
			place_beetnik (scene, center);
		}
		else if (t == "CrabApple") {
			place_crab_apple (scene, center);
		}
		else if (t == "Egg") {
			place_egg (scene, center, obj.name);
		}
		else if (t == "Venus") {
			auto e = scene.add_entity ();
			scene.positions [e] = center;
			scene.velocities [e] = vec3 (0.0f);
			scene.anim_t [e] = 0.0f;
			scene.venuses [e] = Venus {0.0f, 0.5f};
			scene.hp [e] = 4;
			scene.player_input [e] = EcsTrue ();
		}
	}
	
	for (auto pair : scene.player_input) {
		auto e = pair.first;
		
		scene.camera = get_camera_target (scene, e);
	}
	
	/*
	place_carrot (scene, vec3 (3.0f, -3.0f, 0.0f));
	place_carrot (scene, vec3 (3.0f, -5.0f, 0.0f));
	place_carrot (scene, vec3 (3.0f, -4.0f, 0.0f));
	
	place_carrot (scene, vec3 (7.0f, -4.0f, 0.0f));
	
	place_carrot (scene, vec3 (0.0f, 1.0f, 0.0f));
	*/
	
	return scene;
}

void shake_screen (SceneEcs & scene, float time) {
	scene.screenshake_t = glm::max (scene.screenshake_t, time);
}

Logic::Logic (const Level & l) : level (l) {
	scene = reset_scene (level);
	quicksave = scene;
	checkpoint = scene;
}

vec2 get_pounce_vec (const InputFrame & input) {
	vec2 pounce_vec (0.0f, 0.0f);
	
	if (input.buttons [(int)InputButton::Left]) {
		pounce_vec.x = -1.0f;
	}
	else if (input.buttons [(int)InputButton::Right]) {
		pounce_vec.x = 1.0f;
	}
	
	if (input.buttons [(int)InputButton::Up]) {
		pounce_vec.y = -1.0f;
	}
	else if (input.buttons [(int)InputButton::Down]) {
		pounce_vec.y = 1.0f;
	}
	
	if (length (pounce_vec) > 1.0f) {
		pounce_vec = normalize (pounce_vec);
	}
	
	return pounce_vec;
}

// Everything from here down is a system I guess yay

bool is_jump_barrier (const Level & level, vec3 target_pos) {
	int tile_x = clamp (floor (target_pos.x), 0.0f, level.width - 1.0f);
	int tile_y = clamp (floor (target_pos.y), 0.0f, level.height - 1.0f);
	
	auto tile = level.data.at (tile_x + tile_y * level.width);
	if (tile == 51 || tile == 55) {
		return true;
	}
	
	return false;
}

bool is_fatal (const Level & level, vec3 target_pos) {
	if (is_jump_barrier (level, target_pos)) {
		return true;
	}
	
	int tile_x = clamp (floor (target_pos.x), 0.0f, level.width - 1.0f);
	int tile_y = clamp (floor (target_pos.y), 0.0f, level.height - 1.0f);
	
	//cerr << "x, y = " << target_pos.x << ", " << target_pos.y << endl;
	
	auto tile = level.data.at (tile_x + tile_y * level.width);
	if (tile >= 52 && tile <= 54) {
		return true;
	}
	
	return false;
}

bool is_walkable (const Level & level, const SceneEcs & scene, vec3 target_pos) {
	bool can_move_there = true;
	
	if (is_fatal (level, target_pos)) {
		can_move_there = false;
	}
	
	for (auto pair : scene.pouncables) {
		auto victim_e = pair.first;
		float radius = get_component (scene.radii, victim_e, 1.0f);
		
		if (! pair.second) {
			continue;
		}
		
		auto victim_pos = scene.positions.at (victim_e);
		
		const auto diff = vec2 (victim_pos - target_pos);
		if (length (diff) < radius) {
			can_move_there = false;
			break;
		}
	}
	
	return can_move_there;
}

vec3 get_walk_pos (const Level & level, const SceneEcs & scene, Entity e, const InputFrame & input) 
{
	auto pos = scene.positions.at (e);
	vec3 target_pos = pos;
	
	vec3 walk_dir;
	
	if (input.was_any (InputButton::Left)) {
		walk_dir += vec3 (-1.0f, 0.0f, 0.0f);
	}
	if (input.was_any (InputButton::Right)) {
		walk_dir += vec3 (1.0f, 0.0f, 0.0f);
	}
	if (input.was_any (InputButton::Up)) {
		walk_dir += vec3 (0.0f, -1.0f, 0.0f);
	}
	if (input.was_any (InputButton::Down)) {
		walk_dir += vec3 (0.0f, 1.0f, 0.0f);
	}
	
	if (length (walk_dir) > 0.0f) {
		target_pos = pos + normalize (walk_dir) * 4.0f / 60.0f;
	}
	
	bool can_move_there = is_walkable (level, scene, target_pos);
	
	if (can_move_there) {
		return target_pos;
	}
	else {
		return pos;
	}
}

void player_walk (SceneEcs & scene, Entity e, const vec3 & pos) {
	scene.positions [e] = pos;
}

// Yeah sure why not?
vector <Entity> get_pounce_victims (const SceneEcs & scene, const vec3 & venus_pos) 
{
	vector <Entity> result;
	
	// Pounced onto target?
	for (auto pair : scene.pounce_target) {
		Entity pouncee_e = pair.first;
		
		auto pos = scene.positions.at (pouncee_e);
		
		if (length (pos - venus_pos) < 1.0f) {
			// Yes
			result.push_back (pouncee_e);
		}
	}
	
	return result;
}

enum class EPounceResult {
	Missed,
	Killed,
	Bounce,
};

EPounceResult kill_enemies (SceneEcs & scene, const vector <Entity> & victims, long t) 
{
	for (Entity pouncee_e : victims) {
		if (get_component (scene.damage_flash, pouncee_e, (long)0) <= t) {
			scene.hp [pouncee_e] -= 1;
			shake_screen (scene, 0.375f);
			if (scene.hp.at (pouncee_e) <= 0) {
				scene.dead [pouncee_e] = true;
				scene.pouncables [pouncee_e] = false;
				scene.play_sound (scene.death_sound.at (pouncee_e));;
				return EPounceResult::Killed;
			}
			else {
				scene.damage_flash [pouncee_e] = t + 30;
				
				scene.play_sound (scene.pain_sound.at (pouncee_e));
				
				// Bounce the venus back
				return EPounceResult::Bounce;
			}
		}
	}
	return EPounceResult::Missed;
}

vec3 get_pounce_velocity (const vec2 & pounce_vec, float /*pounce_range*/) {
	// Pounce!
	// Default vertical jump
	float jump_power = 1.0f;
	vec2 pounce_xy (0.0f);
	
	const float max_range = 10.0f;
	
	if (length (pounce_vec) > 0.0f) {
		jump_power = length (pounce_vec) / max_range;
		pounce_xy = normalize (pounce_vec) * 40.0f / 60.0f;
	}
	
	jump_power = glm::max (0.25f / max_range, jump_power);
	
	return vec3 (pounce_xy.x, pounce_xy.y, 1.0f * jump_power);
}

void start_pounce (SceneEcs & scene, Entity e, const vec3 & velocity) 
{
	scene.velocities [e] = velocity;
}

Entity get_closest_pouncable (const SceneEcs & scene, const vec3 & pos, float range, const vec2 & pounce_vec) {
	Entity closest_victim = -1;
	vec2 closest_vec = pounce_vec;
	
	// Find closest pouncable
	for (auto pair : scene.pouncables) {
		auto victim_e = pair.first;
		
		if (! pair.second) {
			continue;
		}
		
		auto victim_pos = scene.positions.at (victim_e);
		
		const auto diff = vec2 (victim_pos - pos);
		bool can_pounce_enemy = length (diff) <= range && dot (normalize (diff), pounce_vec) >= 0.707f;
		
		bool already_on_enemy = length (diff) == 0.0f;
		
		if (already_on_enemy || can_pounce_enemy) {
			if (closest_victim == -1 || length (diff) < length (closest_vec)) {
				closest_victim = victim_e;
				closest_vec = diff;
			}
		}
	}
	
	return closest_victim;
}

// True if it hits an obstacle
bool ray_cast (const Level & level, vec3 start, vec3 end) {
	// Stupid ray-marching cause I'm running VERY short of hours
	
	float l = length (end - start);
	
	for (float t = 0.5f / l; t <= 1.0f; t += (0.5f / l)) {
		vec3 midpoint = end * t + start * (1.0f - t);
		
		if (is_jump_barrier (level, midpoint)) {
			return true;
		}
	}
	
	return false;
}

void apply_venus_input (const Level & level, SceneEcs & scene, Entity e, Venus & venus, const InputFrame & input) 
{
	auto pos = scene.positions.at (e);
	
	bool pounce_button_pressed = input.buttons [(int)InputButton::Pounce];
	bool on_ground = pos.z == 0.0f;
	bool pounce_charged = venus.pounce_anim >= 20.0f / 60.0f;
	
	bool could_pounce = on_ground && pounce_charged;
	const vec2 input_pounce_vec = get_pounce_vec (input);
	
	scene.pounce_vec [e] = input_pounce_vec;
	
	if (on_ground) {
		const auto pounce_range = venus.pounce_range;
		vec2 pounce_vec = input_pounce_vec;
		
		if (could_pounce) {
			// Search for pouncables
			
			const float range = venus.pounce_anim * pounce_range;
			const Entity closest_victim = get_closest_pouncable (scene, pos, range, input_pounce_vec);
			
			scene.pounce_target.clear ();
			
			if (closest_victim >= 0) {
				scene.targeted [closest_victim] = true;
				scene.pounce_target [closest_victim] = EcsTrue ();
				pounce_vec = scene.positions.at (closest_victim) - pos;
			}
			else {
				// The pounce is charged but no enemies are
				// available - Just pounce at the ground
				pounce_vec *= pounce_range * venus.pounce_anim;
			}
		}
		{
			const vec2 pounce_vec_2 = pounce_vec;
			
			bool landing_would_be_fatal = is_fatal (level, pos + vec3 (pounce_vec_2, 0.0f));
			
			bool crosses_barrier = ray_cast (level, pos, pos + vec3 (pounce_vec_2, 0.0f));
			
			if (pounce_button_pressed) 
			{
				venus.pounce_anim += 2.0f / 60.0f;
			}
			else if (pounce_charged && ! landing_would_be_fatal && ! crosses_barrier) 
			{
				auto vel = get_pounce_velocity (pounce_vec_2, pounce_range);
				scene.pounce_vel [e] = vel;
				start_pounce (scene, e, vel);
				scene.play_sound (ESound::Swooce);
				venus.pounce_anim = 0.0f;
			}
			else {
				venus.pounce_anim -= 10.0f / 60.0f;
			}
		}
	}
	else {
		venus.pounce_anim = 0.0f;
	}
	
	venus.pounce_anim = clamp (venus.pounce_anim, 0.0f, 1.0f);
}

void apply_player_input (const Level & level, SceneEcs & scene, Entity e, const InputFrame & input, long t) 
{
	auto pos = scene.positions.at (e);
	auto vel = scene.velocities.at (e);
	
	bool can_move = true;
	
	if (pos.z > 0.0f) {
		can_move = false;
	}
	else if (pos.z < 0.0f || vel.z < 0.0f) {
		//cerr << "Untz" << endl;
		scene.velocities [e] = vec3 (0.0f);
		//pos.x = floor (pos.x + 1.0f) - 0.5f;
		//pos.y = floor (pos.y + 1.0f) - 0.5f;
		pos.z = 0.0f;
		scene.positions [e] = pos;
		
		auto victims = get_pounce_victims (scene, pos);
		auto pounce_result = kill_enemies (scene, victims, t);
		
		switch (pounce_result) {
			case EPounceResult::Bounce:
			{
				scene.positions [e] = scene.positions.at (victims.at (0)) * vec3 (1.0f, 1.0f, 0.0f);
				scene.pounce_vel [e] = scene.pounce_vel.at (e) * vec3 (-1.0f, -1.0f, 1.0f);
				start_pounce (scene, e, scene.pounce_vel.at (e));
			}
				break;
			default:
				break;
		}
	}
	
	pos = scene.positions.at (e);
	vel = scene.velocities.at (e);
	
	if (input.buttons [(int)InputButton::Pounce]) {
		can_move = false;
	}
	
	auto venus_it = scene.venuses.find (e);
	if (venus_it != scene.venuses.end ()) {
		auto & venus = (*venus_it).second;
		
		apply_venus_input (level, scene, e, venus, input);
	}
	
	if (can_move) {
		auto new_pos = get_walk_pos (level, scene, e, input);
		player_walk (scene, e, new_pos);
		if (new_pos != pos) {
			scene.last_walk = new_pos - pos;
		}
		pos = new_pos;
	}
	
	
	if (pos.z > 0.0f || vel.z > 0.0f) {
		// Mid-pounce
		scene.velocities [e] = vel + vec3 (0.0f, 0.0f, -0.125f);
		pos += scene.velocities.at (e);
	}
	
	pos.x = clamp (pos.x, 1.5f, 2048.0f / 32.0f - 1.5f);
	pos.y = clamp (pos.y, 1.5f, 1024.0f / 32.0f - 1.5f);
	pos.z = clamp (pos.z, 0.0f, 100.0f);
	
	scene.positions [e] = pos;
	
	bool debug = false;
	if (debug) {
		cerr << "X = " << scene.positions [e].x << endl;
	}
}

vector <Entity> get_savable_eggs (const SceneEcs & scene, vec3 pos) {
	vector <Entity> result;
	
	for (auto pair : scene.eggs) {
		auto e = pair.first;
		auto egg_pos = scene.positions.at (e);
		bool available = ! pair.second;
		
		const float radius = scene.radii.at (e);
		
		if (available && length (egg_pos - pos) < radius) {
			result.push_back (e);
		}
	}
	
	return result;
}

void save_at_egg (SceneEcs & scene, Entity touched_e) {
	for (auto pair : scene.eggs) {
		auto e = pair.first;
		if (e == touched_e) {
			scene.eggs [e] = true;
		}
		else {
			scene.eggs [e] = false;
		}
	}
}

void damage_player (SceneEcs & scene, Entity player_e, int amount, long t) {
	if (! get_component (scene.dead, player_e, false)) {
		scene.damage_flash [player_e] = glm::max (t + 60, get_component (scene.damage_flash, player_e, (long)0));
		scene.hp [player_e] -= amount;
		if (get_component (scene.hp, player_e, 0) <= 0) {
			cerr << "You are dead" << endl;
			scene.play_sound (ESound::Crunch);
			scene.dead [player_e] = true;
			scene.respawn_time = t + 90;
			scene.play_sound (ESound::Respawn);
		}
		else {
			scene.play_sound (ESound::Gasp);
		}
	}
}

void Logic::step (const InputFrame & input, long t) {
	if (t == scene.respawn_time) {
		scene = checkpoint;
		return;
	}
	
	if (input.was_tapped (InputButton::QuickSave)) {
		quicksave = scene;
	}
	if (input.was_tapped (InputButton::QuickLoad)) {
		scene = quicksave;
		return;
	}
	
	scene.targeted.clear ();
	
	scene.audio_frame = AudioFrame ();
	
	if (input.was_tapped (InputButton::Reset)) {
		scene = reset_scene (level);
	}
	
	vec3 player_pos;
	Entity player_e;
	
	for (auto pair : scene.player_input) {
		auto e = pair.first;
		
		if (get_component (scene.dead, e, false)) {
			continue;
		}
		
		apply_player_input (level, scene, e, input, t);
		
		auto eggs = get_savable_eggs (scene, scene.positions.at (e));
		if (eggs.size () == 1) {
			auto egg_e = eggs.at (0);
			save_at_egg (scene, egg_e);
			scene.hp [e] = glm::max (4, get_component (scene.hp, e, 0));
			
			auto powerup = get_component (scene.powerups, egg_e, EPowerup::NoPowerup);
			switch (powerup) {
				case EPowerup::Pounce_4:
					scene.venuses [e].pounce_range = glm::max (4.0f, scene.venuses.at (e).pounce_range);
					break;
				case EPowerup::Pounce_7:
					scene.venuses [e].pounce_range = glm::max (7.0f, scene.venuses.at (e).pounce_range);
					break;
				case EPowerup::Pounce_10:
					scene.venuses [e].pounce_range = glm::max (10.0f, scene.venuses.at (e).pounce_range);
					break;
				case EPowerup::Health_10:
					scene.hp [e] = glm::max (10, get_component (scene.hp, e, 0));
					break;
				default:
					break;
			}
			
			checkpoint = scene;
			scene.play_sound (ESound::Bling);
		}
		
		// Target camera
		vec2 camera_target = get_camera_target (scene, e);
		
		scene.camera = mix (scene.camera, camera_target, 0.05f);
		
		player_e = e;
		player_pos = scene.positions.at (e);
	}
	
	for (auto pair : scene.crabapples) {
		auto e = pair.first;
		auto pos = scene.positions.at (e);
		
		bool alive = ! get_component (scene.dead, e, false);
		
		bool targeted = scene.pounce_target.find (e) != scene.pounce_target.end ();
		
		scene.ai_active [e] = alive && ! targeted && length (player_pos - pos) < 11.0f;
		
		if (! targeted && alive) {
			vec3 direction = normalize (player_pos - pos);
			vec3 crab = cross (direction, vec3 (0.0f, 0.0f, 1.0f));
			
			bool player_vulnerable = get_component (scene.damage_flash, player_e, (long)0) < t;
			
			if (! player_vulnerable) {
				auto target_pos = pos + 3.0f / 60.0f * (-1.0f * direction);
				
				bool can_go = ! is_fatal (level, target_pos);
				
				if (can_go) {
					scene.positions [e] = target_pos;
				}
			}
			else {
				if (length (player_pos - pos) <= 1.0f) {
					// Attack!
					damage_player (scene, player_e, 1, t);
				}
				else {
					auto target_pos = pos + 3.0f / 60.0f * (0.25f * direction + crab * sin (3.0f * t / 60.0f));
					
					bool can_go = ! is_fatal (level, target_pos);
					
					if (can_go) {
						scene.positions [e] = target_pos;
					}
				}
			}
		}
	}
	
	
	
	scene.screenshake_t = glm::max (0.0f, scene.screenshake_t - 1.0f / 60.0f);
}
