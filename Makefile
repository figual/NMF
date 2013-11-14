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

FLAGS_FLOAT    := -DRANDOM -DREAL -DBLAS -DFATLAS 
FLAGS_DOUBLE   := -DRANDOM        -DBLAS -DFATLAS 

TEST_SRC_PATH  := .  
TEST_OBJ_PATH  := obj

# Gather all local object files.
TEST_OBJS      := $(patsubst $(TEST_SRC_PATH)/%.c, \
                             $(TEST_OBJ_PATH)/%.o, \
                             $(wildcard $(TEST_SRC_PATH)/*.c))

# Binary executable name.
TEST_BIN       := test_libblis.x

# Add installed and local header paths to CFLAGS
CFLAGS         += -I$(BLIS_INC_PATH) -I$(TEST_SRC_PATH)

LINKER         := $(CC)
#LDFLAGS        := -L/usr/lib/gcc/x86_64-redhat-linux/4.1.2 -L/usr/lib/gcc/x86_64-redhat-linux/4.1.2/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -lgfortranbegin -lgfortran -lm
#LDFLAGS        += -lpthread
LDFLAGS        := -L/usr/lib/gcc/i486-linux-gnu/4.4.3 -L/usr/lib/gcc/i486-linux-gnu/4.4.3/../../../../lib -L/lib/../lib -L/usr/lib/../lib -L/usr/lib/gcc/i486-linux-gnu/4.4.3/../../.. -L/usr/lib/i486-linux-gnu -lgfortranbegin -lgfortran -lm



#
# --- Targets/rules ------------------------------------------------------------
#

all: $(TEST_BIN)



# --Object file rules --

$(TEST_OBJ_PATH)/%.o: $(TEST_SRC_PATH)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# -- Executable file rules --

$(TEST_BIN): $(TEST_OBJS) $(BLIS_LIB)
	$(LINKER) $(TEST_OBJS) $(BLIS_LIB) $(LDFLAGS) -o $@


# -- Clean rules --

clean:
	- $(RM_F) $(TEST_OBJS) $(TEST_BIN)

