# name of the program (Minix service)
PROG=lab5

# source code files to be compiled
SRCS = lab5.c kbc.c timer.c utils.c video.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf
CPPFLAGS += -D LAB5

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>