#include "utils.h"

void throw_error(int code, t_log* logger, bool should_end) {
    char * custom_message = string_new();
    switch (code) {
        case CONFIG_ERROR: {
            string_append(&custom_message, "Config error");
            break;
        }    
        case CLIENT_DISCONNECTED: {
            string_append(&custom_message, "Client disconnected");
            break;
        }
        default:
            string_append(&custom_message, "Unknown error");
            break;
    }
    log_error(logger, "[Error]: (code: %d) %s", code, custom_message);

    if (should_end) exit(code);
}