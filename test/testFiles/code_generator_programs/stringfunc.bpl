string truefalse(int x) {
    if (x) {
        return "true";
    }
    return "false";
}

void main() {
    write(truefalse(1));
    writeln();
    write(truefalse(0));
    writeln();
}
