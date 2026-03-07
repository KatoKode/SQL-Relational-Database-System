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


#ifndef BTREE_H
#define BTREE_H 1

#include <chrono>
#include <memory>
#include <unordered_map>
#include <iostream>
#include "eno.h"
#include "logger.h"
#include "buffer.h"
#include "schema.h"
#include "row.h"
#include "const.h"
#include "lock.h"
#include "cache.h"

#undef BTREE_DEBUG

namespace red::tree {

  class node;
  class tree;
  class iterator2;
  class iterator;
  class emplace;

  uint8_t const ROW_LIVE  = 0xFF;
  uint8_t const ROW_DEAD  = 0x00;

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * STRUCT ELEMENT
   */
  struct element {
    public:
      element () { }

      element (int32_t row_position, off_t block_offset,
          std::string const& ndx_name, std::string const& key)
        : m_row_position {row_position}, m_block_offset {block_offset},
        m_ndx_name {ndx_name}, m_key {key}
      { }

      element (element const& e)
        : m_row_position {e.m_row_position}, m_block_offset {e.m_block_offset},
        m_ndx_name {e.m_ndx_name}, m_key {e.m_key}
      { }

      element (element&& e)
        : m_row_position {e.m_row_position}, m_block_offset {e.m_block_offset},
        m_ndx_name {e.m_ndx_name}, m_key {e.m_key}
      { }

      element& operator=(element const&);
      element& operator=(element&&);

      bool operator<(element& e) const { return (m_key < e.m_key); }

      bool operator>(element& e) const { return (m_key > e.m_key); }

      bool operator==(element& e) const { return (m_key == e.m_key); }

      bool operator==(std::string const& key) const { return (m_key == key); }

      bool operator<(std::string const& key) const { return (m_key < key); }

      bool is_valid () const;

      int32_t     m_row_position;
      off_t       m_block_offset;
      std::string m_ndx_name;
      std::string m_key;
  };

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream&, element&);
#endif

  /****************************************************************************
   * STRUCT ELEMENT: Invalid
   */
  element const INVALID_ELEMENT { -1, -1L, "", "" };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS DATA
   */
  class data {
    public:
      data (off_t offset, tree* a_tree,
          int32_t capacity, std::shared_ptr<uint8_t> buffer)
        : m_dead {false},
        m_offset {offset},
        m_tree {a_tree},
        m_access_time {std::chrono::system_clock::now()},
        m_buffer {capacity, buffer}
      { }

      ~data () { }

      data (data const& o)
        : m_dead {o.m_dead},
        m_offset {o.m_offset},
        m_tree {o.m_tree},
        m_access_time {o.m_access_time},
        m_buffer {o.m_buffer}
      { }

      data (data&& o)
        : m_dead {o.m_dead},
        m_offset {o.m_offset},
        m_tree {o.m_tree},
        m_access_time {o.m_access_time},
        m_buffer {o.m_buffer}
      { }

      data& operator=(data const&);
      data& operator=(data&&);
    private:
      bool      m_dead;
      off_t     m_offset;     // offset block in file
      tree* m_tree; // parent tree
      std::chrono::time_point<std::chrono::system_clock> m_access_time;
      buffer  m_buffer;

      void commit ();

      friend std::ostream& operator<<(std::ostream&, data const&);

      template<typename T>
      friend off_t get_prev_sibling_offset (std::shared_ptr<T>);
      template<typename T>
      friend off_t get_next_sibling_offset (std::shared_ptr<T>);
      template<typename T>
      friend void set_prev_sibling_offset (std::shared_ptr<T>, off_t);
      template<typename T>
      friend void set_next_sibling_offset (std::shared_ptr<T>, off_t);
      template<typename T>
      friend void insert_sibling (std::shared_ptr<T>, std::shared_ptr<T>);

      friend class node;
      friend class iterator2;
      friend class iterator;
      friend class tree;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS NODE
   */
  class node {
    public:
      node (bool leaf, off_t offset, tree* a_tree,
          std::string const& ndx_name, int32_t capacity,
          std::shared_ptr<uint8_t> buffer)
        : m_leaf {leaf},
        m_dead {false},
        m_num_key {0},
        m_offset {offset},
        m_tree {a_tree},
        m_ndx_name {ndx_name},
        m_access_time {std::chrono::system_clock::now ()},
        m_buffer {capacity, buffer}
      { }

      ~node () { }

      node (node const& o) :
        m_leaf {o.m_leaf},
        m_dead {o.m_dead},
        m_num_key {o.m_num_key},
        m_offset {o.m_offset},
        m_tree {o.m_tree},
        m_ndx_name {o.m_ndx_name},
        m_access_time {o.m_access_time},
        m_buffer {o.m_buffer}
      { }

      node (node&& o) :
        m_leaf {o.m_leaf},
        m_dead {o.m_dead},
        m_num_key {o.m_num_key},
        m_offset {o.m_offset},
        m_tree {o.m_tree},
        m_ndx_name {o.m_ndx_name},
        m_access_time {o.m_access_time},
        m_buffer {o.m_buffer}
      { }

      node& operator=(node const& n);
      node& operator=(node&& n);

      iterator find (std::string const&);
#ifdef BTREE_DEBUG
      void tree_traverse (int32_t&);

      void leaf_traverse (int32_t&);
#endif
    private:
      bool        m_leaf;
      bool        m_dead;
      int32_t     m_num_key;
      off_t       m_offset;   // offset of node in file
      tree*       m_tree; // parent tree
      std::string m_ndx_name; // name of index
      std::chrono::time_point<std::chrono::system_clock> m_access_time;
      buffer      m_buffer;

      std::shared_ptr<node> child_at (int32_t);

      void commit ();

      void dec_num_key (int32_t);

      void borrow_from_prev (int32_t);

      void borrow_from_next (int32_t);

      void fill (int32_t);

      int32_t find_key (std::string const&);

      void flush ();

      int64_t get_child_offset (int32_t);

      element get_element (int32_t);

      element get_next_element (int32_t);

      std::shared_ptr<node> get_next_sibling ();

      element get_prev_element (int32_t);

      std::shared_ptr<node> get_prev_sibling ();

      void inc_num_key (int32_t);

      void insert_non_full (element&);

      bool is_full ();

      void merge (int32_t);

      void put_child_offset (int32_t, int64_t);

      void put_element (int32_t, element const&);

      void remove (element&, std::string const&);

      void remove_from_leaf (element&, int32_t);

      void remove_from_non_leaf (element&, int32_t);

      void set_num_key (int32_t);

      void split_child (int32_t, std::shared_ptr<node>);

      friend std::ostream& operator<<(std::ostream&, std::shared_ptr<node>);

      template<typename T>
      friend off_t get_prev_sibling_offset (std::shared_ptr<T>);
      template<typename T>
      friend off_t get_next_sibling_offset (std::shared_ptr<T>);
      template<typename T>
      friend void set_prev_sibling_offset (std::shared_ptr<T>, off_t);
      template<typename T>
      friend void set_next_sibling_offset (std::shared_ptr<T>, off_t);
      template<typename T>
      friend void insert_sibling (std::shared_ptr<T>, std::shared_ptr<T>);

      friend class tree;
      friend class iterator;
  };

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream&, std::shared_ptr<node>);
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   *
   * CLASSES DATA & NODE: Support routine declarations
   *
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ***/

  /****************************************************************************
   * Get the offset of the node that precedes node (n).
   */
  template<typename T>
  int64_t get_prev_sibling_offset (std::shared_ptr<T>);

  /****************************************************************************
   * Get the offset of the node that succedes node (n).
   */
  template<typename T>
  int64_t get_next_sibling_offset (std::shared_ptr<T>);

  /****************************************************************************
   * Set the previous sibling offset of the node (n) with the value of offset
   * (o).
   */
  template<typename T>
  void set_prev_sibling_offset (std::shared_ptr<T>, int64_t);

  /****************************************************************************
   * Set the next sibling offset of the node (n) with the value of offset
   * (o).
   */
  template<typename T>
  void set_next_sibling_offset (std::shared_ptr<T>, int64_t);

  /****************************************************************************
   * Insert new_node into the double-linked list of the sibling nodes. Nodes at
   * each level of the B-Tree are doubly-linked together to facilitate sibling
   * traversal.
   */
  template<typename T>
  void insert_sibling (std::shared_ptr<T>, std::shared_ptr<T>);

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS TREE
   */
  class tree {
    public:
      tree (std::shared_ptr<block_cache> block_cache)
        : m_block_cache {block_cache}
      { }

      tree (tree const&) = delete;
      tree (tree&&);

      tree& operator=(tree const&) = delete;
      tree& operator=(tree&&);

      iterator2 begin ();

      iterator begin (std::string const&);

      std::string db_name () const {
        return get_schema()->get_header().db_name();
      }

      bool empty ();
      bool empty (std::string const&);

      iterator2 end ();

      iterator end (std::string const&);

      iterator find (std::string const&, std::string const&);

      std::shared_ptr<schema::schema> get_schema () const {
        return m_block_cache->get_schema();
      }

      void insert (row&);

      row make_row () const;

      std::string name () const { return get_schema()->name(); }

      void remove (row&);

      void set_shared_lock () {
        m_shared_lock = std::shared_ptr<shared_lock>{
          new shared_lock{m_block_cache->get_lock()}};
      }

      std::string tbl_name () const {
        return get_schema()->get_header().tbl_name();
      }
#ifdef BTREE_DEBUG
      void traverse (std::string const&);
#endif
      void set_unique_lock () {
        m_unique_lock = std::shared_ptr<unique_lock>{
          new unique_lock{m_block_cache->get_lock()}};
      }
    private:
      std::shared_ptr<block_cache>  m_block_cache;
      std::shared_ptr<shared_lock>  m_shared_lock;
      std::shared_ptr<unique_lock>  m_unique_lock;
      std::unordered_map<off_t, std::shared_ptr<node> > m_node;
      std::unordered_map<off_t, std::shared_ptr<data> > m_data;

      auto column_pos (std::string const& name) {
        try { return get_schema()->column_pos(name); } catch (...) { throw; }
      }

      schema::column& column_at (int32_t i) {
        return get_schema()->column_at(i);
      }

      void fill_index (std::string const&);

      std::shared_ptr<data> get_data (off_t);

      schema::header& get_header () { return get_schema()->get_header(); }

      schema::index& get_index (std::string const& name) {
        try { return get_schema()->get_index(name); } catch (...) { throw; }
      }

      std::shared_ptr<node> get_node (std::string const&, off_t);

      off_t get_offset ();

      schema::index& index_at (size_t const i) {
        return get_schema()->index_at(i);
      }

      void insert (element);

      bool is_leaf_node (bool&, off_t);

      std::shared_ptr<data> load_data (off_t);

      std::shared_ptr<node> load_node (std::string const&, off_t);

      std::shared_ptr<data> new_data (off_t);

      std::shared_ptr<node> new_node (bool, std::string const&, off_t);

      off_t next_offset (bool);

      void purge_data_map ();

      void purge_node_map ();

      void remove (element&, std::string const&, std::string const&);

      void remove_index (std::string const&);

      void update (row&);

      friend std::ostream& operator<<(std::ostream&, tree&);

      friend class data;
      friend class node;
      friend class iterator2;
      friend class iterator;
      friend class emplace;
      friend class fill_index;
      friend class remove_index;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS EMPLACE: Special class to enable in-place update of a row.
   * NOTE: No indexes are updated by this object or by the the corresponding
   * tree object.
   */
  class emplace {
    public:
      emplace (std::shared_ptr<tree> tree_) : m_tree {tree_} { }

      void update (row& rw) { m_tree->update(rw); }

    private:
      std::shared_ptr<tree> m_tree;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS FILL_INDEX: Special class to enable the filling of a newly added
   * index with keys composed from existing row(s) in the table.
   */
  class fill_index {
    public:
      fill_index (std::string const& ndx_name, std::shared_ptr<tree> a_tree)
        : m_ndx_name {ndx_name}, m_tree {a_tree} { }

      void fill () { m_tree->fill_index(m_ndx_name); }

    private:
      std::string m_ndx_name;
      std::shared_ptr<tree> m_tree;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS REMOVE_INDEX: Special class to enable the removal of an existing
   * index from a table.
   */
  class remove_index {
    public:
      remove_index (std::string const& ndx_name, std::shared_ptr<tree> a_tree)
        : m_ndx_name {ndx_name}, m_tree {a_tree} { }

      void remove () { m_tree->remove_index(m_ndx_name); }

    private:
      std::string m_ndx_name;
      std::shared_ptr<tree> m_tree;
  };

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS ITERATOR2: *WARNING* An iterator should be constructed by tree
   * members begin(), end(). Constructing an invalid iterator will lead to a
   * catastrophic event.
   */
  class iterator2 {
    public:
      iterator2 ()
        : m_position {0}, m_offset {red::tree::INVALID_SIBLING_REF},
        m_tree {nullptr}
      { }

      iterator2 (int32_t position, off_t offset, tree* a_tree)
        : m_position {position}, m_offset {offset}, m_tree {a_tree}
      { }

      iterator2 (iterator2 const& o)
        : m_position {o.m_position}, m_offset {o.m_offset}, m_tree {o.m_tree}
      { }

      iterator2 (iterator2&& o)
        : m_position {o.m_position}, m_offset {o.m_offset}, m_tree {o.m_tree}
      { }

      iterator2& operator=(iterator2 const&);
      iterator2& operator=(iterator2&&);

      bool operator==(iterator2 const&);
      bool operator!=(iterator2 const&);

      iterator2& operator++();
      iterator2& operator--() { return *this; }

      iterator2& operator+(int32_t const);
      iterator2& operator-(int32_t const) { return *this; }

      iterator2& operator+=(int32_t const);
      iterator2& operator-=(int32_t const) { return *this; }

      row operator*();
    private:
      int32_t   m_position;
      off_t     m_offset;
      tree*     m_tree;

      friend std::ostream& operator<<(std::ostream&, iterator2&);
      friend class tree;
  };

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream&, iterator2&);
#endif

  /****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****  ****
   * CLASS ITERATOR: *WARNING* An iterator should be constructed by tree mem-
   * bers begin(), end() and search(). Constructing an invalid iterator will
   * lead to a catastrophic event.
   */
  class iterator {
    public:
      iterator ()
        : m_index {0}, m_offset {red::tree::INVALID_SIBLING_REF},
        m_tree {nullptr}
      { }

      iterator (int32_t index, off_t offset, std::string const& ndx_name,
          tree* tr)
        : m_index {index}, m_offset {offset}, m_ndx_name {ndx_name},
        m_tree {tr}
      { }

      iterator (iterator const& o)
        : m_index {o.m_index}, m_offset {o.m_offset},
        m_ndx_name {o.m_ndx_name}, m_tree {o.m_tree}
      { }

      iterator (iterator&& o)
        : m_index {o.m_index}, m_offset {o.m_offset},
        m_ndx_name {o.m_ndx_name}, m_tree {o.m_tree}
      { }

      iterator& operator=(iterator const&);
      iterator& operator=(iterator&&);

      bool operator==(iterator const&);
      bool operator!=(iterator const&);

      iterator& operator++();
      iterator& operator--();

      iterator& operator+(int32_t const);
      iterator& operator-(int32_t const);

      iterator& operator+=(int32_t const);
      iterator& operator-=(int32_t const);

      row operator*();

      int32_t index () const { return m_index; }
      off_t offset () const { return m_offset; }
      std::string ndx_name () const { return m_ndx_name; }
    private:
      int32_t     m_index;    // element-vector [ index ]
      off_t       m_offset;   // offset of current node in table
      std::string m_ndx_name;
      tree*       m_tree;

      friend std::ostream& operator<<(std::ostream&, iterator&);
      friend class tree;
  };

  /****************************************************************************
   * DEBUGGING ROUTINE
   */
#ifdef BTREE_DEBUG
  std::ostream& operator<<(std::ostream&, iterator&);
#endif

} // namespace

#endif  // BTREE_H

