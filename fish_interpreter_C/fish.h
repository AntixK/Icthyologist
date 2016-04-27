#ifndef fish_H_
#define fish_H_

#include<string>
#include<vector>
#include<time.h>
#include"type.h"
#include<fstream>
#include<iostream>
#include<stdlib.h>


class fish
{

private:
	IP p;	
	double reg;
	string file;
	bool str_flag;
	vect in_stack;
	bool reg_flag;
	vect out_stack;
	bool load_flag;
	bool error_flag;
	uint8_t dir_flag;
	uint16_t stack_id;
	vect_int curr_pos;
	streak instr_data;
	vect_2d prog_stack;
	ifstream fish_file;
	segment fish_script;

public:
	fish();
	void run();	
	void reset();
	void incr_IP();	
	void display();
	void misc(int v);
	bool get_script();
	void show_output();
	void read_script();
	void error_handler();
	void movement(int v);
	void operation(int v);
	void stack_manip(int v);
	void data_transfer(int v);	
	int search_sym(char sym, bool ovr);
	
};

#endif

/** End of Header File -----------------------------------------------**/