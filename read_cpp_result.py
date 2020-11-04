import numpy as np


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


def printPathsRec(root, path = [], pathLen=0): 
        
    # Base condition - if binary tree is 
    # empty return 
    if root is None: 
        return

    # add current root's data into  
    # path_ar list 
    root_translations = translate_board(root)
    #print('root translations', root_translations)
    for t in root_translations:
        if t in all_dict[(n-r-1)-pathLen].keys():
            break
    root = t
    current_queue = all_dict[(n-r-1)-pathLen][root]      
    # if length of list is gre 
    if(len(path) > pathLen):  
        path[pathLen] = root
    else: 
        path.append(root) 
    #print(all_dict[9-4-pathLen])
    #print(root)

    pathLen += 1
    
    if not current_queue: 
        
        # leaf node then print the list 
        #path.reverse()
        with open(str(r)+'_all_knight_seq.txt','a') as f:
            f.write(':'.join(path[::-1]))
            f.write('\n')
            #print(path[::-1])
    else: 
        #print(current_queue)
        for l in current_queue:
            printPathsRec(l, path, pathLen) 



if __name__ == "__main__":
        
    global r
    global c
    global n

    r = c = 5
    n = r*c
    #print(translate_board(np.array([0,0,0,1,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0])))
    #all_states = translate_board(np.array([0,0,0,1,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0]))
    all_states = translate_board(np.array([0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]))
    states_file = str(r) +  "_knight_seq_stateType.txt"
    # states_file = "5_knight_seq_stateType.txt"


    all_dict = {}

    # {num_pieces: {state : last_move}}
    for line in reversed(list(open(states_file))):
        
        state, last_moves = line.rstrip().split(" :: ")
        indexes = findOccurrences(last_moves, '1')
        possible_last_states = set()
        for ind in indexes:
            last_state = state[:ind] + '0' + state[(ind+1):]
            possible_last_states.add(last_state)
        
        n_pieces = count_pieces(state)
        if n_pieces not in all_dict.keys():
            states_dict={}
            states_dict[state] = possible_last_states
            all_dict[n_pieces]= states_dict
        else:
            all_dict[n_pieces][state] = possible_last_states

    for i in all_states:
        #print(count_pieces(i))
        if i in all_dict[3].keys():
            print(i, all_dict[count_pieces(i)][i])
    # for ss in all_dict[(n-r-1)].keys():
    #     printPathsRec(ss)

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