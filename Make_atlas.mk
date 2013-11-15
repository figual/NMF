# ATLAS library and header path. This is simply wherever it was installed.
ATLAS_PATH      := /home/figual/atlas/
ATLAS_LIB_PATH  := $(ATLAS_PATH)/lib
ATLAS_INC_PATH  := $(ATLAS_PATH)/include

# ATLAS library.
ATLAS_LIB_ST       := $(ATLAS_LIB_PATH)/libf77blas.a $(ATLAS_LIB_PATH)/libatlas.a
ATLAS_LIB_MT       := $(ATLAS_LIB_PATH)/libf77blas.a $(ATLAS_LIB_PATH)/libatlas.a

# BLAS library.
BLAS_LIB_ST  := $(ATLAS_LIB_ST)
BLAS_LIB_MT  := $(ATLAS_LIB_MT)
