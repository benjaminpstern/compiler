void switch (int A[], int i, int j) {
    int temp;
    if (i != j) {
        temp = A[i];
        A[i] = A[j];
        A[j] = temp;
    }
}

void writeArray(int A[], int size) {
    int i;
    i = 0;
    while (i < size) {
        write(A[i]);
        i = i + 1;
    }
}

void range(int x[], int n) {
    int i;
    i = 0;
    while (i < n) {
        x[i] = i;
        i = i + 1;
    }
}
void sort( int A[], int first, int last ) {
    int i;
    int j;
    int small;

    i = first;
    while (i < last-1) {
        /* get smallest remaining value and put it at position i */
        j = i;
        small = j;
        while (j < last) {
            if (A[j] < A[small]) {
                small = j;
            }
            j = j+1;
        }
        switch(A, i, small);
        i = i + 1;
    }
}
void main() {
    int A[10];
    range(A, 10);
    switch(A, 1, 2);
    switch(A, 6, 3);
    writeArray(A, 10);
    writeln();
    sort(A, 0, 10);
    writeArray(A, 10);
}
