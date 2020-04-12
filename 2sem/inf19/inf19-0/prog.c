#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

typedef double (*libfunc)(double);

int main(int argc, char* argv[]) {
    char* libfile_name = argv[1];
    char* libfunc_name = argv[2];
    void* dyn_lib = dlopen(libfile_name, RTLD_NOW);
    if (dyn_lib == NULL) {
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return 1;
    }
    libfunc func_ptr = dlsym(dyn_lib, libfunc_name);
    if (func_ptr == NULL) {
        fprintf(stderr, "dlsym error: %s\n", dlerror());
        return 1;
    }
    double readed;
    while (scanf("%lf", &readed) != EOF) {
        printf("%.3f\n", func_ptr(readed));
    }
    dlclose(dyn_lib);
    return 0;
}
