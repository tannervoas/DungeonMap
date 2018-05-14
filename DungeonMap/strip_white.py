ifile = open('Map1.txt')
ofile = open('GameMap1.txt','w')
first = ifile.readline()
first = first.strip().split()
ofile.write(first[0] + ' ' + first[1])
for line in ifile:
    line = line.strip().split()
    nline = ''
    for c in line:
        nline += c
    ofile.write('\n' + nline)

ofile.close()
ifile.close()
