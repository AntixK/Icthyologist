#ifndef type_H_
#define type_H_

#define EAST  4
#define WEST  3
#define NORTH 1
#define SOUTH 2
#define FISH_EXTN ".fish"

using namespace std;

typedef struct
{
	int16_t x;
	int16_t y;
}IP;

typedef struct
{
	char symbol;
	int idx;
}indx_packet;

typedef struct
{
	char symbol;
	int idx;
	IP I;
}code_packet;

typedef vector<double> vect;
typedef vector < vect> vect_2d; 
typedef vector<uint16_t> vect_int;
typedef vector<indx_packet> streak;
typedef vector<vector<code_packet> > segment;

#endif

/** End of Header File -----------------------------------------------**/