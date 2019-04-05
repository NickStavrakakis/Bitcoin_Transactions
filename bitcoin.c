#include "functions.h"

int main(int argc, char *argv[]){

    printf("\n./bitcoin started succesfully\n");

    int i, tr_ammount, transaction_id_max = 0, generated_input;
    FILE *bitCoinBalancesFile, *transactionsFile, *inputFile_TEMP;
    char file_line[INPUT_MAXLINES], *token, tr_id[ID_MAXSIZE], tr_sender_id[ID_MAXSIZE], tr_receiver_id[ID_MAXSIZE], tr_date[ID_MAXSIZE], tr_time[ID_MAXSIZE];
    char cmd[CMD_MAXSIZE];

    struct SystemRoot *sys_root = (struct SystemRoot*)malloc(sizeof(struct SystemRoot));
    sys_root->last_date = malloc(strlen("01-01-1970")+1);
    sys_root->last_time = malloc(strlen("00:01")+1);
    strcpy(sys_root->last_date, "01-01-1970");
    strcpy(sys_root->last_time, "00:01");


    //getting the pramaters
    if(argc!=13){
        printf("Error: Wrong Arguments\n");
        return 0;
    }
    else{
        for(i=1; i<=12; i+=2){

            if(strcmp(argv[i], "-a")==0){
                bitCoinBalancesFile = fopen(argv[i+1], "r");
                if(bitCoinBalancesFile==NULL){
                    printf("Error: Can't Open bitCoinBalancesFile\n");
                    return 0;
                }
            }
            else if(strcmp(argv[i], "-t")==0){
                transactionsFile = fopen(argv[i+1], "r");
                if(transactionsFile==NULL){
                    printf("Error: Can't Open transactionsFile\n");
                    return 0;
                }
            }
            else if(strcmp(argv[i], "-v")==0){
                sys_root->bitCoinValue = atoi(argv[i+1]);
            }
            else if(strcmp(argv[i], "-h1")==0){
                sys_root->senderHashtableNumOfEntries = atoi(argv[i+1]);
            }
            else if(strcmp(argv[i], "-h2")==0){
                sys_root->receiverHashtableNumOfEntries = atoi(argv[i+1]);
            }
            else if(strcmp(argv[i], "-b")==0)
                sys_root->bucketSize = atoi(argv[i+1]);
            else{
                printf("Error: Wrong Arguments\n");
                return 0;
            }
        }
    }

    sys_root->bucketPos = (sys_root->bucketSize-sizeof(struct Bucket*))/(sizeof(struct Entry));

    //initialising senderHashTable and receiverHashTable
    sys_root->senderHashTable = malloc(sizeof(struct Bucket)*(sys_root->senderHashtableNumOfEntries));
    for(i=0; i<sys_root->senderHashtableNumOfEntries; i++){
        sys_root->senderHashTable[i].entry = malloc(sys_root->bucketSize-(sizeof(struct Bucket*)));
        sys_root->senderHashTable[i].entry->wallet = NULL;
        sys_root->senderHashTable[i].entry->transaction = NULL;
        sys_root->senderHashTable[i].next = NULL;
    }
    sys_root->receiverHashTable = malloc(sizeof(struct Bucket)*(sys_root->receiverHashtableNumOfEntries));
    for(i=0; i<sys_root->receiverHashtableNumOfEntries; i++){
        sys_root->receiverHashTable[i].entry = malloc(sys_root->bucketSize-(sizeof(struct Bucket*)));
        sys_root->receiverHashTable[i].entry->wallet = NULL;
        sys_root->receiverHashTable[i].entry->transaction = NULL;
        sys_root->receiverHashTable[i].next = NULL;
    }

    //getting bitCoinBalancesFile
    printf("\nInserting bitCoinBalancesFile...\n");
    sys_root->wallet_root = NULL;
    sys_root->bctree_root = NULL;
    while(fgets(file_line, sizeof(file_line), bitCoinBalancesFile)!=NULL){
        if(Wallet_Add(&sys_root, file_line) == 0){
            return 0;
        }
    }
    fclose(bitCoinBalancesFile);

    //getting transactionsFile
    printf("\nInserting transactionsFile...\n");
    int lines = 0;
    while(fgets(file_line, INPUT_MAX, transactionsFile))
        lines++;
    char transactionsIds[lines][ID_MAXSIZE];
    for(i=0; i<lines; i++)
        transactionsIds[i][0] = '#';
    rewind(transactionsFile);

    while(fgets(file_line, INPUT_MAX, transactionsFile)){
        //getting the transaction info
        token = strtok(file_line, " "); //transaction id
        if(token==NULL){
            printf("Error: transactionsFile\n");
            return 0;
        }
        strcpy(tr_id, token);
        for(i=0; i<lines; i++){
            if(transactionsIds[i][0]=='#')
                break;
            if(strcmp(transactionsIds[i], tr_id)==0){
                printf("Error. The is already a transaction with id %s\n", tr_id);
                return 0;
            }
        }
        strcpy(transactionsIds[i], tr_id);

        if(transaction_id_max < atoi(tr_id)){
            transaction_id_max = atoi(tr_id);
        }
        token = strtok(NULL, " ");  //sender id
        if(token==NULL){
            printf("Error: transactionsFile\n");
            return 0;
        }
        strcpy(tr_sender_id, token);
        token = strtok(NULL, " ");  //receiver id
        if(token==NULL){
            printf("Error: transactionsFile\n");
            return 0;
        }
        strcpy(tr_receiver_id, token);
        token = strtok(NULL, " ");  //transaction ammount
        if(token==NULL){
            printf("Error: transactionsFile\n");
            return 0;
        }
        tr_ammount = atoi(token);
        token = strtok(NULL, " ");  //date of transaction
        if(token==NULL){
            printf("Error: transactionsFile\n");
            return 0;
        }
        strcpy(tr_date, token);
        token = strtok(NULL, " ");  //time of transaction
        if(token==NULL){
            printf("Error: transactionsFile\n");
            return 0;
        }
        if(strchr(token, '\n')){
            token[strlen(token)-2] = '\0';
        }
        strcpy(tr_time, token);

        if(strcmp(tr_sender_id, tr_receiver_id)==0){
            printf("Transaction %s: Error. User %s can not make a transaction to himself.\n", tr_id, tr_sender_id);
            continue;
        }
        requestTransaction(&sys_root, tr_id, tr_sender_id, tr_receiver_id, tr_ammount, tr_date, tr_time);
    }
    fclose(transactionsFile);

    char function_name[20], *parameters, *current_transaction, arg1[ID_MAXSIZE], arg2[ID_MAXSIZE], arg3[ID_MAXSIZE], arg4[ID_MAXSIZE], arg5[ID_MAXSIZE];//, arg2[MAX_ACC_LENG], arg3[MAX_ACC_LENG];
    char transaction_id_current[50];
    FILE *inputFile;
    while(1){
        printf("\n/");
        arg1[0] = '\0';
        arg2[0] = '\0';
        arg3[0] = '\0';
        arg4[0] = '\0';
        arg5[0] = '\0';
        generated_input = 0;
        fgets(cmd, CMD_MAXSIZE, stdin);
        sscanf(cmd, "%s", function_name);
        parameters = strchr(cmd, ' ');

        if(strcmp(function_name, "requestTransaction") == 0){
            if(parameters != NULL){
                sscanf(parameters, "%s %s %s %s %s\n", arg1, arg2, arg3, arg4, arg5);
                if( (arg1[0] != '\0') && (arg2[0] != '\0') && (arg3[0] != '\0') ){
                    if(strcmp(arg1, arg2)==0){
                        printf("Transaction %d: Error. User %s can not make a transaction to himself.\n", transaction_id_max+1, arg1);
                        continue;
                    }
                    if((arg4[0] != '\0') && (arg5[0] == '\0')){
                        if(strlen(arg4)>6){ //if the given value its the date
                            GetTime(arg5);
                        }
                        else{ //if its the time
                            strcpy(arg5, arg4);
                            GetDate(arg4);
                        }
                    }
                    else if((arg4[0] == '\0') && (arg5[0] == '\0')){
                        GetDate(arg4);
                        GetTime(arg5);
                    }
                    else if(CompareTimestamps(arg4, arg5, sys_root->last_date, sys_root->last_time) == 0){
                        printf("Transaction %d: Error. Invalid Timestamp\n", transaction_id_max+1);
                        continue;
                    }
                    sprintf(transaction_id_current, "%d", ++transaction_id_max);
                    requestTransaction(&sys_root, transaction_id_current, arg1, arg2, atoi(arg3), arg4, arg5);
                }
                continue;
            }
        }
        else if(strcmp(function_name, "requestTransactions") == 0){
            if(parameters != NULL){
                sscanf(parameters, "%s %s\n", arg1, arg2);
                if(arg2[0]!='\0'){
                    inputFile_TEMP = fopen("inputFile_TEMP", "w+");
                    if(inputFile_TEMP==NULL){
                        printf("Error executing the command\n");
                        continue;
                    }
                    fprintf(inputFile_TEMP, "%s", parameters);
                    fgets(parameters, CMD_MAXSIZE, stdin);
                    while(parameters[0]!='\n'){
                        fprintf(inputFile_TEMP, "%s", parameters);
                        fgets(parameters, CMD_MAXSIZE, stdin);
                    }
                    fclose(inputFile_TEMP);

                    inputFile = fopen("inputFile_TEMP", "r");
                    if(inputFile == NULL){
                        printf("Error: Can't Open inputFile\n");
                        continue;
                    }

                    generated_input = 1;
                }
                else{
                    inputFile = fopen(arg1, "r");
                    if(inputFile == NULL){
                        printf("Error: Can't Open inputFile\n");
                        continue;
                    }
                }
                while(fgets(file_line, sizeof(file_line), inputFile)!=NULL){
                    if(strchr(file_line, ';')){
                        current_transaction = strtok(file_line, ";");
                        arg1[0] = '\0';
                        arg2[0] = '\0';
                        arg3[0] = '\0';
                        arg4[0] = '\0';
                        arg5[0] = '\0';
                        sscanf(current_transaction, "%s %s %s %s %s\n", arg1, arg2, arg3, arg4, arg5);
                        if( (arg1[0] != '\0') && (arg2[0] != '\0') && (arg3[0] != '\0') ){
                            if(strcmp(arg1, arg2)==0){
                                printf("Transaction %d: Error. User %s can not make a transaction to himself.\n", transaction_id_max+1, arg1);
                                break;
                            }
                            if((arg4[0] != '\0') && (arg5[0] == '\0')){
                                if(strlen(arg4)>6){ //if the given value its the date
                                    GetTime(arg5);
                                }
                                else{ //if its the time
                                    strcpy(arg5, arg4);
                                    GetDate(arg4);
                                }
                            }
                            else if((arg4[0] == '\0') && (arg5[0] == '\0')){
                                GetDate(arg4);
                                GetTime(arg5);
                            }
                            else if(CompareTimestamps(arg4, arg5, sys_root->last_date, sys_root->last_time) == 0){
                                printf("Transaction %d: Error. Invalid Timestamp\n", transaction_id_max+1);
                                break;
                            }
                            sprintf(transaction_id_current, "%d", ++transaction_id_max);
                            requestTransaction(&sys_root, transaction_id_current, arg1, arg2, atoi(arg3), arg4, arg5);
                        }
                    }
                }
                if(generated_input){
                    fclose(inputFile_TEMP);
                    remove("inputFile_TEMP");
                    generated_input = 0;
                }
                else{
                    fclose(inputFile);
                }
                continue;
            }
        }
        else if(strcmp(function_name, "findEarnings") == 0){
            if(parameters != NULL){
                ReplaceStr(parameters, "[]", "# ");
                ReplaceStr(parameters, "[", " ");
                ReplaceStr(parameters, "]", " ");
                sscanf(parameters, "%s %s %s %s %s\n", arg1, arg2, arg3, arg4, arg5);
                if( (arg1[0] != '\0') && ( ((arg2[0]=='#') && (arg4[0]=='#')) || ((arg2[0]!='#') && (arg4[0]!='#')) ) && ( ((arg3[0]=='#') && (arg5[0]=='#')) || ((arg3[0]!='#') && (arg5[0]!='#')) ) ){
                    findEarnings(sys_root, arg1, arg2, arg3, arg4, arg5);
                    continue;
                }
            }
        }
        else if(strcmp(function_name, "findPayments") == 0){
            if(parameters != NULL){
                ReplaceStr(parameters, "[]", "# ");
                ReplaceStr(parameters, "[", " ");
                ReplaceStr(parameters, "]", " ");
                sscanf(parameters, "%s %s %s %s %s\n", arg1, arg2, arg3, arg4, arg5);
                if( (arg1[0] != '\0') && ( ((arg2[0]=='#') && (arg4[0]=='#')) || ((arg2[0]!='#') && (arg4[0]!='#')) ) && ( ((arg3[0]=='#') && (arg5[0]=='#')) || ((arg3[0]!='#') && (arg5[0]!='#')) ) ){
                    findPayments(sys_root, arg1, arg2, arg3, arg4, arg5);
                    continue;
                }
            }
        }
        else if(strcmp(function_name, "walletStatus") == 0){
            if(parameters != NULL){
                sscanf(parameters, "%s %s \n", arg1, arg2);
                if( (arg1[0] != '\0') && (arg2[0] == '\0') ){
                    struct Wallet *current_wallet = Wallet_Get(sys_root->wallet_root, arg1);
                    if(current_wallet == NULL){
                        printf("Error: Wallet %s does not exist\n", arg1);
                    }
                    else{
                        printf("Wallet %s has %d credits\n", arg1, current_wallet->balance);
                    }
                    continue;
                }
            }
        }
        else if(strcmp(function_name, "bitCoinStatus") == 0){
            if(parameters != NULL){
                sscanf(parameters, "%s %s \n", arg1, arg2);
                if( (arg1[0] != '\0') && (arg2[0] == '\0')){
                    bitCoinStatus(sys_root->bctree_root, arg1);
                    continue;
                }
            }
        }
        else if(strcmp(function_name, "traceCoin") == 0){
            if(parameters != NULL){
                sscanf(parameters, "%s %s \n", arg1, arg2);
                if( (arg1[0] != '\0') && (arg2[0] == '\0')){
                    traceCoin(sys_root, arg1);
                    continue;
                }
            }
        }
        else if(strcmp(function_name, "exit") == 0){
            if(parameters == NULL){
                printf("Exiting\n");
                break;
            }
        }
        printf("Error: Invalid Command\n");
    }

    //freeing the memory
    struct BCTroot *previous_btc_root;
    struct BCTroot *current_btc_root;
    current_btc_root=sys_root->bctree_root;
    while(current_btc_root!=NULL){
        freeTree(current_btc_root->node);
        previous_btc_root = current_btc_root;
        current_btc_root = current_btc_root->next;
        free(previous_btc_root);
    }

    struct Wallet *previous_wallet;
    struct Wallet *current_wallet;
    current_wallet=sys_root->wallet_root;
    while(current_wallet!=NULL){
        previous_wallet=current_wallet;
        current_wallet=current_wallet->next;
        free(previous_wallet);
    }

    int j;
    struct Entry current_entry;
    struct Bucket current_bucket;
    struct Bucket *previous_overflow_bucket;
    struct Bucket *current_overflow_bucket;
    struct Transaction *previous_transaction;
    struct Transaction *curr_transaction;
    for(i=0; i<sys_root->senderHashtableNumOfEntries; i++){
        current_bucket = sys_root->senderHashTable[i];
        for(j=0; j<sys_root->bucketPos; j++){
            current_entry = current_bucket.entry[j];
            curr_transaction = current_entry.transaction;
            while(curr_transaction!=NULL){
                previous_transaction = curr_transaction;
                curr_transaction = curr_transaction->next;
                free(previous_transaction);
            }
        }
        free(current_bucket.entry);
        current_overflow_bucket = current_bucket.next;
        while(current_overflow_bucket!=NULL){
            for(j=0; j<sys_root->bucketPos; j++){
                current_entry = current_overflow_bucket->entry[j];
                curr_transaction = current_entry.transaction;
                while(curr_transaction!=NULL){
                    previous_transaction = curr_transaction;
                    curr_transaction = curr_transaction->next;
                    free(previous_transaction);
                }
                free(current_entry.wallet);
            }
            previous_overflow_bucket = current_overflow_bucket;
            current_overflow_bucket = current_overflow_bucket->next;
            free(previous_overflow_bucket);
        }
    }
    for(i=0; i<sys_root->receiverHashtableNumOfEntries; i++){
        current_bucket = sys_root->receiverHashTable[i];
        for(j=0; j<sys_root->bucketPos; j++){
            current_entry = current_bucket.entry[j];
            curr_transaction = current_entry.transaction;
            while(curr_transaction!=NULL){
                previous_transaction = curr_transaction;
                curr_transaction = curr_transaction->next;
                free(previous_transaction);
            }
        }
        free(current_bucket.entry);
        current_overflow_bucket = current_bucket.next;
        while(current_overflow_bucket!=NULL){
            for(j=0; j<sys_root->bucketPos; j++){
                current_entry = current_overflow_bucket->entry[j];
                curr_transaction = current_entry.transaction;
                while(curr_transaction!=NULL){
                    previous_transaction = curr_transaction;
                    curr_transaction = curr_transaction->next;
                    free(previous_transaction);
                }
                free(current_entry.wallet);
            }
            previous_overflow_bucket = current_overflow_bucket;
            current_overflow_bucket = current_overflow_bucket->next;
            free(previous_overflow_bucket);
        }
    }
    free(sys_root->senderHashTable);
    free(sys_root->receiverHashTable);
    free(sys_root->last_date);
    free(sys_root->last_time);
    free(sys_root);
    return 0;

}
