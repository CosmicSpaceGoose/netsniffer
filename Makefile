################################ compiler variables ############################
# General
CC		:= gcc
CFLAGS	:= -Wall -Wextra -Werror -O2
DIROBJ	:= obj/
DIRBIN	:= obj/bin/
DIRSRC	:= src/
DIRINC	:= inc/
INC		:= -I$(DIRINC)
# Command Line Interface
BINCLI	:= netsniffercli
DIRCLI	:= obj/cli/
SRCCLI	:=	main_cli.c\
			cli_commands.c
OBJCLI	:= $(addprefix $(DIRCLI), $(SRCCLI:.c=.o))
INCCLI	:= inc/netsniffer_cli.h
# Daemon
BINDMN	:= netsnifferd
DIRDMN	:= obj/dmn/
SRCDMN	:= main_d.c
OBJDMN	:= $(addprefix $(DIRDMN), $(SRCDMN:.c=.o))
INCDMN	:= inc/netsniffer_d.h
################################## rules #######################################

all: $(DIROBJ) $(DIRBIN)$(BINCLI) $(DIRBIN)$(BINDMN)
	@echo "Compilation complete"

# Command Line Interface
$(DIRBIN)$(BINCLI): $(DIROBJ) $(OBJCLI)
	$(CC) $(CFLAGS) $(OBJCLI) -o $(DIRBIN)$(BINCLI)

$(DIRCLI)%.o: $(DIRSRC)%.c $(INCCLI)
	$(CC) $(INC) $(CFLAGS) -o $@ -c $<

# Daemon
$(DIRBIN)$(BINDMN): $(DIROBJ) $(OBJDMN)
	$(CC) $(CFLAGS) $(OBJDMN) -o $(DIRBIN)$(BINDMN)

$(OBJDMN): $(DIRDMN)%.o: $(DIRSRC)%.c $(INCDMN)
	$(CC) $(INC) $(CFLAGS) -o $@ -c $<

# General
$(DIROBJ):
	mkdir -p $(DIROBJ)
	mkdir -p $(DIRCLI)
	mkdir -p $(DIRDMN)
	mkdir -p $(DIRBIN)

clean:
	rm -rf $(DIRCLI)
	rm -rf $(DIRDMN)

fclean: clean
	rm -rf $(DIROBJ)
