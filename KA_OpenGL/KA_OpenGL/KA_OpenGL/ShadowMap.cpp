#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
    // FRAME BUFFER 
    FBO = 0;
    shadowMap = 0;
}

bool ShadowMap::Init(GLuint width, GLuint height)
{
    shadowWidth = width; shadowHeight = height;

    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &shadowMap);

    // Setup the texture, saying that it will be of type "GL_TEXTURE_2D"
    glBindTexture(GL_TEXTURE_2D, shadowMap);

    // Import texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    // Set the texture filtering ( Linear, nearest )
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set the texture Wrapping ( here it is in repeat )
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    // Tell the FrameBuffer that we do not render color values
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer Error : %i\n", status);
        return false;
    }

    // Free the FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void ShadowMap::Write()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void ShadowMap::Read(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
}

ShadowMap::~ShadowMap()
{
    if (FBO)
    {
        glDeleteFramebuffers(1, &FBO);
    }

    if (shadowMap)
    {
        glDeleteTextures(1, &shadowMap);
    }
}
