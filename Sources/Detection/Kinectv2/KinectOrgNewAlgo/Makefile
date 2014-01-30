NAME = kinect
SRCS = main.cpp		\
	Mutex.cpp	\
	MyFreenectDevice.cpp	\
	Button.cpp

OBJS = $(SRCS:.cpp=.o)

$(NAME) : $(OBJS)
	g++ -lfreenect -W -Wall -Wextra -lsfml-window -lsfml-graphics -lSDL -lGL -lGLU $(SRCS) -o $(NAME)

all : $(NAME)

clean :
	rm $(OBJS)
	rm -f *~

fclean	:	clean
		rm $(NAME)

re	:	fclean all