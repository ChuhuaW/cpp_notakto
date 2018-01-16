#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <bitset>

typedef uint64_t state_type;
using namespace std;
int print_log_g=0;

//uint BOARD_SIZE;
uint BOARD_ROWS, BOARD_COLS;
state_type WIN_MASK_HORIZ, WIN_MASK_VERT, WIN_MASK_DIAG2, WIN_MASK_DIAG1;
bool p1(state_type, int);
bool disable_diag=false;

// print a state in a more readable way
//
string state_to_string(state_type state)
{
  string r;
    string s= bitset<64>(state).to_string('.','X').substr(64-BOARD_COLS*BOARD_ROWS);
    for(uint i=0; i<BOARD_ROWS; i++)
      r += s.substr(i * BOARD_COLS, BOARD_COLS) + "\n";

    return r;
}

// see if a row, column, or diagonal is completed
//
bool check_win(state_type state)
{
    for(uint i=0; i<BOARD_ROWS; i++)
      if((state | (WIN_MASK_HORIZ << (i * BOARD_COLS))) == state)
	return true;

    for(uint i=0; i<BOARD_COLS; i++)
      if((state | (WIN_MASK_VERT << i)) == state)
	return true; 

    if(!disable_diag)
      {
	for(uint i=0; i<BOARD_COLS-BOARD_ROWS+1; i++)
	  if((state | (WIN_MASK_DIAG1 << i)) == state || (state | (WIN_MASK_DIAG2 << i)) == state)
	    return true;
	//	  }
	  //    return !disable_diag && (((state &  WIN_MASK_DIAG2) == WIN_MASK_DIAG2) || ((state & WIN_MASK_DIAG1) == WIN_MASK_DIAG1));
      }
    
    return false;
}

// simple bit cache -- see if we already know value for this state, if so return it,
//  if not compute and return it
//
char *computed_hash, *value_hash;
void cache_init(state_type count)
{
    computed_hash = (char *) memset(new char[count/8+1], 0, count/8+1);
    value_hash = (char *) memset(new char[count/8+1], 0, count/8+1);
}

inline uint8_t ht_bitpattern(state_type state)  { return uint8_t(1) << (uint8_t(state) & uint8_t(7)); }
inline state_type ht_index(state_type state) { return state >> state_type(3); }

inline bool cache_get_val(state_type state, int arg2)
{
    state_type index = ht_index(state);
    uint8_t bitpat = ht_bitpattern(state);

    if(computed_hash[ index ] & bitpat)
        return value_hash[ index ] & bitpat;

    bool val;
    if( (val = p1(state, arg2)) )
        value_hash[ index ] |= bitpat;
    computed_hash[ index ] |= bitpat;

    return val;
}

// return true iff at least *1* of my possible moves from this state results in a win
//   (or if the other player has already lost)
bool p1(state_type state, int depth=0)
{
  //  cout << state_to_string(state) << " " << check_win(state) << " " << depth << endl;
    if(check_win(state))
        return true;

    state_type new_state;
    for(state_type i=0; i<BOARD_ROWS*BOARD_COLS; i++)
      if(( new_state = (state | (state_type(1) << i)) ) != state && !cache_get_val(new_state, depth+1))
	  return true;

    return false;
}

void init_check_win()
{
  WIN_MASK_HORIZ = WIN_MASK_VERT = WIN_MASK_DIAG2 = WIN_MASK_DIAG1 = 0;
  for(uint i=0; i < BOARD_COLS; i++)
    WIN_MASK_HORIZ = ((WIN_MASK_HORIZ << 1) | 1);
  for(uint i=0; i < BOARD_ROWS; i++)
    WIN_MASK_VERT = ((WIN_MASK_VERT << BOARD_COLS) | 1);
  
  for(uint i=0; i < BOARD_ROWS; i++)
    {
      WIN_MASK_DIAG2 = ((WIN_MASK_DIAG2 << (BOARD_COLS+1)) | 1);
      WIN_MASK_DIAG1 = ((WIN_MASK_DIAG1 << BOARD_COLS) | (state_type)pow(2,(i)));
    }

  if(BOARD_COLS < BOARD_ROWS)
    throw string("unsupported");

  //  cout <<  state_to_string(WIN_MASK_HORIZ)  << endl;
  //  cout <<  state_to_string(WIN_MASK_VERT)  << endl;
  //  cout <<  state_to_string(WIN_MASK_DIAG1)  << endl;
  //  cout <<  state_to_string(WIN_MASK_DIAG2)  << endl;
  /*
  for(uint i=0; i<BOARD_COLS-BOARD_ROWS+1; i++)
    {
      cout << state_to_string((WIN_MASK_DIAG1 << i)) << endl;
      cout << state_to_string((WIN_MASK_DIAG2 << i)) << endl;
    }
  */
}

int main(int argc, char *argv[])
{
    try
    {
        if(argc < 2)
            throw string("usage: ./notaktoe board_rows board_cols [log_depth, default=0, currently broken]");

        BOARD_ROWS = atoi(argv[1]);
        BOARD_COLS = atoi(argv[2]);
        if(argc > 3)
	  print_log_g = atoi(argv[3]);

	init_check_win();

        cache_init(pow(2,BOARD_ROWS*BOARD_COLS));

        cout << "Player 1 " << (p1(0000,1) ? "can" : "cannot") << " force a win." << endl;
	/*
	bool p2_force=true;
	for(int i=0; i<BOARD_ROWS*BOARD_COLS; i++)
	  {
	    state_type board = 1 << i;
	    if(!p1(board, 2))
	      {
		p2_force=false;
		break;
	      }
	  }

        cout << "Player 2 " << (p2_force ? "can" : "cannot") << " force a win." << endl;
	*/


        return 0;
    }
    catch(string err)
    {
        cerr << err << endl;
    }
    catch (std::exception const err)
    {
        cerr << err.what() << endl;
    }
}
