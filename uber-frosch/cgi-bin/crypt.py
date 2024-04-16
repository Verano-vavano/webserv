import sys

# Impossible a dechiffrer, c'est une vraie muraille.
# Le summum de la securite, jamais egalee
def caesar(string:str, val:int) -> None:
    user = string.find('"user"')
    password = string.find('"password"')
    i = 0
    while i < len(string):
        if (i == user):
            print('"user"', end='')
            i += 6
            continue
        elif (i == password):
            print('"password"', end='')
            i += 10
            continue
        elif (string[i].isalpha()):
            c = ord(string[i]) + val
            if (string[i].isupper()):
                if (c > ord('Z')):
                    c -= 26
            else:
                if (c > ord('z')):
                    c -= 26
            print(chr(c), end='')
        elif (string[i].isdigit()):
            print(chr(ord('f') + ord(string[i]) - ord('0')), end='')
        else:
            print(string[i], end='')
        i += 1

def main() -> int:
    if (len(sys.argv) == 1):
        return (1)
    caesar(sys.argv[1], 5)
    return (0)

if __name__ == "__main__":
    main()
