# BUILD DETAILS

CXX      = clang++

CXXFLAGS = -W -g -std=c++17 											\
		   -I../ThirdParty/glm-0.9.9.8/glm 														\
		   -I./ 														\
		   -I../ThirdParty/rapidjson-1.1.0/include 												\
		   -I../ThirdParty/SDL2-linux-win/SDL2_image-devel-2.6.3-VC/SDL2_image-2.6.3/include 	\
		   -I../ThirdParty/SDL2-linux-win/SDL2_mixer-devel-2.6.3-VC/SDL2_mixer-2.6.3/include 	\
		   -I../ThirdParty/SDL2-linux-win/SDL2_ttf-devel-2.20.2-VC/SDL2_ttf-2.20.2/include 		\
		   -I../ThirdParty/SDL2-linux-win/SDL2-devel-2.28.5-VC/SDL2-2.28.5/include

LFLAGS   = -lm 																					\
		   -L../ThirdParty/SDL2-linux-win/SDL2_image-devel-2.6.3-VC/SDL2_image-2.6.3/lib/x64 	\
		   -L../ThirdParty/SDL2-linux-win/SDL2_mixer-devel-2.6.3-VC/SDL2_mixer-2.6.3/lib/x64 	\
		   -L../ThirdParty/SDL2-linux-win/SDL2_ttf-devel-2.20.2-VC/SDL2_ttf-2.20.2/lib/x64 		\
		   -L../ThirdParty/SDL2-linux-win/SDL2-devel-2.28.5-VC/SDL2-2.28.5/lib/x64				\
		   -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf  -lstdc++fs

BUILD    = build

# TARGETS

TARGET   = game_engine_linux

default: release

debug: CXX = g++
debug: CXXFLAGS += -g -DDEBUG=1 -Wno-class-memaccess
debug: $(BUILD)/debug.mode $(TARGET)

release: CXXFLAGS += -O3 -DNDEBUG=1
release: $(BUILD)/release.mode $(TARGET)

gprof: CXX = g++
gprof: CXXFLAGS += -pg -g -DDEBUG=1 -Wno-class-memaccess
gprof: $(BUILD)/gprof.mode $(TARGET)

# EXECUTABLE DETAILS

OBJ      = main.o Utils.o Modding.o Player.o Scene.o Actor.o 	\
		   TextAdventure.o Resources.o Renderer.o Input.o Map.o \
		   b2_world_callbacks.o b2_world.o b2_wheel_joint.o b2_weld_joint.o b2_timer.o b2_time_of_impact.o b2_stack_allocator.o b2_settings.o b2_rope.o b2_revolute_joint.o b2_pulley_joint.o b2_prismatic_joint.o b2_polygon_shape.o b2_polygon_contact.o b2_polygon_circle_contact.o b2_mouse_joint.o b2_motor_joint.o b2_math.o b2_joint.o b2_island.o b2_gear_joint.o b2_friction_joint.o b2_fixture.o b2_edge_shape.o b2_edge_polygon_contact.o b2_edge_circle_contact.o b2_dynamic_tree.o b2_draw.o b2_distance_joint.o b2_distance.o b2_contact_solver.o b2_contact_manager.o b2_contact.o b2_collision.o b2_collide_polygon.o b2_collide_edge.o b2_collide_circle.o b2_circle_shape.o b2_circle_contact.o b2_chain_shape.o b2_chain_polygon_contact.o b2_chain_circle_contact.o b2_broad_phase.o b2_body.o b2_block_allocator.o \
		   lapi.o lauxlib.o lbaselib.o lcode.o lcorolib.o lctype.o ldblib.o ldebug.o ldo.o ldump.o lfunc.o lgc.o linit.o liolib.o llex.o lmathlib.o lmem.o loadlib.o lobject.o lopcodes.o loslib.o lparser.o lstate.o lstring.o lstrlib.o ltable.o ltablib.o ltm.o lundump.o lutf8lib.o lvm.o lzio.o  \
		   IntroStageView.o MainStageView.o EndStageView.o

OBJBD    = $(OBJ:%=$(BUILD)/%)

$(TARGET): $(OBJBD)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LFLAGS)

# COMPILE TO OBJECTS

$(BUILD)/%.o: %.cpp
	@mkdir -p "$(BUILD)"
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# MODE CONTROL

$(BUILD)/%.mode:
	@if [ ! -f "$@" ]; then \
		if [ -d "$(BUILD)" ]; then \
			echo "Switching to $* mode..."; \
			$(MAKE) clean; \
		fi; \
		mkdir -p "$(BUILD)"; \
		touch "$@"; \
	fi

# CLEAN UP

clean:
	rm -rf "$(TARGET)" "$(BUILD)"

# PHONY

.PHONY: default debug release clean

# DEPENDENCIES

SRC = $(wildcard *.h *.hpp *.c *.cpp *.cc)

$(BUILD)/depend.mk: $(SRC)
	@mkdir -p "$(BUILD)"
	@rm -f "$@"
	@$(CXX) $(CXXFLAGS) -MM *.cpp | sed 's/^\(.*\).o:/$$(BUILD)\/\1.o:/' >> $@

include $(BUILD)/depend.mk

