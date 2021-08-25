TARGET = app

SOURCE_DIR = src
INCLUDE_DIR = inc
OBJECT_DIR = bin

SOURCES = $(shell find $(SOURCE_DIR)/*.c)
INCLUDES = $(shell find $(INCLUDE_DIR)/*.h)
OBJECTS = $(SOURCES:$(SOURCE_DIR)/%.c=$(OBJECT_DIR)/%.o)

$(TARGET): $(OBJECT_DIR) $(OBJECTS) $(INCLUDES) 
	gcc -o $(TARGET) $(OBJECTS)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	gcc -c -o $@ $< -I$(INCLUDE_DIR)

$(OBJECT_DIR):
	@mkdir $(OBJECT_DIR)

mrproper:
	@rm -f $(TARGET)
	@rm -rf $(OBJECT_DIR)/*

out.bin: test.rom
	@./app test.rom -o out.bin

hd: $(TARGET) out.bin 
	@hexdump -C out.bin