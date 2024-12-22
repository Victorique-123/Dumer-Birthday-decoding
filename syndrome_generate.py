#!/usr/bin/env python3

import sys
import random
import os
import math

prefix = os.getcwd() + "/SD/"

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def usage():
    eprint("ERROR the script expects 2 integer arguments: 'n' and 'seed'.")
    eprint("\b - 'n' is an integer corresponding to the size of the matrix: H will be of size n//2 * n")
    eprint("\b - 'seed' is an integer corresponding to the initial value of the random seed")
    eprint("This script generates an instance of the syndrome decoding problem.")
    eprint("This matrix H is given in systematic form. The identity part is omitted.")
    eprint("The instance is stored in 'Challenges/SD/SD_n_seed'.")

def dGV(n, k):
    d = 0
    aux = 2**(n-k)
    b = 1
    while aux >= 0:
        aux -= b
        d += 1
        b *= (n-d+1)
        b /= d
    return d

def generate_identity_matrix(size):
    return [[1 if i == j else 0 for j in range(size)] for i in range(size)]

def transpose_matrix(matrix):
    return [[matrix[j][i] for j in range(len(matrix))] for i in range(len(matrix[0]))]

def main(n, seed, case_number=1):
    k = n//2
    w = math.ceil(1.05 * dGV(n,k))
    random.seed(seed)
    
    # Generate H_transpose first (as in original code)
    H_transpose = []
    for i in range(n-k):
        row = []
        for j in range(k):
            row.append(random.randint(0,1))
        H_transpose.append(row)
    
    # Transpose H to get the correct orientation
    H = transpose_matrix(H_transpose)
    
    # Generate s vector
    s = []
    for j in range(k):
        s.append(random.randint(0,1))
    
    # Generate identity matrix
    I = generate_identity_matrix(k)
    
    # Write to file
    text = f"### TEST CASE {case_number} ###\n"
    text += f"{k} {n}\n"
    
    # Write [I | H] matrix
    for i in range(k):
        # Write I part
        row = I[i]
        # Write H part (now using transposed H)
        row.extend(H[i])
        # Convert to string with spaces
        line = " ".join(str(x) for x in row)
        text += line + "\n"
    
    # Write s vector
    text += " ".join(str(x) for x in s) + "\n"
    # Write w
    text += f"{w}\n"
    
    filename = prefix + "SD_" + str(n) + "_" + str(seed)
    file = open(filename, "w")
    file.write(text)
    file.close()

if __name__ == "__main__":
    # execute only if run as a script
    if len(sys.argv)!=3:
        usage()
        exit(1)
    try:
        n = int(sys.argv[1])
        seed = int(sys.argv[2])
    except:
        usage()
        exit(1)
    main(n, seed)