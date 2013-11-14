# BLIS library and header path. This is simply wherever it was installed.
BLIS_PATH      := /home/odroid/blis/
BLIS_LIB_PATH  := $(BLIS_PATH)/lib
BLIS_INC_PATH  := $(BLIS_PATH)/include

# BLIS library.
BLIS_LIB       := $(BLIS_LIB_PATH)/libblis.a

# ATLAS library and header path. This is simply wherever it was installed.
ATLAS_PATH      := /home/figual/atlas/
ATLAS_LIB_PATH  := $(ATLAS_PATH)/lib
ATLAS_INC_PATH  := $(ATLAS_PATH)/include

# ATLAS library.
ATLAS_LIB_ST       := $(ATLAS_LIB_PATH)/libf77blas.a $(ATLAS_LIB_PATH)/libatlas.a
ATLAS_LIB_MT       := $(ATLAS_LIB_PATH)/libf77blas.a $(ATLAS_LIB_PATH)/libatlas.a

#
# --- Optional overrides -------------------------------------------------------
#

# Uncomment and modify these definitions if you wish to override the values
# present in the current BLIS configuration's makefile definitions file
# (ie: config/<configname>/make_defs.mk).
CC             := gcc
LINKER         := $(CC)
CFLAGS         := -O2 -Wall -Wno-comment
LDFLAGS        := 

#
# --- General build definitions ------------------------------------------------
#

USE_BLAS       := -DBLAS -DFATLAS

FLAGS_FLOAT    := -DREAL 
FLAGS_DOUBLE   :=        

TEST_SRC_PATH  := .  
TEST_OBJ_PATH  := obj

# Gather all local object files.
TEST_OBJS      := $(patsubst $(TEST_SRC_PATH)/%.c, \
                             $(TEST_OBJ_PATH)/%.o, \
                             $(wildcard $(TEST_SRC_PATH)/*.c))

# Binary executable names.
FLOAT_MT_BIN       := NMF_mt_float.x
FLOAT_ST_BIN       := NMF_st_float.x

DOUBLE_MT_BIN      := NMF_mt_double.x
DOUBLE_ST_BIN      := NMF_st_double.x

# Add installed and local header paths to CFLAGS

#
# --- Targets/rules ------------------------------------------------------------
#

all: $(FLOAT_MT_BIN) $(FLOAT_ST_BIN) $(DOUBLE_MT_BIN) $(DOUBLE_ST_BIN)



# --Object file rules --

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# -- Executable file rules --

$(FLOAT_MT_BIN): $(TEST_OBJS) $(BLIS_LIB)
	$(LINKER) $(TEST_OBJS) $(BLIS_LIB) $(LDFLAGS) -o $@


# -- Clean rules --

clean:
	- $(RM_F) $(TEST_OBJS) $(TEST_BIN)

