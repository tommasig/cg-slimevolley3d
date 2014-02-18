
SRC=split.c collision.cpp texture.c camera.cpp env.cpp player.cpp wall.cpp ball.cpp slime.cpp game.cpp main.cpp
COBJ=$(SRC:.c=.o)
OBJ=$(COBJ:.cpp=.o)
#replaces the .c from SRC with .o
EXE=slimev

CC=c++
#CFLAGS=-Wall -O3
#LDFLAGS=-mwindows
INCLUDEDIRS = -I/usr/include/SDL
#LIBDIRS = -L../linux/lib
LDLIBS = -lglut -lGL -lGLU -lX11 -lm -L/usr/X11R6/lib -lSDL -lSDL_console -lSDL_image -lpthread -lXext -ldl
#-lXmu 
RM=rm -rf

TAR = tar
TAR_OPTIONS = cvfz
ARCHIVE_NAME_RELEASE = slimev_release.tgz
ARCHIVE_NAME_SOURCE = slimev_source.tgz

%.o: %.c
#combined w/ next line will compile recently changed .c files
	$(CC) $(CFLAGS) $(INCLUDEDIRS) -o $@ -c $<

%.o: %.cpp
#combined w/ next line will compile recently changed .c files
	$(CC) $(CFLAGS) $(INCLUDEDIRS) -o $@ -c $<

.PHONY : all
# .PHONY ignores files named all
all: $(EXE)
# all is dependent on $(EXE) to be complete

$(EXE): $(OBJ)
# $(EXE) is dependent on all of the files in $(OBJ) to exist
	$(CC) $(OBJ) $(LDFLAGS) $(LIBDIRS) $(LDLIBS) -o $@
   
.PHONY : clean   
# .PHONY ignores files named clean
clean:
	-$(RM) $(OBJ) core    
# '-' causes errors not to exit the process

.PHONY : realclean   
# .PHONY ignores files named realclean
realclean:
	-$(RM) $(OBJ) core $(EXE) $(ARCHIVE_NAME_SOURCE) $(ARCHIVE_NAME_RELEASE)   
# '-' causes errors not to exit the process

########################################################
#
# Package redistribution rules
#
########################################################

.PHONY : rclean
rclean: 
	$(RM) $(ARCHIVE_NAME_RELEASE) $(ARCHIVE_NAME_SOURCE)

.PHONY : spackage   
# .PHONY ignores files named package
spackage: realclean
	$(TAR) $(TAR_OPTIONS) $(ARCHIVE_NAME_SOURCE) *    
# '-' causes errors not to exit the process

.PHONY : rpackage   
# .PHONY ignores files named package
rpackage: clean rclean
	$(TAR) $(TAR_OPTIONS) $(ARCHIVE_NAME_RELEASE) *    
# '-' causes errors not to exit the process
