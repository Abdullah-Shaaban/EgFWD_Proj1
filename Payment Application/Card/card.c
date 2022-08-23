#include <stdio.h>
#include "card.h"

void flush_input_stream()
{
	//fflush(stdin);
	char c;
	while ((c = getchar()) != '\n' && c != EOF) {}
};

char* EN_cardERROR_to_STR(EN_cardError_t err)
{
	switch (err)
	{
		case OK:				return "OK";
		case WRONG_NAME:		return "WRONG_NAME";
		case WRONG_EXP_DATE:	return "WRONG_EXP_DATE";
		case WRONG_PAN:			return "WRONG_PAN";
	}
}

EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	//Initially, put all name entries as NULL
	for (int i = 0; i < sizeof(cardData->cardHolderName) / sizeof(uint8_t); i++)
	{
		cardData->cardHolderName[i] = '\0';			
	}
	//Get input from user
	printf("\nPLease, enter cardholder's name: ");
	fgets((char*)cardData->cardHolderName, 25, stdin);
	//Checking if the input Format is correct
	int char_count = 0;
	for (int i = 0; i < sizeof(cardData->cardHolderName) / sizeof(uint8_t); i++)
	{
		if (cardData->cardHolderName[i] != '\0' && cardData->cardHolderName[i] != '\n')
			char_count++;
	}
	if (char_count>=20 && char_count<=24) 
	{
		if (char_count == 24)
			flush_input_stream();
		return OK;
	}
	else 
	{
		return WRONG_NAME;
	}
};


EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	//Initially, put all name entries as NULL
	for (int i = 0; i < sizeof(cardData->cardExpirationDate) / sizeof(uint8_t); i++)
	{
		cardData->cardExpirationDate[i] = '\0';
	}
	//Get input from user
	printf("\nPLease, enter the card expiry date: ");
	fgets((char*)cardData->cardExpirationDate, 6, stdin);
	//Checking if the input Format is correct
	int char_count	= 0;
	EN_InputFormat format = CORRECT_FORMAT;
	for (int i = 0; i < sizeof(cardData->cardExpirationDate) / sizeof(uint8_t); i++)
	{
		if (cardData->cardExpirationDate[i] != '\0' && cardData->cardExpirationDate[i] != '\n')
		{
			char_count++;
			//Check if input is not an ASCII number from 0 to 9
			if (i != 2 && (cardData->cardExpirationDate[i] < 48 || cardData->cardExpirationDate[i] > 57))
				format = WRONG_FORMAT;
			else if (i == 2 && cardData->cardExpirationDate[i] != '/')
				format = WRONG_FORMAT;
		}
	}
	if (char_count == 5 && format==CORRECT_FORMAT)
	{
		//**************
		//Flushing the input stream because it causes the next fgets to read only '\n' for some reason!!
		flush_input_stream();
		//**************
		return OK;
	}
	else
	{
		return WRONG_EXP_DATE;
	}
};


EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	//Initially, put all name entries as NULL
	for (int i = 0; i < sizeof(cardData->primaryAccountNumber) / sizeof(uint8_t); i++)
	{
		cardData->primaryAccountNumber[i] = '\0';
	}
	//Get input from user
	printf("\nPLease, enter the card's Primary Account Number: ");
	fgets((char*)cardData->primaryAccountNumber, 20, stdin);
	//Checking if the user input is correct
	int char_count = 0;
	EN_InputFormat format = CORRECT_FORMAT;
	for (int i = 0; i < sizeof(cardData->primaryAccountNumber) / sizeof(uint8_t); i++)
	{
		if (cardData->primaryAccountNumber[i] != '\0' && cardData->primaryAccountNumber[i] != '\n')
		{
			char_count++;
			//Allowing Alphanumerics only
			if ( !(cardData->primaryAccountNumber[i] > 48 || cardData->primaryAccountNumber[i] < 57) &&
				 !(cardData->primaryAccountNumber[i] > 65 || cardData->primaryAccountNumber[i] < 90) &&
				 !(cardData->primaryAccountNumber[i] > 97 || cardData->primaryAccountNumber[i] < 122))
			{
				format = WRONG_FORMAT;
			}
		}
	}
	if (char_count >= 16 && char_count <= 19 && format==CORRECT_FORMAT)
	{
		if (char_count == 19)
			flush_input_stream();
		return OK;
	}
	else
	{
		return WRONG_PAN;
	}
};

void card(ST_cardData_t* cardData , EN_cardError_t* cardError)
{
	//----
	*cardError = getCardHolderName(cardData);
	printf("\nChecking the entered name Format...\t%s", EN_cardERROR_to_STR(*cardError));
	//----
	*cardError = getCardPAN(cardData);
	printf("\nChecking the entered Primary Account Number Format...\t%s", EN_cardERROR_to_STR(*cardError));
	//----
	*cardError = getCardExpiryDate(cardData);
	printf("\nChecking the entered Card Expiry Date Format...\t%s", EN_cardERROR_to_STR(*cardError));
	//----
};
