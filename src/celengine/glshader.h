// glshader.h
//
// Copyright (C) 2001-2006, Chris Laurel <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "glsupport.h"

class GLShaderLoader;

enum GLShaderStatus
{
    ShaderStatus_OK,
    ShaderStatus_CompileError,
    ShaderStatus_LinkError,
    ShaderStatus_OutOfMemory,
    ShaderStatus_EmptyProgram,
};

class GLShader
{
 protected:
    GLShader(GLuint _id);
    virtual ~GLShader();

 public:
    GLuint getID() const;

 private:
    GLuint id;

    GLShaderStatus compile(const std::vector<std::string>& source);

    friend class GLShaderLoader;
};


class GLVertexShader : public GLShader
{
 private:
    GLVertexShader(GLuint _id) : GLShader(_id) {};

 friend class GLShaderLoader;
};


class GLGeometryShader : public GLShader
{
 private:
    GLGeometryShader(GLuint _id) : GLShader(_id) {};

 friend class GLShaderLoader;
};


class GLFragmentShader : public GLShader
{
 private:
    GLFragmentShader(GLuint _id) : GLShader(_id) {};

 friend class GLShaderLoader;
};


class GLProgram
{
 private:
    GLProgram(GLuint _id);

    void attach(const GLShader&);

 public:
    virtual ~GLProgram();

    GLShaderStatus link();

    void use() const;
    GLuint getID() const { return id; }

 private:
    GLuint id;

 friend class GLShaderLoader;
};


class FloatShaderParameter
{
 public:
    FloatShaderParameter();
    FloatShaderParameter(GLuint obj, const char* name);

    FloatShaderParameter& operator=(float);

 private:
    int slot;
};


class Vec3ShaderParameter
{
 public:
    Vec3ShaderParameter();
    Vec3ShaderParameter(GLuint obj, const char* name);

    Vec3ShaderParameter& operator=(const Eigen::Vector3f&);

 private:
    int slot;
};


class Vec4ShaderParameter
{
 public:
    Vec4ShaderParameter();
    Vec4ShaderParameter(GLuint obj, const char* name);

    Vec4ShaderParameter& operator=(const Eigen::Vector4f&);

 private:
    int slot;
};


class IntegerShaderParameter
{
 public:
    IntegerShaderParameter();
    IntegerShaderParameter(GLuint obj, const char* name);

    IntegerShaderParameter& operator=(int);

 private:
    int slot;
};


class Mat3ShaderParameter
{
 public:
    Mat3ShaderParameter();
    Mat3ShaderParameter(GLuint obj, const char* name);

    Mat3ShaderParameter& operator=(const Eigen::Matrix3f&);

 private:
    int slot;
};


class Mat4ShaderParameter
{
 public:
    Mat4ShaderParameter();
    Mat4ShaderParameter(GLuint obj, const char* name);

    Mat4ShaderParameter& operator=(const Eigen::Matrix4f&);

 private:
    int slot;
};


class GLShaderLoader
{
 public:
    static GLShaderStatus CreateVertexShader(const std::vector<std::string>&,
                                             GLVertexShader**);
    static GLShaderStatus CreateGeometryShader(const std::vector<std::string>&,
                                               GLGeometryShader**);
    static GLShaderStatus CreateFragmentShader(const std::vector<std::string>&,
                                               GLFragmentShader**);
    static GLShaderStatus CreateVertexShader(const std::string&,
                                             GLVertexShader**);
    static GLShaderStatus CreateFragmentShader(const std::string&,
                                               GLFragmentShader**);

    static GLShaderStatus CreateProgram(const GLVertexShader& vs,
                                        const GLFragmentShader& fs,
                                        GLProgram**);
    static GLShaderStatus CreateProgram(const GLVertexShader& vs,
                                        const GLGeometryShader& gs,
                                        const GLFragmentShader& fs,
                                        GLProgram**);
    static GLShaderStatus CreateProgram(const std::vector<std::string>& vs,
                                        const std::vector<std::string>& fs,
                                        GLProgram**);
    static GLShaderStatus CreateProgram(const std::vector<std::string>& vs,
                                        const std::vector<std::string>& gs,
                                        const std::vector<std::string>& fs,
                                        GLProgram**);
    static GLShaderStatus CreateProgram(const std::string& vsSource,
                                        const std::string& fsSource,
                                        GLProgram**);
    static GLShaderStatus CreateProgram(const std::string& vsSource,
                                        const std::string& fsSource,
                                        const std::string& gsSource,
                                        GLProgram**);
};


extern std::ostream* g_shaderLogFile;
