#define RECV_BUF_SIZE 2048

void connect_socket(int sock, int port, unsigned int addr);
void send_atg_inst(int sock, char inst[8]);
void read_atg(int sock, char buf[RECV_BUF_SIZE]);