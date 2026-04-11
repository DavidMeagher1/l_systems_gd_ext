#!/usr/bin/env python
import os
import sys

# You can find documentation for SCons and SConstruct files at:
# https://scons.org/documentation.html

# This lets SCons know that we're using godot-cpp, from the godot-cpp folder.
env = SConscript("godot-cpp/SConstruct")

# Configures the 'src' directory as a source for header files.
env.Append(CPPPATH=["src/"])

# Source files for the extension.
sources = [
    "src/register_types.cpp",
    "src/l_systems/l_systems.cpp",
    "src/l_systems/l_systems_vm.cpp",
    "src/l_systems/compute_2d.cpp",
]
# XML doc class files for documentation generation.
doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=[
    "doc_classes/LSystem.xml",
])
sources.append(doc_data)

# The filename for the dynamic library for this GDExtension.
# $SHLIBPREFIX is a platform specific prefix for the dynamic library ('lib' on Unix, '' on Windows).
# $SHLIBSUFFIX is the platform specific suffix for the dynamic library (for example '.dll' on Windows).
# env["suffix"] includes the build's feature tags (e.g. '.windows.template_debug.x86_64')
# (see https://docs.godotengine.org/en/stable/tutorials/export/feature_tags.html).
# The final path should match a path in the '.gdextension' file.
lib_filename = "{}l_systems{}{}".format(env.subst('$SHLIBPREFIX'), env["suffix"], env.subst('$SHLIBSUFFIX'))

# Creates a SCons target for the path with our sources.
library = env.SharedLibrary(
    "project/bin/{}".format(lib_filename),
    source=sources,
)

# Copy the built library into the test project's addon bin folder.
addon_bin = "project/l-system-test/addons/l_systems/bin"
addon_library = env.Install(addon_bin, library)

#NOTE: REMOVE ADDON_LIBRARY AFTER TESTING. THIS IS ONLY FOR CONVENIENCE TO AVOID MANUALLY COPYING THE BUILT LIBRARY INTO THE TEST PROJECT.
# Selects the shared library as the default target.
Default([library, addon_library])
