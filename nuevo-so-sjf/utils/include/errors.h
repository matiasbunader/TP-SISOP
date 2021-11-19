typedef enum {
    SUCCESS = 100,
    CONFIG_ERROR = -1,
    UNKNOWN_BACKEND_ERROR = -2,
    CONNECTION_ERROR = -3,
    COMMUNICATION_ERROR = -4,
    CLIENT_DISCONNECTED = -5
} error_code;

typedef enum { UNKNOWN = -1, KERNEL = 1, MEMORIA = 2 } backend_code;

void throw_error(int code, t_log *logger, bool should_end); 