#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

const int NPROC = 10;
const int INTERVAL = 1000;

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

int main() {
    int to_child[NPROC][2];
    int to_parent[NPROC][2];

    for (int i = 0; i < NPROC; i++) {
        pipe(to_child[i]);
        pipe(to_parent[i]);
    }

    for (int i = 0; i < NPROC; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            for (int j = 0; j < NPROC; j++) {
                if (j != i) {
                    close(to_child[j][0]);
                    close(to_child[j][1]);
                    close(to_parent[j][0]);
                    close(to_parent[j][1]);
                }
            }
            close(to_child[i][1]);
            close(to_parent[i][0]);

            int range[2];
            read(to_child[i][0], range, sizeof(range));
            close(to_child[i][0]);

            for (int n = range[0]; n < range[1]; n++) {
                if (is_prime(n))
                    write(to_parent[i][1], &n, sizeof(n));
            }
            close(to_parent[i][1]);
            _exit(0);
        }
    }

    for (int i = 0; i < NPROC; i++) {
        close(to_child[i][0]);
        close(to_parent[i][1]);
        int range[2] = {i * INTERVAL, (i + 1) * INTERVAL};
        write(to_child[i][1], range, sizeof(range));
        close(to_child[i][1]);
    }

    for (int i = 0; i < NPROC; i++) {
        int p;
        while (read(to_parent[i][0], &p, sizeof(p)) > 0)
            cout << p << " ";
        close(to_parent[i][0]);
    }
    cout << endl;

    for (int i = 0; i < NPROC; i++)
        wait(nullptr);

    return 0;
}
