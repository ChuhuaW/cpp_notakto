import numpy as np
from copy import deepcopy

def count_pieces(board):
    return board.count('1')

def init_dict(n):
    all_dict = {}
    for i in range(n):
        all_dict[i] = []
    return all_dict

def last_to_index(last_moves):
    return

def findOccurrences(s, ch):
    return [i for i, letter in enumerate(s) if letter == ch]

def translate_board(state):
    np_state = np.fromiter(state, dtype=int).reshape(r,c)
    all_states = set()
    for k in range(4):
        state= np.rot90(np_state, k)
        flip_state = np.fliplr(state)
        all_states.add(''.join([str(x) for x in state.flatten()]))
        all_states.add(''.join([str(x) for x in flip_state.flatten()]))
        #print(''.join([str(x) for x in state.flatten()]))
        # print(state)
        # print(flip_state)
        # print()
        #print([''.join(x.str()) for x in state])
        # all_states.add(state)
        # all_states.add(np.flip(state))
        

    #print(all_states)
    return all_states


def generate_win():
    win_state = []
    import math
    r = c = int(math.sqrt(n))
    # row
    for i in range(r):
        init = np.array([[0]*r]*c)
        init[i,:] = np.array([1]*r)
        win_state.append(flat_array(init))
        #print(init)
    # col
    for j in range(c):
        init = np.array([[0]*r]*c)
        init[:,j] = np.array([1]*r)
        win_state.append(flat_array(init))
        #print(init)
    # diagonal
    di = np.diag_indices(3)
    init = np.array([[0]*r]*c)
    init[di] = 1
    #print(init)
    win_state.append(flat_array(init))
    win_state.append(flat_array(np.fliplr(init)))
    #print(init)
    return win_state


def display(board):
    n = board.shape[0]
    print("   ", end="")
    for y in range(n):
        print (y,"", end="")
    print("")
    print("  ", end="")
    for _ in range(n):
        print ("-", end="-")
    print("--")
    for y in range(n):
        print(y, "|",end="")    # print the row #
        for x in range(n):
            piece = board[y][x]    # get the piece to print
            if piece == 1: print("X ",end="")
            elif piece == 8: print("W ",end="")
            else:
                if x==n:
                    print("-",end="")
                else:
                    print("- ",end="")
        print("|")

    print("  ", end="")
    for _ in range(n):
        print ("-", end="-")
    print("--")

def flat_array(board):
    array_str = ''.join([str(x) for x in board.flatten()])
    return array_str

def find_available_knight(move):
    x,y = move
    all_knight = []
    for i in [1,-1]:
        for j in [2,-2]:
            if 0<=(x+i)<=(c-1) and 0<=(y+j)<=(c-1) :
                all_knight.append((x+i, y+j))
            if 0<=(x+j)<=(c-1) and 0<=(y+i)<=(c-1) :
                 all_knight.append((x+j, y+i))
    #print(all_knight)
    return all_knight


if __name__ == "__main__":
        
    global r
    global c
    global n






    r = c = 5
    n = r*c
    states_file = str(r) +  "_knight_seq_stateType.txt"
    # # states_file = "5_knight_seq_stateType.txt"

    win_state = generate_win()
    initial_board = np.array([[0]*r]*c)

    all_dict = {}

    # {num_pieces: {state : last_move}}
    for line in (list(open(states_file))):
        
        state, last_moves = line.rstrip().split(" :: ")
        # indexes = findOccurrences(last_moves, '1')
        # possible_last_states = set()
        all_dict[state] = last_moves
        # for ind in indexes:
        #     last_state = state[:ind] + '0' + state[(ind+1):]
        #     possible_last_states.add(last_state)
        
        # #n_pieces = count_pieces(state)
        # if n_pieces not in all_dict.keys():
        #     states_dict={}
        #     states_dict[state] = possible_last_states
        #     all_dict[n_pieces]= states_dict
        # else:
        #     all_dict[n_pieces][state] = possible_last_states
    
    #display(initial_board)
    # print(''.join([str(x) for x in initial_board.flatten()]))
    player = 1
    current_board = initial_board
    last_move = (-1,-1)
    #pieces = 0
    display(current_board)
    keys_set = set(all_dict.keys())
    while flat_array(current_board) not in win_state:
        last_board = deepcopy(current_board)
        if player == 1:
            if last_move == (-1,-1) and r ==3:
                availble_move = (1,1)
                temp_board = deepcopy(current_board)
                x,y = availble_move
                temp_board[x][y] = 8
                display(temp_board)
            elif last_move == (-1,-1) and r ==5:
                temp_board = deepcopy(current_board)
                temp_board[temp_board==0] = 8
                display(temp_board)

            else:
                #import pdb; pdb.set_trace()
                if sum(current_board.flatten()) == n-r:
                    print('GAME OVER')
                    break
                availble_moves = find_available_knight(last_move)
                print(availble_moves)
                display_board = deepcopy(current_board)
                x,y = last_move
                last_move_index = x*r+y
                #empty_string = '0'*n
                #move_string = empty_string[:last_move_index] + '1' + empty_string[(last_move_index+1):]
                for m in availble_moves:
                    temp_board = deepcopy(current_board)
                    mx, my = m
                    if temp_board[mx][my] ==0:
                        temp_board[mx][my] = 1
                    #import pdb; pdb.set_trace()
                    all_translates = translate_board(flat_array(temp_board))
                    
                    interset_board = all_translates.intersection(keys_set)
                    if all_dict[flat_array(temp_board)][last_move_index] =='1':
                        
                        # if display_board[mx][my] ==0:
                        display_board[mx][my] = 8
                display(display_board)
        #print(initial_board)

        
        
        print('Current player: {}'.format(player))
        print('Please enter next move:')
        move = input()
        x,y = move.split(' ')
        x,y = int(x), int(y)
        current_board[x][y] = 1


        last_move = (x,y)
        display(current_board)
        player = 1 - player
        #pieces +=1



    # # for ss in all_dict[(n-r-1)].keys():
    # #     printPathsRec(ss)
    
    # initial_board = [0]*25


    # for i in sorted(all_dict.keys(),reverse=True):
    #     print(i)
    #     for j in all_dict[i].keys():
    #         # print((all_dict[i][j]))
    #         # print(findOccurrences((all_dict[i][j]), '1'))
    #         indexes = findOccurrences((all_dict[i][j]), '1')
    #         possible_last_states = set()
    #         for ind in indexes:
    #             last_state = j[:ind] + '0' + j[(ind+1):]
    #             possible_last_states.add(last_state)
    #         # for s in possible_last_states:
    #         #     translations = translate_board(possible_last_states)
    #         #print(translate_board(j))
    #         # [all_dict[i][j]]
    #         print(possible_last_states)
        
        
    # print(all_dict.keys())

        
    

    #printPathsRec('110011100')
    # printPathsRec('1101111101010111111010111')

    #player1 , player2: # round 1
    #player2 , player1: # round 2

    # statistics share by player1 and player2
    # stat1, stat2
    # stat1 first player
    # stat2 second player