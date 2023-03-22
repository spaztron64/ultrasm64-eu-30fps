// 0x070285F0 - 0x07028660
const Gfx ssl_dl_pyramid_sand_pathway_floor_begin[] = {
    gsDPPipeSync(),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x07028660 - 0x070286A0
const Gfx ssl_dl_pyramid_sand_pathway_floor_end[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsSPGeometryModeSetFirst(0, G_LIGHTING | G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
    gsSPEndDisplayList(),
};

// 0x070286A0 - 0x07028718
const Gfx ssl_dl_pyramid_sand_pathway_begin[] = {
    gsDPPipeSync(),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetEnvColor(255, 255, 255, 180),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x07028718 - 0x07028760
const Gfx ssl_dl_pyramid_sand_pathway_end[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsSPGeometryModeSetFirst(0, G_LIGHTING | G_CULL_BACK),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
    gsSPEndDisplayList(),
};

// 0x07028760 - 0x070287B8
Movtex ssl_movtex_tris_pyramid_sand_pathway_front[] = {
    MOV_TEX_SPD(   50),
    MOV_TEX_TRIS( 102, 1229, -742, 0, 0),
    MOV_TEX_TRIS( 102, 4275, -742, 5, 0),
    MOV_TEX_TRIS( 102, 4300, -768, 6, 0),
    MOV_TEX_TRIS( 102, 4300, -870, 8, 0),
    MOV_TEX_TRIS(-102, 1229, -742, 0, 1),
    MOV_TEX_TRIS(-102, 4275, -742, 5, 1),
    MOV_TEX_TRIS(-102, 4300, -768, 6, 1),
    MOV_TEX_TRIS(-102, 4300, -870, 8, 1),
    MOV_TEX_END(),
    0, // alignment?
};

// 0x070287B8 - 0x070287F0
const Gfx ssl_dl_pyramid_sand_pathway_front_end[] = {
    gsSP2Triangles( 0,  1,  4, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 1,  2,  5, 0x0,  5,  2,  6, 0x0),
    gsSP2Triangles( 2,  3,  6, 0x0,  6,  3,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x070287F0 - 0x07028844
Movtex ssl_movtex_tris_pyramid_sand_pathway_floor[] = {
    MOV_TEX_SPD(     8),
    MOV_TEX_TRIS( 1178, 1229, 2150, 0, 0),
    MOV_TEX_TRIS(-1741, 1229, 2150, 2, 0),
    MOV_TEX_TRIS(-1741, 1229, -589, 4, 0),
    MOV_TEX_TRIS(  154, 1229, -589, 5, 0),
    MOV_TEX_TRIS( 1178, 1229, 2560, 0, 1),
    MOV_TEX_TRIS(-2150, 1229, 2560, 2, 1),
    MOV_TEX_TRIS(-2150, 1229, -794, 4, 1),
    MOV_TEX_TRIS(  154, 1229, -794, 5, 1),
    MOV_TEX_END(),
};

// 0x07028844 - 0x07028888
Movtex ssl_movtex_tris_pyramid_sand_pathway_side[] = {
    MOV_TEX_SPD(   50),
    MOV_TEX_TRIS(1229, -307, 2150, 0, 0),
    MOV_TEX_TRIS(1229, 1168, 2150, 1, 0),
    MOV_TEX_TRIS(1178, 1229, 2150, 2, 0),
    MOV_TEX_TRIS(1229, -307, 2560, 0, 1),
    MOV_TEX_TRIS(1229, 1168, 2560, 1, 1),
    MOV_TEX_TRIS(1178, 1229, 2560, 2, 1),
    MOV_TEX_END(),
    0, // alignment?
};

// 0x07028888 - 0x070288B0
const Gfx ssl_dl_pyramid_sand_pathway_side_end[] = {
    gsSP2Triangles( 0,  1,  3, 0x0,  1,  4,  3, 0x0),
    gsSP2Triangles( 1,  2,  4, 0x0,  2,  5,  4, 0x0),
    gsSPEndDisplayList(),
};
