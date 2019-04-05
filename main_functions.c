#include "functions.h"

void requestTransaction(struct SystemRoot** sys_root, char *transaction_id, char *sender_id, char *receiver_id, int ammount, char date[11], char time[6]){
    int canceled_tr = 0, owed_ammount = ammount;
    int i, sht_pos, rht_pos, flag, skip = 0;
    //checking the sender_id and receiver_id
    if(Wallet_Get((*sys_root)->wallet_root, sender_id)==NULL){
        printf("Transaction %s: Error. User %s is not added.\n", transaction_id, sender_id);
        return;
    }
    if(Wallet_Get((*sys_root)->wallet_root, receiver_id)==NULL){
        if(Wallet_Add(sys_root, receiver_id) == 0){
            return;
        }
    }

    struct Wallet *sender, *receiver;

    //creating the transactions that will be added into the buckets (senderHashTable + receiverHashTable)
    struct Transaction *new_s_transaction = (struct Transaction*)malloc(sizeof(struct Transaction));
    strcpy(new_s_transaction->id, transaction_id);
    new_s_transaction->sender = Wallet_Get((*sys_root)->wallet_root, sender_id);
    new_s_transaction->receiver = Wallet_Get((*sys_root)->wallet_root, receiver_id);
    new_s_transaction->ammount = ammount;
    strcpy(new_s_transaction->time, time);
    strcpy(new_s_transaction->date, date);
    new_s_transaction->next = NULL;
    //
    struct Transaction *new_r_transaction = (struct Transaction*)malloc(sizeof(struct Transaction));
    strcpy(new_r_transaction->id, transaction_id);
    new_r_transaction->sender = new_s_transaction->sender;
    new_r_transaction->receiver = new_s_transaction->receiver;
    new_r_transaction->ammount = ammount;
    strcpy(new_r_transaction->time, time);
    strcpy(new_r_transaction->date, date);
    new_r_transaction->next = NULL;

    //inserting the data into the SENDER hashTable
    sht_pos = hash(sender_id, (*sys_root)->senderHashtableNumOfEntries);

    flag = 0;
    for(i=0; i<(*sys_root)->bucketPos; i++){

        //if we reached the end of the filled part of the bucket
        if((*sys_root)->senderHashTable[sht_pos].entry[i].wallet == NULL){

            sender = Wallet_Get((*sys_root)->wallet_root, sender_id);
            if(sender->balance < ammount){ //if the sender has not enough money to send
                printf("Transaction %s: Error. User %s has not enough credits for this transaction.\n", transaction_id, sender_id);
                canceled_tr = 1;
            }
            else{
                 (*sys_root)->senderHashTable[sht_pos].entry[i].wallet = sender;
                 Transaction_Add(&(*sys_root)->senderHashTable[sht_pos].entry[i].transaction, new_s_transaction);
            }
            flag = 1;
            break;
        }
        //if we found that the sender is already in the hash table
        else if(strcmp((*sys_root)->senderHashTable[sht_pos].entry[i].wallet->id, sender_id)==0){
            sender = (*sys_root)->senderHashTable[sht_pos].entry[i].wallet;
            if(sender->balance < ammount){ //if the sender has not enough money to send
                printf("Transaction %s: Error. User %s has not enough credits for this transaction.\n", transaction_id, sender_id);
                canceled_tr = 1;
            }
            else{
                Transaction_Add(&(*sys_root)->senderHashTable[sht_pos].entry[i].transaction, new_s_transaction);
            }
            flag = 1;
            break;
        }
    }
    if(canceled_tr == 1){
        return;
    }

    if(flag==0){
        struct Bucket *newBucket = (struct Bucket*)malloc(sizeof(struct Bucket));
        while(1){
            if(flag == 0){ //if we are coming from the senderHashTable
                if((*sys_root)->senderHashTable[sht_pos].next == NULL){
                    newBucket->entry = malloc((*sys_root)->bucketSize-(sizeof(struct Bucket*)));
                    newBucket->entry->wallet = NULL;
                    newBucket->entry->transaction = NULL;
                    newBucket->next = NULL;
                    (*sys_root)->senderHashTable[sht_pos].next = newBucket;
                }
                else{
                    newBucket = (*sys_root)->senderHashTable[sht_pos].next;
                }
            }
            else{
                if(newBucket->next == NULL){
                    struct Bucket *last = newBucket;
                    newBucket = (struct Bucket*)malloc(sizeof(struct Bucket));
                    newBucket->entry = malloc((*sys_root)->bucketSize-(sizeof(struct Bucket*)));
                    newBucket->entry->wallet = NULL;
                    newBucket->entry->transaction = NULL;
                    newBucket->next = NULL;
                    last->next = newBucket;
                }
                else{
                    newBucket = newBucket->next;
                }
            }
            for(i=0; i<(*sys_root)->bucketPos; i++){
                //if we reached the end of the filled part of the current bucket
                if(newBucket->entry[i].wallet == NULL){
                    sender = Wallet_Get((*sys_root)->wallet_root, sender_id);
                    if(sender->balance < ammount){ //if the sender has not enough money to send
                        printf("Transaction %s: Error. User %s has not enough credits for this transaction.\n", transaction_id, sender_id);
                        canceled_tr = 1;
                    }
                    else{
                        newBucket->entry[i].wallet = sender;
                        Transaction_Add(&newBucket->entry[i].transaction, new_s_transaction);
                    }
                    flag = -1;
                    break;
                }
                //if we found that the sender is already in the current bucket
                else if(strcmp(newBucket->entry[i].wallet->id, sender_id)==0){
                    sender = newBucket->entry[i].wallet;
                    if(sender->balance < ammount){ //if the sender has not enough money to send
                        printf("Transaction %s: Error. User %s has not enough credits for this transaction.\n", transaction_id, sender_id);
                        canceled_tr = 1;
                    }
                    else{
                        Transaction_Add(&newBucket->entry[i].transaction, new_s_transaction);
                    }
                    flag = -1;
                    break;
                }
            }
            if(flag==-1){
                break;
            }
            else{
                flag = 1;
            }
        }
    }
    if(canceled_tr == 1){
        return;
    }
    //inserting the data into the RECEIVER hashTable
    rht_pos = hash(receiver_id, (*sys_root)->receiverHashtableNumOfEntries);
    flag = 0;
    for(i=0; i<(*sys_root)->bucketPos; i++){
        //if we reached the end of the filled part of the bucket
        if((*sys_root)->receiverHashTable[rht_pos].entry[i].wallet == NULL){
            receiver = Wallet_Get((*sys_root)->wallet_root, receiver_id);
            (*sys_root)->receiverHashTable[rht_pos].entry[i].wallet = receiver;
            Transaction_Add(&(*sys_root)->receiverHashTable[rht_pos].entry[i].transaction, new_r_transaction);
            flag = 1;
            break;
        }
        //if we found that the sender is already in the hash table
        else if(strcmp((*sys_root)->receiverHashTable[rht_pos].entry[i].wallet->id, receiver_id)==0){
            receiver = (*sys_root)->receiverHashTable[rht_pos].entry[i].wallet;
            Transaction_Add(&(*sys_root)->receiverHashTable[rht_pos].entry[i].transaction, new_r_transaction);
            flag = 1;
            break;
        }
    }

    if(canceled_tr == 1){
        return;
    }
    if(flag==0){
        struct Bucket *newBucket = (struct Bucket*)malloc(sizeof(struct Bucket));
        while(1){
            if(flag == 0){ //if we are coming from the receiverHashTable
                if((*sys_root)->receiverHashTable[rht_pos].next == NULL){
                    newBucket->entry = malloc((*sys_root)->bucketSize-(sizeof(struct Bucket*)));
                    newBucket->entry->wallet = NULL;
                    newBucket->entry->transaction = NULL;
                    newBucket->next = NULL;
                    (*sys_root)->receiverHashTable[rht_pos].next = newBucket;
                }
                else{
                    newBucket = (*sys_root)->receiverHashTable[rht_pos].next;
                }
            }
            else{
                if(newBucket->next == NULL){
                    struct Bucket *last = newBucket;
                    newBucket = (struct Bucket*)malloc(sizeof(struct Bucket));
                    newBucket->entry = malloc((*sys_root)->bucketSize-(sizeof(struct Bucket*)));
                    newBucket->entry->wallet = NULL;
                    newBucket->next = NULL;
                    last->next = newBucket;
                }
                else{
                    newBucket = newBucket->next;
                }
            }
            for(i=0; i<(*sys_root)->bucketPos; i++){
                //if we reached the end of the filled part of the current bucket
                if(newBucket->entry[i].wallet == NULL){
                    receiver = Wallet_Get((*sys_root)->wallet_root, receiver_id);
                    newBucket->entry[i].wallet = receiver;
                    Transaction_Add(&newBucket->entry[i].transaction, new_r_transaction);
                    flag = -1;
                    break;
                }
                //if we found that the sender is already in the current bucket
                else if(strcmp(newBucket->entry[i].wallet->id, receiver_id)==0){
                    receiver = newBucket->entry[i].wallet;
                    Transaction_Add(&newBucket->entry[i].transaction, new_r_transaction);
                    flag = -1;
                    break;
                }
            }
            if(flag==-1){
                break;
            }
            else{
                flag = 1;
            }
        }
    }

    struct BCTnode **current_bitcoin = NULL, **previous_bitcoin = NULL, *tmp_bitcoin = NULL;
    struct BCTroot **current_bct_root = (struct BCTroot**)malloc(sizeof(struct BCTroot));
    while(owed_ammount){
        //selecting the bitcoin of the sender thats about to be spent
        if(current_bitcoin == NULL){
            previous_bitcoin = NULL;
            current_bitcoin = &sender->bitcoin;
        }
        else{
            if(skip == 0){ //if we have not skip any bictoin
                previous_bitcoin = current_bitcoin;
                *current_bitcoin = (*current_bitcoin)->next;
            }
            else{ //else we are already in the next, so do nothing
                skip = 0;
            }
        }
        if((*current_bitcoin)->ammount > owed_ammount){
            //updating the bitcoin root data
            *current_bct_root = BCTroot_Get((*sys_root)->bctree_root, (*current_bitcoin)->id);
            (*current_bct_root)->numOfTransactions++;
            if((*current_bct_root)->node == *current_bitcoin){
                if((*current_bct_root)->unspent != 0){
                    (*current_bct_root)->unspent -= owed_ammount;
                }
            }
            //inserting the new bitcoin of the sender in the tree
            struct BCTnode *sender_newbc = (struct BCTnode*)malloc(sizeof(struct BCTnode));
            strcpy(sender_newbc->id, (*current_bitcoin)->id);
            sender_newbc->ammount = (*current_bitcoin)->ammount - owed_ammount;
            sender_newbc->wallet = sender;
            sender_newbc->transaction = new_s_transaction;
            sender_newbc->left = NULL;
            sender_newbc->right = NULL;
            sender_newbc->next = (*current_bitcoin)->next;


            //inserting the new bitcoin of the receiver in the tree
            struct BCTnode *receiver_newbc = (struct BCTnode*)malloc(sizeof(struct BCTnode));
            strcpy(receiver_newbc->id, (*current_bitcoin)->id);
            receiver_newbc->ammount = owed_ammount;
            receiver_newbc->wallet = receiver;
            receiver_newbc->transaction = new_r_transaction;
            receiver_newbc->left = NULL;
            receiver_newbc->right = NULL;
            receiver_newbc->next = NULL;
            //adding the bitcoin on his list with the new one
            tmp_bitcoin = receiver->bitcoin;
            if(receiver->bitcoin == NULL){
                receiver->bitcoin = receiver_newbc;
            }
            else{
                while(tmp_bitcoin->next != NULL){
                    tmp_bitcoin = tmp_bitcoin->next;
                }
                tmp_bitcoin->next = receiver_newbc;
            }

            //insering the two new nodes as childs in the current bitcoin
            (*current_bitcoin)->left = receiver_newbc;
            (*current_bitcoin)->right = sender_newbc;

            //replacing the old bitcoin on his list with the new one
            (*current_bitcoin) = sender_newbc;
            owed_ammount = 0;
        }
        else{
            owed_ammount = owed_ammount - (*current_bitcoin)->ammount;

            //updatint the bitcoin root data
            *current_bct_root = BCTroot_Get((*sys_root)->bctree_root, (*current_bitcoin)->id);
            (*current_bct_root)->numOfTransactions++;
            (*current_bct_root)->unspent = 0;
            if((*current_bct_root)->node == *current_bitcoin){
                (*current_bct_root)->unspent = 0;
            }
            //inserting the new current_bct_rootbitcoin of the receiver in the tree
            struct BCTnode *receiver_newbc = (struct BCTnode*)malloc(sizeof(struct BCTnode));
            strcpy(receiver_newbc->id, (*current_bitcoin)->id);
            receiver_newbc->ammount = (*current_bitcoin)->ammount;
            receiver_newbc->wallet = receiver;
            receiver_newbc->transaction = new_r_transaction;
            receiver_newbc->left = NULL;
            receiver_newbc->right = NULL;
            receiver_newbc->next = NULL;
            //adding the bitcoin on his list with the new one
            tmp_bitcoin = receiver->bitcoin;

            if(receiver->bitcoin == NULL){
                receiver->bitcoin = receiver_newbc;
            }
            else{
                while(tmp_bitcoin->next != NULL){
                    tmp_bitcoin = tmp_bitcoin->next;
                }
                tmp_bitcoin->next = receiver_newbc;
            }
            //inserting the new node as child in the current bitcoin
            (*current_bitcoin)->left = receiver_newbc;

            //deleting the current bitcoin because it was all spended in the transaction
            if(previous_bitcoin == NULL){
                sender->bitcoin = sender->bitcoin->next;
                skip = 1; //now we know we are already at the next bitcoin, so we dont have to get here in the next loop;
            }
            else{
                (*previous_bitcoin)->next = (*current_bitcoin)->next;
            }
        }
    }

    sender->balance -= ammount;
    receiver->balance += ammount;

    //updating last timestamp
    if(CompareTimestamps(date, time, (*sys_root)->last_date, (*sys_root)->last_time)){
        strcpy((*sys_root)->last_date, date);
        strcpy((*sys_root)->last_time, time);
    }
    printf("Transaction %s: Success. The user %s transfered %d credits in %s's wallet at %s %s\n", new_s_transaction->id, new_s_transaction->sender->id, new_s_transaction->ammount, new_s_transaction->receiver->id, new_s_transaction->date, new_s_transaction->time);

}

void bitCoinStatus(struct BCTroot* bctree_root, char *bitcoin_id){
    struct BCTroot *current_bct_root = (struct BCTroot*)malloc(sizeof(struct BCTroot));
    current_bct_root = BCTroot_Get(bctree_root, bitcoin_id);
    if(current_bct_root == NULL){
        printf("Error: bitCoin %s doesn't exist.\n", bitcoin_id);
        return;
    }
    else{
        printf("%s %d %d\n", bitcoin_id, current_bct_root->numOfTransactions, current_bct_root->unspent);
    }
    return;
}

void findEarnings(struct SystemRoot* sys_root, char *wallet_id, char time1[6], char date1[11], char time2[6], char date2[11]){

    int i, rht_pos, entry_pos, total_income = 0;

    rht_pos = hash(wallet_id, sys_root->receiverHashtableNumOfEntries);
    struct Transaction *current_transaction = (struct Transaction*)malloc(sizeof(struct Transaction));
    if(Wallet_Get(sys_root->wallet_root, wallet_id)==NULL){
        printf("Error: User %s is not added.\n", wallet_id);
        return;
    }

    if(sys_root->receiverHashTable[rht_pos].entry[0].wallet == NULL){
        printf("Total Income: 0\n");
        return;
    }

    for(i=0; i<sys_root->bucketPos; i++){
        if(strcmp(sys_root->receiverHashTable[rht_pos].entry[i].wallet->id, wallet_id)==0){
            entry_pos = i;
            break;
        }
    }

    current_transaction = sys_root->receiverHashTable[rht_pos].entry[entry_pos].transaction;
    while(current_transaction != NULL){
        if(time1[0]=='\0')  // if the first (or anyone) time/date parameters is empty, that means that the user gave no other inputs
            total_income += current_transaction->ammount;
        else if( (time1[0]=='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) )
                total_income += current_transaction->ammount;
        }
        else if( (date1[0]=='#') && (time1[0]!='#') ){
            if( (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                total_income += current_transaction->ammount;
        }
        else if( (time1[0]!='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) && (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                total_income += current_transaction->ammount;
        }
        else{
           printf("Error: Wrong parameters\n");
           return;
        }
        current_transaction = current_transaction->next;

    }
    printf("Total Income: %d\n", total_income);

    current_transaction = sys_root->receiverHashTable[rht_pos].entry[entry_pos].transaction;
    while(current_transaction != NULL){
        if(time1[0]=='\0')  // if the first (or anyone) time/date parameters is empty, that means that the user gave no other inputs
            printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        else if( (time1[0]=='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) )
                printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        }
        else if( (date1[0]=='#') && (time1[0]!='#') ){
            if( (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        }
        else if( (time1[0]!='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) && (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        }
        else{
           printf("Error: Wrong parameters\n");
           return;
        }
        current_transaction = current_transaction->next;
    }

    return;

}


void findPayments(struct SystemRoot* sys_root, char *wallet_id, char time1[6], char date1[11], char time2[6], char date2[11]){

    int i, sht_pos, entry_pos, total_outgoings = 0;

    sht_pos = hash(wallet_id, sys_root->senderHashtableNumOfEntries);
    struct Transaction *current_transaction = (struct Transaction*)malloc(sizeof(struct Transaction));

    if(Wallet_Get(sys_root->wallet_root, wallet_id)==NULL){
        printf("Error: User %s is not added.\n", wallet_id);
        return;
    }

    if(sys_root->senderHashTable[sht_pos].entry[0].wallet == NULL){
        printf("Total Outgoings: 0\n");
        return;
    }

    for(i=0; i<sys_root->bucketPos; i++){
        if(strcmp(sys_root->senderHashTable[sht_pos].entry[i].wallet->id, wallet_id)==0){
            entry_pos = i;
            break;
        }
    }
    current_transaction = sys_root->senderHashTable[sht_pos].entry[entry_pos].transaction;
    while(current_transaction != NULL){
        if(time1[0]=='\0')  // if the first (or anyone) time/date parameters is empty, that means that the user gave no other inputs
            total_outgoings += current_transaction->ammount;
        else if( (time1[0]=='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) )
                total_outgoings += current_transaction->ammount;
        }
        else if( (date1[0]=='#') && (time1[0]!='#') ){
            if( (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                total_outgoings += current_transaction->ammount;
        }
        else if( (time1[0]!='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) && (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                total_outgoings += current_transaction->ammount;
        }
        else{
           printf("Error: Wrong parameters\n");
           return;
        }
        current_transaction = current_transaction->next;
    }
    printf("Total Outgoings: %d\n", total_outgoings);

    current_transaction = sys_root->senderHashTable[sht_pos].entry[entry_pos].transaction;
    while(current_transaction != NULL){
        if(time1[0]=='\0')  // if the first (or anyone) time/date parameters is empty, that means that the user gave no other inputs
            printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        else if( (time1[0]=='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) )
                printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        }
        else if( (date1[0]=='#') && (time1[0]!='#') ){
            if( (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        }
        else if( (time1[0]!='#') && (date1[0]!='#') ){
            if( (CompareDates(date1, current_transaction->date)==0) && (CompareDates(date2, current_transaction->date)==1) && (CompareTimes(time1, current_transaction->time)==0) && (CompareTimes(time2, current_transaction->time)==1) )
                printf("%s %s %s %d %s %s\n", current_transaction->id, current_transaction->sender->id, current_transaction->receiver->id, current_transaction->ammount, current_transaction->date, current_transaction->time);
        }
        else{
           printf("Error: Wrong parameters\n");
           return;
        }
        current_transaction = current_transaction->next;
    }

    return;

}

void traceCoin(struct SystemRoot* sys_root, char *bitcoin_id){

    struct BCTroot *current_bct_root = (struct BCTroot*)malloc(sizeof(struct BCTroot));
    struct BCTnode *current_bitcoin = (struct BCTnode*)malloc(sizeof(struct BCTnode));
    current_bct_root = BCTroot_Get(sys_root->bctree_root, bitcoin_id);

    if(current_bct_root == NULL){
        printf("Error: bitCoin %s doesn't exist.\n", bitcoin_id);
        return;
    }

    current_bitcoin = current_bct_root->node;
    printTree(current_bitcoin->left, 1);
    printTree(current_bitcoin->right, 0);

    return;
}
