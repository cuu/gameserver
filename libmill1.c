#include <stdio.h>
#include <libmill.h>

coroutine void worker(int count, const char *text) {
    int i;
		while(1) {
        printf("%s\n", text);
        msleep(now() + 1000+count);
    }
}

int main() {
    go(worker(4, "a"));
    go(worker(2, "b"));
    go(worker(3, "c"));
		printf("after \n");
		
		
    msleep(now() + 4000);
    return 0;
}

