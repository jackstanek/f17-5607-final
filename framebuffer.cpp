#include "glad/glad.h"

#include "framebuffer.hpp"

int RenderPass::next_id = 0;

RenderPass::RenderPass(int w, int h) :
    id(RenderPass::next_id++)
{
    /* Create the target texture */
    glGenTextures(1, &target_tex);
    glBindTexture(GL_TEXTURE_2D, target_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    /* Get a framebuffer for this pass */
    glGenFramebuffers(1, &frame_buf);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_tex, 0);

    /* Get a depth buffer for this pass */
    glGenRenderbuffers(1, &z_buf);
    glBindRenderbuffer(GL_RENDERBUFFER, z_buf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, z_buf);
}

RenderPass::~RenderPass()
{
    glDeleteTextures(1, &target_tex);
    glDeleteFramebuffers(1, &frame_buf);
    glDeleteRenderbuffers(1, &z_buf);
}

int RenderPass::GetID() const
{
    return id;
}

GLuint RenderPass::GetTarget() const
{
    return target_tex;
}

void RenderPass::Activate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buf);

    /*  Clear color to black, I guess. */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
