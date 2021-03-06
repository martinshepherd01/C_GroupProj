#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "clients.h"
#include "transactions.h"
#include "encrypt.h"
#include "BankingApp.h"

void BankingApp(void)
{
	clients_t* clients = construct_clients();
	load_client_db(clients);

	start_menu(clients);
}


void print_start_menu(void)
{
	printf("\n"
	"1.    Login\n"
	"2.    Register\n"
	"3.    Exit Program\n\n"
	"Enter option(1 - 4)> \n");
}

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

void start_menu(clients_t* clients)
{
	int option = start_menu_read_option();

	while (option != 3)
	{
		switch (option)
		{
			case 1:
				login_client(clients);
				break;
			case 2:
				register_client(clients);
				break;
			default:
				error();
		}

		option = start_menu_read_option();
	}
}


void print_client_menu(void)
{
	printf("\n"
		   "1.    View Account Detail\n"
		   "2.    Transfer\n"
		   "3.    Deposit\n"
		   "4.    Withdraw\n"
		   "5.    Change Password\n"
		   "6.    Delete Account\n"
		   "7.    Exit Program\n\n"
		   "Enter option(1 - 7)> \n");
}

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

void client_menu(client_t* current, clients_t* clients, transactions_t* transactions)
{
	int option = client_menu_read_option();

	while (option != 7)
	{
		switch (option)
		{
			case 1:
				view_account(current, transactions);
				break;
			case 2:
				//register_client(clients);
				break;
			case 3:
				//register_client(clients);
				break;
			case 4:
				//register_client(clients);
				break;
			case 5:
				//register_client(clients);
				break;
			case 6:
				//register_client(clients);
				break;
			case 7:
				exit_program();
				break;
			default:
				error();
		}

		option = client_menu_read_option();
	}
}


void print_client_transactions(client_t* client, transactions_t* transactions)
{
	node_t* current = (transactions->transaction_list->head);
	while (current != NULL)
	{
		transaction_t* transaction = (transaction_t*)get_data(current);

		if(strcmp(transaction->sender_id, client->id) || strcmp(transaction->receiver_id, client->id))
			print_transaction(transaction);
		current = get_next(current);
	}
}

void view_account(client_t* current, transactions_t* transactions)
{
	printf("#Account Information\n");
	print_client(current);
	printf("#Transaction Details\n");
	print_client_transactions(current, transactions);
}


void transfer(client_t* current, clients_t* clients, transactions_t* transactions)
{
	printf("Transfer Money\n");

	char receiver_id[CLIENT_ID_LEN + 1];
	int receiver_check = check_receiver(current, clients, transactions, receiver_id);

	switch (receiver_check)
	{
		case '1':
			transfer_amount(current, get_client_by_id(clients, receiver_id), transactions);
			break;
		case '2':
			printf("Incorrect ID format\n");
			break;
		case '3':
			printf("Receiver cannot be yourself\n");
			break;
		case '4':
			printf("Receiver ID not found\n");
			break;
		default:;
	}
}

int check_receiver(client_t* current, clients_t* clients, transactions_t* transactions, char* receiver_id)
{
	printf("Enter the receiver info\n");
	if (!read_client_id(receiver_id))
		return 2;
	
	if (strcmp(receiver_id, current->id))
		return 3;

	if (get_client_by_id(clients, receiver_id) == NULL)
		return 4;

	return 1;
}

void transfer_amount(client_t* sender, client_t* receiver, transactions_t* transactions)
{
	double amount = 0.0;
	int read_result = read_amount(&amount);

	if(!read_result)
		printf("Input type mismatch\n");
	else
	{
		if (!check_amount(sender, amount))
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
		}
	}
}

void generate_random_ints(int* buffer, int min, int max)
{
	*buffer = (rand() % (max - min + 1)) + min;
}

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

void generate_date_time(date_time_t* date_time)
{
	generate_random_ints(&(date_time->month), MONTH_START, MONTH_END);
	generate_random_ints(&(date_time->day), DAY_START, DAY_END);
	generate_random_ints(&(date_time->hour), HOUR_START, HOUR_END);
	generate_random_ints(&(date_time->minute), MINUTE_START, MINUTE_END);
}

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

int check_amount(client_t* sender, double amount)
{
	if (get_balance(sender) < amount)
		return 0;

	return 1;
}


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
		if (buffer[i] > ASCII_NUM_ZERO && buffer[i] < ASCII_NUM_NINE)
		{
			++result;
			break;
		}
	}

	/*contains upper case*/
	for (i = 0; i < length; i++)
	{
		if (buffer[i] > ASCII_UPPER_A && buffer[i] < ASCII_UPPER_Z)
		{
			++result;
			break;
		}
	}

	/*contains lower case*/
	for (i = 0; i < length; i++)
	{
		if (buffer[i] > ASCII_LOWER_A && buffer[i] < ASCII_LOWER_Z)
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

int read_client_id(char* id)
{
	char buffer[GENERAL_BUFFER_SIZE];
	int check = 1;

	printf("Client ID> \n");
	scanf("%[^\n]", buffer);

	if (!check_client_id_format(buffer))
		check = 0;
	else
		strcpy(id, buffer);
	while ((getchar()) != '\n');

	return check;
}

int read_client_pw(char* pw)
{
	char buffer[GENERAL_BUFFER_SIZE];
	int check = 1;

	printf("Client Password> \n");
	scanf("%[^\n]", buffer);

	if (!check_client_pw_format(buffer))
		check = 0;
	else
		strcpy(pw, buffer);
	while ((getchar()) != '\n');

	return check;
}

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


void login(clients_t* clients, char* id, char* pw)
{
	client_t* current_client = NULL;
	char pw_cipher[MAX_CLIENT_PW_CIPHER_LEN + 1];

	unsigned long pw_hash = encrypt(pw, strlen(pw), MAX_CLIENT_PW_CIPHER_LEN);
	sprintf(pw_cipher, "%08lx", pw_hash);

	current_client = login_check(clients, id, pw_cipher);
	if (current_client == NULL)
		printf("Incorrect client ID or password\n");
	else
	{
		printf("Logged in as client %s\n", current_client->id);
		transactions_t* transactions = construct_transactions();
		client_menu(current_client, clients, transactions);
	}
}

void login_client(clients_t* clients)
{
	printf("Login\n");

	char id[CLIENT_ID_LEN + 1];
	char pw[MAX_CLIENT_PW_LEN + 1];

	int read_result = read_client_id_pw(id, pw);
	switch (read_result)
	{
		case '1':
			login(clients, id, pw);
			break;
		case '2':
			printf("Incorrect ID format\n");
			break;
		case '3':
			printf("Incorrect password format\n");
			break;
		case '4':
			printf("Incorrect ID and password format\n");
			break;
		default:;
	}
}


void regist(clients_t* clients, char* id, char* pw)
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
		
		transactions_t* transactions = construct_transactions();
		client_menu(current_client, clients, transactions);
	}
}

void register_client(clients_t* clients)
{
	printf("Register\n");
	char id[CLIENT_ID_LEN + 1];
	char pw[MAX_CLIENT_PW_LEN + 1];

	int read_result = read_client_id_pw(id, pw);
	switch (read_result)
	{
		case '1':
			regist(clients, id, pw);
			break;
		case '2':
			printf("Incorrect ID format\n");
			break;
		case '3':
			printf("Incorrect password format\n");
			break;
		case '4':
			printf("Incorrect ID and password format\n");
			break;
		default:;
	}

}


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

int load_client(FILE* fp, client_t* temp)
{
	int result = fscanf(fp, "%s   %s   %lf\n", temp->id, temp->pw_cipher, &(temp->balance));

	return result;
}

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


void exit_program(void)
{
	printf("Program ends\n");
}

void error(void)
{
	printf("Option not found\n");
}