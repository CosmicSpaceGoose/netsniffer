################################ output variables ##############################
ifeq ($(findstring xterm,${TERM}),xterm)
	CYAN	:= \x1b[1;36m
	NON		:= \x1b[0m
	CYANN	:= \x1b[36m
	GREEN	:= \x1b[32m
endif
################################ compiler variables ############################
# General
CC		:= gcc
CFLAGS	:= -Wall -Wextra -Werror -O2
DIROBJ	:= ./obj/
DIRBIN	:= ./obj/bin/
DIRSRC	:= ./src/
DIRINC	:= ./inc/
INC		:= -I$(DIRINC)
# Command Line Interface
BINCLI	:= netsniffercli
DIRCLI	:= ./obj/cli/
SRCCLI	:=	main_cli.c\
			cli_commands.c
OBJCLI	:= $(addprefix $(DIRCLI), $(SRCCLI:.c=.o))
INCCLI	:= inc/netsniffer_cli.h
# Daemon
BINDMN	:= netsnifferd
DIRDMN	:= ./obj/dmn/
SRCDMN	:= main_d.c
OBJDMN	:= $(addprefix $(DIRDMN), $(SRCDMN:.c=.o))
INCDMN	:= inc/netsniffer_d.h
################################## rules #######################################

all: $(DIROBJ) $(BINCLI) $(BINDMN)
	@echo "$(GREEN)Job done$(NON)"

# Command Line Interface
$(BINCLI): $(DIROBJ) $(OBJCLI)
	@$(CC) $(CFLAGS) $(OBJCLI) -o $(DIRBIN)$(BINCLI)
	@echo "$(CYAN)comp$(NON)..."$(BINCLI)

$(DIRCLI)%.o : $(DIRSRC)%.c $(INCCLI)
	@$(CC) $(INC) $(CFLAGS) -o $@ -c $<
	@echo "$(CYANN)comp$(NON)..."$@

# Daemon
$(BINDMN): $(DIROBJ) $(OBJDMN)
	@$(CC) $(CFLAGS) $(OBJDMN) -o $(DIRBIN)$(BINDMN)
	@echo "$(CYAN)comp$(NON)..."$(BINDMN)

$(DIRDMN)%.o : $(DIRSRC)%.c $(INCDMN)
	@$(CC) $(INC) $(CFLAGS) -o $@ -c $<
	@echo "$(CYANN)comp$(NON)..."$@

# General
$(DIROBJ):
	@mkdir -p $(DIROBJ)
	@mkdir -p $(DIRCLI)
	@mkdir -p $(DIRDMN)
	@mkdir -p $(DIRBIN)
	@echo "$(CYANN)Project tree created$(NON)"

clean:
	@rm -rf $(DIRCLI)
	@rm -rf $(DIRDMN)
	@echo "$(CYANN)Remove object files$(NON)"

fclean: clean
	@rm -rf $(DIROBJ)
	@echo "$(CYANN)Remove binary files$(NON)"
