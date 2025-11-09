# ===== Config =====
CXX        ?= g++
TARGET      = bin/raytracing
SRC_DIR     = src
OBJ_DIR     = obj
INC_DIRS    = include include/stb
PROFILE_DIR = profiling

# Trouver les sources (.cc / .cpp)
SRCS := $(shell find $(SRC_DIR) -type f \( -name '*.cc' -o -name '*.cpp' \))
OBJS := $(SRCS:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)
OBJS := $(OBJS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS  = $(OBJS:.o=.d)

# ===== Flags =====
CPPFLAGS  := $(addprefix -I,$(INC_DIRS))   # chemins d’include
CXXFLAGS  ?= -std=c++20 -Wall -Wextra -Wpedantic -MMD -MP
LDFLAGS   ?=

# ===== Cibles phony =====
.PHONY: all debug release profiling run clean distclean profile-report

# Build par défaut
all: release

# ---- Debug ----
debug: CXXFLAGS += -O0 -g3
debug: $(TARGET)

# ---- Release ----
release: CXXFLAGS += -O3 -DNDEBUG
release: $(TARGET)

# ---- Profiling (gprof) ----
profiling: CXXFLAGS += -O2 -pg -g
profiling: LDFLAGS  += -pg
profiling: $(TARGET)

# ---- Edition de liens ----
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# ---- Compilation ----
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# ---- Raccourcis ----
run: $(TARGET)
	./$(TARGET)

# Générer un rapport gprof dans profiling/profile.txt
profile-report: $(TARGET)
	@mkdir -p $(PROFILE_DIR)
	@[ -f gmon.out ] || { echo "gmon.out introuvable. Lance d'abord l'appli compilée avec 'make profiling' puis exécute ./bin/raytracing"; exit 1; }
	gprof $(TARGET) gmon.out > $(PROFILE_DIR)/profile.txt
	@echo "Rapport gprof : $(PROFILE_DIR)/profile.txt"

clean:
	@$(RM) -r $(OBJ_DIR)

distclean: clean
	@$(RM) -r bin gmon.out $(PROFILE_DIR)

# ---- Dépendances auto ----
-include $(DEPS)
