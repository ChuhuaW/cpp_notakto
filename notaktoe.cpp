#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <bitset>
#include<algorithm>
#include<parallel/algorithm>
#include "xxHash/xxhash.h"
#include <vector>
#include <iostream>
#include <iomanip>

typedef uint64_t state_type;
using namespace std;

uint BOARD_ROWS, BOARD_COLS, BOARD_SIZE;
state_type WIN_MASK_HORIZ, WIN_MASK_VERT, WIN_MASK_DIAG2, WIN_MASK_DIAG1;
bool p1(state_type, int);
typedef enum {BITCACHE=0, HASHCACHE=1, BOTTOMUP=2, HYBRID=3, LEVELED=4} Algos;
Algos  ALGO;
// set disable_diag to true if you don't want to count complete diagonals as terminal states
bool DISABLE_DIAG=false;
int READ_EQUIV=0, WRITE_EQUIV=0;
int INTERACTIVE=0;
inline state_type get_hash(state_type s2) {    return XXH64((char *) &s2, sizeof(state_type), 0); }

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
void init_check_win()
{
  if(BOARD_COLS < BOARD_ROWS)
    throw string("unsupported");

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
}

bool check_win(state_type state)
{
  state_type state2 = state;
  for(uint i=0; i<BOARD_ROWS; i++, state2 >>= BOARD_COLS)
    if((state2 & WIN_MASK_HORIZ) == WIN_MASK_HORIZ)
      return true;
  
  for(uint i=0; i<BOARD_COLS; i++)
    if((state | (WIN_MASK_VERT << i)) == state)
      return true; 

  if(!DISABLE_DIAG)
    for(uint i=0; i<BOARD_COLS-BOARD_ROWS+1; i++)
      if((state | (WIN_MASK_DIAG1 << i)) == state || (state | (WIN_MASK_DIAG2 << i)) == state)
	return true;
  
  return false;
}

// simple bit cache -- see if we already know value for this state, if so return it,
//  if not compute and return it
//
state_type TOP_BITS=11,  NOTOPBITS_MASK, TOP_BITS_MASK;
state_type **computed_hash;
state_type *CACHE_MASK;
state_type MAX_BIT=0;
unsigned int *CACHE_BITS;
state_type HYBRID_MASK, HYBRID_BITS = 30;
state_type *ch_hybrid;
state_type *bitvalue_hash;
void bitcache_init(state_type count)
{
  cerr << "initializing cache..." << endl;
  //    bitcomputed_hash = (char *) memset(new char[count/8+1], 0, count/8+1);
  if(ALGO==HYBRID)
    {
      cerr << "caches have size " << sizeof(state_type) * (count/4/8/(state_type)pow(2,TOP_BITS)+1) << " bytes, " << (state_type)pow(2, HYBRID_BITS) * sizeof(state_type) << " bytes" << endl;
      bitvalue_hash = (state_type *) memset(new state_type[count/4/8/(state_type)pow(2,TOP_BITS)+1], 0, (count/4/8/pow(2,TOP_BITS)+1) * sizeof(state_type));
      TOP_BITS_MASK = state_type(pow(2, TOP_BITS)-1);
      NOTOPBITS_MASK = (~((TOP_BITS_MASK) << (BOARD_SIZE-TOP_BITS))) & (MAX_BIT-1);
      

      HYBRID_MASK = state_type(pow(2, HYBRID_BITS) - 1);
      ch_hybrid = (state_type *) memset(new state_type[(int)pow(2, HYBRID_BITS)], 0, sizeof(state_type) * (int)pow(2, HYBRID_BITS));
      ch_hybrid[0] = -1;
    }
  else
    bitvalue_hash = (state_type *) memset(new state_type[count/4/8+1], 0, (count/4/8+1) * sizeof(state_type));
   cerr << "done" << endl;
 }

 //inline uint8_t ht_bitpattern(state_type state)  { return uint8_t(1) << (uint8_t(state) & uint8_t(7)); }
 //inline uint8_t ht_bitpattern(state_type state)  { return uint8_t(3) << ((uint8_t(state) & uint8_t(3)) << 1); }
 //inline state_type ht_index(state_type state) { return state >> state_type(5); }
 //inline state_type ht_index(state_type state) { return state >> state_type(3); }


 // Functions to quickly compute equivalent states (rotations, etc.) for a given state
 //
 /////
 // fast bit reversal code from: https://stackoverflow.com/questions/746171/most-efficient-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c
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

 // fast bit rversal
 inline state_type bit_reverse8(state_type state, uint bits) {   return ((BitReverseTable256[(state) & 0xff])) >> (8-bits);  }
 inline state_type bit_reverse(state_type state, uint bits)
 {
   return (           (BitReverseTable256[(state) & 0xff] << 56) | 
	   (BitReverseTable256[(state>> 8) & 0xff] << 48) | 
	   (BitReverseTable256[(state>> 16) & 0xff] << 40) |
	   (BitReverseTable256[(state>> 24) & 0xff] << 32) |
	   (BitReverseTable256[(state>> 32) & 0xff] << 24) | 
	   (BitReverseTable256[(state>> 40) & 0xff] << 16) | 
	   (BitReverseTable256[(state>> 48) & 0xff] << 8) |
	   (BitReverseTable256[(state>> 56) & 0xff] << 0) 
     ) >> (64-bits);
 }
 ////////////////

 // each value of type from 0 to 5 gives a different reflection, rotation, etc of the state.
 //
 state_type get_equiv_state(state_type state, int type)
 {
   state_type s2 = 0, state2=state;

   switch(type) 
     {
     case 0:
       return state;
     case 1: // 180 deg rotation
       return bit_reverse(state, BOARD_SIZE);
     case 2: // vert flip
       for(int i=0; i<BOARD_ROWS; i++)
	 {
	   s2 = (s2 << BOARD_COLS) | (state2 & WIN_MASK_HORIZ);
	   state2 >>= BOARD_COLS;
	 }
       return s2;
     case 3: // horiz flip
       for(int i=0; i<BOARD_COLS; i++)
	 {
	   s2 = (s2 << 1) | (state2 & WIN_MASK_VERT);
	   state2 >>= 1;
	 }
       return s2;
     case 4: // transpose and 90 deg
     case 5:
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

 void cache_init(state_type count, int levels=1)
 {
   cerr << "initializing " << endl;
   computed_hash = new state_type *[levels];
   CACHE_MASK = new state_type[levels];
   CACHE_BITS = new unsigned int[levels];

   for(int i=0; i<levels; i++)
     {
       CACHE_BITS[i] = min(5*(i+1), 26);
       CACHE_MASK[i] = state_type(pow(2, CACHE_BITS[i]) - 1);
       computed_hash[i] = (state_type *) memset(new state_type[(int)pow(2, CACHE_BITS[i])], 0, sizeof(state_type) * (int)pow(2, CACHE_BITS[i]));
       computed_hash[i][0] = -1;
     }

   cerr << "done initializing " << endl;
 }

vector<state_type> ct;
state_type ct2=0;

state_type r_hybrid=0, w_hybrid=0, r_bit=0, w_bit=0;

void print_status()
{
  for(int i=0; i<pow(2,TOP_BITS); i++)
    if(ct[i] > 0)
      cout << i << " " << std::bitset<15>(i) << " " << ct[i] << endl;
  

  cout << "----- Total states written: " << ct2 << "  " << (double(ct2) / pow(2.0, BOARD_SIZE))*100 << "%" << endl;
  cout << "Bit cache    reads: " << r_bit << "  writes: " << w_bit << endl;
  cout << "Hybird cache reads: " << r_hybrid << "  writes: " << w_hybrid << endl;
  
}



 inline bool cache_get_val(state_type state, int arg2)
 {
   state_type *ch;
   if(ALGO == HASHCACHE)
       ch = computed_hash[arg2];

   // look at equivalent states (optionally)
   for(int i=0; i<=READ_EQUIV; i++)
     {
       state_type s2 = get_equiv_state(state, i);
       if(ALGO == HYBRID)
	 {
	   if(((s2 >> (BOARD_SIZE-TOP_BITS)) == TOP_BITS_MASK))
	     {
	       r_bit++;
	       state_type val = bitvalue_hash[ (s2 & NOTOPBITS_MASK) >> state_type(5) ] >> ((s2 & 31) << 1);
	       if(val & 1)
		 return val & 2;
	     }
	   else
	     {
	       r_hybrid++;
	       state_type ch_hash = ch_hybrid[get_hash(s2) & HYBRID_MASK];
	       if((ch_hash >> 8) == s2)
		 return ch_hash & 1;
	     }
	 }
       else if(ALGO == BITCACHE)
	 {
	   state_type val = bitvalue_hash[ (s2) >> state_type(5) ] >> ((s2 & 31) << 1);
	   if(val & 1)
	     return val & 2;
	 }
       else 
	 {
	   state_type ch_hash = ch[get_hash(s2) & CACHE_MASK[arg2-2]]; 
	   if((ch_hash >> 8) == s2)
	     return ch_hash & 1;
	 }
     }

   // cache miss, so need to calculate
   bool val = p1(state, arg2);
   for(int i=0; i<=WRITE_EQUIV; i++)
     {
       state_type s2 = get_equiv_state(state, i);
       ct2++;
       ct[( s2 >> (BOARD_SIZE-TOP_BITS))]++;
       if(!(ct2 % 50000000))
	   print_status();
       if(ALGO == HYBRID)
	 {
	   if(((s2 >> (BOARD_SIZE-TOP_BITS)) == TOP_BITS_MASK))
	     {
	     bitvalue_hash[ (s2 & NOTOPBITS_MASK) >> state_type(5) ] |= ((val?3L:1L) << ((uint8_t(s2) & uint8_t(31)) << 1));
	     w_bit++;
	     }
	   else
	     {
	       //	       cout << std::bitset<25>(s2) << endl;
	     ch_hybrid[get_hash(s2) & HYBRID_MASK] = (s2 << 8) | val;
	       w_hybrid++;
	     }
	 }
       else if(ALGO == BITCACHE)
	 bitvalue_hash[ (s2) >> state_type(5) ] |= ((val?3L:1L) << ((uint8_t(s2) & uint8_t(31)) << 1));
      else
	ch[ get_hash(s2) & CACHE_MASK[arg2-2] ] = (s2 << 8) | val;
    }

    return val;
}

#define COMPLETE 

// return true iff at least *1* of my possible moves from this state results in a win
//   (or if the other player has already lost)
bool p1(state_type state, int depth=0)
{
    if(check_win(state))
        return true;

#ifdef COMPLETE
    bool a=false;
    for(state_type i=(MAX_BIT >> 1); i!=0; i >>=1)
      if( !(state & i) && !cache_get_val(state | i, depth+1))
	a=true;
    return a;
#else
    for(state_type i=(MAX_BIT >> 1); i!=0; i >>=1)
      if( !(state & i) && !cache_get_val(state | i, depth+1))
	  return true;
#endif

    return false;
}

// calculate binomial coeffs
state_type binom(state_type n, state_type k)
{
  if(k==0 || k==n)
    return 1;
  return binom(n-1,k-1) + binom(n-1,k);                                                                                                                                          
}


int main(int argc, char *argv[])
{
    try
    {
        if(argc < 3)
	  throw string("usage: ./notaktoe [options] board_rows board_cols \n"
		       "  --algo n:    0 = cache all states in bitvectors, requires O(2^(board_rows*board_cols)) *bits* of RAM, but fast\n"
		       "               1 = cache states in hash tables, requires O(board_rows*board_cols*n) \n"
		       "                   (where n is huge number specified at compile time) but not fast\n"
		       "               2 = incrementally consider states from bottom up, uses about \n"
		       "                  ((board_rows*board_cols) choose ((board_rows*board_cols/2)))*(board_rows*board_cols/2) 64-bit *words* of storage\n"
		       "                  but doesn't need to be in ram (current implementation is in ram but could be done on disk instead\n"
		       "  --nodiag:    ignore diagonal wins \n"
		       "  --readeq n:  when looking up states, look also for up to n equivalent states (rotations, flips, etc)\n"
		       "  --writeeq n: after calculating for new state, update cache for up to n equivalent states (rotations, flips, etc)\n"
		       "  --interact:  interactive mode \n"
		       );
	int ii=1;
	for( ; ii<argc; ii++)
	  if(string(argv[ii]) == "--algo") ALGO = (Algos) atoi(argv[++ii]);
	  else if(string(argv[ii]) == "--nodiag") DISABLE_DIAG = true;
	  else if(string(argv[ii]) == "--readeq") READ_EQUIV = atoi(argv[++ii]);
	  else if(string(argv[ii]) == "--writeeq") WRITE_EQUIV = atoi(argv[++ii]);
	  else if(string(argv[ii]) == "--interact") INTERACTIVE = 1;
	  else break;

	BOARD_ROWS = atoi(argv[ii]);
        BOARD_COLS = atoi(argv[ii+1]);
	BOARD_SIZE = BOARD_ROWS * BOARD_COLS;
	MAX_BIT = pow(2, BOARD_SIZE);
	if(ALGO <0 || ALGO > 4)
	  throw string("invalid algo " + ALGO);

	init_check_win();

	if(ALGO == BOTTOMUP)
	  {
	    // start with full board
	    vector< state_type > states, new_states;
	    states.push_back((state_type(MAX_BIT-1) << 1) | 1);
	    
	    // remove one piece at a time
	    for(int ii=BOARD_ROWS*BOARD_COLS; ii>0; ii--)
	      {
		cerr << "LEVEL " << ii << endl;
		cerr << "listing..." << endl;
		new_states.clear();
		new_states.reserve(states.size()*ii);
		for(int j=0; j<states.size(); j++)
		  {
		    state_type state = states[j] >> 1;
		    bool state_won = states[j] & 1;
		    
		    for(int i=0; i<BOARD_ROWS*BOARD_COLS; i++)
		      if(state & (1 << i))
			new_states.push_back(( (state ^ (1 << i)) << 1) | (!state_won));
		  }
		
		states.clear();
		states.reserve(binom(BOARD_ROWS*BOARD_COLS, ii-1));
		cerr << "sorting... " << new_states.size() << endl;
		__gnu_parallel::sort(new_states.begin(), new_states.end());
		cerr << "uniq'ing..." << endl;
		state_type last = -1;
		for(int i=0; i<new_states.size(); )
		  {
		    last = new_states[i] >> 1;
		    int new_i=i;
		    bool any=false;
		    while(last == (new_states[new_i] >> 1) && new_i < new_states.size())
		      {
			any =  any || (new_states[new_i] & 1);
			new_i++;
		      }
		    states.push_back(( last << 1) | ( any || check_win(last)));

		    i = new_i;
		  }
		
		cerr << states.size() << " " << (states[0] & 1) << endl;


		for(int i=0; i<states.size(); i++)
		  cout << (states[i] >> 1) << " " << (states[i] & 1) << endl;
		
	      }
	    
	    return 0;
	  }
	
	if(ALGO == BITCACHE || ALGO == HYBRID)
	  bitcache_init(MAX_BIT);
	else if(ALGO == HASHCACHE)
	  cache_init(pow(2,BOARD_ROWS*BOARD_COLS), BOARD_ROWS*BOARD_COLS);

	ct=vector<state_type>(pow(2,TOP_BITS));
	
        cout << "Player 1 " << (p1(0000,1) ? "can" : "cannot") << " force a win." << endl;
	
	

	/*	vector<state_type> ct(16);
	for(state_type i=0; i<MAX_BIT/32; i++)
	  {
	    bool f = true;
	    for(state_type j=0; j<32; j++)
	      {
		//		cout << (i >> (BOARD_SIZE-4)) << endl;
		ct[(i*32) >> (BOARD_SIZE-4)] += ((bitvalue_hash[i] >> (j*2)) & 1);
		//		cout << (((bitvalue_hash[i] >> (j*2)) & 1)?"X":"_");
		//		if((bitvalue_hash[i] >> (j*2)) & 1)
		//		  f = false;
	      }
	    //	    cout << endl;
	    //    if(!f) ct2+=4;
	  }
	*/
	print_status();

	if(INTERACTIVE)
	  {
	    state_type state=0, new_state=0;

	    int p=0;
	    while(1) 
	      {
		cout << "------" << endl;
		cout << "CURRENT BOARD:" << endl;
		cout << state_to_string(state) << endl;

		char status[BOARD_SIZE];
		for(int i=0; i<BOARD_SIZE; i++)
		  {
		  if(state & (1 << i))
		    status[i] = 'X';
		  else if(check_win(state | (1 << i)))
		    status[i] = '!';
		  else if(!cache_get_val(state | (1 << i), 0))
		    status[i] = 'w';
		  else 
		    status[i] = '.';
		  }

		cout << "Here's what would happen if current player chooses each square:\n";
		cout << "X = square already occupied, ! = immediate loss, w = current player can force win, . = current player cannot force win\n";
		for(uint i=0, n=BOARD_SIZE-1; i<BOARD_ROWS; i++)
		  {
		  for(uint j=0; j<BOARD_COLS; j++, n--)
		    cout << status[n];
		  cout << endl;
		  }

		int row,col;
		cout << "Player " << p+1 << ", ";
		cout << "where to play? (give row col pair, e.g. '0 0' for upper-left): " << endl;
		cin >> row >> col;
		row = BOARD_ROWS-row-1;
		col = BOARD_COLS-col-1;
		new_state |= (1 << (row*BOARD_ROWS+col));

		if(new_state != state)
		  p=(++p)%2;
		state = new_state;
	      }

	  }



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
