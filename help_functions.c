#include "functions.h"

int hash(char *str, int key){

    int ascii_sum=0, i, len;

    len = strlen(str);

    for(i=0; i<len; i++)
        ascii_sum += str[i];

    return ascii_sum%key;
}

struct BCTroot* BCTroot_Get(struct BCTroot* bctree_root, char* bitcoin_id){

    struct BCTroot *current_bct_root;
    current_bct_root = bctree_root;

    while(current_bct_root!=NULL){
        if(strcmp(current_bct_root->node->id, bitcoin_id)==0){
            return current_bct_root;
        }
        current_bct_root = current_bct_root->next;
    }

    return NULL;
}

void BCTnode_Create(struct SystemRoot** sys_root, struct BCTnode** bitcoin_root, struct Wallet *wallet, char *bc_id, int ammount){

    //creating the bitcoin, that is also a node in a BCtree
    struct BCTnode *new_bitcoin = (struct BCTnode*)malloc(sizeof(struct BCTnode));
    struct BCTnode *last = *bitcoin_root;
    strcpy(new_bitcoin->id, bc_id);
    new_bitcoin->ammount = ammount;
    new_bitcoin->wallet = wallet;
    new_bitcoin->transaction = NULL;
    new_bitcoin->left = NULL;
    new_bitcoin->right = NULL;
    new_bitcoin->next = NULL;

    //creating the tree for the current bitcoin
    struct BCTroot *new_bct_root = (struct BCTroot*)malloc(sizeof(struct BCTroot));
    struct BCTroot *last_bct_root = (*sys_root)->bctree_root;
    new_bct_root->numOfTransactions = 0;
    new_bct_root->unspent = ammount;
    new_bct_root->node = new_bitcoin;
    new_bct_root->next = NULL;
    //adding the new tree in the tree list
    if((*sys_root)->bctree_root == NULL){
        (*sys_root)->bctree_root = new_bct_root;
    }
    else{
        while(last_bct_root->next != NULL){
            last_bct_root = last_bct_root->next;
        }
        last_bct_root->next = new_bct_root;
    }
    //adding the new bitcoin in the bitcoin list of the wallet
    if(*bitcoin_root == NULL){
        *bitcoin_root = new_bitcoin;
        return;
    }
    while(last->next != NULL){
        last = last->next;
    }
    last->next = new_bitcoin;
    return;
}

int Wallet_Add(struct SystemRoot** sys_root, char *entry){

    char *token;
    token = strtok(entry, " ");
    if(token==NULL){
        printf("Error: bitCoinBalancesFile\n");
        return 0;
    }
    if(strchr(token, '\n')){
        token[strlen(token)-2] = '\0';
    }

    if(Wallet_Get((*sys_root)->wallet_root, token)){
        printf("Error. User %s is already added.\n", token);
        return 0;
    }

    struct Wallet *new_wallet = (struct Wallet*)malloc(sizeof(struct Wallet));
    struct Wallet *last = (*sys_root)->wallet_root;

    //inserting the data into the current wallet
    strcpy(new_wallet->id, token);
    // printf("Added Wallet with id %s\n", new_wallet->id);
    token = strtok(NULL, " ");
    if(token == NULL){
        new_wallet->balance = 0;
        new_wallet->bitcoin = NULL;
    }
    while(token!=NULL){
        if(strchr(token, '\n')){
            token[strlen(token)-2] = '\0';
        }
        if(BCTroot_Get((*sys_root)->bctree_root, token)){
            printf("Error. Bitcoin %s is already added.\n", token);
            return 0;
        }
        BCTnode_Create(sys_root, &new_wallet->bitcoin, new_wallet, token, (*sys_root)->bitCoinValue);
        token = strtok(NULL, " ");
        new_wallet->balance += (*sys_root)->bitCoinValue;
    }
    new_wallet->next = NULL;

    if((*sys_root)->wallet_root == NULL){
        (*sys_root)->wallet_root = new_wallet;
        return 1;
    }

    while(last->next != NULL){
        last = last->next;
    }

    last->next = new_wallet;

    return 1;
}

struct Wallet* Wallet_Get(struct Wallet* wallet_root, char* id){

    struct Wallet *current_wallet;
    current_wallet = wallet_root;
    while(current_wallet!=NULL){
        if(strcmp(current_wallet->id, id)==0){
            return current_wallet;
        }
        current_wallet = current_wallet->next;
    }

    return NULL;
}

struct Transaction *Transaction_Add(struct Transaction** transaction_root, struct Transaction *new_transaction){

    struct Transaction *last = *transaction_root;

    if(*transaction_root == NULL){
        *transaction_root = new_transaction;

        return new_transaction;
    }
    while(last->next != NULL){
        last = last->next;
    }

    last->next = new_transaction;

    return new_transaction;
}

void GetDate(char date[11]){
    int day, month, year;

    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    day = local->tm_mday;
    month = local->tm_mon + 1;
    year = local->tm_year + 1900;

    sprintf(date, "%02d-%02d-%d", day, month, year);

    return;
}

void GetTime(char mytime[6]){
    int hours, minutes;

    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    hours = local->tm_hour;
    minutes = local->tm_min;

    sprintf(mytime, "%02d:%02d\n", hours, minutes);

    return;
}

//returns true if given timestamps > last timestamps
int CompareTimestamps(char *given_date, char *given_time, char *last_date, char *last_time){

    int given_day, given_month, given_year;
    int last_day, last_month, last_year;
    int given_hours, given_minutes;
    int last_hours, last_minutes;

    char *last_date_tmp = malloc(sizeof(last_date));
    char *given_date_tmp = malloc(sizeof(given_date));
    char *last_time_tmp = malloc(sizeof(last_time));
    char *given_time_tmp = malloc(sizeof(given_time));

    strcpy(last_date_tmp, last_date);
    strcpy(given_date_tmp, given_date);
    strcpy(last_time_tmp, last_time);
    strcpy(given_time_tmp, given_time);

    given_day = atoi(strtok(given_date_tmp, "-"));
    given_month = atoi(strtok(NULL, "-"));
    given_year = atoi(strtok(NULL, "-"));

    last_day = atoi(strtok(last_date_tmp, "-"));
    last_month = atoi(strtok(NULL, "-"));
    last_year = atoi(strtok(NULL, "-"));

    if(given_year < last_year){
        free(last_date_tmp);
        free(given_date_tmp);
        free(last_time_tmp);
        free(given_time_tmp);
        return 0;
    }


    if( (given_year == last_year) && (given_month < last_month)){
        free(last_date_tmp);
        free(given_date_tmp);
        free(last_time_tmp);
        free(given_time_tmp);
        return 0;
    }


    if( (given_year == last_year) && (given_month == last_month) && (given_day < last_day)){
        free(last_date_tmp);
        free(given_date_tmp);
        free(last_time_tmp);
        free(given_time_tmp);
        return 0;
    }


    if( (given_year == last_year) && (given_month == last_month) && (given_day == last_day)){

        given_hours = atoi(strtok(given_time_tmp, ":"));
        given_minutes = atoi(strtok(NULL, ":"));
        last_hours = atoi(strtok(last_time_tmp, ":"));
        last_minutes = atoi(strtok(NULL, ":"));

        if(given_hours < last_hours){
            free(last_date_tmp);
            free(given_date_tmp);
            free(last_time_tmp);
            free(given_time_tmp);
            return 0;
        }

        if( (given_hours == last_hours) && (given_minutes <= last_minutes)){
            free(last_date_tmp);
            free(given_date_tmp);
            free(last_time_tmp);
            free(given_time_tmp);
            return 0;
        }


    }

    free(last_date_tmp);
    free(given_date_tmp);
    free(last_time_tmp);
    free(given_time_tmp);

    return 1;
}

//returns true if given times > last times
int CompareTimes(char *given_time, char *last_time){

    int given_hours, given_minutes;
    int last_hours, last_minutes;

    char *last_time_tmp = malloc(sizeof(last_time));
    char *given_time_tmp = malloc(sizeof(given_time));

    strcpy(last_time_tmp, last_time);
    strcpy(given_time_tmp, given_time);

    given_hours = atoi(strtok(given_time_tmp, ":"));
    given_minutes = atoi(strtok(NULL, ":"));
    last_hours = atoi(strtok(last_time_tmp, ":"));
    last_minutes = atoi(strtok(NULL, ":"));

    if(given_hours < last_hours)
        return 0;

    if( (given_hours == last_hours) && (given_minutes <= last_minutes))
        return 0;

    return 1;
}
//returns true if given dates > last dates
int CompareDates(char *given_date, char *last_date){

    int given_day, given_month, given_year;
    int last_day, last_month, last_year;

    char *last_date_tmp = malloc(sizeof(last_date));
    char *given_date_tmp = malloc(sizeof(given_date));

    strcpy(last_date_tmp, last_date);
    strcpy(given_date_tmp, given_date);

    given_day = atoi(strtok(given_date_tmp, "-"));
    given_month = atoi(strtok(NULL, "-"));
    given_year = atoi(strtok(NULL, "-"));

    last_day = atoi(strtok(last_date_tmp, "-"));
    last_month = atoi(strtok(NULL, "-"));
    last_year = atoi(strtok(NULL, "-"));

    if(given_year < last_year)
        return 0;

    if( (given_year == last_year) && (given_month < last_month))
        return 0;

    if( (given_year == last_year) && (given_month == last_month) && (given_day < last_day))
        return 0;

    return 1;
}

void ReplaceStr(char *src, char *str, char *rep){

    char *p = strstr(src, str);
    do{
        if(p){
            char buf[1024];
            memset(buf,'\0',strlen(buf));

            if(src == p){
                strcpy(buf,rep);
                strcat(buf,p+strlen(str));
            }
            else{
                strncpy(buf,src,strlen(src) - strlen(p));
                strcat(buf,rep);
                strcat(buf,p+strlen(str));
            }

            memset(src,'\0',strlen(src));
            strcpy(src,buf);
        }

    }while(p && (p = strstr(src, str)));
}

void printTree(struct BCTnode* current_bitcoin, int flag) {
   if(current_bitcoin != NULL){
       if(flag){
           printf("%s %s %s %d %s %s\n", current_bitcoin->transaction->id, current_bitcoin->transaction->sender->id, current_bitcoin->transaction->receiver->id, current_bitcoin->transaction->ammount, current_bitcoin->transaction->date, current_bitcoin->transaction->time);
       }
       printTree(current_bitcoin->left, 1);
       printTree(current_bitcoin->right, 0);
   }
}

void freeTree(struct BCTnode* current_bitcoin) {
   if(current_bitcoin == NULL)
       return;

   freeTree(current_bitcoin->left);
   freeTree(current_bitcoin->right);

   free(current_bitcoin);
}
