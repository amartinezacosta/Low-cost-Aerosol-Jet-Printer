/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef RPC_H_
#define RPC_H_

#define MAX_ARGS    10

typedef int(*cmd_fxn)(int, char**);

int rpc_interpret(char *rcv, int len);
int rpc_register(char *cmd, cmd_fxn fxn);
void rpc_ack(int sid, int status);



#endif /* RPC_H_ */
