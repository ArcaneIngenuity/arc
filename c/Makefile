CC = gcc

BITS = 64
#...unless otherwise set to 32

#ifeq ($(shell echo "check_quotes"),"check_quotes")
ifeq ($(OS),Windows_NT)
	OS   := win
	EXT  := .exe
	slash:= \\
	CD   := $(notdir $(shell cd))
	PRJDIR := $(subst \,/,$(shell cd .. & cd & cd $(CD)))
	rm   := del /q
	cp   := xcopy /s /y /i
	ln	 := mklink
	ls	 := dir /b /s
	lsd	 := dir /b /s /ad
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CFLAGS += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CFLAGS += -D IA32
	endif
	CFLAGS += -D EZXML_NOMMAP
else
	UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		OS  := lin
		EXT := .elf
		slash:= /
		PRJDIR := $(shell cd ..;pwd;cd $(CD))
		CDABS:=$(shell pwd)
		CD  := $(subst $(PRJDIR)/,,$(shell pwd))
		rm  := rm -rf
		cp  := cp -a
		ln	:= ln -s
		ls	:= find . -print
		lsd	:= find . -type d -print
        CCFLAGS += -D LINUX
		BITS = $(shell getconf LONG_BIT)
    endif
    ifeq ($(UNAME_S),Darwin)
		OS := osx
		EXT :=
		slash:= /
		rm  := rm -rf
		cp  := cp -a
		ln	:= ln -s
        CCFLAGS += -D OSX
		BITS = $(shell getconf LONG_BIT)
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

#file lists to operate on
SRCS	 = \
		 ./arctyper.c \
		 ./ezxml/ezxml.c
OBJS 	 = $(SRCS:%.c=%.o)
DEPS     = $(OBJS:%.o=%.d)

#Define flags for compile & link
CFLAGS = -I. -Wall -Werror -Wmissing-prototypes -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wlogical-op -Wcast-align -Wconversion -Wpedantic -Wfloat-equal -w -O0 -MMD -DEZXML_NOMMAP 
LFLAGS = 
#-Wl,--verbose

TARGET = arctyper$(EXT)

#Rules
#||: - see http://stackoverflow.com/questions/11871921/suppress-and-ignore-output-for-makefile


#"2> nul" shuts windows up when mkdir finds directories already exist. See:
#http://stackoverflow.com/questions/99132/how-to-prevent-directory-already-exists-error-in-a-makefile-when-using-mkdir (andrewdotnich's answer)
#http://stackoverflow.com/questions/4507312/how-to-redirect-stderr-to-null-in-cmd-exe

.PHONEY: clean
clean:
ifeq ($(OS),win)
	@$(rm) *.o
	@$(rm) *.d
	@$(rm) $(subst /,\, ezxml/*.o)
	@$(rm) $(subst /,\, ezxml/*.d)
	@$(rm) *.exe
	@echo $(subst /,\,All build artifacts cleaned.)
else ifeq ($(OS),lin)
	@$(rm) *.o
	@$(rm) *.d
	@$(rm) ezxml/*.o
	@$(rm) ezxml/*.d
	@$(rm) *.exe
	@echo $(subst /,\,All build artifacts cleaned.)
endif

all: $(TARGET) | clean 

$(TARGET): $(OBJS)
	@echo link...
	$(CC) -o ./$@ $(OBJS) $(LFLAGS) $(LLIBS) 
	@echo Linking complete.
	
VPATH = ./

$(OBJS): %.o: %.c 
	@$(CC) -g $(CFLAGS) -c $< -o ./$@
	@echo Compiled $<.

#include gcc-generated (-MD, -MMD) rules to remake each .o (thus forcing relink as well) if a header or source dep changes
-include $(DEPS)

#gcc/ld switches order: http://askubuntu.com/questions/194193/why-do-i-get-undefined-reference-errors-when-linking-against-openssl