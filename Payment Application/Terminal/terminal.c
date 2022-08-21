#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Terminal/terminal.h"
#include "../Card/card.h"

//---------
#define MAX_AMOUNT 50000
//#define DEBUG
//---------

char* EN_teminalERROR_to_STR(EN_terminalError_t err)
{
	switch (err)
	{
		case OK_T:			return "OK_T"; 
		case WRONG_DATE:		return "WRONG_DATE";
		case EXPIRED_CARD:		return "EXPIRED_CARD";
		case INVALID_CARD:		return "INVALID_CARD";
		case INVALID_AMOUNT:	return "INVALID_AMOUNT";
		case EXCEED_MAX_AMOUNT:	return "EXCEED_MAX_AMOUNT";
		case INVALID_MAX_AMOUNT:return "INVALID_MAX_AMOUNT";
	}
}


EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	//Initially, put all name entries as NULL
	for (int i = 0; i < sizeof(termData->transactionDate) / sizeof(uint8_t); i++)
	{
		termData->transactionDate[i] = '\0';
	}
	//Get input from user
	printf("\nPLease, enter the transaction date: ");
	fgets((char*)termData->transactionDate, 11, stdin);
	//Checking if the user input is correct: If the transaction date is NULL, less than 10 characters or wrong format will return WRONG_DATE error
	int char_count = 0;
	EN_InputFormat format = CORRECT_FORMAT;
	for (int i = 0; i < sizeof(termData->transactionDate) / sizeof(uint8_t); i++)
	{
		if (termData->transactionDate[i] != '\0' && termData->transactionDate[i] != '\n')
		{
			char_count++;
			//Check if input is not an ASCII number from 0 to 9
			if ( !(i==2 || i==5) && (termData->transactionDate[i] < 48 || termData->transactionDate[i] > 57))
				format = WRONG_FORMAT;
			else if ( (i==2 || i==5) && termData->transactionDate[i] != '/')
				format = WRONG_FORMAT;
		}
	}
	if (char_count == 10 && format == CORRECT_FORMAT)
	{
		return OK_T;
		//**************
		//Flushing the input stream because it causes the next fgets to read only '\n' for some reason!!
		flush_input_stream();
		//**************
	}
	else
	{
		return WRONG_DATE;
	}
};


EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
	char card_month[3] = {'\0'};
	char card_year[3] = { '\0' };
	char transact_month[3] = { '\0' };
	char transact_year[3] = { '\0' };
	char* tmp;
	strncpy_s(card_month, 3, cardData.cardExpirationDate, 2);
	tmp = &cardData.cardExpirationDate[3];
	strncpy_s(card_year, 3, tmp, 2);
	//---------
	tmp = &termData.transactionDate[3];
	strncpy_s(transact_month, 3, tmp, 2);
	tmp = &termData.transactionDate[8];
	strncpy_s(transact_year, 3, tmp, 2);
	//---------
#ifdef DEBUG
	printf("\ncard_year=%s  -- transact_year= %s -- card_month= %s -- transact_month= %s ", card_year, transact_year, card_month, transact_month );
	printf("\natoi(card_year)=%d  -- atoi(transact_year)= %d -- atoi(card_month)= %d -- atoi(transact_month)= %d ", atoi(card_year),atoi(transact_year),atoi(card_month),atoi(transact_month) );
#endif // DEBUG
	//---------
	if ( atoi(card_year) > atoi(transact_year) || ( atoi(card_year) == atoi(transact_year) && atoi(card_month) > atoi(transact_month) ) )
		return OK_T;
	else
		return EXPIRED_CARD;
};


EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	printf("\nPLease, enter the transaction amount: ");
	scanf_s("%f" , &termData->transAmount);
	if (termData->transAmount<=0.0)
		return INVALID_AMOUNT; 
	else
		return OK_T;
};


/*********************    REVISIT      ****************************/
//Why -> the user should not say the max amount, this is set by the "bank". 
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{
	if (MAX_AMOUNT <= 0.0)
		return INVALID_MAX_AMOUNT;
	else
	{
		termData->maxTransAmount = MAX_AMOUNT;
		return OK_T;
	}
};


EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData) {
	if (termData->transAmount > termData->maxTransAmount)
		return EXCEED_MAX_AMOUNT;
	else
		return OK_T;
};


/*********************    REVISIT      ****************************/
//Why -> no functions description
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{

};


void terminal(ST_terminalData_t* termData , EN_terminalError_t* terminalError, ST_cardData_t* cardData)
{
	*terminalError = getTransactionDate(termData);
	printf("\nChecking the entered Transaction Date Format...\t%s", EN_teminalERROR_to_STR(*terminalError));
	//----
	*terminalError = isCardExpired(*cardData, *termData);
	printf("\nChecking if the card is expired...\t%s", EN_teminalERROR_to_STR(*terminalError));
	if (*terminalError != OK_T) { return; };
	//----
	*terminalError = getTransactionAmount(termData);
	printf("\nChecking the entered Transaction Amount Format...\t%s", EN_teminalERROR_to_STR(*terminalError));
	//----
	*terminalError = setMaxAmount(termData);
	printf("\nThe Max Transaction Amount is %u...\t%s", MAX_AMOUNT, EN_teminalERROR_to_STR(*terminalError));
	//----
	*terminalError = isBelowMaxAmount(termData);
	printf("\nChecking if the entered amount is Below Max Amount...\t%s", EN_teminalERROR_to_STR(*terminalError));
};