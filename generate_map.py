from random import choice, random


W, H = 15, 20
MAP = [[0]*W for _ in range(H)]

def trace_block(map_, x, y):
    MAP[y][x] = 0
    if x != 0 and MAP[y][x-1]:
        trace_block(map_, x-1, y)
    if y != 0 and MAP[y-1][x]:
        trace_block(map_, x, y-1)
    if x + 1 != W and MAP[y][x+1]:
        trace_block(map_, x+1, y)
    if y + 1 != H and MAP[y+1][x]:
        trace_block(map_, x, y+1)

def find_block():
    for x in range(W):
        for y in range(H):
            if MAP[y][x]:
                return x, y

def check_map():
    x, yfind_block()
    map_ = [MAP[y].copy() for y in range(H)]
    trace_block(map_, x, y)
    for x in range(W):
        for y in range(H):
            if MAP[y][x]:
                return False
    return True

def check_block(x, y):
    sum_ = 0
    if x != 0:
        sum_ += MAP[y][x-1]
    if y != 0:
        sum_ += MAP[y-1][x]
    if x + 1 != W:
        sum_ += MAP[y][x+1]
    if y + 1 != H:
        sum_ += MAP[y+1][x]
    return sum_

def show():
    print('---------------------------')
    for x in range(W):
        for y in range(H):
            print('#' if MAP[y][x] else ' ', end='')
        print()

def generate():
    blocks = []
    for x in range(W):
        for y in range(H):
            if random() < 0.2:
                MAP[y][x] = 1
                blocks.append((y, x))

    show()

    while blocks:
        block = choice(blocks)
        blocks.remove(block)
        y, x = block

        min_value, min_block = float('inf'), None

        if x != 0:
            b = (x-1, y)
            new_value = check_block(*b)
            if new_value < min_value:
                min_value = new_value
                min_block = b
        if y != 0:
            b = (x, y-1)
            new_value = check_block(*b)
            if new_value < min_value:
                min_value = new_value
                min_block = b
        if x + 1 != W:
            b = (x+1, y)
            new_value = check_block(*b)
            if new_value < min_value:
                min_value = new_value
                min_block = b
        if y + 1 != H:
            b = (x, y+1)
            new_value = check_block(*b)
            if new_value < min_value:
                min_value = new_value
                min_block = b

        if min_value <= 2:
            x, y = min_block
            MAP[y][x] = 1

    show()
    print(x, y)
    trace_block(x, y)
    show()

generate()
