CC = g++ -std=c++1z
CFLAGS = -g -Wall -Wno-unused-variable
TARGET = hex
LINKER_FLAGS = -lSDL2 -ldl #-lassimp
OBJS = hex.cpp glad.c Game.cpp Clock.cpp InputManager.cpp mesh.cpp ../include/imgui.cpp ../include/imgui_demo.cpp ../include/imgui_draw.cpp ../include/imgui_impl_sdl_gl3.cpp

first: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LINKER_FLAGS) -o $(TARGET)