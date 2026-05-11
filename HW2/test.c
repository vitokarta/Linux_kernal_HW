#include <stdio.h>
#include <stdint.h>

// 印出 32-bit binary
void print_binary(uint32_t x) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (x >> i) & 1);
        if (i % 8 == 0) printf(" ");
    }
    printf("\n");
}

// 你的 bit reverse
uint32_t reverse_bits(uint32_t num) {
    uint32_t new = num;
    new = ((new & 0xffff0000) >> 16) | ((new & 0x0000ffff) << 16);
    new = ((new & 0xff00ff00) >> 8)  | ((new & 0x00ff00ff) << 8);
    new = ((new & 0xf0f0f0f0) >> 4)  | ((new & 0x0f0f0f0f) << 4);
    new = ((new & 0xcccccccc) >> 2)  | ((new & 0x33333333) << 2);
    new = ((new & 0xaaaaaaaa) >> 1)  | ((new & 0x55555555) << 1);
    return new;
}

int main() {
    uint32_t x = 0x12345678;

    printf("Original : 0x%08x\n", x);
    print_binary(x);

    uint32_t r = reverse_bits(x);

    printf("Reversed : 0x%08x\n", r);
    print_binary(r);

    return 0;
}