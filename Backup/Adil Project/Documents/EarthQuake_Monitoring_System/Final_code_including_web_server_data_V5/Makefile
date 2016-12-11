# You need to "sudo apt-get install libgtkmm-3.0-dev" to build the demo_3d binary

all: EARTHQUAKE

HDRS = I2Cdev.h MPU6050.h
CMN_OBJS = I2Cdev.o MPU6050.o
RAW_OBJS = EARTHQUAKE.o

CXXFLAGS = -Wall -g -O2 `pkg-config gtkmm-3.0 --cflags --libs`

$(CMN_OBJS) $(RAW_OBJS) : $(HDRS)

EARTHQUAKE: $(CMN_OBJS) $(RAW_OBJS)
	$(CXX) -o $@ $^ -lm 

clean:
	rm -f $(CMN_OBJS) $(RAW_OBJS) EARTHQUAKE

