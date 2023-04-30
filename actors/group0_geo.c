#include <ultra64.h>
#include "sm64.h"
#include "geo_commands.h"

#include "make_const_nonconst.h"

#include "common1.h"
#include "group0.h"
#include "game/farcall_helpers.h"

#include "bubble/geo.inc.c"
#include "walk_smoke/geo.inc.c"
#include "burn_smoke/geo.inc.c"
#include "stomp_smoke/geo.inc.c"
#include "water_wave/geo.inc.c"
#include "sparkle/geo.inc.c"
#include "water_splash/geo.inc.c"
#include "sparkle_animation/geo.inc.c"
#include "mario/geo.inc.c"

#include "game/behaviors/bubble.inc.c"
#include "game/behaviors/white_puff.inc.c"
#include "game/behaviors/flame_mario.inc.c"
#include "game/behaviors/white_puff_explode.inc.c"
#include "game/behaviors/water_wave.inc.c"
#include "game/behaviors/water_splashes_and_waves.inc.c"
#include "game/behaviors/sparkle_spawn.inc.c"
#include "game/behaviors/sparkle_spawn_star.inc.c"

#include "game/behaviors/explosion.inc.c"
#include "game/behaviors/butterfly.inc.c"
#include "game/behaviors/triplet_butterfly.inc.c"
#include "game/behaviors/red_coin.inc.c"
#include "game/behaviors/coin.inc.c"
#include "game/behaviors/moving_coin.inc.c"
#include "game/behaviors/door.inc.c"
#include "game/behaviors/bowser_key.inc.c"
#include "game/behaviors/bowser_key_cutscene.inc.c"
#include "game/behaviors/bouncing_fireball.inc.c"
#include "game/behaviors/flamethrower.inc.c"
#include "game/behaviors/lll_rotating_hex_flame.inc.c"
#include "game/behaviors/lll_volcano_flames.inc.c"
#include "game/behaviors/blue_fish.inc.c"
#include "game/behaviors/tree_particles.inc.c"
#include "game/behaviors/cap.inc.c"
#include "game/behaviors/orange_number.inc.c"
#include "game/behaviors/mushroom_1up.inc.c"
#include "game/behaviors/reds_star_marker.inc.c"
#include "game/behaviors/celebration_star.inc.c"
#include "game/behaviors/fire_piranha_plant.inc.c"
#include "game/behaviors/flame.inc.c"
#include "game/behaviors/cloud.inc.c"

#include "game/behaviors/pole.inc.c"
#include "game/behaviors/water_objs.inc.c"
#include "game/behaviors/warp.inc.c"
#include "game/behaviors/water_mist.inc.c"
#include "game/behaviors/tumbling_bridge.inc.c"
#include "game/behaviors/fish.inc.c"
#include "game/behaviors/sound_spawner.inc.c"
#include "game/behaviors/pole_base.inc.c"
#ifndef VERSION_JP
#include "game/behaviors/music_touch.inc.c"
#endif