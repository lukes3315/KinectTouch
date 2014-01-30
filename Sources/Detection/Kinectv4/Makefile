NAME = kinect
SRCS = main.cpp		\
	Mutex.cpp	\
	MyFreenectDevice.cpp	\
	Button.cpp

OBJS = $(SRCS:.cpp=.o)

$(NAME) : $(OBJS)
	g++ $(SRCS) -o $(NAME)  -lfreenect -lfreenect_sync -W -Wall -Wextra -lsfml-window -lsfml-graphics -lsfml-system -lSDL -lGL -lGLU

all : $(NAME)

clean :
	rm -f $(OBJS)
	rm -f *~

fclean	:	clean
		rm -f $(NAME)

re	:	fclean all