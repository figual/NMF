include blas.mk

#
# --- Optional overrides -------------------------------------------------------
#

# Uncomment and modify these definitions if you wish to override the values
# present in the current BLIS configuration's makefile definitions file
# (ie: config/<configname>/make_defs.mk).
CC             := gcc
LINKER         := $(CC)
CFLAGS         := -O2 -Wall -Wno-comment -I $(BLAS_INCLUDE)
LDFLAGS        := -lgfortran

#
# --- General build definitions ------------------------------------------------
#

USE_BLAS       := -DBLAS -DFATLAS

FLAGS_FLOAT    := -DREAL 
FLAGS_DOUBLE   :=        

SRC_PATH  := .  
OBJ_PATH  := .

# Gather all local object files.
#OBJS      := $(patsubst $(SRC_PATH)/%.c, 
OBJS      := $(OBJ_PATH)/%.o, \
                             $(wildcard $(SRC_PATH)/*.c))

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

NMF_%_float.o: NMF.c
	 $(CC) $(CFLAGS) -DREAL -DBLAS -DFATLAS -DRANDOM $(BLAS_LIB_MT) -c $< -o $@

NMF_%_double.o: NMF.c
	 $(CC) $(CFLAGS)        -DBLAS -DFATLAS -DRANDOM $(BLAS_LIB_MT) -c $< -o $@

# -- Executable file rules --

$(FLOAT_ST_BIN): NMF_st_float.o $(BLAS_LIB)
	$(LINKER) $< $(BLAS_LIB_ST) $(LDFLAGS) -o $@

$(FLOAT_MT_BIN): NMF_mt_float.o $(BLAS_LIB)
	$(LINKER) $< $(BLAS_LIB_MT) $(LDFLAGS) -o $@

$(DOUBLE_ST_BIN): NMF_st_double.o $(BLAS_LIB)
	$(LINKER) $< $(BLAS_LIB_ST) $(LDFLAGS) -o $@

$(DOUBLE_MT_BIN): NMF_mt_double.o $(BLAS_LIB)
	$(LINKER) $< $(BLAS_LIB_MT) $(LDFLAGS) -o $@


# -- Clean rules --

clean:
	- $(RM_F) $(OBJS) $(BIN) *.x *.o

