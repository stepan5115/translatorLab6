int add(int x, int y) {
    return x + y;
}

int main() {
    print(add(1, add(2, 3)));
    return 0;
}

main();