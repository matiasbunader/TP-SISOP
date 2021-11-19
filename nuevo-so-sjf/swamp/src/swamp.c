#include "swamp.h"

int main(int argc, char **argv) {

  t_log *logger = log_create("./log/swamp.log", "SWAMP", true, LOG_LEVEL_INFO);
  log_info(logger, "Soy el proceso Swamp! \n");
  log_destroy(logger);
}