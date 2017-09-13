CC:= g++
SRC_DIR:= ./src
INC_DIR:= ./inc
OBJ_DIR:= ./obj
BIN_DIR:= ./bin
CFLAG:= -std=c++11
SRCEXT:= cpp
#LIB=-lm -H -c -Wall

APP= mul

all: main.o cfg_info.o inter_bus.o sched.o tick_cfg.o timer.o 
	
	g++ $(CFLAG) -g -o $(APP) $(OBJ_DIR)/main.o $(OBJ_DIR)/cfg_info.o $(OBJ_DIR)/inter_bus.o $(OBJ_DIR)/sched.o $(OBJ_DIR)/tick_cfg.o $(OBJ_DIR)/timer.o 
	mv $(APP) $(BIN_DIR)

main.o: $(SRC_DIR)/main.$(SRCEXT) \
	$(INC_DIR)/cfg_info.h \
	$(INC_DIR)/main.h \
	$(INC_DIR)/dvfs_info.h \
	$(INC_DIR)/sched.h \
	$(INC_DIR)/timer.h \
	$(INC_DIR)/inter_bus.h

	g++ $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/main.$(SRCEXT)
	mv *.o obj/ 	

cfg_info.o: $(SRC_DIR)/cfg_info.$(SRCEXT) $(INC_DIR)/cfg_info.h $(INC_DIR)/dvfs_info.h $(INC_DIR)/main.h 

	g++ $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/cfg_info.$(SRCEXT)
	mv *.o obj/

inter_bus.o : $(SRC_DIR)/inter_bus.$(SRCEXT) $(INC_DIR)/sched.h $(INC_DIR)/main.h

	g++ $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/inter_bus.$(SRCEXT)
	mv *.o obj/

sched.o : $(SRC_DIR)/sched.$(SRCEXT) $(INC_DIR)/sched.h $(INC_DIR)/dvfs_info.h

	g++ $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/sched.$(SRCEXT)
	mv *.o obj/

tick_cfg.o : $(SRC_DIR)/tick_cfg.$(SRCEXT) $(INC_DIR)/cfg_info.h $(INC_DIR)/dvfs_info.h $(INC_DIR)/main.h

	g++ $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/tick_cfg.$(SRCEXT)
	mv *.o obj/

timer.o : $(SRC_DIR)/timer.$(SRCEXT) $(INC_DIR)/timer.h

	g++ $(INC_DIR) $(CFLAG) -g -c $(SRC_DIR)/timer.$(SRCEXT)
	mv *.o obj/

clean:
	clear
	rm  ./obj/*.o
	rm  ./bin/*
	rm  ./inc/*.h.gch
