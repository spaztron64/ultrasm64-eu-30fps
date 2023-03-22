// 0x0701FDC8 - 0x0701FEB8
static const Vtx hmc_seg7_vertex_0701FDC8[] = {
    {{{   410,      0,     51}, 0, {     0,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,    717,    -50}, 0, {   990,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,    717,     51}, 0, {     0,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,    717,     51}, 0, {  3034,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,    717,    -50}, 0, {  3034,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,    717,    -50}, 0, { -1052,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,    717,     51}, 0, { -1052,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,    717,     51}, 0, {     0,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,    717,    -50}, 0, {   990,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,      0,    -50}, 0, {   990,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,      0,     51}, 0, {     0,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,      0,     51}, 0, { -1052,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,      0,    -50}, 0, { -1052,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,      0,    -50}, 0, {  3034,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,      0,     51}, 0, {  3034,      0}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x0701FEB8 - 0x0701FF68
static const Vtx hmc_seg7_vertex_0701FEB8[] = {
    {{{   410,    717,     51}, 0, {  2012,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,      0,     51}, 0, { -2074,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,      0,     51}, 0, {  2012,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,      0,     51}, 0, {     0,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,      0,    -50}, 0, {   990,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,    717,    -50}, 0, {   990,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,      0,    -50}, 0, {  2012,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,      0,    -50}, 0, { -2074,   4054}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,    717,    -50}, 0, { -2074,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   410,    717,    -50}, 0, {  2012,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
    {{{     0,    717,     51}, 0, { -2074,  -3098}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x0701FF68 - 0x0701FFF8
static const Gfx hmc_seg7_dl_0701FF68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, cave_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(hmc_seg7_vertex_0701FDC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(hmc_seg7_vertex_0701FEB8, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP1Triangle( 0, 10,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701FFF8 - 0x070200B0
const Gfx hmc_seg7_dl_0701FFF8[] = {
    gsDPPipeSync(),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(hmc_seg7_dl_0701FF68),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};
