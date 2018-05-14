import random
file = open('RandMap3.txt','w')
file.write('1 0 0 0\n')
for i in range(10000):
    x = random.randint(-1000, 1000)
    y = random.randint(-1000, 1000)
    z = random.randint(-1000, 1000)
    hx = random.random() * 28 + 2
    hy = random.random() * 28 + 2
    hz = random.random() * 28 + 2
    rx = random.randint(-180, 180)
    ry = random.randint(-180, 180)
    rz = random.randint(-180, 180)
    vx = random.random() * 20 - 10
    vy = random.random() * 20 - 10
    vz = random.random() * 20 - 10
    lt = -1 * (random.random() * 1000)**0.5
    ut = (random.random() * 1000)**0.5
    s = '5 ' + str(x) + ' ' + str(y) + ' ' + str(z) + ' '
    s += str(hx) + ' ' + str(hy) + ' ' + str(hz) + ' '
    s += str(rx) + ' ' + str(ry) + ' ' + str(rz) + ' '
    s += str(vx) + ' ' + str(vy) + ' ' + str(vz) + ' ' + str(lt) + ' ' + str(ut) + '\n'
    file.write(s)
file.close()
    
    
