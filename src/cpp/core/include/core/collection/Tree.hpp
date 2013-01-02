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

#include "tree.hh"

template <typename T>
class Tree
{
public:

   typedef typename tree<T>::iterator iterator;
   typedef typename tree<T>::sibling_iterator sibling_iterator;
   typedef typename tree<T>::leaf_iterator leaf_iterator;

   Tree()
      : impl_()
   {
   }

   explicit Tree(const iterator& subTree)
      : impl_(subTree)
   {
   }

   iterator set_head(const T& x)
   {
      return impl_.set_head(x);
   }

   iterator append_child(iterator pos, const T& x)
   {
      return impl_.append_child(pos, x);
   }


   sibling_iterator replace(sibling_iterator pos, const T& x)
   {
      return impl_.replace(pos, x);
   }

   void replace(sibling_iterator pos, const iterator& subtree)
   {
      impl_.insert_subtree_after(pos, subtree);
      impl_.erase(pos);
   }

   void replace(iterator pos, const iterator& subtree)
   {
      impl_.insert_subtree_after(pos, subtree);
      impl_.erase(pos);
   }

   void erase(sibling_iterator it)
   {
      impl_.erase(it);
   }

   void erase_children(const iterator& it)
   {
      impl_.erase_children(it);
   }

   iterator begin() const
   {
      return impl_.begin();
   }

   iterator end() const
   {
      return impl_.end();
   }

   sibling_iterator begin(const iterator& it) const
   {
      return impl_.begin(it);
   }

   sibling_iterator end(const iterator& it) const
   {
      return impl_.end(it);
   }

   leaf_iterator begin_leaf() const
   {
      return impl_.begin_leaf();
   }

   leaf_iterator end_leaf() const
   {
      return impl_.end_leaf();
   }

   template<class StrictWeakOrdering>
   void sort(sibling_iterator from,
             sibling_iterator to,
             StrictWeakOrdering comp,
             bool deep=false)
   {
      return impl_.sort(from, to, comp, deep);
   }



private:
   tree<T> impl_;
};

#endif // CORE_COLLECTION_TREE_HPP
