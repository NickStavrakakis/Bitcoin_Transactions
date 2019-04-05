#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "structs.h"

//help_functions.c

int hash(char *str, int key);

struct BCTroot* BCTroot_Get(struct BCTroot* bctree_root, char* bitcoin_id);

void BCTnode_Create(struct SystemRoot** sys_root, struct BCTnode** bitcoin_root, struct Wallet *wallet, char *bc_id, int ammount);

int Wallet_Add(struct SystemRoot** sys_root, char *entry);

struct Wallet* Wallet_Get(struct Wallet* wallet_root, char* id);

struct Transaction *Transaction_Add(struct Transaction** transaction_root, struct Transaction *new_transaction);

void GetDate(char date[11]);

void GetTime(char mytime[6]);

int CompareTimestamps(char *given_date, char *given_time, char *last_date, char *last_time);

int CompareTimes(char *given_time, char *last_time);

int CompareDates(char *given_date, char *last_date);

void ReplaceStr(char *src, char *str, char *rep);

void printTree(struct BCTnode* current_bitcoin, int flag);

void freeTree(struct BCTnode* current_bitcoin);

//main_functions.c

void requestTransaction(struct SystemRoot** sys_root, char *transaction_id, char *sender_id, char *receiver_id, int ammount, char date[11], char time[6]);

void bitCoinStatus(struct BCTroot* bctree_root, char *bitcoin_id);

void findEarnings(struct SystemRoot* sys_root, char *wallet_id, char time1[6], char date1[11], char time2[6], char date[11]);

void findPayments(struct SystemRoot* sys_root, char *wallet_id, char time1[6], char date1[11], char time2[6], char date2[11]);

void traceCoin(struct SystemRoot* sys_root, char *bitcoin_id);
