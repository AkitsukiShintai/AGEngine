rem ***************************************************************************
rem *** Make sure you are using the Visual Studio Developer Command Prompt  ***
rem ***************************************************************************
cl /EHsc /I. ParticleDriver.cpp Particle.cpp imgui_demo.cpp imgui_draw.cpp imgui_impl_opengl3.cpp imgui_impl_sdl.cpp imgui_widgets.cpp imgui.cpp opengl32.lib lib/*.lib /link /subsystem:console

