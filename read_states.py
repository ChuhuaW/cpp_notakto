from __future__ import print_function
import numpy as np

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

it = 0
count_board = np.array([0]*25)
print(count_board)
f =  open('5x5_2_clear.txt', 'r')
for line in reversed(list(f)):
    content = line.rstrip().split(':')
    states = content[0].replace("X","1")
    states = states.replace(".","0")
    moves = content[1]
    if states.count('1')==19:
        it +=1
        board = np.array(list(states), dtype=int)
        board = board.reshape((5,5))
        board_list = getSymmetries(board)
        for b in board_list:
            count_board +=b
            it+=1
        #print(count_board)
            #board = board.reshape((5,5))
            #display(board)
        # if it ==100:
            #break

count_board = count_board.reshape((5,5))
print(it)
print(count_board)

count_board = np.array([0]*9)
board = np.array([0,1,0,0,1,1,1,0,1])
board = board.reshape((3,3))
board_list = getSymmetries(board)
for b in board_list:
    count_board +=b
count_board = count_board.reshape((3,3))
print(count_board)