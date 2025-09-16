CXX = g++
CXXFLAGS = -std=c++17
LIBS = -lcurl
TARGET = graphcrawler
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)