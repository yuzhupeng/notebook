



```c
#include <stdio.h>
#include <stdlib.h>

void cleanup1() {
    printf("Cleanup 1\n");
}

void cleanup2() {
    printf("Cleanup 2\n");
}

void cleanup3() {
    printf("Cleanup 3\n");
}

int main() {
    atexit(cleanup1);
    atexit(cleanup2);
    atexit(cleanup3);

    printf("Main function\n");
    return 0;
}

```

