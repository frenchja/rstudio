/*
 * Tree.hpp
 *
 * Copyright (C) 2009-12 by RStudio, Inc.
 *
 * This program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */

//	STL-like templated tree class.
//
// Copyright (C) 2001-2011 Kasper Peeters <kasper@phi-sci.com>
// Distributed under the GNU General Public License version 3.
//
// When used together with the htmlcxx library to create 
// HTML::Node template instances, the GNU Lesser General Public 
// version 2 applies. Special permission to use tree.hh under
// the LGPL for other projects can be requested from the author.

#ifndef CORE_COLLECTION_TREE_HPP
#define CORE_COLLECTION_TREE_HPP


#include "tcl/unique_tree.h"
#include "tree.hh"

template <typename T>
class Tree
{
public:

   typedef typename tree<T>::pre_order_iterator iterator;
   typedef const iterator const_iterator;
   typedef typename tree<T>::sibling_iterator child_iterator;
   typedef child_iterator const_child_iterator;

   Tree()
      : impl_()
   {
   }

   explicit Tree(const T& x)
      : impl_(x)
   {
   }

   explicit Tree(const_iterator subTree)
      : impl_(subTree)
   {
   }

   child_iterator insert(child_iterator parent, const T& x)
   {
      return impl_.append_child(parent, x);
   }

   template <typename IteratorType>
   void replace(IteratorType pos, const T& x)
   {
      impl_.replace(pos, x);
   }

   void replace(child_iterator pos, const_iterator subtree)
   {
      impl_.insert_subtree_after(pos, subtree);
      impl_.erase(pos);
   }

   void erase(child_iterator it)
   {
      impl_.erase(it);
   }

   template <typename IteratorType>
   void eraseChildren(const IteratorType& it)
   {
      impl_.erase_children(it);
   }

   child_iterator find(const T& x)
   {
      return std::find(begin(), end(), x);
   }

   iterator begin()
   {
      return impl_.begin();
   }

   iterator end()
   {
      return impl_.end();
   }

   const_iterator begin() const
   {
      return impl_.begin();
   }

   const_iterator end() const
   {
      return impl_.end();
   }

   template <typename IteratorType>
   child_iterator begin(IteratorType it)
   {
      return impl_.begin(it);
   }

   template <typename IteratorType>
   child_iterator end(IteratorType it)
   {
      return impl_.end(it);
   }

   template <typename IteratorType>
   const_child_iterator begin(IteratorType it) const
   {
      return impl_.begin(it);
   }

   template <typename IteratorType>
   const_child_iterator end(IteratorType it) const
   {
      return impl_.end(it);
   }

   void sortChildren(child_iterator from,
                     child_iterator to)
   {
      impl_.sort(from, to, false);
   }

private:
   tree<T> impl_;
};


template <typename T>
class TclTree
{
public:

   typedef typename tcl::unique_tree<T>::pre_order_iterator_type iterator;
   typedef typename tcl::unique_tree<T>::const_pre_order_iterator_type const_iterator;
   typedef typename tcl::unique_tree<T>::associative_iterator_type child_iterator;
   typedef typename tcl::unique_tree<T>::associative_const_iterator_type const_child_iterator;

   TclTree()
      : impl_()
   {
   }

   explicit TclTree(const T& x)
      : impl_(x)
   {
   }

   explicit TclTree(const_iterator subTree)
      : impl_(*subTree.node())
   {
   }

   child_iterator insertRoot(const T& x)
   {
      return impl_.insert(x);
   }

   child_iterator insert(child_iterator parent, const T& x)
   {
      return parent.node()->insert(x);
   }

   template <typename IteratorType>
   void replace(IteratorType pos, const T& x)
   {
      *pos = x;
   }

   void replace(child_iterator pos, const_iterator subtree)
   {
      impl_.insert(pos, *subtree.node());
      impl_.erase(pos);
   }

   void erase(child_iterator it)
   {
      impl_.erase(it);
   }

   template <typename IteratorType>
   void eraseChildren(const IteratorType& it)
   {
      impl_.erase(it.node()->begin(), it.node()->end());
   }

   child_iterator find(const T& x)
   {
      return impl_.find_deep(x);
   }

   iterator begin()
   {
      return impl_.pre_order_begin();
   }

   iterator end()
   {
      return impl_.pre_order_end();
   }

   const_iterator begin() const
   {
      return impl_.pre_order_begin();
   }

   const_iterator end() const
   {
      return impl_.pre_order_end();
   }

   template <typename IteratorType>
   child_iterator begin(IteratorType it)
   {
      return it.node()->begin();
   }

   template <typename IteratorType>
   child_iterator end(IteratorType it)
   {
      return it.node()->end();
   }

   template <typename IteratorType>
   const_child_iterator begin(IteratorType it) const
   {
      return it.node()->begin();
   }

   template <typename IteratorType>
   const_child_iterator end(IteratorType it) const
   {
      return it.node()->end();
   }

   void sortChildren(child_iterator from,
                     child_iterator to)
   {
      // no-op (already guaranteed to be sorted)
   }


private:
   tcl::unique_tree<T> impl_;
};


#endif // CORE_COLLECTION_TREE_HPP
