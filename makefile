CC       = g++
CFLAGS   = -std=c++17 -DWITH_FASTCGI# -Wall 
LDFLAGS  = -lfcgi
SRCS	 = networkManagementServer.cpp
OBJFILES = networkManagementServer.cpp stdsoap2.cpp soapC.cpp soapmanagerService.cpp
TARGET   = server.fcgi

all: $(TARGET)

$(TARGET): $(OBJFILES)
	${CC} ${CFLAGS} -o $(TARGET) $(OBJFILES) $(LDFLAGS)

%.o: %.cpp
	${CC} ${CFLAGS} ${INCLUDES} -c $< 

clean:
	rm -f $(OBJFILES) Makefile.bak

mrproper: clean
	rm -f $(TARGET)

depend: 
	makedepend -I. $(SRCS)

exe: $(TARGET)
	./$(TARGET)
