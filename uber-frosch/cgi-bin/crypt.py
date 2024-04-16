import sys

def caesar(string:str, val:int) -> None:
    for c in string:
        if (c == None or c == '"' or c == ':'):
            continue
        print(chr(ord(c) + val), end='')

def main() -> int:
    if (len(sys.argv) == 1):
        return (1)
    caesar(sys.argv[1], 5)
    return (0)

if __name__ == "__main__":
    main()
