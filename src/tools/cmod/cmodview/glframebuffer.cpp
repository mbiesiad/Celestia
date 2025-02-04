// glframebuffer.cpp
//
// Copyright (C) 2004-2010, Chris Laurel <claurel@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// C++ wrapper for OpenGL framebuffer objects.

#include "glframebuffer.h"


GLFrameBufferObject::GLFrameBufferObject(GLuint width, GLuint height, unsigned int attachments) :
    m_width(width),
    m_height(height)
{
    if (attachments != 0)
        generateFbo(attachments);
}


GLFrameBufferObject::~GLFrameBufferObject()
{
    cleanup();
}


bool
GLFrameBufferObject::isValid() const
{
    return m_status == GL_FRAMEBUFFER_COMPLETE;
}


GLuint
GLFrameBufferObject::colorTexture() const
{
    return m_colorTexId;
}


GLuint
GLFrameBufferObject::depthTexture() const
{
    return m_depthTexId;
}


void
GLFrameBufferObject::generateColorTexture()
{
    // Create and bind the texture
    glGenTextures(1, &m_colorTexId);
    glBindTexture(GL_TEXTURE_2D, m_colorTexId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Clamp to edge
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Set the texture dimensions
    // Do we need to set GL_DEPTH_COMPONENT24 here?
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}


void
GLFrameBufferObject::generateDepthTexture()
{
    // Create and bind the texture
    glGenTextures(1, &m_depthTexId);
    glBindTexture(GL_TEXTURE_2D, m_depthTexId);

    // Only nearest sampling is appropriate for depth textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set up the depth texture for shadow mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

    // Clamp to edge
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Set the texture dimensions
    // Do we need to set GL_DEPTH_COMPONENT24 here?
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}


void
GLFrameBufferObject::generateFbo(unsigned int attachments)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

    glReadBuffer(GL_NONE);

    if ((attachments & ColorAttachment) != 0)
    {
        generateColorTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_colorTexId, 0);
        m_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (m_status != GL_FRAMEBUFFER_COMPLETE)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            cleanup();
            return;
        }
    }
    else
    {
        // Depth-only rendering; no color buffer.
        glDrawBuffer(GL_NONE);
    }

    if ((attachments & DepthAttachment) != 0)
    {
        generateDepthTexture();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexId, 0);
        m_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (m_status != GL_FRAMEBUFFER_COMPLETE)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            cleanup();
            return;
        }
    }
    else
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    }

    // Restore default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// Delete all GL objects associated with this framebuffer object
void
GLFrameBufferObject::cleanup()
{
    if (m_fboId != 0)
        glDeleteFramebuffers(1, &m_fboId);

    if (m_colorTexId != 0)
        glDeleteTextures(1, &m_colorTexId);

    if (m_depthTexId != 0)
        glDeleteTextures(1, &m_depthTexId);
}


bool
GLFrameBufferObject::bind()
{
    if (!isValid())
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
    return true;
}


bool
GLFrameBufferObject::unbind()
{
    // Restore default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}


