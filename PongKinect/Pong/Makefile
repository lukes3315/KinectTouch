
NAME	=	Pong
SRCS	=	Pong.cpp	
OBJS	=	$(SRCS:.cpp=.o)
CC	=	g++
CFLAGS	=	-lSDL -lSDL_image -lSDL_ttf
RM	=	rm -f

$(NAME)	: $(OBJS)
	$(CC) $(OBJS) -o $(NAME) $(CFLAGS)

all	: $(NAME)

clean	:
	$(RM) *~
	$(RM) *#
	$(RM) *.o

fclean	: clean
	$(RM) $(NAME)

re	: fclean all
