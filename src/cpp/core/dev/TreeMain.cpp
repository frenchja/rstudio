/*
 * TreeMain.cpp
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

#include <iostream>
#include <algorithm>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/test/minimal.hpp>

#include <core/Error.hpp>
#include <core/Log.hpp>
#include <core/FileInfo.hpp>

#include <core/system/System.hpp>

#include <core/collection/Tree.hpp>

using namespace core ;


template <typename Iterator>
Iterator findFile(Iterator begin, Iterator end, const std::string& path)
{
   return std::find_if(begin, end, boost::bind(fileInfoHasPath,
                                               _1,
                                               path));
}

template<typename TreeType>
TreeType create()
{
   TreeType t(FileInfo("/root", true));
   return t;
}

template <typename TreeType>
TreeType subTree(const TreeType& tree)
{
   typename TreeType::const_iterator it = findFile(tree.begin(), tree.end(), "/b/y");
   return TreeType(it);
}


template <typename TreeType>
void sortChildren(TreeType* pTree)
{
   typename TreeType::iterator it = findFile(pTree->begin(), pTree->end(), "/b/y");
   pTree->sortChildren(pTree->begin(it), pTree->end(it));
}

template <typename TreeType>
void eraseSingle(TreeType* pTree)
{
   typename TreeType::child_iterator it = pTree->find(FileInfo("/b/y/d/r", false));
   pTree->erase(it);
}

template <typename TreeType>
void eraseChildren(TreeType* pTree)
{
   typename TreeType::iterator it = findFile(pTree->begin(), pTree->end(), "/b/y/d");
   pTree->eraseChildren(it);
}


template<typename TreeType>
void replaceSubtree(TreeType* pTree)
{
   TreeType subTree(FileInfo("/b/y", true));
   subTree.insert(subTree.begin(subTree.begin()), FileInfo("/b/y/p", false));
   typename TreeType::child_iterator itSub = subTree.insert(subTree.begin(subTree.begin()), FileInfo("/b/y/d", true));
      subTree.insert(itSub, FileInfo("/b/y/d/q", false));
      subTree.insert(itSub, FileInfo("/b/y/d/r", false));
      subTree.insert(itSub, FileInfo("/b/y/d/s", false));

   itSub = subTree.insert(subTree.begin(subTree.begin()), FileInfo("/b/y/q", false));
      subTree.insert(itSub, FileInfo("/b/y/q/r", false));
      subTree.insert(itSub, FileInfo("/b/y/q/s", false));

   typename TreeType::child_iterator it = pTree->find(FileInfo("/b/y", true));
   pTree->replace(it, subTree.begin());
}


template<typename TreeType>
void replaceSingle(TreeType* pTree)
{
   typename TreeType::iterator it = findFile(pTree->begin(), pTree->end(), "/b/y/q/s");
   pTree->replace(it, FileInfo("/b/y/q/t", false));
}

template<typename TreeType>
void insertChildren(TreeType* pTree)
{
   pTree->insert(pTree->begin(pTree->begin()), FileInfo("/a", false));
   typename TreeType::child_iterator it = pTree->insert(pTree->begin(pTree->begin()), FileInfo("/b", true));
      pTree->insert(it, FileInfo("/b/x", false));
      typename TreeType::child_iterator it2 = pTree->insert(it, FileInfo("/b/y", true));
         pTree->insert(it2, FileInfo("/b/y/g", false));
         pTree->insert(it2, FileInfo("/b/y/h", false));
         pTree->insert(it2, FileInfo("/b/y/i", false));
      pTree->insert(it, FileInfo("/b/z", false));
      pTree->insert(pTree->begin(pTree->begin()), FileInfo("/c", false));
}

void printFileInfo(const FileInfo& fileInfo, int indentLevel)
{
   std::string pad(indentLevel, ' ');
   std::cerr << pad << fileInfo.absolutePath() << std::endl;
}

template<typename TreeType>
void print(const TreeType& tree,
           typename TreeType::const_child_iterator begin,
           typename TreeType::const_child_iterator end,
           int indentLevel)
{
   while (begin != end)
   {
      printFileInfo(*begin, indentLevel);

      print(tree, tree.begin(begin), tree.end(begin), indentLevel + 1);

      begin++;
   }
}

template<typename TreeType>
void printTree(const TreeType& tree)
{
   typename TreeType::const_iterator rootIt = tree.begin();
   printFileInfo(*rootIt, 0);
   print(tree, tree.begin(rootIt), tree.end(rootIt), 1);
   std::cerr << std::endl;
}

template<typename TreeType1, typename TreeType2>
bool areEqual(const TreeType1& tree1, const TreeType2& tree2)
{
   if (std::distance(tree1.begin(), tree1.end()) !=
       std::distance(tree2.begin(), tree2.end()))
   {
      return false;
   }

   return std::equal(tree1.begin(), tree1.end(), tree2.begin());
}

int test_main(int argc, char * argv[])
{
   try
   { 

      // initialize log
      initializeSystemLog("coredev", core::system::kLogLevelWarning);

      TclTree<FileInfo> tree1 = create<TclTree<FileInfo> >();
      TclTree<FileInfo> tree2 = create<TclTree<FileInfo> >();
      BOOST_CHECK(areEqual(tree1, tree2));

      insertChildren(&tree1);
      insertChildren(&tree2);
      printTree(tree1);
      printTree(tree2);


      BOOST_CHECK(areEqual(tree1, tree2));

      replaceSubtree(&tree1);
      replaceSubtree(&tree2);
      BOOST_CHECK(areEqual(tree1, tree2));

      eraseSingle(&tree1);
      eraseSingle(&tree2);
      BOOST_CHECK(areEqual(tree1, tree2));

      eraseChildren(&tree1);
      eraseChildren(&tree2);
      BOOST_CHECK(areEqual(tree1, tree2));

      replaceSingle(&tree1);
      replaceSingle(&tree2);
      BOOST_CHECK(areEqual(tree1, tree2));

      sortChildren(&tree1);
      sortChildren(&tree2);
      BOOST_CHECK(areEqual(tree1, tree2));

      TclTree<FileInfo> subTree1 = subTree(tree1);
      TclTree<FileInfo> subTree2 = subTree(tree2);
      BOOST_CHECK(areEqual(subTree1, subTree2));

      printTree(tree1);
      printTree(tree2);
      return EXIT_SUCCESS;
   }
   CATCH_UNEXPECTED_EXCEPTION
   
   // if we got this far we had an unexpected exception
   return EXIT_FAILURE ;
}

