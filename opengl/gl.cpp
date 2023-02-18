
#include "gl.hh"
namespace gl {
 void (KHRONOS_APIENTRY* glEnable)(GLenum  cap);
 GLboolean (KHRONOS_APIENTRY* glIsEnabled)(GLenum  cap);
 void (KHRONOS_APIENTRY* glVertexAttribPointer)(GLuint  index, GLint  size, GLenum  type, GLboolean  normalized, GLsizei  stride, const void * pointer);
 void (KHRONOS_APIENTRY* glBufferData)(GLenum  target, GLsizeiptr  size, const void * data, GLenum  usage);
 void (KHRONOS_APIENTRY* glGenBuffers)(GLsizei  n, GLuint * buffers);
 void (KHRONOS_APIENTRY* glGenVertexArrays)(GLsizei  n, GLuint * arrays);
 void (KHRONOS_APIENTRY* glBindVertexArray)(GLuint  array);
 void (KHRONOS_APIENTRY* glBindBuffer)(GLenum  target, GLuint  buffer);
 void (KHRONOS_APIENTRY* glEnableVertexAttribArray)(GLuint  index);
 void (KHRONOS_APIENTRY* glDrawElements)(GLenum  mode, GLsizei  count, GLenum  type, const void * indices);
 void (KHRONOS_APIENTRY* glDebugMessageCallback)(GLDEBUGPROC  callback, const void * userParam);
 void (KHRONOS_APIENTRY* glDebugMessageControl)(GLenum  source, GLenum  type, GLenum  severity, GLsizei  count, GLuint * ids, GLboolean  enabled);
 GLuint (KHRONOS_APIENTRY* glCreateShader)(GLenum  type);
 void (KHRONOS_APIENTRY* glDeleteShader)(GLuint  shader);
 void (KHRONOS_APIENTRY* glShaderSource)(GLuint  shader, GLsizei  count, GLchar *const* string, GLint * length);
 void (KHRONOS_APIENTRY* glCompileShader)(GLuint  shader);
 GLuint (KHRONOS_APIENTRY* glCreateProgram)();
 void (KHRONOS_APIENTRY* glAttachShader)(GLuint  program, GLuint  shader);
 void (KHRONOS_APIENTRY* glLinkProgram)(GLuint  program);
 void (KHRONOS_APIENTRY* glUseProgram)(GLuint  program);
 void (KHRONOS_APIENTRY* glGetShaderiv)(GLuint  shader, GLenum  pname, GLint * params);
 void (KHRONOS_APIENTRY* glGetProgramiv)(GLuint  program, GLenum  pname, GLint * params);
 void (KHRONOS_APIENTRY* glDeleteProgram)(GLuint  program);
 void (KHRONOS_APIENTRY* glPolygonMode)(GLenum  face, GLenum  mode);
 void (KHRONOS_APIENTRY* glClear)(GLbitfield  mask);
 void (KHRONOS_APIENTRY* glClearColor)(GLfloat  red, GLfloat  green, GLfloat  blue, GLfloat  alpha);
 void (KHRONOS_APIENTRY* glDrawArrays)(GLenum  mode, GLint  first, GLsizei  count);
 void (KHRONOS_APIENTRY* glViewport)(GLint  x, GLint  y, GLsizei  width, GLsizei  height);
}
namespace wgl {
 HGLRC (KHRONOS_APIENTRY* wglCreateContextAttribsARB)(HDC  hDC, HGLRC  hShareContext, const int * attribList);
 BOOL (KHRONOS_APIENTRY* wglChoosePixelFormatARB)(HDC  hdc, const int * piAttribIList, FLOAT * pfAttribFList, UINT  nMaxFormats, int * piFormats, UINT * nNumFormats);
}
#ifdef _WIN32
inline void* loadOpenGlFunction(const char* function_name, HMODULE gl_lib) {
    void* p = (void*)wglGetProcAddress(function_name);

    if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1)) {
        p = (void*)GetProcAddress(gl_lib, function_name);
    }
    return p;
}

inline HMODULE getOpenglLib() {
    return ::LoadLibraryA("opengl32.dll");
}

inline void unloadOpenglLib(HMODULE gl) {
    ::FreeLibrary(gl);
}

struct function_table_entry {
    const char* function_name;
    void **function;
};
function_table_entry functions[] {
    {"glEnable", (void**)&glEnable},{"glIsEnabled", (void**)&glIsEnabled},{"glVertexAttribPointer", (void**)&glVertexAttribPointer},{"glBufferData", (void**)&glBufferData},{"glGenBuffers", (void**)&glGenBuffers},{"glGenVertexArrays", (void**)&glGenVertexArrays},{"glBindVertexArray", (void**)&glBindVertexArray},{"glBindBuffer", (void**)&glBindBuffer},{"glEnableVertexAttribArray", (void**)&glEnableVertexAttribArray},{"glDrawElements", (void**)&glDrawElements},{"glDebugMessageCallback", (void**)&glDebugMessageCallback},{"glDebugMessageControl", (void**)&glDebugMessageControl},{"glCreateShader", (void**)&glCreateShader},{"glDeleteShader", (void**)&glDeleteShader},{"glShaderSource", (void**)&glShaderSource},{"glCompileShader", (void**)&glCompileShader},{"glCreateProgram", (void**)&glCreateProgram},{"glAttachShader", (void**)&glAttachShader},{"glLinkProgram", (void**)&glLinkProgram},{"glUseProgram", (void**)&glUseProgram},{"glGetShaderiv", (void**)&glGetShaderiv},{"glGetProgramiv", (void**)&glGetProgramiv},{"glDeleteProgram", (void**)&glDeleteProgram},{"glPolygonMode", (void**)&glPolygonMode},{"glClear", (void**)&glClear},{"glClearColor", (void**)&glClearColor},{"glDrawArrays", (void**)&glDrawArrays},{"glViewport", (void**)&glViewport},extern HGLRC (KHRONOS_APIENTRY* wglCreateContextAttribsARB)(HDC  hDC, HGLRC  hShareContext, const int * attribList);
extern BOOL (KHRONOS_APIENTRY* wglChoosePixelFormatARB)(HDC  hdc, const int * piAttribIList, FLOAT * pfAttribFList, UINT  nMaxFormats, int * piFormats, UINT * nNumFormats);
};
namespace core {
void LoadOpenglFunctions() {
  auto gllib = getOpenglLib();
  for (auto& table_entry : functions) {
    *table_entry.function = loadOpenGlFunction(table_entry.function_name, gllib);
  }
  unloadOpenglLib(gllib);
}
}
#endif
