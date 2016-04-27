#include "fish.h"

void clear_cin_buffer()
{
	cin.clear();
	cin.ignore();
	fflush(stdin);	
}

fish::fish()
{
	srand((unsigned int)time(NULL));
	p.x = 0;
	p.y = 0;
	reg = 0.0;
	stack_id = 0;
	dir_flag = EAST;
	str_flag = false;
	curr_pos.resize(1);
	out_stack.reserve(1);
	prog_stack.reserve(1);
	prog_stack.resize(1);
	instr_data.reserve(57);

	instr_data = {
		{ ';',  -1 },{ '~', 401 },{ '$', 402 },{ '@', 403 },{ '}', 404 },{ '{', 405 },{ 'r', 406 },
		{ 'l', 407 },{ '[', 408 },{ ']', 409 },{ ':', 410 },{ '+', 301 },{ '-', 302 },{ '*', 303 },
		{ ',', 304 },{ '%', 305 },{ '=', 306 },{ ')', 307 },{ '(', 308 },{ '\'',309 },{ '\"',310 },
		{ '>', 131 },{ '<', 132 },{ '^', 133 },{ 'v', 134 },{ '\\',135 },{ '/', 136 },{ '|', 137 },
		{ '_', 138 },{ '#', 139 },{ 'x', 140 },{ '!', 141 },{ '?', 142 },{ '.', 143 },{ ' ', 144 },
		{ 'o', 201 },{ 'n', 202 },{ 'i', 203 },{ 'a',  10 },{ 'b',  11 },{ 'c',  12 },{ 'd',  13 },
		{ 'e',  14 },{ 'f',  15 },{ '0',   0 },{ '1',   1 },{ '2',   2 },{ '3',   3 },{ '4',   4 },
		{ '5',   5 },{ '6',   6 },{ '7',   7 },{ '8',   8 },{ '9',   9 },{ '&', 501 },{ 'g', 502 },
		{ 'p', 503 }
	};
	cout << "\tIcthyologist - A fish interpreter in C++\t";
	cout << "\n\t  (C) Copyright Anand krish April 2016";
	cout << "\n-------------------------------------------------------------\n";
}
bool fish::get_script()
{
	char c = '\0';
	start: 
	while (1)
	{		
		cout << "IchtyC : >>> ";
		cin.get(c);
		if ( c == '\n')
			continue;
		else
		{
			string s(1, c);
			cin >> file;
			file.insert(0, s);
			break;
		}		
	}
	
	if (file == "exit" || file == "quit")
		return 0;
	if (file == "load")
	{
		load_flag = true;
		cout << " Enter input(s) : ";
		cin >> file;
		clear_cin_buffer();

		for (uint16_t i = 0; i < (uint16_t)file.size(); ++i)
			in_stack.push_back((double) file[i]);
		cout << "<<Input stack updated>>\n";
		goto start;
	}
	else if (file.find(FISH_EXTN) == std::string::npos)
		file += FISH_EXTN;
	return 1;
}

void fish::reset()
{
	p.x = 0;
	p.y = 0;
	reg = 0.0;
	stack_id = 0;	
	dir_flag = EAST;
	str_flag = false;
	in_stack.clear();
	out_stack.clear();
	prog_stack.clear();	
	fish_script.clear();
	prog_stack.resize(1);
	curr_pos[stack_id] = -1;
}

/* Fish Script Parser*/
void fish::read_script()
{
	if(!load_flag)
		reset(); 

	load_flag = false;
	fish_file.open(file, ios::in);
	if (!fish_file)
	{
		cout << "\n   File could not be opened!!\n";
		error_flag = true;
		error_handler();
	}

	uint16_t i = 0, j = 0;
	int val = 0;
	while (std::getline(fish_file, file))
	{
		val = 0;
		vector<code_packet> script_line;
		code_packet word;
		while (j < file.length())
		{
			word.symbol = file[j];
			word.I.x = i;
			word.I.y = j;
			val = search_sym(file[j], false);
			if (val >= -1)
				word.idx = val;
			++j;
			script_line.push_back(word);
		}
		if (i == 0)
			fish_script.insert(fish_script.begin(), script_line);
		else
			fish_script.push_back(script_line);
		j = 0;
		++i;
	}

	error_handler();
	fish_file.close();
}

void fish::run()
{
	int val = 0;
	p.x = 0;
	p.y = 0;
	str_flag = false;
	curr_pos[stack_id] = -1;
	while (val > -1 && error_flag == false)
	{
		
		val = fish_script[p.x][p.y].idx;
		if (!str_flag)
		{
			val = search_sym(fish_script[p.x][p.y].symbol,true);
		}

		if (val < 0)
			break;
		else if (val > -1 && (val < 16 || (val > 31 && val < 127)))
		{
			prog_stack[stack_id].push_back((double)val);
			curr_pos[stack_id]++;
		}
		else if (val >= 500)
		{
			misc(val);
		}
		else if (val >= 400)
		{
			stack_manip(val);
		}
		else if (val >= 300)
		{
			operation(val);
		}
		else if (val >= 200)
		{
			data_transfer(val);
		}
		else if (val >= 100)
		{
			movement(val);
		}
		error_handler();
		incr_IP();
	}

	if (!error_flag)
	{
		cout << "\n Program has termimated\n";
		cout << "\n Program Stack \n";
		if (prog_stack[stack_id].size() != 0)
		{
			for (uint16_t k = 0;k < prog_stack[stack_id].size();++k)
				cout << " " << prog_stack[stack_id][k];
		}
		else
			cout << " <<Program Stack empty>>";
		cout << "\n";
	}

	clear_cin_buffer();

}

void fish::incr_IP()
{
	switch (dir_flag)
	{
	case EAST:
		p.y += 1;
		if (p.y >=(uint16_t)fish_script[p.x].size())
			p.y = 0;
		break;
	case WEST:
		p.y -= 1;
		if (p.y <0)
			p.y = (uint16_t)fish_script[p.x].size()-1;
		break;
	case SOUTH:
		p.x += 1;
		if (p.x >=(uint16_t) fish_script.size())
			p.x = 0;
		break;
	case NORTH:
		p.x -= 1;
		if (p.x <0)
			p.x = (uint16_t)fish_script.size()-1;
		break;
	}
}

int fish::search_sym(char sym, bool ovr)
{
	uint16_t i = 0;

	if (str_flag && (sym !='\'' && sym != '\"'))
		return (int)sym;
	else
	{
		while (i < 57)
		{
			if (instr_data[i++].symbol == sym)
			{
				if (!ovr)
				{
					if (instr_data[i - 1].idx == 309 || instr_data[i - 1].idx == 310)
					{
						str_flag = str_flag ? false : true;
					}
				}
				return instr_data[i - 1].idx;
			}
				
		}
	}

	error_flag = true;
	return -2;
}

void fish::movement(int v)
{
	switch (v)
	{
	case 131: // East Direction
		dir_flag = EAST;
		break;
	case 132: //West Direction
		dir_flag = WEST;
		break;
	case 133: // North Direction
		dir_flag = NORTH;
		break;
	case 134: //South Direction
		dir_flag = SOUTH;
		break;
	case 135: //'\' Reflect
		if (dir_flag == EAST)
			dir_flag = SOUTH;
		else if (dir_flag == WEST)
			dir_flag = NORTH;
		else if (dir_flag == NORTH)
			dir_flag = WEST;
		else if (dir_flag == SOUTH)
			dir_flag = EAST;
		break;
	case 136: // '/' Reflection
		if (dir_flag == EAST)
			dir_flag = NORTH;
		else if (dir_flag == WEST)
			dir_flag = SOUTH;
		else if (dir_flag == NORTH)
			dir_flag = EAST;
		else if (dir_flag == SOUTH)
			dir_flag = WEST;
		break;
	case 137: //'|' Reflection
		if (dir_flag == EAST)
			dir_flag = WEST;
		else if (dir_flag == WEST)
			dir_flag = EAST;
		break;
		break;
	case 138://'_' Reflection
		if (dir_flag == NORTH)
			dir_flag = SOUTH;
		else if (dir_flag == SOUTH)
			dir_flag = NORTH;
	case 139://'#' (Unconditional) Reflection
		if (dir_flag == EAST)
			dir_flag = WEST;
		else if (dir_flag == WEST)
			dir_flag = EAST;
		else if (dir_flag == NORTH)
			dir_flag = SOUTH;
		else if (dir_flag == SOUTH)
			dir_flag = NORTH;
		break;
	case 140://Random Direction
		dir_flag = rand() % 4 + 1;
		break;
	case 141://! Trampoline-skip next instruction
		incr_IP();
		break;
	case 142:// ? conditional trampoline 
		if (!prog_stack[stack_id][curr_pos[stack_id]])
			incr_IP();
		prog_stack[stack_id].pop_back();
		curr_pos[stack_id]--;
		break;
	case 143:// Jump
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			p.x = (uint16_t)num1;
			p.y = (uint16_t)num2;
		}
		else
			error_flag = true;
		break;
	case 144:// ' 'space
		break;

	}
}

void fish::misc(int v)
{
	switch (v)
	{
	case 501:
		if (1 <= prog_stack[stack_id].size())
		{
			if (reg_flag == false)
			{
				reg = prog_stack[stack_id][curr_pos[stack_id]];
				prog_stack[stack_id].pop_back();
				curr_pos[stack_id]--;
				reg_flag = true;
			}
			else
			{
				prog_stack[stack_id].push_back(reg);
				curr_pos[stack_id]++;
				reg = 0.0;
				reg_flag = false;
			}
		}
		else
			error_flag = true;
		break;
	case 502: // 'g'
		int i, j;
		i = (int) prog_stack[stack_id][curr_pos[stack_id]];
		prog_stack[stack_id].pop_back();
		curr_pos[stack_id]--;
		j = (int) prog_stack[stack_id][curr_pos[stack_id]];
		prog_stack[stack_id].pop_back();
		curr_pos[stack_id]--;
		if (i > -1 && i < (int) fish_script.size())
		{
			if (j > -1 && j < (int)fish_script[i].size())
			{
				prog_stack[stack_id].push_back((double)fish_script[i][j].symbol);
				curr_pos[stack_id]++;
			}
		}
		else
			prog_stack[stack_id].push_back(0);
		break;
	case 503:
		if (3 <= prog_stack[stack_id].size())
		{
			int x = (int)prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--; 

			int y = (int)prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;

			if (x <= (int)fish_script.size() && y <= (int)fish_script[x].size() && x >= 0 && y >= 0)
			{
				fish_script[x][y].idx = prog_stack[stack_id][curr_pos[stack_id]];
				prog_stack[stack_id].pop_back();
				curr_pos[stack_id]--;
			}
		}
		else
			error_flag = true;
		break;
	}

}

void fish::data_transfer(int v)
{
	switch (v)
	{
	case 201: // 'o' char output
		out_stack.push_back(prog_stack[stack_id][curr_pos[stack_id]]);
		prog_stack[stack_id].pop_back();
		curr_pos[stack_id]--;
		break;
	case 202: //'n' num output
		out_stack.push_back(prog_stack[stack_id][curr_pos[stack_id]]);
		prog_stack[stack_id].pop_back();
		curr_pos[stack_id]--;
		break;
	case 203: // 'i' input
		if ((uint16_t)in_stack.size() == 0)
		{
			char val;
			cout << "\n Enter Input : ";
			cin >> val;
			prog_stack[stack_id].push_back((double)val);
		}
		else
		{
			prog_stack[stack_id].push_back(in_stack.front());
			in_stack.erase(in_stack.begin());
		}
		curr_pos[stack_id]++;
		break;
	}
}
void fish::stack_manip(int v)
{
	switch (v)
	{
	case 401: //Pop the last element in the stack
		if (1 <= (uint16_t)prog_stack[stack_id].size())
		{
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;

		}
		else
			error_flag = true;
		break;
	case 402: //Swap top 2 values of the stack
		double num1;
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			num1 = prog_stack[stack_id][curr_pos[stack_id] - 1];
			prog_stack[stack_id][curr_pos[stack_id] - 1] = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id][curr_pos[stack_id]] = num1 ;
		}
		else
			error_flag = true;
		break;
	case 403: // Right shift top 3 values of the stack
		if (3 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			double num2 = prog_stack[stack_id][curr_pos[stack_id] - 1];
			prog_stack[stack_id][curr_pos[stack_id]] = num2;
			prog_stack[stack_id][curr_pos[stack_id] - 1] = prog_stack[stack_id][curr_pos[stack_id] - 2];
			prog_stack[stack_id][curr_pos[stack_id] - 2] = num1;
		}
		else
			error_flag = true;
		break;
	case 404: //Right shift the whole stack
		if (1 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			for (int i = curr_pos[stack_id]; i > 0;--i)
				prog_stack[stack_id][i] = prog_stack[stack_id][i - 1];
			prog_stack[stack_id][0] = num1;
		}
		else
			error_flag = true;
		break;
	case 405: //Left shift the whole stack
		if (1 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][0];
			for (int i = 0; i < curr_pos[stack_id];++i)
				prog_stack[stack_id][i] = prog_stack[stack_id][i + 1];
			prog_stack[stack_id][curr_pos[stack_id]] = num1;
		}
		else
			error_flag = true;
		break;
	case 406: //Reverse the stack
		int start, end;
		start = 0;
		num1=0;
		end = prog_stack[stack_id].size() - 1;
		while(start < end)
		{
			num1 = prog_stack[stack_id][start];
			prog_stack[stack_id][start]  = prog_stack[stack_id][end];
			prog_stack[stack_id][end] = num1;
			start++;
			end--;
		}
		break;
	case 407: //Push the length of the stack onto the stack
		if (0 <= (uint16_t)prog_stack[stack_id].size())
		{
			//if ((uint16_t)prog_stack[stack_id].size() == 0)
			//	prog_stack[stack_id].resize(1);

			prog_stack[stack_id].push_back(prog_stack[stack_id].size());
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 408: // '[' Pop x and Create a new stack of x elements from prog_stack 
		int val;
		num1=0;
		val = (int)prog_stack[stack_id][curr_pos[stack_id]];
		prog_stack[stack_id].pop_back();
		curr_pos[stack_id]--;

		stack_id++;
		curr_pos.resize(stack_id+1);
		prog_stack.resize(stack_id+1);
		curr_pos[stack_id] = -1;
		for (int i = 0; i < val;++i)
		{
			num1 = prog_stack[stack_id - 1][curr_pos[stack_id - 1]];
			prog_stack[stack_id - 1].pop_back();
			prog_stack[stack_id].push_back(num1);
			curr_pos[stack_id]++;
			curr_pos[stack_id - 1]--;
		}
		break;
	case 409: // ']' Delete the new stack and add its elements to the prog_stack 
		if (stack_id >= 0)
		{
			double num;
			for (int i = 0; i < (int)prog_stack[stack_id].size();++i)
			{
				num = prog_stack[stack_id][curr_pos[stack_id]];
				prog_stack[stack_id - 1].push_back(num);
				prog_stack[stack_id].pop_back();
				curr_pos[stack_id]--;
				curr_pos[stack_id - 1]++;
			}
			stack_id--;
		}
		else
			error_handler();
		break;
	case 410: // Duplicate the values of the stack
		if (1 <= (int)prog_stack[stack_id].size())
		{
			prog_stack[stack_id].push_back(prog_stack[stack_id][curr_pos[stack_id]]);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	}
}
void fish::operation(int v)
{
	switch (v)
	{
	case 301: //Pop and Add
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back(num2 + num1);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 302://Pop and Subtract
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back(num2 - num1);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 303://Pop and Multiply
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back(num2 * num1);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 304: //Pop and divide
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back(num2 / num1);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 305://Pop and Modulus
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back(fmod(num2, num1));
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 306://Equal to
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back((double)num2 == num1);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 307://Greater than
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back((double)num2 > num1);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 308://Lesser than
		if (2 <= (uint16_t)prog_stack[stack_id].size())
		{
			double num1 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			double num2 = prog_stack[stack_id][curr_pos[stack_id]];
			prog_stack[stack_id].pop_back();
			curr_pos[stack_id]--;
			prog_stack[stack_id].push_back((double)num2 < num1);
			curr_pos[stack_id]++;
		}
		else
			error_flag = true;
		break;
	case 309:// String parsing '\''

	
	case 310:// String parsing '\"'
		if (str_flag == false)
			str_flag = true;
		else
			str_flag = false;
		break;

	}
}

void fish::display()
{
	if (!error_flag)
	{
		cout << " Fish Script: \n";
		for (uint16_t i = 0;i < (uint16_t)fish_script.size();++i)
		{
			for (uint16_t j = 0;j < (uint16_t)fish_script[i].size();++j)
			{
				cout << " " << fish_script[i][j].symbol;
			}
			cout << "\n";
		}
	}	
}

void fish::error_handler()
{
	if (error_flag)
		cout << "\n  <<Something smells fishy...at ("<<p.x<<","<<p.y<<") >>  \n";
}

void fish::show_output()
{
	cout << "\n Output Stack \n";
	if (out_stack.size() > 0)
	{
		for (int i = 0; i < (int)out_stack.size(); ++i)
			if(out_stack[i] > 31 && out_stack[i] < 127)
				cout << " " <<(char)out_stack[i];
			else
				cout << " " << out_stack[i];
	}
	else
		cout << " <<Output stack empty>>";
	cout << "\n";
}