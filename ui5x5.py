from tkinter import *
master = Tk()

player=StringVar()


def get_state():
    state = 0
    for i in range(0,5):
        for j in range(0,5):
            state <<=1
            state |= board_boxes[4-i][4-j].get()
            print(board_boxes[4-i][4-j].get())
    return state

def check():
    state = get_state()
    print(state)

    ppp=0
    s=1
    for i in range(0,5):
        for j in range(0,5):
            if(state | s == state):
                ch = 'X'
                ppp += 1
            else:
                ch = cache[state | s]
            s <<= 1
            display_boxes[i][j].set(ch)
            
    player.set("From perspective of Player " + str(ppp % 2 + 1))

def make_board():
    
    BOARD_ROWS = BOARD_COLS = 5
    labelframe = LabelFrame(master)
    labelframe.pack(fill="both", expand="yes")
    boxes = []
    for i in range(0,5):
        b = []
        for j in range(0,5):
            b += [IntVar()]
            chk = Checkbutton(labelframe, variable = b[j], command=check).grid(row=i, column=j)
        boxes += [b,]

    return boxes

def make_display():
    
    BOARD_ROWS = BOARD_COLS = 5
    labelframe = LabelFrame(master)
    labelframe.pack(fill="both", expand="yes")
    boxes = []
    for i in range(0,5):
        b = []
        for j in range(0,5):
            b += [IntVar()]
            Label(labelframe, text="_", textvariable=b[j], width=2).grid(row=i, column=j)
        boxes += [b,]

    labelframe = LabelFrame(master)
    labelframe.pack(fill="both", expand="yes")
    Label(labelframe, textvariable=player).pack()
    return boxes


f = open("cache.dump", "r")
cache = f.readline()

board_boxes = make_board()
print(board_boxes)
display_boxes = make_display()
check()
mainloop()
