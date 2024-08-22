import random

def generate_matrix(size):
    return [[random.randint(1, 9) for _ in range(size)] for _ in range(size)]

def write_matrix_to_file(matrix, filename):
    with open(filename, 'w') as file:
        for row in matrix:
            file.write(''.join(map(str, row)) + '\n')

def main():
    size = 100
    matrix = generate_matrix(size)
    filename = 'test/big.txt'
    write_matrix_to_file(matrix, filename)

if __name__ == "__main__":
    main()
