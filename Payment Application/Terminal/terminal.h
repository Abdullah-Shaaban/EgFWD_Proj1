#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include "../Card/card.h"

typedef struct ST_terminalData_t
{
	float transAmount;
	float maxTransAmount;
	uint8_t transactionDate[11]; //2. Transaction date is 10 characters string in the format DD/MM/YYYY, e.g 25/06/2022.
}ST_terminalData_t;

typedef enum EN_terminalError_t
{
	//Can't use "OK" again because of a redefinition error!!!
	OK_T, WRONG_DATE, EXPIRED_CARD, INVALID_CARD, INVALID_AMOUNT, EXCEED_MAX_AMOUNT, INVALID_MAX_AMOUNT
}EN_terminalError_t;

EN_terminalError_t getTransactionDate (ST_terminalData_t* termData);
EN_terminalError_t isCardExpired (ST_cardData_t cardData , ST_terminalData_t termData);
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData);
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData);
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData);
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData);

char* EN_teminalERROR_to_STR(EN_terminalError_t err);

void terminal(ST_terminalData_t* termData, EN_terminalError_t* terminalError, ST_cardData_t* cardData);

#endif