# BLIS library and header path. This is simply wherever it was installed.
BLIS_PATH      := /home/odroid/blis/
BLIS_LIB_PATH  := $(BLIS_PATH)/lib
BLIS_INC_PATH  := $(BLIS_PATH)/include

# BLIS library.
BLIS_LIB       := $(BLIS_LIB_PATH)/libblis.a

# BLAS library.
BLAS_LIB_ST  := $(BLIS_LIB)
BLAS_LIB_MT  := $(BLIS_LIB)
