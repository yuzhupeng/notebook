

# strcmp



```c++
int fstrcmp(char* x, char* y) {
    int i;
    for (i=0; x[i]&&y[i]; i++) {
        if (x[i] != y[i]) {
            return 0;
        }
    }
    return 1;
}
```





# strlen



```c++
int fstrlen(char* x){
	int i;
	for (i = 0; x[i];){
		i++;
	}
	return &x[i] - x + 1;
}
```

