#ifndef GUI_GLBUFFER_H
#define GUI_GLBUFFER_H

#include <FCGlobal.h>
#include <map>
#include <Inventor/C/glue/gl.h>

namespace Gui
{

class GuiExport OpenGLBuffer
{
public:
    OpenGLBuffer(GLenum type);
    ~OpenGLBuffer();

    static bool isVBOSupported(uint32_t ctx);

    void setCurrentContext(uint32_t ctx);
    bool create();
    bool isCreated() const;

    void destroy();
    void allocate(const void* data, int count);
    bool bind();
    void release();
    GLuint getBufferId() const;
    uint32_t getBoundContext() const;
    int size() const;

private:
    static void context_destruction_cb(uint32_t context, void* userdata);
    static void buffer_delete(void* closure, uint32_t contextid);

    GLenum target;
    GLuint bufferId;
    uint32_t context;
    uint32_t currentContext;
    const cc_glglue* glue;
};

class GuiExport OpenGLMultiBuffer
{
public:
    OpenGLMultiBuffer(GLenum type);
    ~OpenGLMultiBuffer();

    void setCurrentContext(uint32_t ctx);
    bool create();
    bool isCreated(uint32_t ctx) const;

    void destroy();
    void allocate(const void* data, int count);
    bool bind();
    void release();
    GLuint getBufferId() const;
    int size() const;

private:
    static void context_destruction_cb(uint32_t context, void* userdata);
    static void buffer_delete(void* closure, uint32_t contextid);

    GLenum target;
    // map context to buffer id
    std::map<uint32_t, GLuint> bufs;
    GLuint* currentBuf;
    uint32_t currentContext;
    const cc_glglue* glue;
};

}  // namespace Gui

#endif  // GUI_GLBUFFER_H
