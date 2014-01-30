NAME = kinect
SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)

$(NAME) : $(OBJS)
	g++  -o $(NAME) $(SRCS) -lfreenect -lfreenect_sync -W -Wall -Wextra -lsfml-window -lsfml-system -lsfml-graphics -lSDL -lGL -lGLU 

all : $(NAME)

clean :
	rm -f $(OBJS)
	rm -f *~

fclean	:	clean
		rm -f $(NAME)

re	:	fclean all