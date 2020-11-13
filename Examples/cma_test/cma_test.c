#include <stdio.h>

#include "Thirdparty/cma/libxlnk_cma.h"

int main() {
    unsigned int *buf = (unsigned int*)cma_alloc(100, 0);

    printf("%d\n", cma_get_phy_addr((void*)buf));

    cma_free((void*)buf);

    return 0;
}