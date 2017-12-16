#ifndef FRAMEBUFFER_HPP_
#define FRAMEBUFFER_HPP_

#include <vector>

#include "glad/glad.h"

class RenderPass {
public:
    RenderPass(int w, int h);

    int GetID() const;

    void Activate();

private:
    /* The ID of this render pass */
    int id;
    static int next_id;

    /* FBO and Z-buffer for this pass */
    GLuint frame_buf, z_buf;

    /* Render target texture */
    GLuint target_tex;
};

class MultiPassRenderer {
public:
    MultiPassRenderer(int w, int h, int passes);

private:
    std::vector<RenderPass*> passes;
};

#endif
