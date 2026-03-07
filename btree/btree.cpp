//-----------------------------------------------------------------------------
// Desc: B+Tree library
//
// Copyright © 2019 J McIntosh
//
//   This file is part of redB.
//
//   redB is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; version 3 of the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with redB.  If not, see <https://www.gnu.org/licenses/>.
//-----------------------------------------------------------------------------

#include "btree.h"

  /* ELEMENT: An element of a non-leaf node is a key, where as, an element of a
   * leaf node includes a combined block-offset/row-position and a key. So,
   * when moving an element to a non-leaf node from a leaf node or vis-versa
   * only the key is COPIED; combined block-offset/row-position values always
   * and only exist in leaf nodes; child-offset values always and only exist in
   * non-leaf nodes. Finally, when moving an element from non-leaf node to non-
   * leaf node only the key is COPIED--as would be expected. NOTE: all keys
   * exist at the leaf level to facilitate horizontal traversal of leaf nodes
   * where every key is present; some keys exist at both the leaf level and one
   * non-leaf level to facilitate the B+Tree algorithm.
   */

namespace red::tree {

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS ELEMENT: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * STRUCT ELEMENT: Copy Assignment
   */
  element& element::operator=(element const& e) {
    if (this == &e) return *this;
    m_row_position  = e.m_row_position;
    m_block_offset  = e.m_block_offset;
    m_ndx_name      = e.m_ndx_name;
    m_key           = e.m_key;
    return *this;
  }

  /****************************************************************************
   * STRUCT ELEMENT: Move Assignment
   */
  element& element::operator=(element&& e) {
    if (this == &e) return *this;
    m_row_position  = e.m_row_position;
    m_block_offset  = e.m_block_offset;
    m_ndx_name      = e.m_ndx_name;
    m_key           = e.m_key;
    return *this;
  }

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream& os, element& e) {
      os << e.m_row_position << "\t0x" << std::hex << e.m_block_offset << '\t'
        << std::dec << e.m_ndx_name << '\t' << e.m_key;
      return os;
  }
#endif

  /****************************************************************************
   * STRUCT ELEMENT: Check validity of element
   */
  bool element::is_valid () const {
    return (not (m_row_position == INVALID_ELEMENT.m_row_position
        || m_block_offset == INVALID_ELEMENT.m_block_offset
        || m_ndx_name == INVALID_ELEMENT.m_ndx_name
        || m_key == INVALID_ELEMENT.m_key));
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DATA: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS DATA: Copy Assignment
   */
  data& data::operator=(data const& o) {
    if (this == &o) return *this;
    m_dead        = o.m_dead;
    m_offset      = o.m_offset;
    m_tree        = o.m_tree;
    m_access_time = o.m_access_time;
    m_buffer      = o.m_buffer;
    return *this;
  }

  /****************************************************************************
   * CLASS DATA: Move Assignment
   */
  data& data::operator=(data&& o) {
    if (this == &o) return *this;
    m_dead        = o.m_dead;
    m_offset      = o.m_offset;
    m_tree        = o.m_tree;
    m_access_time = o.m_access_time;
    m_buffer      = o.m_buffer;
    return *this;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS DATA: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Commit data-block; flush data-block.
   */
  void data::commit () {
    m_tree->m_block_cache->flush_block(m_offset);
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS NODE: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /***************************************************************************
   * CLASS NODE: Copy Assignment
   */
  node& node::operator=(node const& o) {
    if (this == &o) return *this;
    m_leaf        = o.m_leaf;
    m_dead        = o.m_dead;
    m_num_key     = o.m_num_key;
    m_offset      = o.m_offset;
    m_tree        = o.m_tree;
    m_ndx_name    = o.m_ndx_name;
    m_access_time = o.m_access_time;
    m_buffer      = o.m_buffer;
    return *this;
  }

  /***************************************************************************
   * CLASS NODE: Move Assignment
   */
  node& node::operator=(node&& o) {
    if (this == &o) return *this;
    m_leaf        = o.m_leaf;
    m_dead        = o.m_dead;
    m_num_key     = o.m_num_key;
    m_offset      = o.m_offset;
    m_tree        = o.m_tree;
    m_ndx_name    = o.m_ndx_name;
    m_access_time = o.m_access_time;
    m_buffer      = o.m_buffer;
    return *this;
  }

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream& os, std::shared_ptr<node> o)
  {
    os << "\no: m_offset:\t" << o->m_offset
      << "\n\tm_leaf:\t" << std::boolalpha << o->m_leaf
      << "\n\tm_ndx_name:\t" << o->m_ndx_name
      << "\n\tm_num_key:\t" << o->m_num_key
      << "\n\tm_buffer.capacity:\t" << o->m_buffer.capacity ();
      return os;
  }
#endif

  /****************************************************************************
   * Find the key and return the matching element or next element or end ele-
   * ment.
   */
  iterator node::find (std::string const& key) {
    int32_t i = find_key(key);

    if (get_element(i).m_key == key)
      return iterator {i, m_offset, m_ndx_name, m_tree};

    return (m_leaf ? m_tree->end(m_ndx_name) : child_at(i)->find(key));
  }
#ifdef BTREE_DEBUG
  /****************************************************************************
   * Traverse the tree by moving up and down the tree. Loop through each child
   * node recursing the sub-tree rooted by each child; then traversing each
   * element in the node retrieving and outputing the corresponding row of data
   * or "__DEAD_ROW__" should a row be marked dead (which should not happen).
   * The last child in each node is not traversed within the loop, but after
   * the loop.
   */
  void node::tree_traverse (int32_t& x) {
    int32_t i;
    for (i = 0; i < m_num_key; ++i) {
      // if this node is not a leaf, then before printing key-element [ i ],
      // traverse the subtree rooted with child-element [ i ]
      if (!m_leaf) child_at(i)->tree_traverse(x);
      else {
        auto e = get_element(i);

        auto d = m_tree->get_data(e.m_block_offset);

        int32_t row_size =
          static_cast<int32_t>(d->m_buffer.get_int16(e.m_row_position));

        bool row_live = static_cast<bool>(d->m_buffer.get());

        if (!row_live) std::cerr << "__DEAD_ROW__\t";

        d->m_buffer.index(e.m_row_position);

        row rw{m_tree->get_schema(), d->m_buffer};

        int32_t empno = std::stoi(rw.value_at(0));
        std::cout << "\"" << rw.value_at(0) << "\", \""
          << rw.value_at(1) << "\", \""
          << rw.value_at(2) << "\", \""
          << rw.value_at(3) << '\"';

        if (empno != x) {
          std::cout << "  >>> expected " << x << " got " << empno << '\n';
          x = empno;
        } else std::cout << '\n';

        ++x;
      }
    }

    if (!m_leaf) child_at(i)->tree_traverse(x);
  }

  /****************************************************************************
   * Traverse the tree by moving down the tree to the leaf level then across
   * the tree from leaf to leaf. First, traverse down the left edge of the tree
   * until a leaf node is encountered. The traverse from leaf node to leaf node
   * until and including the last leaf node. The rest of the function is ident-
   * ical to tree_traverse().
   */
  void node::leaf_traverse (int32_t& x) {
    if (m_leaf) {
      int32_t i;
      for (i = 0; i < m_num_key; ++i, ++x) {
        auto e = get_element(i);

        auto d = m_tree->get_data(e.m_block_offset);

        int32_t row_size =
          static_cast<int32_t>(d->m_buffer.get_int16(e.m_row_position));

        bool row_live = static_cast<bool>(d->m_buffer.get());

        if (!row_live) std::cerr << "__DEAD_ROW__\t";

        d->m_buffer.index(e.m_row_position);

        row rw{m_tree->get_schema(), d->m_buffer};

        std::cout << "\"" << rw.value_at(0) << "\", \""
          << rw.value_at(1) << "\", \""
          << rw.value_at(2) << "\", \""
          << rw.value_at(3) << '\"';

        int32_t empno = std::stoi(rw.value_at(0));
        if (empno != x) {
          std::cout << "  >>> expected " << x << " got " << empno << '\n';
          x = empno;
        } else std::cout << '\n';
      }
      // Traverse next sibling (leaf) node.
      std::shared_ptr<node> next;
      if ((next = get_next_sibling()) != nullptr) next->leaf_traverse(x);
    }
    else child_at(0)->leaf_traverse(x); // traverse to first leaf node
  }
#endif
  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS NODE: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Move element [ n - 1 ] of parent to element [ 0 ] of child [ n ]. If
   * child [ n ] is a non-leaf node then move child-offset [ num_key ] of
   * child [ n - 1 ] to child-offset [ 0 ] of child [ n ]. Then move element
   * [ num_key - 1 ] of child [ n - 1 ] to element [ n - 1 ] of parent.  The
   * number of keys in child [ n ] is incremented by one and the number of keys
   * in child [ n - 1 ] is decremented by one.
   */
  void node::borrow_from_prev (int32_t n) {
    auto child = child_at(n);       // child [ n ]
    auto sibling = child_at(n - 1); // child [ n - 1 ]

    // move all elements of child [ n ] one position to the right
    for (int32_t i = child->m_num_key - 1; i >= 0; --i)
      child->put_element(i + 1, child->get_element(i));

    // move element [ n - 1 ] of parent to element [ 0 ] of child [ n ]
    child->put_element(0, get_element(n - 1));

    // increment number of keys in child [ n ]
    child->inc_num_key(1);

    // if child [ n ] is not a leaf, move all its child-offsets one step to
    // the right
    if (!child->m_leaf) {
      for (int32_t i = child->m_num_key; i >= 0; --i)
        child->put_child_offset(i + 1, child->get_child_offset(i));

      // move sibling's last child-offset to child's first child-offset
      child->put_child_offset(0,
          sibling->get_child_offset(sibling->m_num_key));
    }

    // move the key from sibling to the parent
    put_element(n - 1, sibling->get_element(sibling->m_num_key - 1));

    // decrement number of keys in sibling
    sibling->dec_num_key(1);
  }

  /****************************************************************************
   * Move element [ n ] of parent to element [ num_key ] of child [ n ]. If
   * child [ n ] is a non-leaf node then move child-offset [ 0 ] of child
   * [ n + 1 ] to child-offset [ num_key + 1 ] of child [ n ]. Then move
   * element [ 0 ] of child [ n + 1 ] to element [ n ] of parent.  The number
   * of keys in child [ n ] is incremented by one and the number of keys in
   * child [ n + 1 ] is decremented by one.
   */
  void node::borrow_from_next (int32_t n) {
    auto child = child_at(n);       // child [ n ]
    auto sibling = child_at(n + 1); // child [ n + 1 ]

    // move element [ n ] of parent to element [ num_key ] of child [ n ]
    child->put_element(child->m_num_key, get_element(n));

    // move child-offset [ 0 ] of child [ n + 1 ] to child-offset
    // [ num_key + 1 ] of child [ n ]
    if (!child->m_leaf)
      child->put_child_offset(child->m_num_key + 1,
          sibling->get_child_offset(0));

    // move element [ 0 ] of sibling to element [ n ] of parent
    put_element(n, sibling->get_element(0));

    // move all remaining elements of child [ n + 1 ] one position left
    for (int32_t i = 1; i < sibling->m_num_key; ++i)
      sibling->put_element(i - 1, sibling->get_element(i));

    // move all remaining child-offsets of child [ n + 1 ] one position left
    if (!sibling->m_leaf)
      for (int32_t i = 1; i <= sibling->m_num_key; ++i)
        sibling->put_child_offset(i - 1, sibling->get_child_offset(i));

    // increment number of keys in child [ n ]
    child->inc_num_key(1);

    // decrement number of keys in child [ n + 1 ]
    sibling->dec_num_key(1);
  }

  /****************************************************************************
   * Return the child node at index (n).
   */
  std::shared_ptr<node> node::child_at (int32_t n) {
    return m_tree->get_node(m_ndx_name,
        static_cast<off_t>(get_child_offset(n)));
  }

  /****************************************************************************
   * Commit node meta-data members to the buffer.
   */
  void node::commit () {
    m_buffer.put(IS_LEAF_POS, static_cast<uint8_t>(m_leaf));
    m_buffer.put_int16(NUM_KEY_POS, static_cast<int16_t>(m_num_key));
  }

  /****************************************************************************
   * Decrement by (n) the number of keys in this node.
   */
  void node::dec_num_key (int32_t n) {
    m_num_key -= n;
    flush();
  }

  /****************************************************************************
   * If child [ n ] has less than (min-dergree - 1) keys then borrow a key from
   * an adjacent sibling. If neither sibling has enough keys then merge child
   * [ n ] with a sibling: if child [ n ] is not the last child then merge
   * child [ n ] with child [ n + 1 ]; else merge child [ n - 1 ] with child
   * [ n ].
   */
  void node::fill (int32_t n) {
    int32_t const min_degree = m_tree->get_index(m_ndx_name).order() / 2;

    // if child [ n - 1 ] has more than (min_degree - 1) keys, borrow a key
    // from that child
    if (n != 0 && child_at (n - 1)->m_num_key >= min_degree)
      borrow_from_prev(n);

    // if child [ n + 1 ] has more than (min_degree - 1) keys, borrow a key
    // from it
    else if (n != m_num_key && child_at(n + 1)->m_num_key >= min_degree)
      borrow_from_next(n);

    // if child [ n ] is not the last child then merge it with child [ n + 1 ]
    // else merge it with child [ n - 1 ]
    else if (n != m_num_key)
      merge (n);
    else
      merge (n - 1);
  }

  /****************************************************************************
   * The classic binary search with a twist; if the target key is not present
   * in this node then return the index of the lexographically lowest key that
   * is still greater than the target key.
   */
  int32_t node::find_key (std::string const& key) {
    int32_t lo = 0;
    int32_t hi = m_num_key - 1;
    int32_t cond;
    int32_t alt_mid = m_num_key; // index of lexographically lowest
                                      // key greater than target key
    while (lo <= hi) {
      int32_t mid = (lo + hi) / 2;
      cond = key.compare(get_element(mid).m_key);
      if (cond < 0) {
        hi = mid - 1;
        alt_mid = mid;
      } else if (cond > 0) lo = mid + 1;
      else return mid;
    }
    return alt_mid;
  }

  /****************************************************************************
   * Flush node-block.
   */
  void node::flush () {
    commit();
    m_tree->m_block_cache->flush_block(m_offset);
  }

  /****************************************************************************
   * Return child_offset [ n ] of node.
   */
  int64_t node::get_child_offset (int32_t n) {
    int32_t VECTOR_ELEMENT_SZ = red::tree::CHILD_OFFSET_SZ
      + m_tree->get_index(m_ndx_name).key_size();

    int32_t position = red::tree::ELEMENT_VECTOR_POS + n * VECTOR_ELEMENT_SZ;

    return m_buffer.get_int64(position);
  }

  /****************************************************************************
   * Return element [ n ] of node
   */
  element node::get_element (int32_t n) {
    // compute vector element size
    int32_t VECTOR_ELEMENT_SZ
      = (m_leaf ? red::tree::BLOCK_OFFSET_SZ : red::tree::CHILD_OFFSET_SZ)
      + m_tree->get_index(m_ndx_name).key_size();
    // compute position of element [ n ]
    int32_t position = red::tree::ELEMENT_VECTOR_POS + n * VECTOR_ELEMENT_SZ;
    // get key size
    int32_t k_size = m_tree->get_index(m_ndx_name).key_size();

    off_t block_offset {0};
    int32_t row_position {0};
    std::string key;

    if (m_leaf) {
      // get combined offset
      int64_t combined_offset = m_buffer
        .get_int64(position + red::tree::BLOCK_OFFSET_POS);
      // compute row position
      row_position = static_cast<int32_t>(combined_offset
          % static_cast<int64_t>(m_tree->get_header().block_size()));
      // compute block offset
      block_offset = static_cast<off_t>(combined_offset
          - static_cast<int64_t>(row_position));
    }
    // get key
    key = m_buffer.get_varchar(position + red::tree::KEY_POS, k_size);

    return element{row_position, block_offset, m_ndx_name, key};
  }

  /****************************************************************************
   * Get successor to key [ n ]. First get child [ n + 1 ] and continue with
   * the first child at each level until a leaf node is encountered. Return the
   * first element in the leaf node.
   */
  element node::get_next_element (int32_t n) {
    auto child = child_at(n + 1);
    // keep moving down and toward the left till we find a leaf
    while (!child->m_leaf)
      child = child_at(0);
    // return the first key of the leaf
    return child->get_element(0);
  }

  /****************************************************************************
   * Return next sibling node.
   */
  std::shared_ptr<node> node::get_next_sibling () {
    off_t offset = static_cast<off_t>(m_buffer
        .get_int64(red::tree::NEXT_SIBLING_POS));
    return (offset == red::tree::INVALID_SIBLING_REF ? nullptr
        : m_tree->get_node(m_ndx_name, offset));
  }

  /****************************************************************************
  * Get predecessor to key [ n ]. First get child [ n ] and continue with the
  * last child at each level until a leaf node is encountered. NOTE: As a side
  * effect of all keys begin present at the leaf level, key [ n ] in this sec-
  * nario is also key [ num_key - 1 ] at the leaf level; so, we return element 
  * [ num_key - 2 ].
  */
  element node::get_prev_element (int32_t n) {
    auto child = child_at(n);
    // keep moving down and toward the right till we find a leaf
    while (!child->m_leaf)
      child = child_at(child->m_num_key);
    /* 20181005: Modification to support all keys present in leaves.
     * return the [second to] last key of the leaf
     * Prohibited: return child->get_element(child->m_num_key - 1); */
    return child->get_element(child->m_num_key - 2);
  }

  /****************************************************************************
   * Return previous sibling node.
   */
  std::shared_ptr<node> node::get_prev_sibling () {
    off_t offset = static_cast<off_t>(m_buffer
        .get_int64(red::tree::PREV_SIBLING_POS));
    return (offset == red::tree::INVALID_SIBLING_REF ? nullptr
        : m_tree->get_node(m_ndx_name, offset));
  }

  /****************************************************************************
   * Increment by (n) the number of keys in this node.
   */
  void node::inc_num_key (int32_t n) {
    m_num_key += n;
    flush();
  }

  /****************************************************************************
   * Insert an element into a non-full node. First get the index of the right-
   * most key. If node is a leaf, starting with the rightmost element, move el-
   * ements one position right until a key is found that is less than new key.
   * Then insert the new element at the current vacant position. Then increment
   * the number of keys in this node by one. If node is a non-leaf, first find
   * the child that will receive the new element. If that child is full then
   * split the child. Next check if key [ i + 1 ] is greater than the new key
   * and increment (i) accordingly. Finally, get child [ i + 1 ] and start the
   * process all over again.
   */
  void node::insert_non_full (element& elem) {
    // initialize index as index of rightmost key
    int32_t i = m_num_key - 1;

    if (m_leaf) {
      // find/make room for new element
      for (; i >= 0 && get_element(i) > elem; --i)
        put_element(i + 1, get_element(i));

      // insert new element at found index
      put_element(i + 1, elem);

      // increment number of elements
      inc_num_key(1);
    } else {
      // find child that will receive new key
      for (; i >= 0 && get_element(i) > elem; --i);

      // get child node
      auto child = child_at(i + 1);

      // is found child full
      if (child->is_full()) {
        split_child(i + 1, child);

        // after split the middle element of child [ i ] goes up and
        // child [ i ] is split into also. See which of the two is going to
        // have the new key
        if (get_element(i + 1) < elem) ++i;
      }
      // get child node
      child = child_at(i + 1);

      child->insert_non_full(elem);
    }
  }
 
  /****************************************************************************
   * Is the node full? Is the number of keys greater than or equal to the order
   * of the tree minus one.
   */
  bool node::is_full () {
    // order is always even
    // minimum degree (t) is (order / 2)
    // never less than t - 1 keys in a node
    // never more than 2t - 1 keys in a node
    return (m_num_key >= m_tree->get_index(m_ndx_name).order() - 1);
  }

  /****************************************************************************
   * merge child [ n ] with child [ n + 1 ] and free child [ n + 1 ]. Move
   * key [ n ] of parent to key [ min_degree - 1 ] of child [ n ]. Then shift
   * all keys in parent (after key [ n ]) one position left. Next, move all
   * child-offsets in parent (after child-offset [ n + 1 ]) one position left.
   * Next, copy all keys from child [ n + 1 ] to end of child [ n ] starting
   * at postion min_degree. If child is a non-leaf node then, copy all child-
   * offsets from child [ n + 1 ] to child [ n ] starting at position
   * min_degree. Finally, increment the number of keys in child [ n ] by the
   * number keys in child [ n + 1 ] plus one; decrement the mumber keys in
   * parent by one; and set child [ n + 1 ] dead flag.
   */
  void node::merge (int32_t n) {
    auto child = child_at(n);
    auto sibling = child_at(n + 1);

    int32_t const min_degree = m_tree->get_index(m_ndx_name).order() / 2;

    // move key [ n ] from parent to key [ min_degree - 1 ] child [ n ]
    child->put_element(min_degree - 1, get_element(n));

    // move all remaining keys in parent one position left; move every
    // key after key [ n ] on position left
    for (int32_t i = n + 1; i < m_num_key; ++i)
      put_element(i - 1, get_element(i));

    // move all child-offsets in parent one position left; move every child-
    // offset after child-offset [ n + 1 ] one position left
    for (int32_t i = n + 2; i < m_num_key; ++i)
      put_child_offset(i - 1, get_child_offset(i));

    // append keys from child [ n + 1 ] to end of child [ n ]; begin appending
    // keys at position min_degree in child [ n ]
    for (int32_t i = 0; i < sibling->m_num_key; ++i)
      child->put_element(min_degree + i, sibling->get_element(i));

    // append child-offsets from child [ n + 1 ] to end of child [ n ]; begin
    // appending child-offsets at position min_degree in child [ n ]
    if (!child->m_leaf)
      for (int32_t i = 0; i <= sibling->m_num_key; ++i)
        child->put_child_offset(min_degree + i,
            sibling->get_child_offset(i));

    // increment key count in child [ n ]
    child->inc_num_key (sibling->m_num_key + 1);

    // decrement key count in parent
    dec_num_key(1);

    // set next-sibling-reference of child [ n ] with next-sibling-reference of
    // child [ n + 1 ]
    set_next_sibling_offset(child, get_next_sibling_offset(sibling));

    // set child [ n + 1 ] to dead
    sibling->m_dead = true;
  }

  /****************************************************************************
   * Put a child-offset into the child-offset [ n ] of this node.
   */
  void node::put_child_offset (int32_t n, int64_t offset) {
    int32_t VECTOR_ELEMENT_SZ = red::tree::CHILD_OFFSET_SZ
      + m_tree->get_index(m_ndx_name).key_size();

    int32_t position = red::tree::ELEMENT_VECTOR_POS + n * VECTOR_ELEMENT_SZ;

    m_buffer.put_int64(position, offset);

    flush();
  }


  /****************************************************************************
   * Put an element into element [ n ] of this node.
   */
  void node::put_element (int32_t n, element const& elem) {
    // compute vector element size
    int32_t VECTOR_ELEMENT_SZ = (m_leaf ? red::tree::BLOCK_OFFSET_SZ
        : red::tree::CHILD_OFFSET_SZ)
      + m_tree->get_index(m_ndx_name).key_size();
    // compute position of element [ n ]
    int32_t position = red::tree::ELEMENT_VECTOR_POS + n * VECTOR_ELEMENT_SZ;

    if (m_leaf) {
      // combine block offset and row position
      int64_t combined_offset
        = static_cast<int64_t>(elem.m_block_offset)
        + static_cast<int64_t>(elem.m_row_position);
      // put combined offset in buffer
      m_buffer.put_int64(position + red::tree::BLOCK_OFFSET_POS,
          combined_offset);
    }
    // put key in buffer
    m_buffer.put_varchar(position + red::tree::KEY_POS, elem.m_key);

    flush();
  }

  /****************************************************************************
   * Remove the row identified by key from the subtree rooted by this node.
   * Start by finding the key in this node. If the key is present in this node
   * then remove the key; else if this is a leaf node the key does not exist in
   * the tree so notifiy user and return; otherwise, set a flag to indicate
   * whether the key is present in the last child of this node.
   * node. If the child that roots the sub-tree where the key is expected to
   * exist has less than min-degree keys then fill that child. Continue with
   * removing the key from the sub-tree rooted by child [ n ] (which might have
   * been merged with an adjacent child so modify (n) accordingly).
   * 
   * 20181005: Modification to support all keys present in leaves.  Added para-
   * meter reference of type element (ret)
   */
  void node::remove (element& ret, std::string const& key) {
    // find the key in this node, if present
    int32_t n = find_key(key);
    // if the target key is present in this node then precede to remove the key
    if (n < m_num_key && get_element(n) == key) {
      if (m_leaf)
        remove_from_leaf(ret, n);
      else
        remove_from_non_leaf(ret, n);
    } else {
      // if this is a leaf node then key is not present in the tree
      if (m_leaf) return;

      // The key to be removed is present in the sub-tree rooted by this node.
      // The flag indicates whether the key should be present in the sub-tree
      // rooted by the last child of this node.
      bool flag = (n == m_num_key);

      int32_t const min_degree = m_tree->get_index(m_ndx_name).order() / 2;

      // if the child that roots the sub-tree, where the key is expected to
      // exist, has less than min_degree keys, we fill that child
      if (child_at (n)->m_num_key < min_degree)
        fill(n);

      // If the last child has been merged, it must have merged with the
      // previous child and so we recurse on child [ n - 1 ]. Else, we recurse
      // on child [ n ] which now has atleast min_degree keys.
      if (flag && n > m_num_key)
        child_at(n - 1)->remove(ret, key);
      else
        child_at(n)->remove(ret, key);
    }
  }

  /****************************************************************************
   * Remove element [ n ] from this leaf node. First copy the target element to
   * the return value (ret). Next, move all the elements right of the target
   * element one position left. Last, decrement the number of keys in this node
   * by one.
   *
   * 20181005: Modification to support all keys present in leaves. Added para-
   * meter reference of type element (ret).
   */
  void node::remove_from_leaf (element& ret, int32_t n) {
    // copy target element
    ret = get_element(n);
    // move all the elements beyond the nth element one position left
    for (int32_t i = n + 1; i < m_num_key; ++i)
      put_element(i - 1, get_element(i));
    // decrement the number of keys (elements) in this node
    dec_num_key(1);
  }

  /****************************************************************************
   * Remove key [ n ] from this non-leaf node. First make a copy the target el-
   * ement. If child [ n ] has at least 2t-1 (minimum degree) keys then
   * find the predecessor to the target key in the sub-tree rooted by child
   * [ n ]; replace the target key with its predecessor; continue to remove the
   * target key from the sub-tree rooted at child [ n ]. Else, if child
   * [ n + 1 ] has at least 2t-1 (minimum degree) keys then find the successor
   * to the target key in the sub-tree rooted by child [ n + 1 ]; replace the
   * target key by its successor; continue to remove the target key from the
   * sub-tree rooted at child [ n + 1 ]. Else, merge child [ n ] with child
   * [ n + 1 ]; then continue to remove the target key from the sub-tree
   * rooted at child [ n ].
   *
   * 20181005: Modification to support all keys present in leaves.
   *           Added parameter reference of type element (ret)
   */
  void node::remove_from_non_leaf (element& ret, int32_t n) {
    element elem = get_element(n);

    int32_t const min_degree = m_tree->get_index(m_ndx_name).order() / 2;

    // If the child [ n ] (that precedes key [ n ]) has at least min_degree
    // keys, then find the predecessor of key in the sub-tree rooted at child
    // [ n ] and replace key by its predecessor.
    if (child_at (n)->m_num_key >= min_degree) {
      element prev = get_prev_element(n);
      put_element(n, prev);
      /* 20181005: Modification to support all keys present in leaves.
       * Recurse to remove key from leaf node. All keys are present in the leaf
       * nodes. So do not remove the previous key from its leaf node.
       * Removed: child_at(n)->remove(prev.m_key);
       * recurse to leaf node and delete element with target key */
      child_at(n)->remove(ret, elem.m_key);
    }
      // If child [ n ] has less than min_degree keys, examine child [ n + 1 ].
      // If child [ n + 1 ] has at least min_degree keys, find the successor
      // of the target key in the subtree rooted at child [ n + 1 ] and replace
      // key by its successor.
    else if (child_at (n + 1)->m_num_key >= min_degree) {
      element next = get_next_element(n);
      put_element(n, next);
      /* 20181005: Modification to support all keys present in leaves.
       * Recurse to remove key from leaf node. All keys are present in the leaf
       * nodes. So do not remove the next key from its leaf node.
       * Removed: child_at(n)->remove(next.m_key);
       * recurse to leaf node and delete element with target key */
      child_at(n + 1)->remove(ret, elem.m_key);
    } else {
      merge(n);
      child_at(n)->remove(ret, elem.m_key);
    }
  }

  /****************************************************************************
   * Update the number of keys in the node; and add the buffer to the flush
   * list.
   */
  void node::set_num_key (int32_t n) {
    m_num_key = n;
    flush();
  }

  /****************************************************************************
   * Split a full node. Construct a node next to store minimum degree minus one
   * keys of the full node prev. Set the number of keys (elements) in node next
   * to minimum degree minus one. Copy the last minimum degree minus one ele-
   * ments of node prev to node next. Copy the last minimum degree child-
   * offsets of node prev to node next. Next set the number of keys (elements)
   * in node prev accordingly. Make room in parent for child-offset of node
   * next, by moving all subsequent child-offsets one position right. Put
   * child-offset of node next in parent. Make root in parent for middle key
   * of child node prev, by moving all subsequent keys one position right. Put
   * middle key from node prev in parent. Increment number of keys in parent.
   * Set sibling pointers in nodes prev and next to appropriate values.
   */
  void node::split_child (int32_t n, std::shared_ptr<node> prev) {
    // calc minimum degree of tree
    int32_t const min_degree = m_tree->get_index(m_ndx_name).order() / 2;

    // construct node new_node to store (min_degree - 1) elements of node prev
    auto new_node = m_tree->new_node(prev->m_leaf, m_ndx_name,
        m_tree->next_offset(false));

    // set number of elements in node new_node
    new_node->set_num_key(min_degree - 1);

    // copy the last min_degree - 1 elements of node prev to node new_node
    for (int32_t i = 0; i < min_degree - 1; ++i)
      new_node->put_element(i, prev->get_element(min_degree + i));

    // copy the last min_degree child-offsets of node prev to node new_node
    if (!prev->m_leaf)
      for (int32_t i = 0; i < min_degree; ++i)
        new_node->put_child_offset(i, prev->get_child_offset(min_degree + i));

    /* This change makes it possible to keep all block-offsets and keys in
     * the leaf nodes. The leaf nodes can be linked together and traversed
     * horizontally (leaf-to-leaf) rather than walking up and down the tree.
     * Removed: prev->set_num_key(min_degree - 1);
     * decrement number of elements in node prev */
    prev->set_num_key((prev->m_leaf ? min_degree : min_degree - 1));

    // make room in parent node for child-offset of node new_node
    for (int32_t i = m_num_key; i >= n + 1; --i)
      put_child_offset(i + 1, get_child_offset(i));

    // assign child-offset of node new_node in parent node
    put_child_offset(n + 1, static_cast<int64_t>(new_node->m_offset));

    // a key of node prev will be copied to parent node. So, move all greater
    // elements one position right
    for (int32_t i = m_num_key - 1; i >= n; --i)
      put_element(i + 1, get_element(i));

    // assign the middle element of node prev to parent node
    put_element(n, prev->get_element(min_degree - 1));

    // increment number of elements in parent node
    inc_num_key(1);

    // Update sibling references in nodes prev, new_node, and next (i.e.
    // insert new_node). NODE PREV--><--NEW NODE--><--NODE NEXT
    insert_sibling (prev, new_node);

    prev->flush();
    new_node->flush();
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASSES DATA & NODE: Support routines
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Get the offset of the node that precedes node (n).
   */
  template<typename T>
  int64_t get_prev_sibling_offset (std::shared_ptr<T> n) {
    return n->m_buffer.get_int64(red::tree::PREV_SIBLING_POS);
  }

  /****************************************************************************
   * Get the offset of the node that succedes node (n).
   */
  template<typename T>
  int64_t get_next_sibling_offset (std::shared_ptr<T> n) {
    return n->m_buffer.get_int64(red::tree::NEXT_SIBLING_POS);
  }

  /****************************************************************************
   * Set the previous sibling offset of the node (n) with the value of offset
   * (o).
   */
  template<typename T>
  void set_prev_sibling_offset (std::shared_ptr<T> n, int64_t o) {
    n->m_buffer.put_int64(red::tree::PREV_SIBLING_POS, o);
  }

  /****************************************************************************
   * Set the next sibling offset of the node (n) with the value of offset
   * (o).
   */
  template<typename T>
  void set_next_sibling_offset (std::shared_ptr<T> n, int64_t o) {
    n->m_buffer.put_int64(red::tree::NEXT_SIBLING_POS, o);
  }

  /****************************************************************************
   * Insert new_node into the double-linked list of the sibling nodes. Nodes at
   * each level of the B-Tree are doubly-linked together to facilitate sibling
   * traversal.
   */
  template<typename T>
  void insert_sibling (std::shared_ptr<T> prev, std::shared_ptr<T> new_node) {
    // Set previous sibling reference of node new_node with the offset of node
    // prev.
    set_prev_sibling_offset(new_node,
        static_cast<int64_t>(prev->m_offset));
    // Set next sibling reference of node new_node with the offset of node
    // next.
    set_next_sibling_offset(new_node, get_next_sibling_offset(prev));
    // Set the previous sibling reference of node next with the offset of node
    // new_node.
    std::shared_ptr<T> next;
    if ((next = prev->get_next_sibling()) != nullptr) {
      set_prev_sibling_offset(next, static_cast<int64_t>(new_node->m_offset));
      next->flush();
    }
    // Set next sibling reference of node prev with the offset of node
    // new_node.
    set_next_sibling_offset(prev,
        static_cast<int64_t>(new_node->m_offset));
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS TREE: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS TREE: Move Constructor
   */
  tree::tree (tree&& tr) {
    m_block_cache = tr.m_block_cache;
    m_node    = tr.m_node;
    m_data    = tr.m_data;
  }

  /****************************************************************************
   * CLASS TREE: Move Assignment
   */
  tree& tree::operator=(tree&& tr) {
    if (this == &tr) return *this;
    m_block_cache = tr.m_block_cache;
    m_node    = tr.m_node;
    m_data    = tr.m_data;
    return *this;
  }

  /****************************************************************************
   *
   */
  iterator2 tree::begin () {
    if (empty())
      return iterator2 {-1, red::tree::INVALID_SIBLING_REF, this};

    auto d = get_data(get_header().data_root());

    int32_t position = static_cast<int32_t>(d->m_buffer
        .get_int16(red::tree::ROW_HEAD_POS));

    return iterator2 {position, d->m_offset, this};
  }

  /****************************************************************************
   * Tree iterator constructor that prepares the iterator for incrementing,
   * by positioning the iterator to the first element in the tree. If the index
   * (corresponding to the ndx_name) is empty, an invalid iterator is returned.
   */
  iterator tree::begin (std::string const& ndx_name) {
    if (empty(ndx_name))
      return iterator {0, red::tree::INVALID_SIBLING_REF, ndx_name,
        this};

    auto n = get_node(ndx_name, get_index(ndx_name).root());

    while (!n->m_leaf)
      n = n->child_at (0);

    return iterator {0, n->m_offset, ndx_name, this};
  }

  /****************************************************************************
   *
   */
  bool tree::empty () {
    return (get_header().data_root() < 0L);
  }

  /****************************************************************************
   * Use this function to determine if an index is empty. Returns true if the
   * index (corresponding to the ndx_name) is empty.
   */
  bool tree::empty (std::string const& ndx_name) {
    return (get_index(ndx_name).root() < 0L);
  }

  /****************************************************************************
   *
   */
  iterator2 tree::end () {
    if (empty())
      return iterator2 {-1, red::tree::INVALID_SIBLING_REF, this};

    auto d = get_data(get_header().data_offset());
    // get position of last row (row-tail)
    int32_t position = static_cast<int32_t>(d->m_buffer
        .get_int16(red::tree::ROW_TAIL_POS));
    // add size of last row (row-tail)
    position += static_cast<int32_t>(d->m_buffer
        .get_int16(position + red::tree::ROW_SIZE_POS));
    // return iterator that points to the end of the last row of last block
    return iterator2 {position, d->m_offset, this};
  }

  /****************************************************************************
   * Tree iterator constructor that prepares the iterator for decrementing,
   * by positioning the iterator to one position past the last element in the
   * tree. If the index (corresponding to the ndx_name) is empty, an invalid
   * iterator is returned.
   */
  iterator tree::end (std::string const& ndx_name) {
    if (empty(ndx_name))
      return iterator {0, red::tree::INVALID_SIBLING_REF, ndx_name, this};

    auto n = get_node(ndx_name, get_index(ndx_name).root());

    while (!n->m_leaf)
      n = n->child_at (n->m_num_key);

    return iterator {n->m_num_key, n->m_offset, ndx_name, this};
  }

  /****************************************************************************
   * Fill a newly added index with elements each with a key composed from ex-
   * isting row(s) in the table.
   */
  void tree::fill_index (std::string const& ndx_name) {
    for (auto diter = begin(); diter != end(); ++diter) {
      auto key = (*diter).make_key(ndx_name);
      element e {diter.m_position, diter.m_offset, ndx_name, key};
      insert(e);
    }
  }

  /****************************************************************************
   * Search for the node that holds the key. Search for the node that holds the
   * key (key) using the index identified by index name (ndx_name).
   */
  iterator tree::find (std::string const& ndx_name, std::string const& key) {
    if (empty(ndx_name))
      return iterator {0, red::tree::INVALID_SIBLING_REF, ndx_name, this};

    auto root = get_node(ndx_name, get_index(ndx_name).root());

    return root->find(key);
  }

  /****************************************************************************
  * Insert a row into the table. Allocate/get the data block where the row will
  * be inserted. Get buffer that contains the column values of the row. Deter-
  * mine if the data block has enough space to hold the row. If not allocate
  * a new data block. Then put the row buffer into the data block. Last, pop-
  * late all the indexes with their corresponding column value.
  */
  void tree::insert (row& rw) {
    std::shared_ptr<data> dt {nullptr};

    if (get_header().data_offset() < 0L) {
      dt = new_data(next_offset(true));
    } else dt = get_data(get_header().data_offset());

    rw.rewind();
    buffer buf = rw.backing();

    int32_t remaining = dt->m_buffer.get_int32(red::tree::REM_POS);
    int32_t size = buf.bound();

    if (remaining < size) {
      auto new_dt = new_data(next_offset(true));
      // update sibling references
      set_next_sibling_offset(dt, static_cast<int64_t>(new_dt->m_offset));
      set_prev_sibling_offset(new_dt, static_cast<int64_t>(dt->m_offset));
      // get remaining
      remaining = new_dt->m_buffer.get_int32(red::tree::REM_POS);
      // assign new data
      dt = new_dt;
    }
    // get position of new row
    int32_t position = dt->m_buffer.size() - remaining;
    // put new row
    dt->m_buffer.put(position, buf);
    // update remaining space
    remaining = dt->m_buffer.size() - dt->m_buffer.index();
    dt->m_buffer.put_int32(red::tree::REM_POS, remaining);
    // get value of row-head
    int32_t row_head = static_cast<int32_t>(dt->m_buffer
        .get_int16(red::tree::ROW_HEAD_POS));
    // update row-head
    if (row_head == 0)
      dt->m_buffer.put_int16(red::tree::ROW_HEAD_POS,
          static_cast<int16_t>(position));
    // get value of row-tail
    int32_t row_tail = static_cast<int32_t>(dt->m_buffer
        .get_int16(red::tree::ROW_TAIL_POS));
    // update row-prev field
    dt->m_buffer.put_int16(position + red::tree::ROW_PREV_POS,
        static_cast<int16_t>(row_tail));
    // update row-next field
    dt->m_buffer.put_int16(position + red::tree::ROW_NEXT_POS,
        static_cast<int16_t>(0));
    // update row-next field of row-tail
    if (row_tail != 0)
      dt->m_buffer.put_int16(row_tail + red::tree::ROW_NEXT_POS,
        static_cast<int16_t>(position));
    // update row-tail
    dt->m_buffer.put_int16(red::tree::ROW_TAIL_POS,
        static_cast<int16_t>(position));
    // commit changes to data-block
    dt->commit();
    // update (indexes) nodes
    for (auto ndx : get_schema()->get_index_list()) {
      std::string key = rw.make_key(ndx.name());
      element e {position, dt->m_offset, ndx.name(), key};
      insert(e);
    }
    // update schema
    get_schema()->get_header()
      .row_count(get_schema()->get_header().row_count() + 1L);
    get_schema()->get_header().generation(get_schema()->get_header()
        .generation() + 1L);
  }

  /****************************************************************************
   * Make a row for the table associated with this tree. Return said row.
   */
  row tree::make_row () const {
    // calculate the maximum capacity of the row buffer
    int32_t capacity = red::tree::COLUMN_SIZE_POS + get_schema()->get_header().
      column_count() * red::tree::COLUMN_SIZE_SZ;
    for (int32_t i = 0; i < get_schema()->get_header().column_count(); ++i)
      capacity += get_schema()->column_at(i).size();
    // make and return the row
    return row {capacity, get_schema()};
  }

  /****************************************************************************
   * Remove a row from the tree. If tree is empty then return. Walk index list
   * removing corresponding key from each index. An invalid element signals key
   * not found. If a valid element is returned then proceed to mark the corre
   * sponding data row as dead.
   */
  void tree::remove (row& rw) {
    // delete keys from indices
    element elem {INVALID_ELEMENT};
    for (auto ndx : get_schema()->get_index_list()) {
      std::string key = rw.make_key(ndx.name());

      element e {INVALID_ELEMENT};
      remove(e, ndx.name(), key);

      if (e.is_valid() && not elem.is_valid()) elem = e;
    }

    if (elem.is_valid()) {
      // get data block
      auto dt = get_data(elem.m_block_offset);
      // get value of row-prev field of deleted row
      int32_t row_prev = static_cast<int32_t>(dt->m_buffer
          .get_int16(elem.m_row_position + red::tree::ROW_PREV_POS));
      // get value of row-next field of deleted row
      int32_t row_next = static_cast<int32_t>(dt->m_buffer
          .get_int16(elem.m_row_position + red::tree::ROW_NEXT_POS));
      // update row-next field of row pointed to by row_prev
      if (row_prev != 0)
        dt->m_buffer.put_int16(row_prev + red::tree::ROW_NEXT_POS,
            static_cast<int16_t>(row_next));
      // update row-prev field of row pointed to by row_next
      if (row_next != 0)
        dt->m_buffer.put_int16(row_next + red::tree::ROW_PREV_POS,
            static_cast<int16_t>(row_prev));
      // get row-head
      int32_t row_head = static_cast<int32_t>(dt->m_buffer
          .get_int16(red::tree::ROW_HEAD_POS));
      // update row-head
      if (row_head == elem.m_row_position)
        dt->m_buffer.put_int16(red::tree::ROW_HEAD_POS,
            static_cast<int16_t>(row_next));
      // get row-tail
      int32_t row_tail = static_cast<int32_t>(dt->m_buffer
          .get_int16(red::tree::ROW_TAIL_POS));
      // update row-tail
      if (row_tail == elem.m_row_position)
        dt->m_buffer.put_int16(red::tree::ROW_TAIL_POS,
            static_cast<int16_t>(row_prev));
      // commit data-block changes
      dt->commit();
        // update schema
      get_schema()->get_header().row_count(get_schema()->get_header()
          .row_count() - 1L);
      get_schema()->get_header().generation(get_schema()->get_header()
          .generation() + 1L);
    }
  }

  /****************************************************************************
   * Drop an index. Get the head and tail nodes and proceed to connect tail
   * nodes to head nodes and vis-versa. Then update schema free-head and free-
   * tail fields accordingly.
   */
  void tree::remove_index (std::string const& ndx_name) {
    // return if index is empty
    if (empty(ndx_name)) return;
    // start with root node
    auto head = get_node(ndx_name, get_schema()->get_index(ndx_name).root());
    auto tail = head;
    // link tail-node of parent-level to head-node of child-level and vis-versa
    while (tail != nullptr && not tail->m_leaf) {
      auto elem = head->get_element(0);
      set_next_sibling_offset(tail, static_cast<int64_t>(elem.m_block_offset));
      tail->flush();
      head = get_node(ndx_name, elem.m_block_offset);
      set_prev_sibling_offset(head, static_cast<int64_t>(tail->m_offset));
      head->flush();
      elem = tail->get_element(tail->m_num_key - 1);
      tail = get_node(ndx_name, elem.m_block_offset);
    }
    // update free-head and free-tail fields of schema
    if (get_schema()->get_header().free_head() == -1L)
      get_schema()->get_header()
        .free_head(get_schema()->get_index(ndx_name).root());
    else {
      head = get_node(ndx_name, get_schema()->get_index(ndx_name).root());
      set_prev_sibling_offset(head,
          static_cast<int64_t>(get_schema()->get_header().free_tail()));
      head->flush();
    }
    get_schema()->get_header().free_tail(tail->m_offset);
    // drop index from schema
    get_schema()->remove_index(ndx_name);
  }
#ifdef BTREE_DEBUG
  /****************************************************************************
   * Traverse the entire tree using the index identified by ndx_name. The leaf
   * node traversal routine is used.
   */
  void tree::traverse (std::string const& ndx_name) {
    if (get_index(ndx_name).root() < 0L) return;

    auto root = get_node(ndx_name, get_index(ndx_name).root());

    int32_t x {0};
    root->leaf_traverse(x);
  }
#endif
  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS TREE: PRIVATE
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Get the block at offset (offset). If the block is not in the data map
   * then load the block from disk.
   */
  std::shared_ptr<data> tree::get_data (off_t offset) {
    if (offset < 0L || (offset % red::tree::BUFFER_SZ) != 0) return nullptr;

    std::shared_ptr<data> new_data;

    auto iter = m_data.find(offset);

    if (iter == m_data.end()) new_data = load_data(offset);
    else new_data = (*iter).second;

    if (new_data != nullptr)
      new_data->m_access_time = std::chrono::system_clock::now();

    return new_data;
  }

  /****************************************************************************
   * Get the node at offset (offset). If the node is not in the node map
   * then load the node from disk.
   */
  std::shared_ptr<node>
  tree::get_node (std::string const& ndx_name, off_t offset)
  {
    if (offset < 0L || (offset % red::tree::BUFFER_SZ) != 0) return nullptr;

    std::shared_ptr<node> new_node;

    auto iter = m_node.find(offset);

    if (iter == m_node.end()) new_node = load_node(ndx_name, offset);
    else new_node = (*iter).second;

    if (new_node != nullptr)
      new_node->m_access_time = std::chrono::system_clock::now();

    return new_node;
  }

  /****************************************************************************
   * Get offset of new/free block. First get the value of free-tail; return
   * offset of a new-block, if free-tail is less than zero (invalid); else load
   * the corresponding block; retreive the previous-sibling-reference; invali-
   * date free-head and free-tail, if previous-sibling-reference is invalid;
   * else set free-tail equal to previous-sibling-reference; and return
   * offset of a free-block.
   */
  off_t tree::get_offset () {
    off_t offset = get_schema()->get_header().free_tail();
    if (offset < 0L) return get_header().next_offset();
    auto blk = m_block_cache->get_block(offset);
    if (blk == nullptr)
      throw std::runtime_error{eno::get_msg(eno::ERRNO_SYSTEM_ERROR)};
    buffer buf {get_schema()->get_header().block_size(), blk};
    int64_t tail_offset = buf.get_int64(red::tree::PREV_SIBLING_POS);
    if (tail_offset == red::tree::INVALID_SIBLING_REF) {
      get_schema()->get_header().free_head(-1);
      get_schema()->get_header().free_tail(-1);
    } else get_schema()->get_header().free_tail(tail_offset);
    return offset;
  }

  /****************************************************************************
   * Insert an element into the table. If index is empty (root offset less than
   * zero) then: set index root offset; construct new root node; put element in
   * root node and increment number of keys in root node. Else, get root node
   * of index: if root node is full construct a new root node; put the offset
   * of the old root into the new root as its first child-offset; then splite
   * the old root (which adds the new child to the new root); determine where
   * to insert the new element; insert the new element in the corresponding
   * child node; finally update the index schema with the offset of the new
   * root.
   */
  void tree::insert (element elem) {
    // if root is empty, insert elem in root
    if (get_index(elem.m_ndx_name).root() < 0L) {

      get_index(elem.m_ndx_name).root(next_offset(false));

      auto root = new_node(true, elem.m_ndx_name,
          get_index(elem.m_ndx_name).root());

      root->put_element(0, elem);

      root->set_num_key(1);
    } else {
      auto root = get_node(elem.m_ndx_name, get_index(elem.m_ndx_name).root());

      // if root is full, promote new root
      if (root->is_full()) {
        // construct new root
        auto new_root = new_node(false, elem.m_ndx_name, next_offset(false));

        // set old root as child of new root
        new_root->put_child_offset(0, root->m_offset);

        // split the old root and move one element to the new root
        new_root->split_child(0, root);

        // determine where to insert new element
        int32_t i {0};
        if (new_root->get_element(0) < elem) ++i;

        get_node(new_root->m_ndx_name, new_root->get_child_offset(i))
          ->insert_non_full(elem);

        // update root in index schema
        get_index(elem.m_ndx_name).root(new_root->m_offset);
      }
      else root->insert_non_full(elem); // if root is not full
    }
  }

  /****************************************************************************
   * Load a data node from disk. If the offset is equal-to or greater-than the
   * size of the file, then return a null pointer; construct a data object;
   * read the data block from disk into the data object; insert the data object
   * into the data map; return the data object.
   */
  std::shared_ptr<data> tree::load_data (off_t offset) {
    auto blk = m_block_cache->get_block(offset);

    if (blk == nullptr) return nullptr;

    std::shared_ptr<data> d {new data {offset, this,
      get_schema()->get_header().block_size(), blk}};

    m_data [ offset ] = d;

    return d;
  }

  /****************************************************************************
   * Load an index node from disk. If the offset is equal-to or greater-than
   * the size of the file, then return a null pointer; construct a node object;
   * read the node block from disk into the node object; copy the leaf value
   * and number of keys value from the node block; insert the node object into
   * the node map; return the node object;
   */
  std::shared_ptr<node>
  tree::load_node (std::string const& ndx_name, off_t offset)
  {
    auto blk = m_block_cache->get_block(offset);

    if (blk == nullptr) return nullptr;

    std::shared_ptr<node> n {new node {true, offset, this, ndx_name,
      get_schema()->get_header().block_size(), blk}};

    n->m_leaf = static_cast<bool>(n->m_buffer.get(IS_LEAF_POS));
    n->m_num_key = static_cast<int32_t>(n->m_buffer.get_int16(NUM_KEY_POS));

    m_node [ offset ] = n;

    return n;
  }

  /****************************************************************************
   * Construct a data object. Construct the new data object; set the number of
   * bytes remaining in the data object buffer at position zero (0) in the data
   * object buffer; write the data object buffer out to disk; insert the data
   * object into the data object map; return the new data object.
   */
  std::shared_ptr<data> tree::new_data (off_t offset)
  {
    auto blk = m_block_cache->new_block(offset);

    if (blk == nullptr) return nullptr;

    std::shared_ptr<data> d {new data {offset, this,
      get_schema()->get_header().block_size(), blk}};

    // intitialize remaining field
    d->m_buffer.put_int32(REM_POS, get_header().block_size() - DATA_HDR_SZ);
    // Invalidate sibling references
    d->m_buffer.put_int64(PREV_SIBLING_POS, INVALID_SIBLING_REF);
    d->m_buffer.put_int64(NEXT_SIBLING_POS, INVALID_SIBLING_REF);
    // initialize row-head and row-tail
    d->m_buffer.put_int16(red::tree::ROW_HEAD_POS, static_cast<int16_t>(0));
    d->m_buffer.put_int16(red::tree::ROW_TAIL_POS, static_cast<int16_t>(0));

    m_data [ offset ] = d;

//    get_header().data_offset(offset);

    return d;
  }

  /****************************************************************************
   * Construct an index node object. Construct the new node object; set the
   * sibling references with invalid values in the node object buffer at their
   * corresponding positions; write the node object buffer out to disk; insert
   * the node object into the node object map; return the new node object.
   */
  std::shared_ptr<node> tree::new_node (bool leaf,
      std::string const& ndx_name, off_t offset)
  {
    auto blk = m_block_cache->new_block(offset);

    if (blk == nullptr) return nullptr;

    std::shared_ptr<node> n {new node {leaf, offset, this, ndx_name,
      get_schema()->get_header().block_size(), blk}};

    // Invalidate sibling references
    n->m_buffer.put_int64(PREV_SIBLING_POS, INVALID_SIBLING_REF);
    n->m_buffer.put_int64(NEXT_SIBLING_POS, INVALID_SIBLING_REF);

    m_node [ offset ] = n;

    return n;
  }

  /****************************************************************************
   * Get the next available disk block offset (which corresponds to the end of
   * the file on disk). Get the next available offset from the table schema
   * header; update the next available offset in the table schema header; if
   * this is a data block offset then set the data offset field in the table
   * schema header; return the next offset.
   */
  off_t tree::next_offset (bool is_data_offset) {
    off_t offset = get_offset();

    get_header().next_offset(offset + get_header().block_size());

    if (is_data_offset) {
      get_header().data_offset(offset);
      if (get_header().data_root() < 0L)
        get_header().data_root(offset);
    }

    return offset;
  }

  /****************************************************************************
   * Purge dead data objects from data object map.
   */
  void tree::purge_data_map () {
    for (auto iter = m_data.begin(); iter != m_data.end();)
      if ((*iter).second->m_dead) iter = m_data.erase(iter);
      else ++iter;
  }

  /****************************************************************************
   * Purge dead node objects from node object map.
   */
  void tree::purge_node_map () {
    for (auto iter = m_node.begin(); iter != m_node.end();)
      if ((*iter).second->m_dead) iter = m_node.erase(iter);
      else ++iter;
 }

  /****************************************************************************
   * Remove a row from the tree. If index is empty then return. Get the root
   * node of the index; call node::remove on the root node to remove the the
   * row with the corresponding key. If the result is an empty root node (no
   * keys), then update the root offset in the index schema with either the
   * offset of the first child of root or negative one (-1) to mark the index
   * as empty.
   */
  void tree::remove (element& elem, std::string const& ndx_name,
      std::string const& key)
  {
    // if tree is empty, return
    if (get_index(ndx_name).root() < static_cast<off_t>(0)) return;

    auto root = get_node(ndx_name, get_index(ndx_name).root());

    // remove the row with the corresponding key value in the index with the
    // corresponding index id.
    root->remove(elem, key);

    // if the result is an empty "root" (no keys), make "root's" child [ 0 ]
    // the new root, otherwise set root to invalid (-1)
    if (root->m_num_key == 0) {
      if (root->m_leaf)
        get_index(ndx_name).root(-1);
      else
        get_index(ndx_name).root(root->get_child_offset(0));

      root->m_dead = true;
    }
  }

  /****************************************************************************
   * Update a row in place. *WARNING* Do not use this routine if any indexed
   * columns have been modified. If searching for the primary-key fails, the
   * update aborts. If no node object can be found, the update aborts. If no
   * data object can be found, the update aborts. If the row-sizes do not
   * match, the update aborts. Otherwise, the update proceeds.
   */
  void tree::update (row& rw) {
    // construct primary-key from row-value(s)
    std::string key = rw.make_key(schema::PK_NDX);
    // find key
    auto iter = find(schema::PK_NDX, key);
    if (iter == end(schema::PK_NDX)) return;
    // get node
    auto n = get_node(schema::PK_NDX, iter.offset());
    if (n == nullptr) return;
    // get element [ m_index ]
    element elem = n->get_element(iter.index());
    // get data block
    auto d = get_data(elem.m_block_offset);
    if (d == nullptr) return;
    // get row size
    size_t row_size = d->m_buffer.get_int16(elem.m_row_position
        + red::tree::ROW_SIZE_POS);
    // compare row-sizes
    if (static_cast<size_t>(rw.size()) != row_size) return;
    // update row
    auto buf = rw.backing();
    d->m_buffer.put(elem.m_row_position, buf);
    d->commit();
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS ITERATOR2: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS DATA::ITERATOR: Copy Assignment
   * */
  iterator2& iterator2::operator=(iterator2 const& o) {
    if (this == &o) return *this;
    m_position  = o.m_position;
    m_offset    = o.m_offset;
    m_tree      = o.m_tree;
    return *this;
  }

  /****************************************************************************
   * CLASS DATA::ITERATOR: Move Assignment
   * */
  iterator2& iterator2::operator=(iterator2&& o) {
    if (this == &o) return *this;
    m_position  = o.m_position;
    m_offset    = o.m_offset;
    m_tree      = o.m_tree;
    return *this;
  }

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream& os, iterator2& o) {
    os << o.m_tree->name() << '\t'
      << std::hex << std::setw(8) << std::setfill('0') << o.m_position << '\t'
      << std::setw(8) << std::setfill('0') << o.m_offset << std::dec;
    return os;
  }
#endif

  /****************************************************************************
   * Is this iterator equal to that iterator?
   */
  bool iterator2::operator==(iterator2 const& it) {
    return (m_tree->name() == it.m_tree->name() && m_offset == it.m_offset
        && m_position == it.m_position);
  }

  /****************************************************************************
   * Is this iterator not equal to that iterator?
   */
  bool iterator2::operator!=(iterator2 const& it) {
    return !(*this == it);
  }

  /****************************************************************************
   * Increment the iterator one position. Get the data object that corresponds
   * to this iterator's offset field; get the position of the next row from the
   * row-next field of current row. If the value of the row-next field is zero
   * (0) then get the next-sibling offset; if the next-sibling offset is inval-
   * id then add the size of the current row to position; else if next-sibling
   * offset is valid then update offset and position fields of this iterator.
   * Return this iterator.
   */
  iterator2& iterator2::operator++() {
    auto d = m_tree->get_data(m_offset);

    m_position = static_cast<int32_t>(d->m_buffer
        .get_int16(m_position + red::tree::ROW_NEXT_POS));

    if (m_position == 0)
    {
      off_t offset = get_next_sibling_offset(d);

      if (offset == red::tree::INVALID_SIBLING_REF) {
        // get position of last row (row-tail)
        m_position = static_cast<int32_t>(d->m_buffer
            .get_int16(red::tree::ROW_TAIL_POS));
        // add size of last row (row-tail)
        m_position += static_cast<int32_t>(d->m_buffer
            .get_int16(m_position + red::tree::ROW_SIZE_POS));
        // now iterator points to the end of the last row of last block
      } else {
        m_offset = offset;
        auto d2 = m_tree->get_data(m_offset);
        m_position = static_cast<int32_t>(d2->m_buffer
            .get_int16(red::tree::ROW_HEAD_POS));
      }
    }

    return *this;
  }

  /****************************************************************************
   * Add (n) to this iterator's position.
   */
  iterator2& iterator2::operator+(int32_t const n) {
    for (int32_t i = 0; i < n; ++i) ++(*this);
    return *this;
  }

  /****************************************************************************
   * Add (n) to this iterator's position.
   */
  iterator2& iterator2::operator+=(int32_t const n) {
    return (*this + n);
  }

  /****************************************************************************
   * Get the row that corresponds to the key at this iterator's current
   * position.
   */
  row iterator2::operator*() {
    // get data block
    auto d = m_tree->get_data(m_offset);
    // position data block
    d->m_buffer.index(m_position);
    // construct row object
    row rw = m_tree->make_row();
    // copy a row of data from data-block's buffer into a row's buffer
    rw.copy_row(d->m_buffer);
    return rw;
  }

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASS ITERATOR: PUBLIC
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * CLASS ITERATOR: Copy Assignment
   */
  iterator& iterator::operator=(iterator const& o) {
    if (this == &o) return *this;
    m_index     = o.m_index;
    m_offset    = o.m_offset;
    m_ndx_name  = o.m_ndx_name;
    m_tree      = o.m_tree;
    return *this;
  }

  /****************************************************************************
   * CLASS ITERATOR: Move Assignment
   */
  iterator& iterator::operator=(iterator&& o) {
    if (this == &o) return *this;
    m_index     = o.m_index;
    m_offset    = o.m_offset;
    m_ndx_name  = o.m_ndx_name;
    m_tree      = o.m_tree;
    return *this;
  }

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream& os, iterator& o) {
    os << o.m_ndx_name << '\t' << o.m_index << '\t' << o.m_offset;
    return os;
  }
#endif

  /****************************************************************************
   * Is this iterator equal to that iterator?
   */
  bool iterator::operator==(iterator const& it) {
    return (m_offset == it.m_offset && m_index == it.m_index);
  }

  /****************************************************************************
   * Is this iterator not equal to that iterator?
   */
  bool iterator::operator!=(iterator const& it) {
    return !(*this == it);
  }

  /****************************************************************************
   * Increment the iterator one position. Get the node that corresponds to this
   * iterator's offset field; if the value of this iterator's index field is
   * less-than the number of keys in the node, then increment this iterator's
   * index field by one; if this iterator's index field is equal-to the number
   * of keys in the node, then get the offset of node's next sibling field; if
   * the offset of node's next sibling field is valid, then update this itera-
   * tor's offset and index field values; return this iterator;
   */
  iterator& iterator::operator++() {
    auto n = m_tree->get_node(m_ndx_name, m_offset);

    if (m_index < n->m_num_key) ++m_index;

    if (m_index == n->m_num_key) {
      off_t offset = get_next_sibling_offset(n);
      if (offset != INVALID_SIBLING_REF) {
        m_offset = offset;
        m_index = 0;
      }
    }

    return *this;
  }

  /****************************************************************************
   * Decrement the iterator one position. If the value of this iterator's index
   * field is equal-to zero, then get the node that corresponds to this iter-
   * ator's offset field; get the offset of node's previous sibling field; if
   * the offset of node's previous sibling field is valid, then update this it-
   * erator's offset and index field values; if this iterator's index field
   * value is greater-than zero (0), then decrement this iterator's index field
   * value by one; return this iterator;
   */
  iterator& iterator::operator--() {
    if (m_index == 0) {
      auto n = m_tree->get_node(m_ndx_name, m_offset);
      off_t offset = get_prev_sibling_offset(n);
      if (offset != INVALID_SIBLING_REF) {
        m_offset = offset;
        m_index = m_tree->get_node(m_ndx_name, m_offset)->m_num_key;
      }
    }

    if (m_index > 0) --m_index;
   
    return *this;
  }

  /****************************************************************************
   * Add (n) to this iterator's position.
   */
  iterator& iterator::operator+(int32_t const n) {
    for (int32_t i = 0; i < n; ++i) ++(*this);
    return *this;
  }

  /****************************************************************************
   * Subtract (n) from this iterator's position.
   */
  iterator& iterator::operator-(int32_t const n) {
    for (int32_t i = n; i > 0; --i) --(*this);
    return *this;
  }

  /****************************************************************************
   * Add (n) to this iterator's position.
   */
  iterator& iterator::operator+=(int32_t const n) {
    return (*this + n);
  }

  /****************************************************************************
   * Subtract (n) from this iterator's position.
   */
  iterator& iterator::operator-=(int32_t const n) {
    return (*this - n);
  }

  /****************************************************************************
   * Get the row that corresponds to the key at this iterator's current
   * position.
   */
  row iterator::operator*() {
    // get node
    auto n = m_tree->get_node(m_ndx_name, m_offset);
    // get element [ m_index ]
    element elem = n->get_element(m_index);
    // get data block
    auto d = m_tree->get_data(elem.m_block_offset);
    // position data block
    d->m_buffer.index(elem.m_row_position);
    // construct row object
    row rw = m_tree->make_row();
    // copy a row of data from data-block's buffer into a row's buffer
    rw.copy_row(d->m_buffer);

    return rw;
  }

} // namespace

