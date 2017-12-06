#!/usr/bin/env python3

import sys
import fileinput

def main():
    verts, normals = [], []

    for line in fileinput.input():
        tokens = list(reversed([x for x in line.split(' ') if x]))
        ident = tokens.pop()
        tokens.reverse()
        if ident == "v":
            for elem in tokens:
                verts.append(float(elem))
        elif ident == "vn":
            for elem in tokens:
                normals.append(float(elem))
        elif ident == "n":
            for elem in tokens:
                v, n =

if __name__ == "__main__":
    main()
