CC = g++ -std=c++1z
CFLAGS = -g -Wall
TARGET = main
LINKER_FLAGS = -lSDL2 -ldl -lassimp
OBJS = main.cpp glad.c ../include/imgui.cpp ../include/imgui_demo.cpp ../include/imgui_draw.cpp ../include/imgui_impl_sdl_gl3.cpp

first: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LINKER_FLAGS) -o $(TARGET)