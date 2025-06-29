/*
    IGraph library.
    Copyright (C) 2025  The igraph development team <igraph@igraph.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "igraph_components.h"
#include "igraph_error.h"
#include "igraph_structural.h"

static igraph_error_t igraph_i_percolate_edge(igraph_vector_int_t *links, igraph_vector_int_t *sizes, igraph_integer_t* biggest, igraph_integer_t a, igraph_integer_t b) {
    // find head of each
    // TODO: Path compression
    while (VECTOR(*links)[a] != a) {
        a = VECTOR(*links)[a];
    }
    while (VECTOR(*links)[b] != b) {
        b = VECTOR(*links)[b];
    }
    // make a child of b
    VECTOR(*links)[a] = b;
    VECTOR(*sizes)[b] += VECTOR(*sizes)[a];
    // if made new biggest component, update size
    if (VECTOR(*sizes)[b] >= *biggest) *biggest = VECTOR(*sizes)[b];
    return IGRAPH_SUCCESS;
}

static igraph_error_t igraph_i_edge_list_percolation(const igraph_vector_int_t *edges, igraph_vector_int_t* output, igraph_integer_t v_count) {
    igraph_integer_t biggest = 0;

    igraph_integer_t lower, upper;

    igraph_vector_int_minmax(edges, &lower, &upper);

    if (v_count < 0) v_count = upper;

    if (v_count < upper) IGRAPH_ERROR("Vertex count given is lower than highest referenced.",IGRAPH_EINVVID);
    if (lower < 0) IGRAPH_ERROR("Negative number in edge list.", IGRAPH_EINVVID);

    igraph_vector_int_t sizes;
    IGRAPH_CHECK(igraph_vector_int_init(&sizes, v_count));
    IGRAPH_FINALLY(igraph_vector_int_destroy, &sizes);

    igraph_vector_int_t links;
    IGRAPH_CHECK(igraph_vector_int_init(&links, v_count));
    IGRAPH_FINALLY(igraph_vector_int_destroy, &links);

    for (igraph_integer_t i = 0; i < upper; i++) {
        VECTOR(sizes)[i] = 1;
        VECTOR(links)[i] = i;
    }

    int edge_count = igraph_vector_int_size(edges);
    if (edge_count % 2 == 1) IGRAPH_ERROR("Invalid edge list, odd number of elements", IGRAPH_EINVAL);

    IGRAPH_CHECK(igraph_vector_int_resize(output, edge_count));

    for (igraph_integer_t i = 0; i < edge_count; i++) {
        igraph_i_percolate_edge(&links, &sizes, &biggest, VECTOR(*edges)[2*i], VECTOR(*edges)[2*i+1]);
        VECTOR(*output)[i] = biggest;
    }
    igraph_vector_int_destroy(&sizes);
    igraph_vector_int_destroy(&links);
    IGRAPH_FINALLY_CLEAN(2);

    return IGRAPH_SUCCESS;
}

IGRAPH_EXPORT igraph_error_t igraph_bond_percolation(const igraph_t *graph, igraph_vector_t * output) {
    return IGRAPH_SUCCESS;
}

IGRAPH_EXPORT igraph_error_t igraph_site_percolation(const igraph_t *graph, igraph_vector_t * output) {
    return IGRAPH_SUCCESS;
}

