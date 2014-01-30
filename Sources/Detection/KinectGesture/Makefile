NAME = kinect
SRCS = main.cpp		\
	Mutex.cpp	\
	MyFreenectDevice.cpp	\
	Button.cpp	\
	Finger.cpp

OBJS = $(SRCS:.cpp=.o)

$(NAME) : $(OBJS)
	g++ -lfreenect -lfreenect_sync -W -Wall -Wextra -lsfml-window -lsfml-graphics -lSDL -lGL -lGLU $(SRCS) -o $(NAME)

all : $(NAME)

clean :
	rm -f $(OBJS)
	rm -f *~

fclean	:	clean
		rm -f $(NAME)

re	:	fclean all