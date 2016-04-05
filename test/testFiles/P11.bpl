/* A program to compute factorials */
int fact( int n) {
	if (n == 0)
		return 1;
	else
		return n*fact(n-1);
}

void main(void) {
	int x;
    int y;
	x = 0;
	while ((x=x+1) < 10) {
		write(x);
		write(fact(x));
		writeln();
	}
}



