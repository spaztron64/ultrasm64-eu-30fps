#include <PR/ultratypes.h>

#include "types.h"
#include "actors/common1.h"
#include "actors/group12.h"
#include "actors/group13.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_misc_macros.h"
#include "level_table.h"
#include "level_update.h"
#include "levels/bob/header.h"
#include "levels/bowser_3/header.h"
#include "levels/castle_inside/header.h"
#include "levels/hmc/header.h"
#include "main.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "mario_step.h"
#include "obj_behaviors.h"
#include "obj_behaviors_2.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "paintings.h"
#include "platform_displacement.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sm64.h"
#include "spawn_object.h"
#include "spawn_sound.h"
#include "rumble_init.h"

#define o gCurrentObject

void curr_obj_random_blink(s32 *blinkTimer) {
    if (*blinkTimer == 0) {
        if ((s16)(random_float() * 100.0f) == 0) {
            o->oAnimState = 1;
            *blinkTimer = 1;
        }
    } else {
        (*blinkTimer)++;

        if (*blinkTimer > 5) {
            o->oAnimState = 0;
        }

        if (*blinkTimer > 10) {
            o->oAnimState = 1;
        }

        if (*blinkTimer > 15) {
            o->oAnimState = 0;
            *blinkTimer = 0;
        }
    }
}

s32 check_if_moving_over_floor(f32 a0, f32 a1) {
    struct Surface *sp24;
    f32 sp20 = o->oPosX + sins(o->oMoveAngleYaw) * a1;
    f32 floorHeight;
    f32 sp18 = o->oPosZ + coss(o->oMoveAngleYaw) * a1;

    floorHeight = find_floor(sp20, o->oPosY, sp18, &sp24);

    if (absf(floorHeight - o->oPosY) < a0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 mario_is_far_below_object(f32 arg0) {
    if (arg0 < o->oPosY - gMarioObject->oPosY) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void bhv_spawn_star_no_level_exit(u32 sp20) {
    struct Object *sp1C = spawn_object(o, MODEL_STAR, bhvSpawnedStarNoLevelExit);
    sp1C->oBehParams = sp20 << 24;
    sp1C->oInteractionSubtype = INT_SUBTYPE_NO_EXIT;
    obj_set_angle(sp1C, 0, 0, 0);
}


/**
 * Wait 50 frames, then play the race starting sound, disable time stop, and
 * optionally begin the timer.
 */
s32 obj_begin_race(s32 noTimer) {
    if (o->oTimer == 50) {
        cur_obj_play_sound_2(SOUND_GENERAL_RACE_GUN_SHOT);

        if (!noTimer) {
            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_LEVEL_SLIDE), 0);

            level_control_timer(TIMER_CONTROL_SHOW);
            level_control_timer(TIMER_CONTROL_START);

            o->parentObj->oKoopaRaceEndpointRaceBegun = TRUE;
        }

        // Unfreeze mario and disable time stop to begin the race
        set_mario_npc_dialog(MARIO_DIALOG_STOP);
        disable_time_stop_including_mario();
    } else if (o->oTimer > 50) {
        return TRUE;
    }

    return FALSE;
}

// Copy of geo_update_projectile_pos_from_parent
Gfx *geo_update_held_mario_pos(s32 run, UNUSED struct GraphNode *node, Mat4 mtx) {
    if (run == TRUE) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        if (obj->prevObj != NULL) {
            obj_update_pos_from_parent_transformation(mtx, obj->prevObj);
            obj_set_gfx_pos_from_pos(obj->prevObj);
        }
    }

    return NULL;
}

Gfx *geo_update_body_rot_from_parent(s32 callContext, UNUSED struct GraphNode *node, Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        if (obj->prevObj != NULL) {
            obj_update_pos_from_parent_transformation(mtx, obj->prevObj);
            obj_set_gfx_pos_from_pos(obj->prevObj);
        }
    }

    return NULL;
}

s32 update_angle_from_move_flags(s32 *angle) {
    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        *angle = o->oWallAngle;
        return 1;
    } else if (o->oMoveFlags & OBJ_MOVE_HIT_EDGE) {
        *angle = o->oMoveAngleYaw + 0x8000;
        return -1;
    }
    return 0;
}

#include "farcall_helpers.h"

#include "game/behaviors/spawn_star_exit.inc.c"
#include "game/behaviors/spawn_star.inc.c"
#include "game/behaviors/hidden_star.inc.c"
#include "behaviors/collide_particles.inc.c"
#include "behaviors/water_mist_particle.inc.c"
#include "behaviors/break_particles.inc.c"
#include "behaviors/ground_particles.inc.c"
#include "behaviors/wind.inc.c"
#include "behaviors/strong_wind_particle.inc.c"
#include "behaviors/corkbox.inc.c"
#include "behaviors/elevator.inc.c"
#include "behaviors/sound_waterfall.inc.c"
#include "behaviors/sound_volcano.inc.c"
#include "behaviors/sound_birds.inc.c"
#include "behaviors/sound_ambient.inc.c"
#include "behaviors/sound_sand.inc.c"


// not in behavior file
static struct SpawnParticlesInfo sMistParticles = {
    /* behParam:        */ 2,
    /* count:           */ 20,
    /* model:           */ MODEL_MIST,
    /* offsetY:         */ 0,
    /* forwardVelBase:  */ 40,
    /* forwardVelRange: */ 5,
    /* velYBase:        */ 30,
    /* velYRange:       */ 20,
    /* gravity:         */ 252,
    /* dragStrength:    */ 30,
    /* sizeBase:        */ 330.0f,
    /* sizeRange:       */ 10.0f,
};

// generate_wind_puffs/dust (something like that)
void spawn_mist_particles_variable(s32 count, s32 offsetY, f32 size) {
    sMistParticles.sizeBase = size;
    sMistParticles.sizeRange = size / 20.0f;
    sMistParticles.offsetY = offsetY;

    if (count == 0) {
        sMistParticles.count = 20;
    } else if (count > 20) {
        sMistParticles.count = count;
    } else {
        sMistParticles.count = 4;
    }

    cur_obj_spawn_particles(&sMistParticles);
}



// not sure what this is doing here. not in a behavior file.
Gfx *geo_move_mario_part_from_parent(s32 run, UNUSED struct GraphNode *node, Mat4 mtx) {

    if (run == TRUE) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        if (obj == gMarioObject && obj->prevObj != NULL) {
            obj_update_pos_from_parent_transformation(mtx, obj->prevObj);
            obj_set_gfx_pos_from_pos(obj->prevObj);
        }
    }

    return NULL;
}

void common_anchor_mario_behavior(f32 sp28, f32 sp2C, s32 sp30) {
    switch (o->parentObj->oChuckyaUnk88) {
        case 0:
            break;

        case 1:
            obj_set_gfx_pos_at_obj_pos(gMarioObject, o);
            break;

        case 2:
            gMarioObject->oInteractStatus |= (INT_STATUS_MARIO_UNK2 + sp30);
            gMarioStates[0].forwardVel = sp28;
            gMarioStates[0].vel[1] = sp2C;
            o->parentObj->oChuckyaUnk88 = 0;
            break;

        case 3:
            gMarioObject->oInteractStatus |= (INT_STATUS_MARIO_UNK2 | INT_STATUS_MARIO_UNK6);
            gMarioStates[0].forwardVel = 10.0f;
            gMarioStates[0].vel[1] = 10.0f;
            o->parentObj->oChuckyaUnk88 = 0;
            break;
    }

    o->oMoveAngleYaw = o->parentObj->oMoveAngleYaw;

    if (o->parentObj->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
        obj_mark_for_deletion(o);
    }
}

void bhv_bobomb_anchor_mario_loop(void) {
    common_anchor_mario_behavior(50.0f, 50.0f, INT_STATUS_MARIO_UNK6);
}


s32 approach_forward_vel(f32 *forwardVel, f32 spC, f32 sp10) {
    s32 sp4 = 0;

    if (*forwardVel > spC) {
        *forwardVel -= sp10;
        if (*forwardVel < spC) {
            *forwardVel = spC;
        }
    } else if (*forwardVel < spC) {
        *forwardVel += sp10;
        if (*forwardVel > spC) {
            *forwardVel = spC;
        }
    } else {
        sp4 = 1;
    }

    return sp4;
}

// not in behavior file
// n is the number of objects to spawn, r if the rate of change of phase (frequency?)
void spawn_sparkle_particles(s32 n, s32 a1, s32 a2, s32 r) {
    static s16 D_8035FF10;
    s32 i;
    s16 separation = 0x10000 / n; // Evenly spread around a circle

    for (i = 0; i < n; i++) {
        spawn_object_relative(0, sins(D_8035FF10 + i * separation) * a1, (i + 1) * a2,
                              coss(D_8035FF10 + i * separation) * a1, o, MODEL_NONE, bhvSparkleSpawn);
    }

    D_8035FF10 += r * 0x100;
}


s32 set_obj_anim_with_accel_and_sound(s16 a0, s16 a1, s32 a2) {
    f32 sp1C;

    if ((sp1C = o->header.gfx.animInfo.animAccel / (f32) 0x10000) == 0) {
        sp1C = 1.0f;
    }

    if (cur_obj_check_anim_frame_in_range(a0, sp1C) || cur_obj_check_anim_frame_in_range(a1, sp1C)) {
        cur_obj_play_sound_2(a2);
        return TRUE;
    }

    return FALSE;
}

Gfx *geo_scale_bowser_key(s32 run, struct GraphNode *node, UNUSED f32 mtx[4][4]) {
    if (run == TRUE) {
        struct Object *sp4 = (struct Object *) gCurGraphNodeObject;
        ((struct GraphNodeScale *) node->next)->scale = sp4->oBowserKeyScale;
    }
    return NULL;
}

/**
 * Bowser's eyes Geo-Switch-Case IDs, defined from Mario's POV
 */
enum BowserEyesGSCId {
    /*0x00*/ BOWSER_EYES_OPEN,
    /*0x01*/ BOWSER_EYES_HALF_CLOSED,
    /*0x02*/ BOWSER_EYES_CLOSED,
    /*0x03*/ BOWSER_EYES_LEFT,
    /*0x04*/ BOWSER_EYES_FAR_LEFT,
    /*0x05*/ BOWSER_EYES_RIGHT,
    /*0x06*/ BOWSER_EYES_FAR_RIGHT,
    /*0x07*/ BOWSER_EYES_DERP, // unused
    /*0x08*/ BOWSER_EYES_CROSS, // unused
    /*0x08*/ BOWSER_EYES_RESET // set eyes back to open
};

/**
 * Controls Bowser's eye open stage, including blinking and look directions
 */
void bowser_open_eye_switch(struct Object *obj, struct GraphNodeSwitchCase *switchCase) {
    s32 eyeCase;
    s16 angleFromMario;

    angleFromMario = abs_angle_diff(obj->oMoveAngleYaw, obj->oAngleToMario);
    eyeCase = switchCase->selectedCase;

    switch (eyeCase) {
        case BOWSER_EYES_OPEN:
            // Mario is in Bowser's field of view
            if (angleFromMario > 0x2000) {
                if (obj->oAngleVelYaw > 0) {
                    switchCase->selectedCase = BOWSER_EYES_RIGHT;
                }
                if (obj->oAngleVelYaw < 0) {
                    switchCase->selectedCase = BOWSER_EYES_LEFT;
                }
            }
            // Half close, start blinking
            if (obj->oBowserEyesTimer > 50) {
                switchCase->selectedCase = BOWSER_EYES_HALF_CLOSED;
            }
            break;

        case BOWSER_EYES_HALF_CLOSED:
            // Close, blinking
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_CLOSED;
            }
            break;

        case BOWSER_EYES_CLOSED:
            // Reset blinking
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_RESET;
            }
            break;

        case BOWSER_EYES_RESET:
            // Open, no longer blinking
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_OPEN;
            }
            break;

        case BOWSER_EYES_RIGHT:
            // Look more on the right if angle didn't change
            // Otherwise, look at the center (open)
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_FAR_RIGHT;
                if (obj->oAngleVelYaw <= 0) {
                    switchCase->selectedCase = BOWSER_EYES_OPEN;
                }
            }
            break;

        case BOWSER_EYES_FAR_RIGHT:
            // Look close right if angle was drastically changed
            if (obj->oAngleVelYaw <= 0) {
                switchCase->selectedCase = BOWSER_EYES_RIGHT;
            }
            break;

        case BOWSER_EYES_LEFT:
            // Look more on the left if angle didn't change
            // Otherwise, look at the center (open)
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_FAR_LEFT;
                if (obj->oAngleVelYaw >= 0) {
                    switchCase->selectedCase = BOWSER_EYES_OPEN;
                }
            }
            break;

        case BOWSER_EYES_FAR_LEFT:
            // Look close left if angle was drastically changed
            if (obj->oAngleVelYaw >= 0) {
                switchCase->selectedCase = BOWSER_EYES_LEFT;
            }
            break;

        default:
            switchCase->selectedCase = BOWSER_EYES_OPEN;
    }

    // Reset timer if eye case has changed
    if (switchCase->selectedCase != eyeCase) {
        obj->oBowserEyesTimer = -1;
    }
}

/**
 * Geo switch for controlling the state of Bowser's eye direction and open/closed
 * state. Checks whether oBowserEyesShut is TRUE and closes eyes if so and processes
 * direction otherwise.
 */
Gfx *geo_switch_bowser_eyes(s32 callContext, struct GraphNode *node, UNUSED Mat4 *mtx) {
    struct Object *obj = (struct Object *) gCurGraphNodeObject;
    struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;

    if (callContext == GEO_CONTEXT_RENDER) {
        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        switch (obj->oBowserEyesShut) {
            case FALSE: // eyes open, handle eye looking direction
                bowser_open_eye_switch(obj, switchCase);
                break;
            case TRUE: // eyes closed, blinking
                switchCase->selectedCase = BOWSER_EYES_CLOSED;
                break;
        }

        obj->oBowserEyesTimer++;
    }

    return NULL;
}

/**
 * Geo switch that sets Bowser's Rainbow coloring (in BitS)
 */
Gfx *geo_bits_bowser_coloring(s32 callContext, struct GraphNode *node, UNUSED s32 context) {
    Gfx *gfxHead = NULL;
    Gfx *gfx;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        struct GraphNodeGenerated *graphNode = (struct GraphNodeGenerated *) node;

        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        // Set layers if object is transparent or not
        if (obj->oOpacity == 255) {
            graphNode->fnNode.node.flags = (graphNode->fnNode.node.flags & 0xFF) | (LAYER_OPAQUE << 8);
        } else {
            graphNode->fnNode.node.flags = (graphNode->fnNode.node.flags & 0xFF) | (LAYER_TRANSPARENT << 8);
        }

        gfx = gfxHead = alloc_display_list(2 * sizeof(Gfx));

        // If TRUE, clear lighting to give rainbow color
        if (obj->oBowserRainbowLight) {
            gSPClearGeometryMode(gfx++, G_LIGHTING);
        }

        gSPEndDisplayList(gfx);
    }

    return gfxHead;
}



/**
 * This geo function shifts snufit's mask when it shrinks down, 
 * since the parts move independently.
 */
Gfx *geo_snufit_move_mask(s32 callContext, struct GraphNode *node, UNUSED Mat4 *c) {
    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        struct GraphNodeTranslationRotation *transNode
            = (struct GraphNodeTranslationRotation *) node->next;

        transNode->translation[0] = obj->oSnufitXOffset;
        transNode->translation[1] = obj->oSnufitYOffset;
        transNode->translation[2] = obj->oSnufitZOffset;
    }

    return NULL;
}

/**
 * This function scales the body of snufit, which needs done seperately from its mask.
 */
Gfx *geo_snufit_scale_body(s32 callContext, struct GraphNode *node, UNUSED Mat4 *c) {
    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        struct GraphNodeScale *scaleNode = (struct GraphNodeScale *) node->next;

        scaleNode->scale = obj->oSnufitBodyScale / 1000.0f;
    }

    return NULL;
}

/** Geo switch logic for Tuxie's mother's eyes. Cases 0-4. Interestingly, case
 * 4 is unused, and is the eye state seen in Shoshinkai 1995 footage.
 */
Gfx *geo_switch_tuxie_mother_eyes(s32 run, struct GraphNode *node, UNUSED Mat4 *mtx) {
    if (run == TRUE) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;
        s32 timer;

        switchCase->selectedCase = 0;

        // timer logic for blinking. uses cases 0-2.
        timer = gGlobalTimer % 50;
        if (timer < 43) {
            switchCase->selectedCase = 0;
        } else if (timer < 45) {
            switchCase->selectedCase = 1;
        } else if (timer < 47) {
            switchCase->selectedCase = 2;
        } else {
            switchCase->selectedCase = 1;
        }

        /** make Tuxie's Mother have angry eyes if Mario takes the correct baby
         * after giving it back. The easiest way to check this is to see if she's
         * moving, since she only does when she's chasing Mario.
         */
        if (obj->behavior == segmented_to_virtual(bhvTuxiesMother)) {
            if (obj->oForwardVel > 5.0f) {
                switchCase->selectedCase = 3;
            }
        }
    }
    return NULL;
}
