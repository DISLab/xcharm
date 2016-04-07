// Copyright 2012 The Trustees of Indiana University.

// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Nick Edmonds
//           Andrew Lumsdaine
#ifndef BOOST_GRAPH_GRAPH500_GENERATOR_HPP
#define BOOST_GRAPH_GRAPH500_GENERATOR_HPP

//#include <boost/graph/graph_traits.hpp>
#include <graph_generator.hpp> // Graph500 generator 
//#include <boost/format.hpp>
#include <iostream>
#include <boost/shared_array.hpp>

namespace boost {

//  template<typename Graph>
  class graph500_iterator
/*    : public iterator_facade<
               graph500_iterator<Graph>,
               std::pair<typename graph_traits<Graph>::vertices_size_type,
                         typename graph_traits<Graph>::vertices_size_type>,
               std::input_iterator_tag,
               const 
                 std::pair<typename graph_traits<Graph>::vertices_size_type,
                           typename graph_traits<Graph>::vertices_size_type>&>*/
  {
//    typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
//    typedef typename graph_traits<Graph>::edges_size_type edges_size_type;

		typedef uint64_t vertices_size_type;
		typedef uint64_t edges_size_type;

  public:
    graph500_iterator() : scale(0), current(0) {}
    graph500_iterator(vertices_size_type scale, edges_size_type start_edge, 
		      uint64_t seed1, uint64_t seed2) 
      : scale(scale), current(start_edge), packed_edges(new packed_edge[1 << 16])
    {
      // make_mrg_seed() from Graph500
      seed[0] = (seed1 & 0x3FFFFFFF) + 1;
      seed[1] = ((seed1 >> 30) & 0x3FFFFFFF) + 1;
      seed[2] = (seed2 & 0x3FFFFFFF) + 1;
      seed[3] = ((seed2 >> 30) & 0x3FFFFFFF) + 1;
      seed[4] = ((seed2 >> 60) << 4) + (seed1 >> 60) + 1;
      generate_kronecker_range(seed, scale, start_edge / (1 << 16) * (1 << 16), start_edge / (1 << 16) * (1 << 16) + (1 << 16), packed_edges.get());
      update_cache(false);
    }

    void update_cache(bool run_from_increment) {
      if (run_from_increment && current % (1 << 16) == 0) {
        generate_kronecker_range(seed, scale, current, current + (1 << 16), packed_edges.get());
      }
      current_edge = std::make_pair(get_v0_from_edge(&packed_edges[current % (1 << 16)]),
				    get_v1_from_edge(&packed_edges[current % (1 << 16)])); 
    }

    const std::pair<vertices_size_type, vertices_size_type>&
    dereference() const { return current_edge; }

    void increment() {
      ++current;
      update_cache(true);
    }

    bool equal(const graph500_iterator& other) const
    { return current == other.current; }

  private:

    vertices_size_type scale;
    edges_size_type current;
    std::pair<vertices_size_type, vertices_size_type> current_edge;

    uint_fast32_t seed[5];
    shared_array<packed_edge> packed_edges;
  };
} // end namespace boost

#endif // BOOST_GRAPH_GRAPH500_GENERATOR_HPP
