#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "BankingApp.h"

/**************************************************************************
* This function is used to construct the clients info and load client and transaction 
* information in to the array
*
*   inputs:
* - none
*   outputs:
* - none
**************************************************************************/
void BankingApp(void)
{
	clients_t* clients = construct_clients();
	transactions_t* transactions = construct_transactions();

	load_client_db(clients);
	load_transaction_db(transactions);

	start_menu(clients, transactions);

	destruct_clients(clients);
	destruct_transactions(transactions);
}

/**************************************************************************
* This function is used to print start menu
*
*   inputs:
* - none
*   outputs:
* - none
**************************************************************************/
void print_start_menu(void)
{
	printf("\n"
	"1.    Login\n"
	"2.    Register\n"
	"3.    Exit Program\n\n"
	"Enter option(1 - 4)> \n");
}

/**************************************************************************
* This function is used to print start menu and read the option and print 
  error message
*
*   inputs:
* - none
*   outputs:
* - int
**************************************************************************/
int start_menu_read_option(void)
{
	print_start_menu();

	int option;
	if (scanf("%d", &option) != 1)
	{
		while ((getchar()) != '\n');
		printf("Input type mismatch\n");
		return start_menu_read_option();
	}

	return option;
}

/**************************************************************************
* This function is used to print start menu and read the option
*
*   inputs:
* - clients_t* clients, transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void start_menu(clients_t* clients, transactions_t* transactions)
{
	int option = start_menu_read_option();

	while (option != 3)
	{
		switch (option)
		{
			case 1:
				login_client(clients, transactions);
				break;
			case 2:
				register_client(clients, transactions);
				break;
			default:
				error();
		}

		option = start_menu_read_option();
	}
}

/**************************************************************************
* This function is used to print client menu
*
*   inputs:
* - none
*   outputs:
* - none
**************************************************************************/
void print_client_menu(void)
{
	printf("\n"
		   "1.    View Account Detail\n"
		   "2.    Transfer\n"
		   "3.    Deposit\n"
		   "4.    Withdraw\n"
		   "5.    Change Password\n"
		   "6.    Cancel Client\n"
		   "7.    Logout\n"
		   "8.    Exit Program\n\n"
		   "Enter option(1 - 8)> \n");
}

/**************************************************************************
* This function is used to print client menu and read the option
*
*   inputs:
* - none
*   outputs:
* - int
**************************************************************************/
int client_menu_read_option(void)
{
	print_client_menu();

	int option;
	if (scanf("%d", &option) != 1)
	{
		while ((getchar()) != '\n');
		printf("Input type mismatch\n");
		return client_menu_read_option();
	}

	return option;
}

/**************************************************************************
* This function is used to read the option
*
*   inputs:
* - client_t* current, clients_t* clients, transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void client_menu(client_t* current, clients_t* clients, transactions_t* transactions)
{
	int option = client_menu_read_option();

	while (option != 8)
	{
		switch (option)
		{
			case 1:
				view_account(current, transactions);
				break;
			case 2:
				transfer(current, clients, transactions);
				break;
			case 3:
				deposit(current, clients);
				break;
			case 4:
				withdraw(current, clients);
				break;
			case 5:
				change_pw(current, clients);
				break;
			case 6:
				option = cancel_client(current, clients, transactions);
				break;
			case 7:
				option = logout(current);
				break;
			default:
				error();
		}

		if (option == -6 || option == -7)
			break;

		option = client_menu_read_option();
	}
}

/**************************************************************************
* This function is used to print client's transactions
*
*   inputs:
* - client_t* client, transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void print_client_transactions(client_t* client, transactions_t* transactions)
{
	node_t* current = (transactions->transaction_list->head);
	while (current != NULL)
	{
		transaction_t* transaction = (transaction_t*)get_data(current);

		if(strcmp(transaction->sender_id, client->id) == 0 || strcmp(transaction->receiver_id, client->id) == 0)
			print_transaction(transaction);
		current = get_next(current);
	}
}

/**************************************************************************
* This function is used to view the account detail
*
*   inputs:
* - client_t* current, transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void view_account(client_t* current, transactions_t* transactions)
{
	printf("#Account Information\n");
	print_client(current);
	printf("#Transaction Details\n");
	print_client_transactions(current, transactions);
}

/**************************************************************************
* This function is used to transfer credit
*
*   inputs:
* - client_t* current, clients_t* clients, transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void transfer(client_t* current, clients_t* clients, transactions_t* transactions)
{
	printf("Transfer Money\n");

	char receiver_id[CLIENT_ID_LEN + 1];
	int receiver_check = check_receiver(current, clients, transactions, receiver_id);

	switch (receiver_check)
	{
		case 1:
			transfer_amount(current, get_client_by_id(clients, receiver_id), transactions, clients);
			break;
		case 2:
			printf("Incorrect ID format\n");
			break;
		case 3:
			printf("Receiver cannot be yourself\n");
			break;
		case 4:
			printf("Receiver ID not found\n");
			break;
		default:;
	}
}

/**************************************************************************
* This function is used to transfer credit
*
*   inputs:
* - client_t* current, clients_t* clients, transactions_t* transactions, char* receiver_id
*   outputs:
* - int
**************************************************************************/
int check_receiver(client_t* current, clients_t* clients, transactions_t* transactions, char* receiver_id)
{
	printf("Enter the receiver info\n");
	if (!read_client_id(receiver_id))
		return 2;
	
	if (strcmp(receiver_id, current->id) == 0)
		return 3;

	if (get_client_by_id(clients, receiver_id) == NULL)
		return 4;

	return 1;
}

/**************************************************************************
* This function is used to check if amount of transfer credit valid
*
*   inputs:
* - client_t* sender, client_t* receiver, transactions_t* transactions, clients_t* clients
*   outputs:
* - none
**************************************************************************/
void transfer_amount(client_t* sender, client_t* receiver, transactions_t* transactions, clients_t* clients)
{
	double amount = 0.0;
	int read_result = read_amount(&amount);

	if(!read_result)
		printf("Input type mismatch\n");
	else
	{
		int amount_result = check_amount(sender, amount);

		if(amount_result == 2)
			printf("Amount must be greater than 0\n");
		else if (amount_result == 3)
			printf("Not enough balance\n");
		else
		{
			decrease_balance(sender, amount);
			increase_balance(receiver, amount);

			srand(atoi(get_client_id(sender)));
			transaction_t transaction;

			char transaction_id[TRANSACTION_ID_LEN + 1];
			generate_transaction_id(transaction_id);

			date_time_t date_time;
			generate_date_time(&date_time);

			set_transaction(&transaction, transaction_id, get_client_id(sender), get_client_id(receiver), amount, &date_time);
			add_transaction(transactions, transaction);

			save_client_db(clients);
			save_transaction_db(transactions);
		}
	}
}

/**************************************************************************
* This function is used to geneerate a random number
*
*   inputs:
* - int* buffer, int min, int max
*   outputs:
* - none
**************************************************************************/
void generate_random_ints(int* buffer, int min, int max)
{
	*buffer = (rand() % (max - min + 1)) + min;
}

/**************************************************************************
* This function is used to geneerate a transaction id
*
*   inputs:
* - char* transaction_id
*   outputs:
* - none
**************************************************************************/
void generate_transaction_id(char* transaction_id)
{
	double max_d = pow(10, TRANSACTION_ID_LEN) - 1;
	char max_c[30];
	sprintf(max_c, "%f", max_d);
	int max = atoi(max_c);
	int min = 0;

	int random = 0;
	generate_random_ints(&random, min, max);
	sprintf(transaction_id, "%06d", random);
}

/**************************************************************************
* This function is used to generate a the random time and date
*
*   inputs:
* - date_time_t* date_time
*   outputs:
* - none
**************************************************************************/
void generate_date_time(date_time_t* date_time)
{
	generate_random_ints(&(date_time->month), MONTH_START, MONTH_END);
	generate_random_ints(&(date_time->day), DAY_START, DAY_END);
	generate_random_ints(&(date_time->hour), HOUR_START, HOUR_END);
	generate_random_ints(&(date_time->minute), MINUTE_START, MINUTE_END);
}

/**************************************************************************
* This function is used to generate a the random time and date
*
*   inputs:
* - date_time_t* date_time
*   outputs:
* - none
**************************************************************************/
int read_amount(double* amount)
{
	printf("Enter amount to transfer\n");

	if (scanf("%lf", amount) != 1)
	{
		while ((getchar()) != '\n');
		return 0;
	}

	return 1;
}

/**************************************************************************
* This function is used to check the amount
*
*   inputs:
* - client_t* sender, double amount
*   outputs:
* - int
**************************************************************************/
int check_amount(client_t* sender, double amount)
{
	if (amount <= 0)
		return 2;

	if (get_balance(sender) < amount)
		return 3;

	return 1;
}

/**************************************************************************
* This function is make a deposit and update the balance of the account 
*
*   inputs:
* - client_t* current, clients_t* clients
*   outputs:
* - none
**************************************************************************/
void deposit(client_t* current, clients_t* clients)
{
	printf("Deposit Money\n");

	double amount = 0.0;
	int read_result = read_amount(&amount);

	if (!read_result)
		printf("Input type mismatch\n");
	else
	{
		if(amount <= 0)
			printf("Amount must be greater than 0\n");
		else
		{
			increase_balance(current, amount);
			save_client_db(clients);
		}
	}
}

/**************************************************************************
* This function is to withdraw cash and update the balance of the account
*
*   inputs:
* - client_t* current, clients_t* clients
*   outputs:
* - none
**************************************************************************/
void withdraw(client_t* current, clients_t* clients)
{
	printf("Withdraw Money\n");

	double amount = 0.0;
	int read_result = read_amount(&amount);

	if (!read_result)
		printf("Input type mismatch\n");
	else
	{
		int amount_result = check_amount(current, amount);

		if (amount_result == 2)
			printf("Amount must be greater than 0\n");
		else if (amount_result == 3)
			printf("Not enough balance\n");
		else
		{
			decrease_balance(current, amount);
			save_client_db(clients);
		}
	}
}


/**************************************************************************
* This function is to change the password
*
*   inputs:
* - client_t* current, clients_t* clients
*   outputs:
* - none
**************************************************************************/
void change_pw(client_t* current, clients_t* clients)
{
	printf("Change password\n");
	
	char current_pw[MAX_CLIENT_PW_LEN + 1];

	int read_result = read_client_pw(current_pw);
	if(!read_result)
		printf("Incorrect password format\n");
	else
	{
		char pw_cipher[MAX_CLIENT_PW_CIPHER_LEN + 1];
		encrypt_pw(current_pw, pw_cipher);
		if (strcmp(get_pw_cipher(current), pw_cipher) != 0)
			printf("Incorrect password\n");
		else
		{
			change_to_new_pw(current);
			save_client_db(clients);
		}
	}
}
/**************************************************************************
* This function is to change to the new password
*
*   inputs:
* - client_t* current
*   outputs:
* - none
**************************************************************************/
void change_to_new_pw(client_t* current)
{
	printf("Password change allowed, enter new password\n");
	char new_pw[MAX_CLIENT_PW_LEN + 1];

	int read_result = read_client_pw(new_pw);
	if (!read_result)
		printf("Incorrect password format\n");
	else
	{
		char pw_cipher[MAX_CLIENT_PW_CIPHER_LEN + 1];
		encrypt_pw(new_pw, pw_cipher);
		set_pw_cipher(current, pw_cipher);
	}
}


/**************************************************************************
* This function is to cancel the current account
*
*   inputs:
* - client_t* current, clients_t* clients, transactions_t* transactions
*   outputs:
* - int
**************************************************************************/
int cancel_client(client_t* current, clients_t* clients, transactions_t* transactions)
{
	printf("Cancel client\n");

	char* client_id = get_client_id(current);
	printf("Client %s cancelled\n", client_id);

	remove_client(clients, get_client_index_by_id(clients, client_id));

	linked_list_t* indexes = construct_linked_list(NULL, sizeof(int));
	unsigned int i;
	for (i = 0; i < get_length(transactions->transaction_list); i++)
	{
		transaction_t* temp = get_transaction_by_index(transactions, i);
		if (strcmp(get_sender_id(temp), get_client_id(current)) == 0 || strcmp(get_receiver_id(temp), get_client_id(current)) == 0)
			push_back(indexes, &i);
	}

	if (get_length(indexes) > 0)
	{
		for (i = *((unsigned int*)get_by_index(indexes, 0)); i < get_length(indexes); i++)
			remove_transaction(transactions, *((unsigned int*)get_by_index(indexes, i)));
	}

	destruct_linked_list(indexes);

	save_client_db(clients);
	save_transaction_db(transactions);

	return -6;
}

/**************************************************************************
* This function is to logout
*
*   inputs:
* - client_t* current
*   outputs:
* - int
**************************************************************************/
int logout(client_t* current)
{
	printf("Client %s\n  logged out", current->id);

	return -7;
}

/**************************************************************************
* This function is to encrypt the origonal password
*
*   inputs:
* - char* pw, char* pw_cipher
*   outputs:
* - void
**************************************************************************/
void encrypt_pw(char* pw, char* pw_cipher)
{
	unsigned long pw_hash = encrypt(pw, strlen(pw), MAX_CLIENT_PW_CIPHER_LEN);
	sprintf(pw_cipher, "%08lx", pw_hash);
}

/**************************************************************************
* This function is to check the id format
*
*   inputs:
* - const char* buffer
*   outputs:
* - int
**************************************************************************/
int check_client_id_format(const char* buffer)
{
	unsigned int length = strlen(buffer);
	if (length != CLIENT_ID_LEN)
		return 0;

	unsigned int i;
	for (i = 0; i < length; i++)
	{
		if (buffer[i] < ASCII_NUM_ZERO || buffer[i] > ASCII_NUM_NINE)
			return 0;
	}

	return 1;
}

/**************************************************************************
* This function is to check if the password format is correct
*
*   inputs:
* - const char* buffer
*   outputs:
* - int
**************************************************************************/
int check_client_pw_format(const char* buffer)
{
	unsigned int length = strlen(buffer);
	if (length < MIN_CLIENT_PW_LEN || length > MAX_CLIENT_PW_LEN)
		return 0;

	int result = 0;
	/*contains numbers*/
	unsigned int i;
	for (i = 0; i < length; i++)
	{
		if (buffer[i] >= ASCII_NUM_ZERO && buffer[i] <= ASCII_NUM_NINE)
		{
			++result;
			break;
		}
	}

	/*contains upper case*/
	for (i = 0; i < length; i++)
	{
		if (buffer[i] >= ASCII_UPPER_A && buffer[i] <= ASCII_UPPER_Z)
		{
			++result;
			break;
		}
	}

	/*contains lower case*/
	for (i = 0; i < length; i++)
	{
		if (buffer[i] >= ASCII_LOWER_A && buffer[i] <= ASCII_LOWER_Z)
		{
			++result;
			break;
		}
	}

	/*contains special character*/
	for (i = 0; i < length; i++)
	{
		if (buffer[i] == ASCII_SPECIAL_1 || 
			buffer[i] == ASCII_SPECIAL_2 ||
			buffer[i] == ASCII_SPECIAL_3 ||
			buffer[i] == ASCII_SPECIAL_4 ||
			buffer[i] == ASCII_SPECIAL_5 ||
			buffer[i] == ASCII_SPECIAL_6 ||
			buffer[i] == ASCII_SPECIAL_7 ||
			buffer[i] == ASCII_SPECIAL_8 ||
			buffer[i] == ASCII_SPECIAL_9)
		{
			++result;
			break;
		}
	}

	if (result != 4)
		return 0;
	
	return 1;
}

/**************************************************************************
* This function is to read the inputed client id
*
*   inputs:
* - char* id
*   outputs:
* - int
**************************************************************************/
int read_client_id(char* id)
{
	char buffer[GENERAL_BUFFER_SIZE];
	int check = 1;

	printf("Client ID> \n");
	scanf("%s", buffer);

	if (!check_client_id_format(buffer))
		check = 0;
	else
		strcpy(id, buffer);
	while ((getchar()) != '\n');

	return check;
}
/**************************************************************************
* This function is to read the inputed password 
*
*   inputs:
* - char* pw
*   outputs:
* - int
**************************************************************************/
int read_client_pw(char* pw)
{
	char buffer[GENERAL_BUFFER_SIZE];
	int check = 1;

	printf("Client Password> \n");
	scanf("%s", buffer);

	if (!check_client_pw_format(buffer))
		check = 0;
	else
		strcpy(pw, buffer);
	while ((getchar()) != '\n');

	return check;
}

/**************************************************************************
* This function is to check if the pasword is matching the ID
*
*   inputs:
* - char* pw
*   outputs:
* - int
**************************************************************************/
int read_client_id_pw(char* id, char* pw)
{
	int result = 0;
	/*
	1. correct
	2. id no pw yes
	3. id yes pw no
	4. id no pw no
	*/

	int id_result = read_client_id(id);
	int pw_result = read_client_pw(pw);

	if (id_result && pw_result)
		result = 1;
	else if (!id_result && pw_result)
		result = 2;
	else if (id_result && !pw_result)
		result = 3;
	else if (!id_result && !pw_result)
		result = 4;

	return result;
}

/**************************************************************************
* This function is to login to the account
*
*   inputs:
* - clients_t* clients, transactions_t* transactions, char* id, char* pw
*   outputs:
* - none
**************************************************************************/
void login(clients_t* clients, transactions_t* transactions, char* id, char* pw)
{
	client_t* current_client = NULL;
	char pw_cipher[MAX_CLIENT_PW_CIPHER_LEN + 1];

	encrypt_pw(pw, pw_cipher);

	current_client = login_check(clients, id, pw_cipher);
	if (current_client == NULL)
		printf("Incorrect client ID or password\n");
	else
	{
		printf("Logged in as client %s\n", current_client->id);
		client_menu(current_client, clients, transactions);
	}
}

/**************************************************************************
* This function is to login to the account
*
*   inputs:
* - clients_t* clients, transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void login_client(clients_t* clients, transactions_t* transactions)
{
	printf("Login\n");

	char id[CLIENT_ID_LEN + 1];
	char pw[MAX_CLIENT_PW_LEN + 1];

	int read_result = read_client_id_pw(id, pw);
	switch (read_result)
	{
		case 1:
			login(clients, transactions, id, pw);
			break;
		case 2:
			printf("Incorrect ID format\n");
			break;
		case 3:
			printf("Incorrect password format\n");
			break;
		case 4:
			printf("Incorrect ID and password format\n");
			break;
	}
}

/**************************************************************************
* This function is to register an account account
*
*   inputs:
* - clients_t* clients, transactions_t* transactions, char* id, char* pw
*   outputs:
* - none
**************************************************************************/
void regist(clients_t* clients, transactions_t* transactions, char* id, char* pw)
{
	client_t* current_client = NULL;
	char pw_cipher[MAX_CLIENT_PW_CIPHER_LEN + 1];

	unsigned long pw_hash = encrypt(pw, strlen(pw), MAX_CLIENT_PW_CIPHER_LEN);
	sprintf(pw_cipher, "%08lx", pw_hash);

	if (!register_check(clients, id))
		printf("Client ID already exists\n");
	else
	{
		printf("New client registered %s\n", id);
		client_t temp;
		set_client(&temp, id, pw_cipher, 0.0);
		add_client(clients, temp);
		current_client = get_client_by_index(clients, get_length(clients->client_list) - 1);
		save_client_db(clients);
		
		client_menu(current_client, clients, transactions);
	}
}

/**************************************************************************
* This function is to register an account account
*
*   inputs:
* - clients_t* clients, transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void register_client(clients_t* clients, transactions_t* transactions)
{
	printf("Register\n");
	char id[CLIENT_ID_LEN + 1];
	char pw[MAX_CLIENT_PW_LEN + 1];

	int read_result = read_client_id_pw(id, pw);
	switch (read_result)
	{
		case 1:
			regist(clients, transactions, id, pw);
			break;
		case 2:
			printf("Incorrect ID format\n");
			break;
		case 3:
			printf("Incorrect password format\n");
			break;
		case 4:
			printf("Incorrect ID and password format\n");
			break;
	}
}

/**************************************************************************
* This function is to save info to database
*
*   inputs:
* - clients_t* clients
*   outputs:
* - none
**************************************************************************/
void save_client_db(clients_t* clients)
{
	FILE* fp;
	fp = fopen(CLIENT_DB, "w+");

	fprintf(fp, "Client ID      PW_Cipher      Balance\n");
	unsigned int i;
	for (i = 0; i < get_length(clients->client_list); i++)
	{
		client_t* current = get_client_by_index(clients, i);
		fprintf(fp, "%s   %s   %f\n", current->id, current->pw_cipher, current->balance);
	}
	fclose(fp);
}

/**************************************************************************
* This function is to save info to database
*
*   inputs:
* - FILE* fp, client_t* temp
*   outputs:
* - int
**************************************************************************/
int load_client(FILE* fp, client_t* temp)
{
	int result = fscanf(fp, "%s   %s   %lf\n", temp->id, temp->pw_cipher, &(temp->balance));

	return result;
}
/**************************************************************************
* This function is to load client database to the array
*
*   inputs:
* - clients_t* clients
*   outputs:
* - none
**************************************************************************/
void load_client_db(clients_t* clients)
{
	FILE* fp;
	fp = fopen(CLIENT_DB, "r");
	if (fp == NULL)
	{
		printf("Read error\n");
		return;
	}

	destruct_clients(clients);
	clients = construct_clients();

	fscanf(fp, "%*[^\n]");

	client_t temp;

	while (load_client(fp, &temp) == CLIENT_FIELD_COUNT)
		add_client(clients, temp);

	fclose(fp);
}

/**************************************************************************
* This function is to save transaction to database
*
*   inputs:
* - transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void save_transaction_db(transactions_t* transactions)
{
	FILE* fp;
	fp = fopen(TRANSACTION_DB, "w+");

	fprintf(fp, "Transaction ID      Sender ID      Receiver ID      Amount      Date(Month-Day Hour:Minute)\n");
	unsigned int i;
	for (i = 0; i < get_length(transactions->transaction_list); i++)
	{
		transaction_t* current = get_transaction_by_index(transactions, i);
		fprintf(fp, "%s   %s   %s   %f   %d   %d   %d   %d\n", current->transaction_id, current->sender_id, current->receiver_id, current->amount, current->date_time->month, current->date_time->day, current->date_time->hour, current->date_time->minute);
	}
	fclose(fp);
}

/**************************************************************************
* This function is to save load transaction from database
*
*   inputs:
* - FILE* fp, transaction_t* temp
*   outputs:
* - none
**************************************************************************/
int load_transaction(FILE* fp, transaction_t* temp)
{
	int result = fscanf(fp, "%s   %s   %s   %lf   %d   %d   %d   %d\n", temp->transaction_id, temp->sender_id, temp->receiver_id, &(temp->amount), &(temp->date_time->month), &(temp->date_time->day), &(temp->date_time->hour), &(temp->date_time->minute));

	return result;
}

/**************************************************************************
* This function is to save load transaction from database
*
*   inputs:
* - transactions_t* transactions
*   outputs:
* - none
**************************************************************************/
void load_transaction_db(transactions_t* transactions)
{
	FILE* fp;
	fp = fopen(TRANSACTION_DB, "r");
	if (fp == NULL)
	{
		printf("Read error\n");
		return;
	}

	destruct_transactions(transactions);
	transactions = construct_transactions();

	fscanf(fp, "%*[^\n]");

	transaction_t temp;

	while (load_transaction(fp, &temp) == TRANSACTION_FIELD_COUNT)
		add_transaction(transactions, temp);

	fclose(fp);
}

/**************************************************************************
* This function is to print error message
*
*   inputs:
* - none
*   outputs:
* - none
**************************************************************************/
void error(void)
{
	printf("Option not found\n");
}