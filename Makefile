# Comum
SRC_DIR						:= ./src
LOG_DIR						:= ./logs
INC_DIR						:= ./includes

CC							:= c++
INCLUDES					:= -I${INC_DIR}

SRC							:= $(wildcard $(SRC_DIR)/**/*.cpp $(SRC_DIR)/*.cpp)

# Main
OBJ_DIR						:= ./obj

NAME						:= webserv

OBJ							:= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
CFLAGS						:= -Wall -Werror -Wextra -std=c++98 ${INCLUDES}
LDFLAGS						:= -Wall -Werror -Wextra -std=c++98 -g -fsanitize=address ${INCLUDES}

# Tests
TEST_OBJ_DIR				:= ./obj_test

TEST_NAME					:= webserv_unit_test

TEST_OBJ					:= $(patsubst $(SRC_DIR)/%.cpp,${TEST_OBJ_DIR}/%.o,$(SRC))
TEST_CFLAGS					:= -pthread ${INCLUDES}
TEST_LDFLAGS				:= -lgtest -lgtest_main -pthread -g -fsanitize=address ${INCLUDES}

########## COMMANDS ##########

# Main
all: ${NAME}

${NAME}: ${OBJ}
	@mkdir -p ${LOG_DIR}
	@${CC} -o ${NAME} $^ ${LDFLAGS}
	@printf "                                               \r"
	@printf "\033[0;32mCompilação completa.\033[0m\n"

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	@mkdir -p $(dir $@)
	@${CC} -c ${CFLAGS} $< -o $@
	@printf "\033[0;36mCompilando: $<              \033[0m\r"

run: fclean all
	@rm -rf ${LOG_DIR}/*.txt
	@./${NAME}

clean:
	@rm -rf ${OBJ_DIR}

fclean: clean
	@rm -rf ${NAME}
	@rm -rf ${LOG_DIR}

re: fclean all

# Tests
test: ${TEST_NAME}

${TEST_NAME}: ${TEST_OBJ}
	@mkdir -p ${LOG_DIR}
	@${CC} -o ${TEST_NAME} $^ ${TEST_LDFLAGS}

${TEST_OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	@mkdir -p $(dir $@)
	@${CC} -c ${TEST_CFLAGS} -DTEST_MODE=1 $< -o $@

testrun: testfclean test
	@rm -rf ${LOG_DIR}/*.txt
	@./${TEST_NAME}

testclean:
	@rm -rf ${TEST_OBJ_DIR}

testfclean: testclean
	@rm -rf ${TEST_NAME}
	@rm -rf ${LOG_DIR}

testre: testfclean test

# Comum

cppcheck:
	@cppcheck ${SRC_DIR} --suppress=missingInclude --error-exitcode=1 --enable=all -I./includes

googletest: testrun testfclean

.PHONY: all run clean fclean re test testrun testclean testfclean testre install cppcheck googletest
