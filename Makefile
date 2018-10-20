################################ compiler variables ############################
# General
CC		:= gcc
CFLAGS	:= -Wall -Wextra -Werror -O2
DIROBJ	:= obj/
DIRBIN	:= obj/bin/
DIRSRC	:= src/
DIRINC	:= inc/
INC		:= -I$(DIRINC)
INCGEN	:= inc/nsniffgen.h
# Command Line Interface
BINCLI	:= nsniffcli
DIRCLI	:= obj/cli/
SRCCLI	:=	maincli.c
OBJCLI	:= $(addprefix $(DIRCLI), $(SRCCLI:.c=.o))
INCCLI	:= inc/nsniffcli.h
# Daemon
BINDMN	:= nsniffd
DIRDMN	:= obj/dmn/
SRCDMN	:= maind.c
OBJDMN	:= $(addprefix $(DIRDMN), $(SRCDMN:.c=.o))
INCDMN	:= inc/nsniffd.h
################################## rules #######################################

all: $(DIROBJ) $(DIRBIN)$(BINCLI) $(DIRBIN)$(BINDMN)
	@echo "Compilation complete"

# Command Line Interface
$(DIRBIN)$(BINCLI): $(DIROBJ) $(OBJCLI)
	$(CC) $(CFLAGS) $(OBJCLI) -o $(DIRBIN)$(BINCLI)

$(DIRCLI)%.o: $(DIRSRC)%.c $(INCCLI) $(INCGEN)
	$(CC) $(INC) $(CFLAGS) -o $@ -c $<

# Daemon
$(DIRBIN)$(BINDMN): $(DIROBJ) $(OBJDMN)
	$(CC) $(CFLAGS) $(OBJDMN) -o $(DIRBIN)$(BINDMN)

$(DIRDMN)%.o: $(DIRSRC)%.c $(INCDMN) $(INCGEN)
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
