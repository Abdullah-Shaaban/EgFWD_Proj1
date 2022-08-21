#include <stdlib.h>
#include <stdio.h>
#include "../Card/card.h"
#include "../Terminal/terminal.h"
#include "../Server/server.h"
#include "app.h"

//------
//#define DEBUG
#pragma warning(disable : 4996) //Turn off deprecation
//------

//Global Arrays for accounts DB & Transactions DB
ST_accountsDB_t		accounts_DB[255];
ST_transaction_t	transactions_DB[255];	//Global -> should be initialized to 0 on its own

void appStart(void)
{
	//-------
	#ifdef DEBUG
		printf("\ntransactions_DB[0].cardHolderData.cardExpirationDate = %u", transactions_DB[0].cardHolderData.cardExpirationDate[5]);
	#endif // DEBUG
	//-------
	ST_cardData_t* cardData		= NULL;
	EN_cardError_t cardError	= OK;
	cardData = malloc(1 * sizeof(ST_cardData_t));
	card(cardData, &cardError);
	if (cardError != OK) {	return;	};
	//-------
	ST_terminalData_t* termData			= NULL;
	EN_terminalError_t terminalError	= OK_T;
	termData = malloc(1 * sizeof(ST_terminalData_t));
	terminal(termData, &terminalError, cardData);
	if (terminalError != OK_T) {  return; };
	//-------
	ST_transaction_t* transData		= NULL;
	transData = malloc(1 * sizeof(ST_transaction_t));
	server(transData, cardData, termData);
};

int main()
{
	enum { FINISH, CONTINUE,  } flag = CONTINUE;
	//Getting accounts data from a file and putting it in an array to handle transactions more easily
	fill_accounts_DB(accounts_DB);
	//-------
	do
	{
		appStart();
		printf("\nPress 1 to make another transaction, OR Press 0 to exit...\t");
		scanf("%u", &flag);
		//**************
		//Flushing the input stream because it causes the next fgets to read only '\n' for some reason!!
		flush_input_stream();
		//**************
	} while (flag != FINISH);
	return 0;
}