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
  state_type state2 = state;
  for(uint i=0; i<BOARD_ROWS; i++, state2 >>= BOARD_COLS)
    {
    if((state2 & WIN_MASK_HORIZ) == WIN_MASK_HORIZ)
	return true;
    }

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
/*
char *computed_hash, *value_hash;
void cache_init(state_type count)
{
    computed_hash = (char *) memset(new char[count/8+1], 0, count/8+1);
    value_hash = (char *) memset(new char[count/8+1], 0, count/8+1);
}

inline uint8_t ht_bitpattern(state_type state)  { return uint8_t(1) << (uint8_t(state) & uint8_t(7)); }
inline state_type ht_index(state_type state) { return state >> state_type(3); }
*/




//const state_type BOARD_MASK=0x1ffffff;


static const state_type BitReverseTable256[] = 
  {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
  };


inline state_type bit_reverse8(state_type state, uint bits)
{

// Option 1:
  return ((BitReverseTable256[(state) & 0xff])) >> (8-bits); 
}



inline state_type bit_reverse(state_type state, uint bits)
{

// Option 1:
  return ( 
          (BitReverseTable256[(state) & 0xff] << 56) | 
	  (BitReverseTable256[(state>> 8) & 0xff] << 48) | 
	  (BitReverseTable256[(state>> 16) & 0xff] << 40) |
	  (BitReverseTable256[(state>> 24) & 0xff] << 32) |
          (BitReverseTable256[(state>> 32) & 0xff] << 24) | 
	  (BitReverseTable256[(state>> 40) & 0xff] << 16) | 
	  (BitReverseTable256[(state>> 48) & 0xff] << 8) |
	  (BitReverseTable256[(state>> 56) & 0xff] << 0) 
    ) >> (64-bits);
  /*
     state_type result=0;
  for(int i=0; i<bits; i++)
    result = (result << 1 | (state & 1)), state>>=1;

    return result;
  */
  //  return ((state & 1) << 4) | ((state & 2) << 2) | ((state & 4)) | ((state&8) >> 2) | ((state & 16) >> 4);
    /*
  state_type result = state;
  bits--;
    state >>= 1; 
    while(state)
      {
	result <<= 1;       
	result |= state & 1;
	state >>= 1;
	bits--;
      }
    result <<= bits;
    return result & BOARD_MASK;
    */
}



state_type get_equiv_state(state_type state, int type)
{
  if(type == 0)
    return state;
  else if(type == 1) 	// 180 deg rotation
    return bit_reverse(state, BOARD_COLS*BOARD_ROWS);
  else if(type == 2)    // vert flip
    {
      state_type s2 = 0, state2=state;
      for(int i=0; i<BOARD_ROWS; i++)
	{
	  s2 = (s2 << BOARD_COLS) | (state2 & WIN_MASK_HORIZ);
	  state2 >>= BOARD_COLS;
	}
      return s2;
    }
  else if(type == 3)    // horiz flip
    {
      state_type s2 = 0, state2=state;
      for(int i=0; i<BOARD_COLS; i++)
	{
	  s2 = (s2 << 1) | (state2 & WIN_MASK_VERT);
	  state2 >>= 1;
	}
      return s2;
    }
  else if(type == 4 || type == 5)  // tranpose and 90 deg
    {
      if(BOARD_ROWS!=BOARD_COLS)
	  cerr << "NOT SUPPORTED " << endl;

      state_type s2 = 0, state2=state;
      for(int i=0; i<BOARD_ROWS; i++)
	{
	  state_type s3=0;
	  state_type row = state2 & WIN_MASK_HORIZ;

	  state2 >>= BOARD_COLS;
	  row = bit_reverse8(row, BOARD_COLS);
	  
	  for(int j=0; j<BOARD_COLS; j++)
	    {
	      s3 = (s3 << BOARD_COLS) | (row & 1);
	      row >>= 1;
	    }
	  
	  s2 = (type == 4) ? (s2 | (s3 << i)) : ((s2 << 1) | s3);
	}

      return s2;
    }
  return state;
}

#include "crc64.cpp"
state_type **computed_hash;
state_type *CACHE_MASK;
state_type MAX_BIT=0;
unsigned int *CACHE_BITS;
//#define CACHE_MASK  0x03ffffff;
//#define CACHE_BITS  26

void cache_init(state_type count, int levels=1)
{
  //  generate_table();
  cerr << "initializing " << endl;
  computed_hash = new state_type *[levels];
  CACHE_MASK = new state_type[levels];
  CACHE_BITS = new unsigned int[levels];
  MAX_BIT = pow(2, BOARD_ROWS*BOARD_COLS);
  for(int i=0; i<levels; i++)
    {
      CACHE_BITS[i] = min(5*(i+1), 26);
      CACHE_MASK[i] = state_type(pow(2, CACHE_BITS[i]) - 1);
      computed_hash[i] = (state_type *) memset(new state_type[(int)pow(2, CACHE_BITS[i])], 0, sizeof(state_type) * (int)pow(2, CACHE_BITS[i]));
      computed_hash[i][0] = -1;
    }
  //    value_hash = (bool *) memset(new bool[(int)pow(2, CACHE_BITS)], 0, sizeof(bool) * (int)pow(2, CACHE_BITS));
  cerr << "done initializing " << endl;
}



//inline uint8_t ht_bitpattern(state_type state)  { return uint8_t(1) << (uint8_t(state) & uint8_t(7)); }
//inline state_type ht_index(state_type state) { return state >> state_type(3); }
//#include "xxhash_cpp/xxhash/xxhash.hpp"
#include "xxHash/xxhash.h"
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

vector<state_type> eval(100), calls(100), collide(100);
state_type tcalls=0;
state_type get_hash(state_type s2)
{
  return XXH64((char *) &s2, sizeof(state_type), 0);
}

void print_calls()
{
      {
	cout << "TCALLS " << tcalls << endl;
	cout << "evals" << "\t" << "collisions" << "\t" << "calls" << endl;
	cout << setw(5);
	for(int i=0; i<BOARD_ROWS*BOARD_COLS; i++)
	  cout << i << "\t" << CACHE_BITS[i] << "\t" << eval[i] << " (" << double(eval[i])/calls[i]*100 << ") \t\t" << collide[i] << " (" << double(collide[i])/calls[i]*100 << ", " << double(collide[i]) / eval[i]*100 << ") \t\t" << calls[i] << endl;
	cout << "----" << endl;
      }
}

inline bool cache_get_val(state_type state, int arg2)
{
  state_type *ch = computed_hash[arg2];
  tcalls++;
  calls[arg2-2] ++;
    for(int i=0; i<=0; i++)
      {
	state_type s2 = get_equiv_state(state, i);
	//	state_type index = ht_index(s2);
	//	uint8_t bitpat = ht_bitpattern(s2);

	//	if(ch[ index ] & bitpat)
	//	  return value_hash[ index ] & bitpat;
	//	cout << s2 << " " << calculate_crc((char *) &s2, 8) << endl;
	state_type hash = get_hash(s2) & CACHE_MASK[arg2-2]; //XXH64((char *) &s2, sizeof(state_type), 0) & CACHE_MASK[arg2];
	//	if(ch[hash] != 0 && ch[hash] != s2)
	//	  cout << ch[hash] << " " << s2 << " " << hash << " " << endl;
	if((ch[hash] >> 8) == s2)
	  return ch[hash] & 1;
	else if((ch[hash] >> 8) != 0 && i==0)
	  {
	  collide[arg2-2]++;
	  /*	  cout << state_to_string(ch[hash] >> 8) << endl << state_to_string(s2) << endl <<endl<<endl;
	  cout << hash;
	  state_type t = ch[hash];
	  t>>=8;
	  t = get_hash(t);
	  t &= CACHE_MASK;
	  cout << " " << t << endl;
	  */
	  }
      }

    //    state_type index = ht_index(state);
    //    uint8_t bitpat = ht_bitpattern(state);

    bool val;
    eval[arg2-2]++;
    state_type hash = get_hash(state) & CACHE_MASK[arg2-2]; // XXH64((char *) &state, sizeof(state_type), 0) & CACHE_MASK;
    //    if( (val = p1(state, arg2)) )
    //      value_hash[hash] = true;
    //    val = value_hash[hash] = p1(state, arg2);
    //    val = value_hash[hash] = p1(state, arg2);
    val = p1(state, arg2);
    ch[hash] = (state << 8) | val;
    //        value_hash[ index ] |= bitpat;
    //    ch[ index ] |= bitpat;

    if(tcalls % 1000000 == 0)
    print_calls();

    return val;
}

// return true iff at least *1* of my possible moves from this state results in a win
//   (or if the other player has already lost)
bool p1(state_type state, int depth=0)
{
    if(depth < 6)
    cout << state_to_string(state) << endl;
    if(check_win(state))
        return true;

    //    state_type new_state;
    //    for(state_type i=0; i<BOARD_ROWS*BOARD_COLS; i++)
    for(state_type i=1; i<MAX_BIT; i <<= 1)
      if( (state | i) != state && !cache_get_val(state | i, depth+1))
	//      if(( new_state = (state | (state_type(1) << i)) ) != state && !cache_get_val(new_state, depth+1))
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

#define LOOP(I,J)   for(state_type (i  ## J)=(i ## I)+1, (bits ## J)=(bits ## I)<<1, (state ## J)=(state ## I) | ((bits ## I)<<1); (i ## J)<BOARD_SZ; (i ## J)++, (bits ## J)<<=1, (state ## J) = (state ## I) | (bits ## J)) 
//if(!check_win(state ## J))
#define LOOP0 	  for(state_type i1=0, bits1=1, state1=state0 | 1; i1<BOARD_SZ; i1++, bits1<<=1, state1 = state0 | bits1) 
//if(!check_win(state1))

#define BOARDS1 LOOP0
#define BOARDS2 BOARDS1 LOOP(1,2)
#define BOARDS3 BOARDS2 LOOP(2,3)
#define BOARDS4 BOARDS3 LOOP(3,4)
#define BOARDS5 BOARDS4 LOOP(4,5)
#define BOARDS6 BOARDS5 LOOP(5,6)
#define BOARDS7 BOARDS6 LOOP(6,7)
#define BOARDS8 BOARDS7 LOOP(7,8)
#define BOARDS9 BOARDS8 LOOP(8,9)
#define BOARDS10 BOARDS9 LOOP(9,10)
#define BOARDS(n) (BOARDS##n)






//state_type binom_lookup[50][50];
state_type binom(state_type n, state_type k)
{
  if(k==0 || k==n)
    return 1;
    return binom(n-1,k-1) + binom(n-1,k);                                                                                                                                          
    //  return binom_lookup[n-1][k-1] + binom_lookup[n-1][k];
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

	    
	//	    for(state_type i2=i1+1, bits2=bits1<<1, state2=state1 | (bits1<<1); i2<BOARD_SZ; i2++, bits2<<=1, state2 = state1 | bits2)
	if(0)
	{
	  /*	  int BOARD_SZ=BOARD_ROWS*BOARD_COLS;
	  state_type state0 = state_type(0);

	  int ct=0, win_ct=0;
	  ct=0, win_ct=0;	  BOARDS1 { ct++; if(check_win(state1)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 1) << endl;
	  ct=0, win_ct=0;	  BOARDS2 { ct++; if(check_win(state2)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 2) << endl;
	  ct=0, win_ct=0;	  BOARDS3 { ct++; if(check_win(state3)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 3) << endl;
	  ct=0, win_ct=0;	  BOARDS4 { ct++; if(check_win(state4)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 4) << endl;
	  ct=0, win_ct=0;	  BOARDS5 { ct++; if(check_win(state5)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 5) << endl;
	  ct=0, win_ct=0;	  BOARDS6 { ct++; if(check_win(state6)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 6) << endl;
	  ct=0, win_ct=0;	  BOARDS7 { ct++; if(check_win(state7)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 7) << endl;
	  ct=0, win_ct=0;	  BOARDS8 { ct++; if(check_win(state8)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 8) << endl;
	  ct=0, win_ct=0;	  BOARDS9 { ct++; if(check_win(state9)) { win_ct++; continue;} }	  cout << win_ct << " " << ct << " " << binom(BOARD_SZ, 9) << endl;
	  */


	}




        cache_init(pow(2,BOARD_ROWS*BOARD_COLS), BOARD_ROWS*BOARD_COLS);

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

	print_calls();
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
