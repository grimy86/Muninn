#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdint>
#include <cmath>
#include <stdio.h>
#include "ESP.h"

// function pointer definitions
t_glBegin        p_glBegin = nullptr;
t_glEnd          p_glEnd = nullptr;
t_glVertex2f     p_glVertex2f = nullptr;
t_glColor3f      p_glColor3f = nullptr;
t_glPointSize    p_glPointSize = nullptr;
t_glDisable      p_glDisable = nullptr;
t_glEnable       p_glEnable = nullptr;
t_glMatrixMode   p_glMatrixMode = nullptr;
t_glLoadIdentity p_glLoadIdentity = nullptr;
t_glPushMatrix   p_glPushMatrix = nullptr;
t_glPopMatrix    p_glPopMatrix = nullptr;
t_glPushAttrib   p_glPushAttrib = nullptr;
t_glPopAttrib    p_glPopAttrib = nullptr;
t_glOrtho        p_glOrtho = nullptr;
t_glGetDoublev   p_glGetDoublev = nullptr;
t_glGetIntegerv  p_glGetIntegerv = nullptr;
t_gluProject     p_gluProject = nullptr;

// matrix globals
GLdouble g_mv[16] = {};
GLdouble g_proj[16] = {};
GLint    g_vp[4] = {};
bool     g_matricesReady = false;

uintptr_t getLocalPlayer() {
    return *(uintptr_t*)AssaultCube::localPlayer;
}

uintptr_t getEntity(int index) {
    uintptr_t listBase = *(uintptr_t*)AssaultCube::entityList;
    if (!listBase) return 0;
    return *(uintptr_t*)(listBase + index * 0x4);
}

Vec3 getPos(uintptr_t entity) {
    return {
        readField<float>(entity, AssaultCube::posX),
        readField<float>(entity, AssaultCube::posY),
        readField<float>(entity, AssaultCube::posZ)
    };
}

int getHealth(uintptr_t entity) {
    return readField<int>(entity, AssaultCube::health);
}

int getTeam(uintptr_t entity) {
    return readField<int>(entity, AssaultCube::team);
}

void resolveGL() {
    HMODULE gl = GetModuleHandleA("opengl32.dll");
    HMODULE glu = GetModuleHandleA("glu32.dll");
    printf("opengl32.dll: %p\n", gl);
    printf("glu32.dll: %p\n", glu);
    if (!gl || !glu) return;

    p_glBegin = (t_glBegin)GetProcAddress(gl, "glBegin");
    p_glEnd = (t_glEnd)GetProcAddress(gl, "glEnd");
    p_glVertex2f = (t_glVertex2f)GetProcAddress(gl, "glVertex2f");
    p_glColor3f = (t_glColor3f)GetProcAddress(gl, "glColor3f");
    p_glPointSize = (t_glPointSize)GetProcAddress(gl, "glPointSize");
    p_glDisable = (t_glDisable)GetProcAddress(gl, "glDisable");
    p_glEnable = (t_glEnable)GetProcAddress(gl, "glEnable");
    p_glMatrixMode = (t_glMatrixMode)GetProcAddress(gl, "glMatrixMode");
    p_glLoadIdentity = (t_glLoadIdentity)GetProcAddress(gl, "glLoadIdentity");
    p_glPushMatrix = (t_glPushMatrix)GetProcAddress(gl, "glPushMatrix");
    p_glPopMatrix = (t_glPopMatrix)GetProcAddress(gl, "glPopMatrix");
    p_glPushAttrib = (t_glPushAttrib)GetProcAddress(gl, "glPushAttrib");
    p_glPopAttrib = (t_glPopAttrib)GetProcAddress(gl, "glPopAttrib");
    p_glOrtho = (t_glOrtho)GetProcAddress(gl, "glOrtho");
    p_glGetDoublev = (t_glGetDoublev)GetProcAddress(gl, "glGetDoublev");
    p_glGetIntegerv = (t_glGetIntegerv)GetProcAddress(gl, "glGetIntegerv");
    p_gluProject = (t_gluProject)GetProcAddress(glu, "gluProject");
}

bool worldToScreen(Vec3 pos, float& sx, float& sy) {
    if (!g_matricesReady) return false;
    GLdouble ox, oy, oz;
    if (!p_gluProject(pos.x, pos.y, pos.z, g_mv, g_proj, g_vp, &ox, &oy, &oz))
        return false;
    if (oz < 0.0 || oz > 1.0) return false;
    sx = (float)ox;
    sy = (float)(g_vp[3] - oy);
    return true;
}

void drawDot(float x, float y) {
    p_glPointSize(6.0f);
    p_glColor3f(1.f, 0.f, 0.f);
    p_glBegin(GL_POINTS);
    p_glVertex2f(x, y);
    p_glEnd();
}

void renderESP() {
    if (!g_matricesReady) return;

    p_glPushAttrib(GL_ALL_ATTRIB_BITS);
    p_glMatrixMode(GL_PROJECTION);
    p_glPushMatrix();
    p_glLoadIdentity();
    p_glOrtho(0, g_vp[2], g_vp[3], 0, -1, 1);
    p_glMatrixMode(GL_MODELVIEW);
    p_glPushMatrix();
    p_glLoadIdentity();
    p_glDisable(GL_DEPTH_TEST);
    p_glDisable(GL_TEXTURE_2D);

    uintptr_t localPl = getLocalPlayer();
    int localTeam = getTeam(localPl);

    for (int i = 0; i < 32; i++) {
        uintptr_t ent = getEntity(i);
        if (!ent || ent == localPl) continue;
        if (IsBadReadPtr((void*)ent, 256)) continue;
        if (getHealth(ent) <= 0) continue;
        if (getTeam(ent) == localTeam) continue;

        Vec3 pos = getPos(ent);
        pos.z += 0.5f;
        float sx, sy;
        if (worldToScreen(pos, sx, sy))
            drawDot(sx, sy);
    }

    p_glMatrixMode(GL_PROJECTION);
    p_glPopMatrix();
    p_glMatrixMode(GL_MODELVIEW);
    p_glPopMatrix();
    p_glPopAttrib();
}