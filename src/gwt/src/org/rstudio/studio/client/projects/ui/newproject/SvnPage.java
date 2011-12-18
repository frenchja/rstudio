/*
 * SvnPage.java
 *
 * Copyright (C) 2009-11 by RStudio, Inc.
 *
 * This program is licensed to you under the terms of version 3 of the
 * GNU Affero General Public License. This program is distributed WITHOUT
 * ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
 * AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
 *
 */
package org.rstudio.studio.client.projects.ui.newproject;

import org.rstudio.core.client.files.FileSystemItem;
import org.rstudio.studio.client.common.vcs.VCSConstants;

public class SvnPage extends VersionControlPage
{
   public SvnPage()
   {
      super("Subversion", 
            "Checkout a project from a Subversion repository",
            "Checkout Subversion Repository",
            NewProjectResources.INSTANCE.svnIcon(),
            NewProjectResources.INSTANCE.svnIconLarge());
   }
   

   @Override
   protected String getVcsId()
   {
      return VCSConstants.SVN_ID;
   }

   @Override
   protected boolean includeCredentials()
   {
      return true;
   }
   
   
   @Override 
   protected String guessRepoDir(String url)
   {
      String guess = FileSystemItem.createFile(url).getStem();
      if (guess.length() > 1)
         return guess;
      else
         return null;
   }
}