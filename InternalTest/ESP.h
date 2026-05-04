#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdint>
#include "Globals.h"

struct Vec3 { float x, y, z; };

template<typename T>
T readField(uintptr_t base, uintptr_t offset) {
    return *(T*)(base + offset);
}

typedef void (APIENTRY* t_glBegin)(GLenum);
typedef void (APIENTRY* t_glEnd)();
typedef void (APIENTRY* t_glVertex2f)(GLfloat, GLfloat);
typedef void (APIENTRY* t_glColor3f)(GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY* t_glPointSize)(GLfloat);
typedef void (APIENTRY* t_glDisable)(GLenum);
typedef void (APIENTRY* t_glEnable)(GLenum);
typedef void (APIENTRY* t_glMatrixMode)(GLenum);
typedef void (APIENTRY* t_glLoadIdentity)();
typedef void (APIENTRY* t_glPushMatrix)();
typedef void (APIENTRY* t_glPopMatrix)();
typedef void (APIENTRY* t_glPushAttrib)(GLbitfield);
typedef void (APIENTRY* t_glPopAttrib)();
typedef void (APIENTRY* t_glOrtho)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (APIENTRY* t_glGetDoublev)(GLenum, GLdouble*);
typedef void (APIENTRY* t_glGetIntegerv)(GLenum, GLint*);
typedef GLint(APIENTRY* t_gluProject)(GLdouble