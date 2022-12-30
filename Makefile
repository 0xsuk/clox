BUILD_DIR := build
SOURCE_DIR := src
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

CFLAGS := -Wall -Wextra -Werror

$(BUILD_DIR)/clox: $(OBJECTS)
	@printf "%8s %-40s %s\n" $(CC) $@ "$(CFLAGS)"
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $^ -o $@


$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@printf "%8s %-40s %s\n" $(CC) $< "$(CFLAGS)"
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<


clean:
	@ rm -rf $(BUILD_DIR)
