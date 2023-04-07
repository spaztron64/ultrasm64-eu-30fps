#ifndef MARIO_ACTIONS_CUTSCENE_H
#define MARIO_ACTIONS_CUTSCENE_H

#include <PR/ultratypes.h>

#include "macros.h"
#include "types.h"

// set_mario_npc_dialog
// actionArg
#define MARIO_DIALOG_STOP       0
#define MARIO_DIALOG_LOOK_FRONT 1 // no head turn
#define MARIO_DIALOG_LOOK_UP    2
#define MARIO_DIALOG_LOOK_DOWN  3
// dialogState
#define MARIO_DIALOG_STATUS_NONE  0
#define MARIO_DIALOG_STATUS_START 1
#define MARIO_DIALOG_STATUS_SPEAK 2

#if defined(VERSION_EU)
    #define TIMER_CREDITS_SHOW      51
    #define TIMER_CREDITS_PROGRESS  80
    #define TIMER_CREDITS_WARP     160
#elif defined(VERSION_SH)
    #define TIMER_CREDITS_SHOW      61
    #define TIMER_CREDITS_PROGRESS  90
    #define TIMER_CREDITS_WARP     204
#else
    #define TIMER_CREDITS_SHOW      61
    #define TIMER_CREDITS_PROGRESS  90
    #define TIMER_CREDITS_WARP     200
#endif

extern Vp sEndCutsceneVp;

void print_displaying_credits_entry(void);
void bhv_end_peach_loop(void);
void bhv_end_toad_loop(void);
s32 geo_switch_peach_eyes(s32 run, struct GraphNode *node, UNUSED s32 a2);
s32 mario_ready_to_speak(void);
s32 set_mario_npc_dialog(s32 actionArg);
s32 mario_execute_cutscene_action(struct MarioState *m);

#endif // MARIO_ACTIONS_CUTSCENE_H
