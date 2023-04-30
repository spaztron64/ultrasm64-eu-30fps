#include <PR/ultratypes.h>

#include "sm64.h"
#include "actors/common1.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "level_update.h"
#include "camera.h"
#include "print.h"
#include "ingame_menu.h"
#include "hud.h"
#include "segment2.h"
#include "area.h"
#include "save_file.h"
#include "print.h"

/* @file hud.c
 * This file implements HUD rendering and power meter animations.
 * That includes stars, lives, coins, camera status, power meter, timer
 * cannon reticle, and the unused keys.
 **/

struct PowerMeterHUD {
    s8 animation;
    s16 x;
    s16 y;
    f32 lerpY;
};

struct CameraHUD {
    s16 status;
};

// Stores health segmented value defined by numHealthWedges
// When the HUD is rendered this value is 8, full health.
static s16 sPowerMeterStoredHealth;

static struct PowerMeterHUD sPowerMeterHUD = {
    POWER_METER_HIDDEN,
    140,
    166,
    1.0f,
};

// Power Meter timer that keeps counting when it's visible.
// Gets reset when the health is filled and stops counting
// when the power meter is hidden.
s32 sPowerMeterVisibleTimer = 0;

static struct CameraHUD sCameraHUD = { CAM_STATUS_NONE };

/**
 * Renders a rgba16 16x16 glyph texture from a table list.
 */
void render_hud_tex_lut(s32 x, s32 y, u8 *texture) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gSPDisplayList(gDisplayListHead++, &dl_hud_img_load_tex_block);
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 15) << 2, (y + 15) << 2,
                        G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders a rgba16 8x8 glyph texture from a table list.
 */
void render_hud_small_tex_lut(s32 x, s32 y, u8 *texture) {
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0,
                G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD);
    gDPTileSync(gDisplayListHead++);
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0,
                G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD);
    gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC);
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES));
    gSPTextureRectangle(gDisplayListHead++, x << 2, y << 2, (x + 7) << 2, (y + 7) << 2, G_TX_RENDERTILE,
                        0, 0, 4 << 10, 1 << 10);
}

/**
 * Renders power meter health segment texture using a table list.
 */
void render_power_meter_health_segment(s16 numHealthWedges) {
    u8 *(*healthLUT)[] = segmented_to_virtual(&power_meter_health_segments_lut);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                       (*healthLUT)[numHealthWedges - 1]);
    gDPLoadSync(gDisplayListHead++);
    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES));
    gSP2Triangles(gDisplayListHead++, 0, 1, 2, 0, 0, 2, 3, 0);
}

/**
 * Renders power meter display lists.
 * That includes the "POWER" base and the colored health segment textures.
 */
void render_dl_power_meter(s16 numHealthWedges) {
    Mtx *mtx = alloc_display_list(sizeof(Mtx));

    if (mtx == NULL) {
        return;
    }

    guTranslate(mtx, (f32) sPowerMeterHUD.x, (f32) sPowerMeterHUD.y, 0);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx++),
              G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gDisplayListHead++, &dl_power_meter_base);

    if (numHealthWedges != 0) {
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_begin);
        render_power_meter_health_segment(numHealthWedges);
        gSPDisplayList(gDisplayListHead++, &dl_power_meter_health_segments_end);
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

/**
 * Power meter animation called when there's less than 8 health segments
 * Checks its timer to later change into deemphasizing mode.
 */
void animate_power_meter_emphasized(void) {
    s16 hudDisplayFlags = gHudDisplay.flags;

    if (!(hudDisplayFlags & HUD_DISPLAY_FLAG_EMPHASIZE_POWER)) {
        if (sPowerMeterVisibleTimer == 45) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
        }
    } else {
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Power meter animation called after emphasized mode.
 * Moves power meter y pos speed until it's at 200 to be visible.
 */
static void animate_power_meter_deemphasizing(void) {
    s16 speed = 5;

    if (sPowerMeterHUD.y > 180) {
        speed = 3;
    }

    if (sPowerMeterHUD.y > 190) {
        speed = 2;
    }

    if (sPowerMeterHUD.y > 195) {
        speed = 1;
    }

    sPowerMeterHUD.y += speed;

    if (sPowerMeterHUD.y > 200) {
        sPowerMeterHUD.y = 200;
        sPowerMeterHUD.animation = POWER_METER_VISIBLE;
    }
}

/**
 * Power meter animation called when there's 8 health segments.
 * Moves power meter y pos quickly until it's at 301 to be hidden.
 */
static void animate_power_meter_hiding(void) {
    sPowerMeterHUD.y += 20;
    if (sPowerMeterHUD.y > 300) {
        sPowerMeterHUD.animation = POWER_METER_HIDDEN;
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Handles power meter actions depending of the health segments values.
 */
void handle_power_meter_actions(s16 numHealthWedges) {
    // Show power meter if health is not full, less than 8
    if (numHealthWedges < 8 && sPowerMeterStoredHealth == 8
        && sPowerMeterHUD.animation == POWER_METER_HIDDEN) {
        sPowerMeterHUD.animation = POWER_METER_EMPHASIZED;
        sPowerMeterHUD.y = 166;
        sPowerMeterHUD.lerpY = 166;
    }

    // Show power meter if health is full, has 8
    if (numHealthWedges == 8 && sPowerMeterStoredHealth == 7) {
        sPowerMeterVisibleTimer = 0;
    }

    // After health is full, hide power meter
    if (numHealthWedges == 8 && sPowerMeterVisibleTimer > 45) {
        sPowerMeterHUD.animation = POWER_METER_HIDING;
    }

    // Update to match health value
    sPowerMeterStoredHealth = numHealthWedges;

    // If Mario is swimming, keep power meter visible
    if (gPlayerCameraState->action & ACT_FLAG_SWIMMING) {
        if (sPowerMeterHUD.animation == POWER_METER_HIDDEN
            || sPowerMeterHUD.animation == POWER_METER_EMPHASIZED) {
            sPowerMeterHUD.animation = POWER_METER_DEEMPHASIZING;
            sPowerMeterHUD.y = 166;
            sPowerMeterHUD.lerpY = 166;
        }
        sPowerMeterVisibleTimer = 0;
    }
}

/**
 * Renders the power meter that shows when Mario is in underwater
 * or has taken damage and has less than 8 health segments.
 * And calls a power meter animation function depending of the value defined.
 */
void render_hud_power_meter(void) {
    s16 shownHealthWedges = gHudDisplay.wedges;

    if (sPowerMeterHUD.animation == POWER_METER_HIDDEN) {
        return;
    }

    sPowerMeterHUD.lerpY = approach_f32_asymptotic(sPowerMeterHUD.lerpY, sPowerMeterHUD.y, gLerpSpeed);

    render_dl_power_meter(shownHealthWedges);
}

#ifdef VERSION_JP
#define HUD_TOP_Y 210
#else
#define HUD_TOP_Y 209
#endif

/**
 * Renders the amount of lives Mario has.
 */
void render_hud_mario_lives(void) {
    print_text(22, HUD_TOP_Y, ","); // 'Mario Head' glyph
    print_text(38, HUD_TOP_Y, "*"); // 'X' glyph
    print_text_fmt_int(54, HUD_TOP_Y, "%d", gHudDisplay.lives);
}

/**
 * Renders the amount of coins collected.
 */
void render_hud_coins(void) {
    print_text(gScreenWidth - 152, HUD_TOP_Y, "+"); // 'Coin' glyph
    print_text(gScreenWidth - 136, HUD_TOP_Y, "*"); // 'X' glyph
    print_text_fmt_int(gScreenWidth - 122, HUD_TOP_Y, "%d", gHudDisplay.coins);
}

#ifdef VERSION_JP
#define HUD_STARS_X 73
#else
#define HUD_STARS_X 78
#endif

/**
 * Renders the amount of stars collected.
 * Disables "X" glyph when Mario has 100 stars or more.
 */
void render_hud_stars(void) {
    s8 showX = 0;

    if (gHudFlash == 1 && gGlobalTimer & 8) {
        return;
    }

    if (gHudDisplay.stars < 100) {
        showX = 1;
    }

    print_text(gScreenWidth - (HUD_STARS_X), HUD_TOP_Y, "-"); // 'Star' glyph
    if (showX == 1) {
        print_text(gScreenWidth - (HUD_STARS_X) + 16, HUD_TOP_Y, "*"); // 'X' glyph
    }
    print_text_fmt_int((showX * 14) + gScreenWidth - (HUD_STARS_X - 16),
                       HUD_TOP_Y, "%d", gHudDisplay.stars);
}

/**
 * Renders the timer when Mario start sliding in PSS.
 */
void render_hud_timer(void) {
    u8 *(*hudLUT)[58] = segmented_to_virtual(&main_hud_lut);
    u16 timerValFrames = gHudDisplay.timer;
    u16 timerMins = timerValFrames / (30 * 60);
    u16 timerSecs = (timerValFrames - (timerMins * 1800)) / 30;
    u16 timerFracSecs = ((timerValFrames - (timerMins * 1800) - (timerSecs * 30)) & 0xFFFF) / 3;

#ifdef VERSION_EU
    switch (eu_get_language()) {
        case LANGUAGE_ENGLISH:
            print_text(gScreenWidth - (150), 185, "TIME");
            break;
        case LANGUAGE_FRENCH:
            print_text(gScreenWidth - (155), 185, "TEMPS");
            break;
        case LANGUAGE_GERMAN:
            print_text(gScreenWidth - (150), 185, "ZEIT");
            break;
    }
#else
    print_text(gScreenWidth - (150), 185, "TIME");
#endif

    print_text_fmt_int(gScreenWidth - (91), 185, "%0d", timerMins);
    print_text_fmt_int(gScreenWidth - (71), 185, "%02d", timerSecs);
    print_text_fmt_int(gScreenWidth - (37), 185, "%d", timerFracSecs);

    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
    render_hud_tex_lut(gScreenWidth - (81), 32, (*hudLUT)[GLYPH_APOSTROPHE]);
    render_hud_tex_lut(gScreenWidth - (46), 32, (*hudLUT)[GLYPH_DOUBLE_QUOTE]);
    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
}

/**
 * Sets HUD status camera value depending of the actions
 * defined in update_camera_status.
 */
void set_hud_camera_status(s16 status) {
    sCameraHUD.status = status;
}

/**
 * Renders camera HUD glyphs using a table list, depending of
 * the camera status called, a defined glyph is rendered.
 */
void render_hud_camera_status(void) {
    u8 *(*cameraLUT)[6] = segmented_to_virtual(&main_hud_camera_lut);
    s32 x = gScreenWidth - (54);
    s32 y = gScreenHeight - 35;

    if (sCameraHUD.status == CAM_STATUS_NONE) {
        return;
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
    render_hud_tex_lut(x, y, (*cameraLUT)[GLYPH_CAM_CAMERA]);

    switch (sCameraHUD.status & CAM_STATUS_MODE_GROUP) {
        case CAM_STATUS_MARIO:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_MARIO_HEAD]);
            break;
        case CAM_STATUS_LAKITU:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_LAKITU_HEAD]);
            break;
        case CAM_STATUS_FIXED:
            render_hud_tex_lut(x + 16, y, (*cameraLUT)[GLYPH_CAM_FIXED]);
            break;
    }

    switch (sCameraHUD.status & CAM_STATUS_C_MODE_GROUP) {
        case CAM_STATUS_C_DOWN:
            render_hud_small_tex_lut(x + 4, y + 16, (*cameraLUT)[GLYPH_CAM_ARROW_DOWN]);
            break;
        case CAM_STATUS_C_UP:
            render_hud_small_tex_lut(x + 4, y - 8, (*cameraLUT)[GLYPH_CAM_ARROW_UP]);
            break;
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
}

#include "profiler.h"
#define    OS_CLOCK_RATE        62500000LL
#define    OS_CPU_COUNTER        (OS_CLOCK_RATE*3/4)
#define OS_CYCLES_TO_USEC(c)    (((u32)(c)*(1000000LL/15625LL))/(OS_CPU_COUNTER/15625LL))
#define FRAMETIME_COUNT 30
#define PROFILER_COUNT 60
#define NUM_PERF_ITERATIONS 30
#define PERF_AGGREGATE NUM_PERF_ITERATIONS
#define PERF_TOTAL NUM_PERF_ITERATIONS + 1


#ifdef PUPPYPRINT_DEBUG
u8 gFPS = 30;
OSTime frameTimes[FRAMETIME_COUNT];
OSTime frameTime1;
u8 curFrameTimeIndex = 0;
extern s16 gCurrentFrameIndex1;
extern s16 gCurrentFrameIndex3;
extern struct ProfilerFrameData gProfilerFrameData[2];
u8 gShowProfilerNew = TRUE;
u32 gVideoTime = 0;
u32 gSoundTime = 0;
u32 gGameTime = 0;
u32 totalCPUReads[PROFILER_COUNT + 2];
u32 totalRSPReads[PROFILER_COUNT + 2];
u32 totalRDPReads[PROFILER_COUNT + 2];
u8 perfIteration = 0;

// Call once per frame
void calculate_and_update_fps(void) {
    OSTime newTime = osGetTime();
    OSTime oldTime = frameTimes[curFrameTimeIndex];
    frameTimes[curFrameTimeIndex] = newTime;

    curFrameTimeIndex++;
    if (curFrameTimeIndex >= FRAMETIME_COUNT) {
        curFrameTimeIndex = 0;
    }
    frameTime1 = (s32)OS_CYCLES_TO_USEC(newTime - oldTime) / 3;
    gFPS = (FRAMETIME_COUNT * 1000000.0f) / (s32)OS_CYCLES_TO_USEC(newTime - oldTime);
}

extern u32 sPoolFreeSpace;

void render_profiler(void) {
    struct ProfilerFrameData *profilerGfx;
    profilerGfx = &gProfilerFrameData[gCurrentFrameIndex3 ^ 1];
    totalRSPReads[PROFILER_COUNT] -= totalRSPReads[perfIteration];
    totalRSPReads[perfIteration] = OS_CYCLES_TO_USEC(profilerGfx->gfxTimes[1] - profilerGfx->gfxTimes[0]);
    totalRSPReads[PROFILER_COUNT] += OS_CYCLES_TO_USEC(profilerGfx->gfxTimes[1] - profilerGfx->gfxTimes[0]);
    totalRDPReads[PROFILER_COUNT] -= totalRDPReads[perfIteration];
    totalRDPReads[perfIteration] = OS_CYCLES_TO_USEC(profilerGfx->gfxTimes[2] - profilerGfx->gfxTimes[0]);
    totalRDPReads[PROFILER_COUNT] += OS_CYCLES_TO_USEC(profilerGfx->gfxTimes[2] - profilerGfx->gfxTimes[0]);
    totalRSPReads[PROFILER_COUNT + 1] = totalRSPReads[PROFILER_COUNT] / PROFILER_COUNT;
    totalRDPReads[PROFILER_COUNT + 1] = totalRDPReads[PROFILER_COUNT] / PROFILER_COUNT;
    calculate_and_update_fps();
    print_text_fmt_int(32, 240 - 32, "FPS   %d", gFPS);
    print_text_fmt_int(32, 240 - 48, "FTIME %d", frameTime1);
    print_text_fmt_int(32, 240 - 64, "CPU   %d", (u32) totalCPUReads[PROFILER_COUNT + 1]);
    print_text_fmt_int(32, 240 - 80, "RSP   %d", (u32) totalRSPReads[PROFILER_COUNT + 1]);
    print_text_fmt_int(32, 240 - 96, "RDP   %d", (u32) totalRDPReads[PROFILER_COUNT + 1]);
    print_text_fmt_int(32, 240 - (gScreenHeight - 24), "RAM   %dK FREE", (u32) sPoolFreeSpace / 1024);
}

void profiler_logic(void) {
    struct ProfilerFrameData *profiler;
    u32 clockStart;
    u32 levelScriptDuration;
    u32 renderDuration;
    u32 soundDuration;
    u32 taskStart;
    u32 rdpTime;
    u32 cpuTime;
    taskStart = 0;
    profiler = &gProfilerFrameData[gCurrentFrameIndex1 ^ 1];
    clockStart = profiler->gameTimes[0] <= profiler->soundTimes[0] ? profiler->gameTimes[0] : profiler->soundTimes[0];
    levelScriptDuration = profiler->gameTimes[1] - clockStart;
    renderDuration = profiler->gameTimes[2] - profiler->gameTimes[1];
    profiler->numSoundTimes &= 0xFFFE;
    for (s32 i = 0; i < profiler->numSoundTimes; i += 2) {
        // calculate sound duration of max - min
        soundDuration = profiler->soundTimes[i + 1] - profiler->soundTimes[i];
        taskStart += soundDuration;
        // was the sound time minimum less than level script execution?
        if (profiler->soundTimes[i] < profiler->gameTimes[1]) {
            // overlay the levelScriptDuration onto the profiler by subtracting the soundDuration.
            levelScriptDuration -= soundDuration;
        } else if (profiler->soundTimes[i] < profiler->gameTimes[2]) {
            // overlay the renderDuration onto the profiler by subtracting the soundDuration.
            renderDuration -= soundDuration;
        }
    }
    profiler->numSoundTimes &= 0xFFFE;

    totalCPUReads[PROFILER_COUNT] -= totalCPUReads[perfIteration];
    cpuTime = MIN(OS_CYCLES_TO_USEC(gVideoTime + (gSoundTime * 2) + gGameTime), 66666);
    totalCPUReads[perfIteration] = cpuTime;
    totalCPUReads[PROFILER_COUNT] += cpuTime;
    totalCPUReads[PROFILER_COUNT + 1] = totalCPUReads[PROFILER_COUNT] / PROFILER_COUNT;

    perfIteration++;
    if (perfIteration == PROFILER_COUNT) {
        perfIteration = 0;
    }
}
#endif

void ui_logic(void) {
    s16 hudDisplayFlags = gHudDisplay.flags;
    
#ifdef PUPPYPRINT_DEBUG
    if (gPlayer1Controller->buttonDown & U_JPAD && gPlayer1Controller->buttonPressed & L_TRIG) {
        gShowProfilerNew ^= 1;
    }
    profiler_logic();
#endif

    if (hudDisplayFlags & HUD_DISPLAY_FLAG_CAMERA_AND_POWER) {
        s16 shownHealthWedges = gHudDisplay.wedges;

        if (sPowerMeterHUD.animation != POWER_METER_HIDING) {
            handle_power_meter_actions(shownHealthWedges);
        }

        if (sPowerMeterHUD.animation == POWER_METER_HIDDEN) {
            return;
        }

        switch (sPowerMeterHUD.animation) {
            case POWER_METER_EMPHASIZED:
                animate_power_meter_emphasized();
                break;
            case POWER_METER_DEEMPHASIZING:
                animate_power_meter_deemphasizing();
                break;
            case POWER_METER_HIDING:
                animate_power_meter_hiding();
                break;
            default:
                break;
        }
        sPowerMeterVisibleTimer++;
    }
}

/**
 * Render HUD strings using hudDisplayFlags with it's render functions,
 * excluding the cannon reticle which detects a camera preset for it.
 */
void render_hud(void) {
    s16 hudDisplayFlags = gHudDisplay.flags;

    if (hudDisplayFlags == HUD_DISPLAY_NONE) {
        sPowerMeterHUD.animation = POWER_METER_HIDDEN;
        sPowerMeterStoredHealth = 8;
        sPowerMeterVisibleTimer = 0;
    } else {
#ifdef VERSION_EU
        // basically create_dl_ortho_matrix but guOrtho screen width is different
        Mtx *mtx = alloc_display_list(sizeof(*mtx));

        if (mtx == NULL) {
            return;
        }

        create_dl_identity_matrix();
        guOrtho(mtx, -16.0f, SCREEN_WIDTH + 16, 0, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);
        gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx),
                  G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
#else
        create_dl_ortho_matrix();
#endif

        if (gCurrentArea != NULL && gCurrentArea->camera->mode == CAMERA_MODE_INSIDE_CANNON) {
            render_hud_cannon_reticle();
        }

        if (hudDisplayFlags & HUD_DISPLAY_FLAG_LIVES) {
            render_hud_mario_lives();
        }

        if (hudDisplayFlags & HUD_DISPLAY_FLAG_COIN_COUNT) {
            render_hud_coins();
        }

        if (hudDisplayFlags & HUD_DISPLAY_FLAG_STAR_COUNT) {
            render_hud_stars();
        }

        if (hudDisplayFlags & HUD_DISPLAY_FLAG_CAMERA_AND_POWER) {
            render_hud_power_meter();
            render_hud_camera_status();
        }

        if (hudDisplayFlags & HUD_DISPLAY_FLAG_TIMER) {
            render_hud_timer();
        }
    }

#ifdef PUPPYPRINT_DEBUG
    if (gShowProfilerNew) {
        render_profiler();
    }
#endif
}
