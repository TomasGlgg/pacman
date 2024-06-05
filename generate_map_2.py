from random import choice, randint


W, H = 15, 20

def fill_map():
    return [[1]*W for _ in range(H)]


objects = (
    ((0, 0), (0, 1), (1, 0), (1, 1)),  # квадрат
    ((0, 0), (1, 0), (2, 0), (1, 1), (2, 1)),  # T
    ((0, 0), (0, 1), (0, 2), (1, 2)),  # L
    ((0, 0), (0, 1), (1, 1), (1, 2)),
    ((0, 0), (0, 1), (0, 2), (0, 3)),  # |
)

count = 30

def trace_block(map_, x, y):
    map_[y][x] = 0
    if x != 0 and map_[y][x-1]:
        trace_block(map_, x-1, y)
    if y != 0 and map_[y-1][x]:
        trace_block(map_, x, y-1)
    if x + 1 != W and map_[y][x+1]:
        trace_block(map_, x+1, y)
    if y + 1 != H and map_[y+1][x]:
        trace_block(map_, x, y+1)

def find_block():
    for x in range(W):
        for y in range(H):
            if MAP[y][x]:
                return x, y

def check_map():
    x, y = find_block()
    print(x, y)
    map_ = [MAP[y].copy() for y in range(H)]
    trace_block(map_, x, y)
    print(map_[y][x])
    print('Traced')
    show(map_)
    for x in range(W):
        for y in range(H):
            if map_[y][x]:
                return False
    return True

def show(map_):
    print('---------------------------')
    for x in range(W):
        for y in range(H):
            print('#' if map_[y][x] else ' ', end='')
        print()

def inject(x, y, obj):
    print(x, y, obj)
    for block_x, block_y in obj:
        block_x += x
        block_y += y
        if block_x < W and block_y < H:
            MAP[block_y][block_x] = 0

def generate():
    for _ in range(count):
        x, y = randint(0, W), randint(0, H)
        obj = choice(objects)

        inject(x, y, obj)
        show(MAP)


done = False
i = 0
while not done:
    MAP = fill_map()
    generate()
    i += 1
    done = check_map()
    print(i, done)
