import xml.etree.ElementTree as et
import pathlib

root_dir: pathlib.Path = pathlib.Path(__file__).parent.resolve()
gl_xml: pathlib.Path = root_dir/"gl.xml"
wgl_xml: pathlib.Path = root_dir/"wgl.xml"
header_template: str = """#pragma once
{}


{}


{}

#ifdef _WIN32
// WGL stuff
#include <Windows.h>
{}

{}

{}
namespace core {{
void LoadOpenglFunctions();
}}
#endif
"""

cpp_template : str = """
#include "gl.hh"
namespace gl {{
{}
}}
namespace wgl {{
{}
}}
#ifdef _WIN32
inline void* loadOpenGlFunction(const char* function_name, HMODULE gl_lib) {{
    void* p = (void*)wglGetProcAddress(function_name);

    if (p == 0 || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1)) {{
        p = (void*)GetProcAddress(gl_lib, function_name);
    }}
    return p;
}}

inline HMODULE getOpenglLib() {{
    return ::LoadLibraryA("opengl32.dll");
}}

inline void unloadOpenglLib(HMODULE gl) {{
    ::FreeLibrary(gl);
}}

struct function_table_entry {{
    const char* function_name;
    void **function;
}};
function_table_entry functions[] {{
    {}
}};
namespace core {{
void LoadOpenglFunctions() {{
  auto gllib = getOpenglLib();
  for (auto& table_entry : functions) {{
    *table_entry.function = loadOpenGlFunction(table_entry.function_name, gllib);
  }}
  unloadOpenglLib(gllib);
}}
}}
#endif
"""

def parse_single_function(command, cname, qualif="extern"):
    return_type = command.find('proto').text
    if not return_type:
        ptype = command.find('proto').find('ptype')
        return_type = ptype.text
        if ptype.tail:
            return_type += ptype.tail

    args = list()
    for arg in command.findall('param'):
        type = str()
        if arg.text:
            type += arg.text
        ptype = arg.find('ptype')
        if ptype is not None:
            if ptype.text:
                type = ptype.text
            if ptype.tail:
                type += ptype.tail
        name = arg.find('name').text
        args.append("{} {}".format(type, name))
    return "{} ".format(qualif) + return_type + "(KHRONOS_APIENTRY* {})".format(cname) + "({});".format(", ".join(args))


def generate_functions_decls(commands, functions, qualif="extern") -> list[str]:
    decls = list()
    for f in functions:
        for c in commands:
            fname = c.find('proto').find('name').text
            if f == fname:
                decls.append("{}".format(parse_single_function(c, f, qualif)))
                break
    return "\n".join(decls)


def generate_typedefs(types) -> str:
    res = list()
    for type in types:
        # dummy types in wgl.hh
        if type.get('name') and not type.text:
            continue
        t = str()
        if type.text:
            t += type.text
        for child in type:
            if child.tag == 'apientry':
                t += 'KHRONOS_APIENTRY'
            if child.text:
                t += child.text
            if child.tail:
                t += child.tail
        res.append(t)
    return "\n".join(res)


def generate_enums(enums) -> str:
    res = list()
    for enum_group in enums:
        for enum in enum_group.findall('enum'):
            if enum.attrib.get('api') is not None:
                continue
            if enum.attrib.get('value') is None:
                continue
            res.append("#define {} {}".format(enum.attrib['name'], enum.attrib['value']))
    return "\n".join(res)

opengl_functions_names : list = [
  "glEnable",
  "glIsEnabled",
  "glVertexAttribPointer",
  "glBufferData",
  "glGenBuffers",
  "glGenVertexArrays",
  "glBindVertexArray",
  "glBindBuffer",
  "glEnableVertexAttribArray",
  "glDrawElements",
  "glDebugMessageCallback",
  "glDebugMessageControl",
  "glCreateShader",
  "glDeleteShader",
  "glShaderSource",
  "glCompileShader",
  "glCreateProgram",
  "glAttachShader",
  "glLinkProgram",
  "glUseProgram",
  "glGetShaderiv",
  "glGetProgramiv",
  "glDeleteProgram",
  "glPolygonMode",
  "glClear",
  "glClearColor",
  "glDrawArrays",
  "glViewport"
]

wgl_function_names : list = [
    "wglCreateContextAttribsARB",
    "wglChoosePixelFormatARB"
]

opengl_functions_names1 = [n[2:] for n in opengl_functions_names]
print(opengl_functions_names1)
wgl_functions_names1 = [n[3:] for n in wgl_function_names]
print(wgl_functions_names1)


root = et.parse(gl_xml).getroot()
commands = generate_functions_decls(root.find('commands'), opengl_functions_names)
commands1 = generate_functions_decls(root.find('commands'), opengl_functions_names, "")
types = generate_typedefs(root.find('types')).replace("<KHR/khrplatform.h>", '"khrplatform.h"')
enums = generate_enums(root.findall('enums'))
wgl_root = et.parse(wgl_xml).getroot()
wgl_types = generate_typedefs(wgl_root.find('types'))
wgl_enums = generate_enums(wgl_root.findall('enums'))
wgl_functions = generate_functions_decls(wgl_root.find('commands'), wgl_function_names)
wgl_functions1 = generate_functions_decls(wgl_root.find('commands'), wgl_function_names, "")
gl_functions = ",".join('{{"{}", (void**)&{}}}'.format(n, n) for n in opengl_functions_names)
wgl_functions_ = ",".join('{{"{}", (void**)&{}}}'.format(n, n) for n in wgl_function_names)
all_functions = "{},{}".format(gl_functions, wgl_functions)

open(root_dir/".."/"opengl"/"gl.hh", "w+").write(
    header_template.format(
        types, enums, commands, 
        wgl_types, wgl_enums, wgl_functions,
    )
)

open(root_dir/".."/"opengl"/"gl.cpp", "w+").write(
    cpp_template.format(
        commands1,
        wgl_functions1,
        all_functions
    )
)


