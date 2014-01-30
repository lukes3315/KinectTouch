NAME = kinect
SRCS = 	main.cpp		\
	CamStreamer.cpp		\
	BlobDetector.cpp	\
	ConvexHullManager.cpp	\
	Tiles.cpp		\
	Menu.cpp

INCLUDE = -I/usr/include/opencv/

OBJS = $(SRCS:.cpp=.o) 

$(NAME) :
	g++ -lpthread -I/usr/include/opencv-2.3.1/opencv/ -I/usr/include/opencv-2.3.1/ -lopencv_highgui  -lfreenect -lfreenect_sync -W -Wall -Wextra $(SRCS) -o $(NAME)

all : $(NAME)

clean :
	rm -f $(OBJS)
	rm -f *~

fclean	:	clean
		rm -f $(NAME)

re	:	fclean all