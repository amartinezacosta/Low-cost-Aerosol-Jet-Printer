#ifndef RPC_H_
#define RPC_H_

#define MAX_ARGS    10

typedef int(*cmd_fxn)(int, char**);

int rpc_interpret(char *rcv, int len);
int rpc_register(char *cmd, cmd_fxn fxn);
void rpc_ack(int sid, int status);



#endif /* RPC_H_ */
