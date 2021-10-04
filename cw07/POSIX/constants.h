#ifndef CONSTANTS_H
#define CONSTANTS_H

#define OVEN_SIZE 5
#define TABLE_SIZE 5

#define DELIVERERS 7
#define CHEFS 7

#define SHM_KEY_ID 0
#define SEM_KEY_ID 1


struct mem {
	int pizzas_in_oven;
	int pizzas_on_table;
	int oven[OVEN_SIZE];
	int table[TABLE_SIZE];
};


#endif