/*
 * GridTools
 *
 * Copyright (c) 2014-2021, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <hwmalloc/mpi/context.hpp>
#include <mpi.h>

namespace hwmalloc
{
namespace mpi
{
context::context(MPI_Comm comm)
: m_comm{comm}
{
    MPI_Info info;
    MPI_Info_create(&info);
    MPI_Info_set(info, "no_locks", "false");
    MPI_Win_create_dynamic(info, m_comm, &m_win);
    MPI_Info_free(&info);
    //MPI_Win_create_dynamic(MPI_INFO_NULL, m_comm, &m_win);
}

} // namespace mpi
} // namespace hwmalloc
