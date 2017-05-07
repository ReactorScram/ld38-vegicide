#include "scene-json.h"

#include <stdint.h>
#include <string>

#include <glm/glm.hpp>

#include "audio/audio-frame.h"
#include "level.h"
#include "scene.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using namespace std;

void to_json (stringstream & s, const bool b) {
	if (b) {
		s << "true";
	}
	else {
		s << "false";
	}
}

void to_json (stringstream & s, const float f) {
	s << f;
}

void to_json (stringstream & s, const int16_t i) {
	s << i;
}

void to_json (stringstream & s, const int32_t i) {
	s << i;
}

void to_json (stringstream & s, const int64_t i) {
	s << i;
}

void to_json (stringstream & s, const ETexture i) {
	s << (int)i;
}

void to_json (stringstream & s, const EMesh i) {
	s << (int)i;
}

void to_json (stringstream & s, const EPowerup i) {
	s << (int)i;
}

void to_json (stringstream & s, const ESound i) {
	s << (int)i;
}

void to_json (stringstream & s, const Venus v) {
	s << "{ ";
	s << "\"pounce_anim\": ";
	to_json (s, v.pounce_anim);
	s << ", \"pounce_range\": ";
	to_json (s, v.pounce_range);
	s << " }";
}

void to_json (stringstream & s, const PumpKing p) {
	s << "{ ";
	s << "\"spotted_player\": ";
	to_json (s, p.spotted_player);
	s << " }";
}

void to_json (stringstream & s, const vec2 & v) {
	s << "[ ";
	
	for (int i = 0; i < 2; i++) {
		if (i > 0) {
			s << ", ";
		}
		s << v [i];
	}
	
	s << " ]";
}

void to_json (stringstream & s, const vec3 & v) {
	s << "[ ";
	
	for (int i = 0; i < 3; i++) {
		if (i > 0) {
			s << ", ";
		}
		s << v [i];
	}
	
	s << " ]";
}

void to_json (stringstream & s, const vec4 & v) {
	s << "[ ";
	
	for (int i = 0; i < 4; i++) {
		if (i > 0) {
			s << ", ";
		}
		s << v [i];
	}
	
	s << " ]";
}

void to_json (stringstream & s, const mat4 & m) {
	s << "[ ";
	
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (x > 0 || y > 0) {
				s << ", ";
			}
			s << m [y][x];
		}
	}
	
	s << " ]";
}

void to_json (stringstream & s, EcsTrue) {
	s << "true";
}

template <typename T>
void to_json (stringstream & s, const Components <T> & comps) {
	s << "{ ";
	
	bool first = true;
	
	for (auto pair : comps) {
		Entity e = pair.first;
		const T & v = pair.second;
		
		if (first) {
			first = false;
		}
		else {
			s << ",\n";
		}
		
		s << "\"" << e << "\": ";
		to_json (s, v);
	}
	
	s << " }";
}

template <typename T>
void to_json (stringstream & s, const vector <T> & v) {
	s << "[ ";
	
	bool first = true;
	
	for (auto e : v) {
		if (first) {
			first = false;
		}
		else {
			s << ",\n";
		}
		
		to_json (s, e);
	}
	
	s << " ]";
}

void to_json (stringstream & s, const Pass & p) {
	s << "{ ";
	
	s << "\"renderables\": ";
	to_json (s, p.renderables);
	s << ", " << endl;
	
	s << "\"proj_view_mat\": ";
	to_json (s, p.proj_view_mat);
	
	s << " }";
}

void to_json (stringstream & s, const SceneEcs & ecs) {
	s << "{ ";
	
	s << "\"positions\": ";
	to_json (s, ecs.positions);
	s << ", " << endl;
	
	s << "\"eggs\": ";
	to_json (s, ecs.eggs);
	s << ", " << endl;
	
	s << "\"powerups\": ";
	to_json (s, ecs.powerups);
	s << ", " << endl;
	
	s << "\"velocities\": ";
	to_json (s, ecs.velocities);
	s << ", " << endl;
	
	s << "\"radii\": ";
	to_json (s, ecs.radii);
	s << ", " << endl;
	
	s << "\"targeted\": ";
	to_json (s, ecs.targeted);
	s << ", " << endl;
	
	s << "\"anim_t\": ";
	to_json (s, ecs.anim_t);
	s << ", " << endl;
	
	s << "\"pouncables\": ";
	to_json (s, ecs.pouncables);
	s << ", " << endl;
	
	s << "\"pounce_target\": ";
	to_json (s, ecs.pounce_target);
	s << ", " << endl;
	
	s << "\"pounce_vec\": ";
	to_json (s, ecs.pounce_vec);
	s << ", " << endl;
	
	s << "\"pounce_vel\": ";
	to_json (s, ecs.pounce_vel);
	s << ", " << endl;
	
	s << "\"damage_flash\": ";
	to_json (s, ecs.damage_flash);
	s << ", " << endl;
	
	s << "\"dead\": ";
	to_json (s, ecs.dead);
	s << ", " << endl;
	
	s << "\"hp\": ";
	to_json (s, ecs.hp);
	s << ", " << endl;
	
	s << "\"pain_sound\": ";
	to_json (s, ecs.pain_sound);
	s << ", " << endl;
	
	s << "\"death_sound\": ";
	to_json (s, ecs.death_sound);
	s << ", " << endl;
	
	s << "\"beetniks\": ";
	to_json (s, ecs.beetniks);
	s << ", " << endl;
	
	s << "\"carrots\": ";
	to_json (s, ecs.carrots);
	s << ", " << endl;
	
	s << "\"crabapples\": ";
	to_json (s, ecs.crabapples);
	s << ", " << endl;
	
	s << "\"pumpkings\": ";
	to_json (s, ecs.pumpkings);
	s << ", " << endl;
	
	s << "\"venuses\": ";
	to_json (s, ecs.venuses);
	s << ", " << endl;
	
	s << "\"player_input\": ";
	to_json (s, ecs.player_input);
	s << ", " << endl;
	
	s << "\"ai_active\": ";
	to_json (s, ecs.ai_active);
	s << ", " << endl;
	
	s << "\"screenshake_t\": ";
	to_json (s, ecs.screenshake_t);
	
	// TODO: Audio, respawn_time, camera, last_walk
	
	s << " }";
}

void to_json (stringstream & s, const GraphicsEcs & ecs) {
	s << "{ ";
	
	s << "\"passes\": ";
	to_json (s, ecs.passes);
	s << ", " << endl;
	
	s << "\"transparent_z\": ";
	to_json (s, ecs.transparent_z);
	s << ", " << endl;
	
	s << "\"rigid_mats\": ";
	to_json (s, ecs.rigid_mats);
	s << ", " << endl;
	
	s << "\"diffuse_colors\": ";
	to_json (s, ecs.diffuse_colors);
	s << ", " << endl;
	
	s << "\"meshes\": ";
	to_json (s, ecs.meshes);
	s << ", " << endl;
	
	s << "\"textures\": ";
	to_json (s, ecs.textures);
	
	s << " }";
}

void to_json (stringstream & s, const Level & level) {
	s << "{ ";
	
	s << "\"width\": " << level.width << "," << endl;
	s << "\"height\": " << level.height << "," << endl;
	s << "\"data\": ";
	
	to_json (s, level.data);
	
	s << " }";
}

void to_json (std::stringstream & s, const AudioFrame & audio_frame) {
	s << "{ ";
	s << "\"bgm\": " << (int)audio_frame.bgm << ", ";
	
	s << "\"sounds\": [ ";
	
	bool first = true;
	for (int i = 0; i < (int)ESound::NUM_SOUNDS; i++) {
		bool b = audio_frame.sounds [i];
		
		if (b) {
			if (first) {
				first = false;
			}
			else {
				s << ", ";
			}
			
			s << i;
		}
	}
	
	s << " ]";
	
	s << " }";
}
