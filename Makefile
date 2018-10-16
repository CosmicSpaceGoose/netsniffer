CC		:= gcc
CFLAGS	:= -Wall -Wextra -Werror

DIROBJ	:= ./obj
DIRBIN	:= ./obj/bin
DIRSRC	:= ./src
DIRINC	:= ./inc

# Command Line Interface
BINCLI	:= netstatcli
DIRCLI	:= ./obj/cli
SRCCLI	:= main_cli.c
OBJCLI	:= $(addprefix $(DIRCLI), $(SRCCLI:.c=.o))
# Daemon
BINDMN	:= netstatd
DIRDMN	:= ./obj/dmn
SRCDMN	:= main_d.c
OBJDMN	:= $(addprefix $(DIRDMN), $(SRCDMN:.c=.o))

all: $(DIROBJ) $(BINCLI) $(BINDMN)

$(BINCLI): $(OBJCLI) $(INCCLI)
	mkdir -p $(DIRCLI)

$(BINDMN):
	mkdir -p $(DIRDMN)

$(DIR_OBJ):
	mkdir -p $(DIROBJ)

clean:
	rm -rf $(DIRCLI)
	rm -rf $(DIRDMN)

fclean: clean
	rm -rf $(DIROBJ)
