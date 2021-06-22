#include <gtest/gtest.h>

#include <hwmalloc/heap.hpp>
#include <hwmalloc/mpi/context.hpp>
#include <iostream>

TEST(mpi, some_test)
{
    hwmalloc::mpi::context c(MPI_COMM_WORLD);

    hwmalloc::heap<hwmalloc::mpi::context> h(&c);
   
    auto ptr = h.allocate(8, 0);

    std::cout << ptr.get() << std::endl;

    h.free(ptr);

}
