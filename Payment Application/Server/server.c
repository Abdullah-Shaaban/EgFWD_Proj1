#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Server/server.h"

//#define DEBUG

#pragma warning(disable : 4996) //Turn off deprecation

extern ST_accountsDB_t		accounts_DB[255];
       uint32_t             current_AccountDB_index;
extern ST_transaction_t	    transactions_DB[255];
       uint32_t             current_TransDB_index;

void fill_accounts_DB(ST_accountsDB_t accounts_DB[])
{
	FILE* file_ptr;
    char  tmp[20] = {'\0'};  //Set to the bigger width of PAN
	file_ptr = fopen("accounts_DB.txt", "r");
    uint8_t i = 0;
    enum {PAN,BALANCE} flag = BALANCE;
    while (fgets(tmp, 20, file_ptr) != NULL)
    {
        if (flag == BALANCE)
        {
            accounts_DB[i].balance = atoi(tmp);
            flag = PAN;
            //----
            #ifdef DEBUG
                printf("\naccounts_DB[%u].balance = %u", i, atoi(tmp));
            #endif // DEBUG
            //----
        }
        else
        {
            strncpy_s(accounts_DB[i].primaryAccountNumber, 20, tmp, 19);
            flag = BALANCE;
            //----
            #ifdef DEBUG
                printf("\naccounts_DB[%u].primaryAccountNumber = %s", i, tmp);
            #endif // DEBUG
            //----
            i++;
        }
    }
};

void fill_transactions_DB_FILE()
{
    FILE* file_ptr;
    file_ptr = fopen("accounts_DB.txt", "w");
};


EN_serverError_t isValidAccount(ST_cardData_t* cardData)
{
    enum {FOUND, NOTFOUND} flag = NOTFOUND;
    for (int i = 0; i < 255; i++) {
        if (!strcmp(cardData->primaryAccountNumber, accounts_DB[i].primaryAccountNumber))
        {
            flag = FOUND;
            current_AccountDB_index = i;
            break;
        }
    }
    if (flag == FOUND)
        return OK_S;
    else
        return ACCOUNT_NOT_FOUND;
};

/*********************    REVISIT      ****************************/
//Why -> The termData can not be the only input to this function. Either the cardData or account_index are needed to get the balance and compare
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData)
{
    if (termData->transAmount <= accounts_DB[current_AccountDB_index].balance)
        return OK_S;
    else
        return LOW_BALANCE;
};

EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
    //4. If transaction can't be saved will return SAVING_FAILED, else will return OK
    if (current_TransDB_index == 256)
        return SAVING_FAILED;
    else
    {
        //2. It gives a sequence number to a transaction, this number is incremented once a transaction is processed into the server.
        transData->transactionSequenceNumber = current_TransDB_index;
        //3. It saves any type of transaction, APPROVED or DECLINED, with the specific reason for declining/transaction state.
        //Directly copying the structs as shown below is allowed because the members are arrays, not pointers!!
        transactions_DB[current_TransDB_index].transactionSequenceNumber = transData->transactionSequenceNumber;
        transactions_DB[current_TransDB_index].transState = transData->transState;
        transactions_DB[current_TransDB_index].cardHolderData = transData->cardHolderData;
        transactions_DB[current_TransDB_index].terminalData = transData->terminalData;
        //------
        //Storing Transactions DB in a text file
        //fill_transactions_DB_FILE();
        //------
        current_TransDB_index++;
        return OK_S;
    }

};

EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t* transData)
{
    enum { FOUND, NOTFOUND } flag = NOTFOUND;
    for (int i = 0; i < 255; i++) {
        if (transactionSequenceNumber==transactions_DB[i].transactionSequenceNumber)
        {
            flag = FOUND;
            break;
        }
    }
    if (flag == FOUND)
    {
        //Directly copying the structs as shown below is allowed because the members are arrays, not pointers!!
        transData->transactionSequenceNumber = transactions_DB[transactionSequenceNumber].transactionSequenceNumber;
        transData->transState               = transactions_DB[transactionSequenceNumber].transState;
        transData->cardHolderData           = transactions_DB[transactionSequenceNumber].cardHolderData;
        transData->terminalData             = transactions_DB[transactionSequenceNumber].terminalData;
        return OK_S;
    }
    else
        return TRANSACTION_NOT_FOUND;
};

char* EN_serverERROR_to_STR(EN_serverError_t err)
{
    switch (err)
    {
    case OK_T:			        return "OK_s";
    case SAVING_FAILED:		    return "SAVING_FAILED";
    case TRANSACTION_NOT_FOUND:	return "TRANSACTION_NOT_FOUND";
    case ACCOUNT_NOT_FOUND:		return "ACCOUNT_NOT_FOUND";
    case LOW_BALANCE:	        return "LOW_BALANCE";
    }
};

char* EN_transState_to_STR(EN_transState_t state)
{
    switch (state)
    {
    case APPROVED:			        return "APPROVED";
    case DECLINED_INSUFFECIENT_FUND:return "DECLINED_INSUFFECIENT_FUND";
    case DECLINED_STOLEN_CARD:	    return "DECLINED_STOLEN_CARD";
    case INTERNAL_SERVER_ERROR:		return "INTERNAL_SERVER_ERROR";
    }
};


EN_transState_t  recieveTransactionData(ST_transaction_t* transData)
{
    //Just Initilization
    EN_serverError_t serverERROR = OK_S; 
    transData->transState = APPROVED; 
    //2. It checks the account details // and amount availability.
    serverERROR = isValidAccount(&transData->cardHolderData);
    printf("\nChecking if the account is valid...\t%s", EN_serverERROR_to_STR(serverERROR));
    if (serverERROR == OK_S)    //If the account is NOT valid, checking the amount won't be done
    {
        //2. It checks the account details // and amount availability.
        serverERROR = isAmountAvailable(&transData->terminalData);
        printf("\nChecking if the amount is available...\t%s", EN_serverERROR_to_STR(serverERROR));
        if (serverERROR != OK_S)
        {
            transData->transState = DECLINED_INSUFFECIENT_FUND;
        }
        else
        {
            //It will update the database with the new balance.
            accounts_DB[current_AccountDB_index].balance = accounts_DB[current_AccountDB_index].balance - transData->terminalData.transAmount;
            //---------
            #ifdef DEBUG
                printf("\nAfter Deduction: accounts_DB[%u].balance= %f -- Associated PAN= %s", i,accounts_DB[i].balance, accounts_DB[i].primaryAccountNumber);
            #endif // DEBUG
            //---------
        }
    }
    else
    {
        transData->transState = DECLINED_STOLEN_CARD;
    }
    serverERROR = saveTransaction(transData);
    printf("\nSaving the transaction in the server database...\t%s", EN_serverERROR_to_STR(serverERROR));
    if (serverERROR != OK_S && transData->transState==OK_S)
    {
        transData->transState = INTERNAL_SERVER_ERROR;
    }
    //return the state, which may be DECLINED_STOLEN_CARD, DECLINED_INSUFFECIENT_FUND, INTERNAL_SERVER_ERROR, or APPROVED.
    return transData->transState;
};


void server(ST_transaction_t* transData, ST_cardData_t* cardData, ST_terminalData_t* termData)
{
    transData->cardHolderData = *cardData;
    transData->terminalData = *termData;
    printf("\nServer is processing the Transaction...");
    EN_transState_t state = APPROVED;   //Just Initialization
    state = recieveTransactionData(transData);
    printf("\nProcessing Finished. The Transaction status is...\t%s", EN_transState_to_STR(state) );
    //END IS REACHED IN FLOWCHART
};

