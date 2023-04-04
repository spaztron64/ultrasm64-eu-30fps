#ifndef SCREEN_TRANSITION_H
#define SCREEN_TRANSITION_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "macros.h"
#include "types.h"

enum TextureTransitionID {
    TEX_TRANS_STAR,
    TEX_TRANS_CIRCLE,
    TEX_TRANS_MARIO,
    TEX_TRANS_BOWSER
};

enum TextureTransitionType {
    TRANS_TYPE_MIRROR,
    TRANS_TYPE_CLAMP
};

extern u8 sTransitionColorFadeCount[4];
extern u16 sTransitionTextureFadeCount[2];
extern f32 sTransitionColorFadeCountLerp[4];
extern f32 sTransitionTextureFadeCountLerp[2];

void render_screen_transition(s8 fadeTimer, s8 transType, u8 transTime, struct WarpTransitionData *transData);
Gfx *geo_cannon_circle_base(s32 callContext, struct GraphNode *node, UNUSED Mat4 mtx);
s32 screen_transition_logic(s8 fadeTimer, s8 transType, u8 transTime, struct WarpTransitionData *transData);

#endif // SCREEN_TRANSITION_H
