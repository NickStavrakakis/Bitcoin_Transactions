#define INPUT_MAXLINES 500
#define ID_MAXSIZE 50
#define CMD_MAXSIZE 200*10
#define INPUT_MAX 200

struct BCTroot{
    int numOfTransactions;
    int unspent;
    struct BCTnode *node;
    struct BCTroot *next;
};

struct BCTnode{
    int ammount;
    char id[50];
    struct Wallet *wallet;
    struct Transaction *transaction;
    struct BCTnode *left;
    struct BCTnode *right;
    struct BCTnode *next;
};

struct Wallet{
    char id[50];
    int balance;
    struct BCTnode *bitcoin;
    struct Wallet *next;
};

struct Transaction{
    char id[50];
    int ammount;
    char time[50];
    char date[50];
    struct Wallet *sender;
    struct Wallet *receiver;
    struct Transaction *next;
};

struct Entry{
    struct Wallet *wallet;
    struct Transaction *transaction;
};

struct Bucket{
    struct Entry *entry; //array of entries
    struct Bucket *next;
};

struct SystemRoot{
    int bitCoinValue;
    int senderHashtableNumOfEntries;
    int receiverHashtableNumOfEntries;
    int bucketPos;
    int bucketSize;
    char *last_date;
    char *last_time;
    struct Bucket *senderHashTable;
    struct Bucket *receiverHashTable;
    struct Wallet *wallet_root;
    struct BCTroot *bctree_root;
};
