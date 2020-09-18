from __future__ import print_function
import numpy as np
from collections import defaultdict 

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
            if piece == -1: print("X ",end="")
            elif piece == 1: print("O ",end="")
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

def getSymmetries(board):
    # mirror, rotational
    l = set()

    for i in range(1, 5):
        for j in [True, False]:
            newB = np.rot90(board, i)
            if j:
                newB = np.fliplr(newB)
            flat_newB = newB.flat
            l.add(tuple(flat_newB))
    #print(len(l))
    return list(l)
def statesToBinary(states):
    binaryStr = ''
    for i in states:
        if i == 'X':
            binaryStr += '1'
        elif i == '.':
            binaryStr += '0'
    return binaryStr

states = defaultdict(list)
it = 0
count_board = np.array([0]*25)
#print(count_board)
f =  open('3_all.txt', 'r')
for line in reversed(list(f)):
    content = line.rstrip().split(':')
    content[0] = statesToBinary(content[0])
    content[0] = np.array([int(p) for p in list(content[0])])
    content[1] = np.array([int(p) for p in list(content[1])])
    count = sum(content[0])
    states[count].append(content)
#     print(content)
#     print(count)
# print(states)
path = []
def loopDict(d,max_count, path):
    if max >=0:
        for i in d[max_count]:
            path.append(i[0])
            #print(i[0])
            loopDict(d, max_count -1, path)
    return path



max_count = max(states.keys())
path = loopDict(states, max_count, path)
#print(path)

with open('3_seq', 'w') as f:
    seq = {}
    for state in path:
        count = sum(state)
        seq[count] = state
        if count == 0:
            print([s for s in seq.values()])
            #f.write([s for s in seq.values()])

    #print(count)

# for i in states[max_count]:
#     current_count = max_count
#     while current_count != 0:
#         current_count -= 1
#         current_states = states[current_count]
#         for k in states[current_count]:
#             print(i[0])
#             #print(sum(i[0]))
#             print(k[0])
#             valid = np.sum(i[0] != k[0])
#             if valid == 1:

            #valid = sum(i[0] - k[0])
            #print(valid)


        #print(piece)

    # states = content[0].replace("X","1")
    # states = states.replace(".","0")
    # moves = content[1]
    # if states.count('1')==19:
    #     it +=1
    #     board = np.array(list(states), dtype=int)
    #     board = board.reshape((5,5))
    #     board_list = getSymmetries(board)
    #     for b in board_list:
    #         count_board +=b
    #         it+=1
#         #print(count_board)
#             #board = board.reshape((5,5))
#             #display(board)
#         # if it ==100:
#             #break

# count_board = count_board.reshape((5,5))
# print(it)
# print(count_board)

# count_board = np.array([0]*9)
# board = np.array([0,1,0,0,1,1,1,0,1])
# board = board.reshape((3,3))
# board_list = getSymmetries(board)
# for b in board_list:
#     count_board +=b
# count_board = count_board.reshape((3,3))
# print(count_board)