'''
Todo:
- if filling with 1 as the initial value, need to take 2's comp or something
'''


def _calc_int_len(x: int) -> int:
    """Calculates the number of bits needed to make an int `x`
    """
    int_len = 0
    while x:
        int_len += 1
        x >>= 1
    return int_len


def crc_remainder(
        x: int, 
        polynomial: int, 
        initial_filler: int
    ) -> int:
    if initial_filler != 0 and initial_filler != 1:
        raise ValueError("initial_filler must be 0 or 1")
    
    # Add padding to the end of x
    len_x: int = _calc_int_len(x)
    len_poly: int = _calc_int_len(polynomial)
    n_padding: int = len_poly - 1
    x <<= n_padding
    if initial_filler == 1:
        for i in range(n_padding):
            x |= (1 << i)
    
    # XOR Long division
    polynomial_arr: list = [int(p) for p in bin(polynomial)[2:]]
    for i in range(len_x + n_padding - 1, n_padding, -1):
        if x & (1 << i) == 0:
            continue
        
        for j, p in enumerate(polynomial_arr):
            idx = i - j
            a = (x & (1 << idx)) >> idx
            b = p
            c = ~(a ^ b)
            x ^= c << idx
    
    mask = 0
    for i in range(len_poly - 1):
        mask |= (1 << i)

    return x & mask

    
    # x_bitstr: str = bin(x)[2:]
    # len_x = len(x_bitstr)
    # polynomial_bitstr: str = bin(polynomial)[2:]
    # initial_filler_char: str = bin(initial_filler)[2:]
    # end_padding: str = initial_filler_char * (len(polynomial_bitstr) - 1)
    # x_padded_arr: str = list(x_bitstr + end_padding)
    
    # for i in range(len_x):
    #     if x_padded_arr[i] == '0':
    #         continue
        
    #     for j, p in enumerate(polynomial_bitstr):
    #         a = int(x_padded_arr[i + j])
    #         b = int(p)
    #         x_padded_arr[i + j] = str(a ^ b)
    
    # return int(''.join(x_padded_arr)[len_x:], 2) # return the remainder


def crc_check2(input_bitstring, polynomial_bitstring, check_value):
    """Calculate the CRC check of a string of bits using a chosen polynomial."""
    polynomial_bitstring = polynomial_bitstring.lstrip('0')
    len_input = len(input_bitstring)
    initial_padding = check_value
    input_padded_array = list(input_bitstring + initial_padding)
    while '1' in input_padded_array[:len_input]:
        cur_shift = input_padded_array.index('1')
        for i in range(len(polynomial_bitstring)):
            input_padded_array[cur_shift + i] \
            = str(int(polynomial_bitstring[i] != input_padded_array[cur_shift + i]))
    return ('1' not in ''.join(input_padded_array)[len_input:])


if __name__ == '__main__':
    x: int = 0b11010011101100
    polynomial: int = 0b1011
    initial_filler: int = 0b0

    crc = crc_remainder(x, polynomial, initial_filler)
    print(bin(crc))