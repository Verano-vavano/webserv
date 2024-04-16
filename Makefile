NAME=./miniweb

SRCS_HTTPServ=HTTPServ.cpp \
	      HTTPServ_send.cpp

SRCS_HTTPConfig=HTTPConfig.cpp \
				HTTPConfig_config.cpp \
				HTTPConfig_parser.cpp \
				HTTPConfig_setters.cpp \
				HTTPConfig_utils.cpp \
				HTTPConfig_printer.cpp

SRCS_HTTPProt=HTTPProtocol_request.cpp \
			  HTTPProtocol_response.cpp \
			  HTTPProtocol_methods.cpp \
			  HTTPProtocol_methods_utils.cpp \
			  HTTPProtocol_get.cpp \
			  HTTPProtocol_cgi.cpp \
			  HTTPProtocol_accept.cpp \
			  HTTPProtocol_errors.cpp \
			  HTTPProtocol_response_headers.cpp \
			  HTTPProtocol_utils.cpp \
			  HTTPProtocol_printer.cpp

SRCS_USERS=Users.cpp \
		   Users_post.cpp \
		   Users_del.cpp

SRCS_CLASSES=$(addprefix classes/, \
			 	$(addprefix HTTPServ/, ${SRCS_HTTPServ})\
			 	$(addprefix HTTPConfig/, ${SRCS_HTTPConfig})\
			 	$(addprefix HTTPProtocol/, ${SRCS_HTTPProt})\
				$(addprefix Users/, ${SRCS_USERS})\
			 )

SRCS=main.cpp\
	 ${SRCS_CLASSES}
SRCS_DIR=$(addprefix ./srcs/, ${SRCS})
DEST=${SRCS_DIR:.cpp=.o}
NO_OF_FILES:=$(words $(SRCS))

RM=rm -f

# TODO make this into c++ at the end.
GCC=g++
CFLAGS=-Wall -Wextra -Werror -std=c++98 -g

INCLUDES=-I ./includes/ -I ./srcs/classes/includes

COUNT=0



START=0
LAST_PERCENT=0

RED=$(shell tput setaf 1)
GREEN=$(shell tput setaf 2)
YELLOW=$(shell tput setaf 3)
BLUE=$(shell tput setaf 4)
MAGENTA=$(shell tput setaf 5)
CYAN=$(shell tput setaf 6)
WHITE=$(shell tput setaf 7)
RESET=$(shell tput sgr0)

HIDE_CURSOR=$(shell tput civis)
SHOW_CURSOR=$(shell tput cnorm)
CURSOR_UP=$(shell tput cuu1)
CURSOR_DOWN=$(shell tput cud1)
BEG_LINE=$(shell tput hpa 0)
BOLD=$(shell tput bold)

define change_bar_color
	if [ $1 -lt 12 ]; then \
		printf "${RED}"; \
	elif [ $1 -lt 24 ]; then \
		printf "${YELLOW}"; \
	elif [ $1 -lt 36 ]; then \
		printf "${GREEN}"; \
	else \
		printf "${CYAN}"; \
	fi
endef

define move_progress_bar
	@if [ ${START} -eq 0 ]; then \
		printf "${MAGENTA}"; \
		echo "COMPILING WEBSERV"; \
		printf "${RESET}"; \
		tput cud1; \
		$(eval START = 1) \
	fi
	@tput civis; tput cuu1; tput hpa 0
	@$(eval COUNT := $(shell bash -c 'echo $$(($(COUNT) + 1))'))
	@$(eval PERCENT := $(shell bash -c 'echo $$(($(COUNT) * 100 / $(NO_OF_FILES)))'))
	@$(eval current := 1)
	@$(eval MAX := $(shell bash -c 'echo $$(($(PERCENT) / 2))'))
	@printf "["
	@tput hpa 0
	@tput cuf ${LAST_PERCENT}
	@for i in $$(seq ${LAST_PERCENT} ${MAX}); do\
		$(call change_bar_color, $$i); \
		printf "#"; \
	done
	@tput hpa 0; tput cuf 51
	@printf "${RESET}"
	@printf "] "
	@if [ "${PERCENT}" -lt 100 ]; then \
		printf "${WHITE}"; \
	else \
		printf "${CYAN}${BOLD}"; \
	fi
	@echo "${PERCENT}% | ${COUNT} / ${NO_OF_FILES}"
	@$(eval LAST_PERCENT = ${MAX})
endef

define max_count
	@$(eval COUNT := $(shell bash -c 'echo $$(($(NO_OF_FILES) - 1))'))
endef



all: ${NAME}

bonus: ${NAME}

.cpp.o:
	@${GCC} ${CFLAGS} ${INCLUDES} ${ADDITIONAL_DEFINES} -c $< -o ${<:.cpp=.o}
	$(call move_progress_bar, COUNT)

${NAME}: ${DEST}
	@$(call max_count)
	@$(call move_progress_bar, COUNT)
	@printf "\033[0m"
	@${GCC} ${CFLAGS} ${DEST} -o ${NAME} ${INCLUDES}
	@printf "${GREEN}${BOLD}"
	@echo "WEBSERV COMPILED"
	@printf "${RESET}"
	@printf ${SHOW_CURSOR}

clean:
	@echo "${GREEN}Cleaning webserv..."
	@${RM} ${DEST}
	@printf "${RESET}"
	@echo "~~~~"

fclean: clean
	@${RM} ${LIBFT}
	@${RM} ${NAME}
	@${RM} .usr .session

re: fclean all

.PHONY:all .c.o clean fclean clean_libft
