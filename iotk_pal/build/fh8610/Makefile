TOOLCHAIN_PATH   =C:/Neon/cl/bin
AS               =${TOOLCHAIN_PATH}/hcac.exe
AR               =${TOOLCHAIN_PATH}/arac.exe
CC               =${TOOLCHAIN_PATH}/hcac.exe
OBJDUMP          =${TOOLCHAIN_PATH}/elfdump.exe

TARGETS          =iotk_pal_fh8610
OBJDIR           =objs
OBJPATH          =$(OBJDIR)/fake/fake
TARGET_DIR       =../lib/fh8610

TARGET_LIBS      =$(foreach lib,$(TARGETS),$(TARGET_DIR)/$(lib).a)
TARGET_MAP       =$(TARGET_ELF:.elf=.map)

UCOS_INC_PATH    ?= ../../../../private/examples/fh8610/OS/inc

CFLAGS            = -DTRUE=1 -DFALSE=0 -DINLINE= -D__UCOS__=1 -c -Os1 -Dmemcpy=memcpy2 -Hnocopyr -Hoff=char_default_unsigned -Hoff=BeHaved -Hon=Each_function_in_own_section -Hsdata0 -arc600 -Ml 

INC_DIR           = inc
SUBDIRS           = src

CFLAGS           += $(foreach dir,$(INC_DIR),$(addprefix -I,../../$(dir)))
CFLAGS           += -I$(UCOS_INC_PATH)

CSRC              = $(subst \,/,$(foreach dir,$(SUBDIRS),$(wildcard ../../$(dir)/*.c)))
CFILES            = $(filter-out $(EXCLUDE_FILES),$(CSRC))
COBJS             = $(patsubst %.c,$(OBJPATH)/%.o,$(CFILES))
WINCSRC           = $(subst /,\,$(CSRC))
WINCOBJ           = $(subst /,\,$(COBJS))
DEPENDS           = $(patsubst %.o,%.dep,$(WINCOBJ))



all: $(TARGET_LIBS)
-include $(DEPENDS)

clean: 
	-@if exist $(OBJDIR). rd /Q /S $(OBJDIR)
	-@del /Q /F $(subst /,\,$(TARGET_LIBS))

$(COBJS): $(OBJPATH)/%.o:%.c
	@if not exist $(subst /,\,$(@D)) mkdir $(subst /,\,$(@D))
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) $< -Hobjdir=$(dir $@) -Hrel -Hmake -Hmakeof=$(patsubst %.o,%.dep,$@)

$(TARGET_DIR)/iotk_pal_fh8610.a: $(COBJS) $(UCOS_INC_PATH)/ucos_ii.h
	@if not exist $(subst /,\,$(@D)) mkdir $(subst /,\,$(@D))
	$(AR) crvs $@ $^
