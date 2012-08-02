
Adding  openCL syntax highlighting for Windows 
1.	Go to Tools->Options from the Visual studio.
2.	Select the File extension sub-tab inside text editor tab.
3.	Add .cl as the new extension and select Microsoft Visual c++ as editor.
4.	Click ADD and then click OK.
5.	Now create a text file( from VS or from notepad) and name it usertype.dat
6.	Now type all the additional keywords that you want to have highlighted. Write only one word in a line. 
7.	Now add this file to the place where devenv.exe is stored in your system. On my system it is: C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE.
8.	In case a usertype.dat file is already present you should append the keywords to the existing file.
9.	Close Visual Studio and start again to see this list highlighted.


Adding Syntax highlighting in Linux gEdit editor
1.	Close all the gEdit windows.
2.	Copy the opencl.lang file into /usr/share/gtksourceview-2.0/language-specs/
3.	Open any *.cl file and enjoy the color coding of the the kernel.


Adding Syntax highlighting in Linux for VIM Editor
1.	You should have a administrative previlages for editing these files.
2.	Take the file c.vim and replace it with c.vim present at /usr/share/vim/vim72/syntax.
3.	Go to  /usr/share/vim and type these lines at the end of vimrc file:
	a)	syntax on
	b)	filetype on
	c)	au BufNewFile,BufRead *.cl set filetype=c

NOTE: You can also create a separate opencl.vim file by copying opencl related parts from c.vim. You will have to copy the code defining every syntax keyword and the code linking the cl Syntax Groups to standard VIM syntax groups.

References:
http://vimdoc.sourceforge.net/htmldoc/syntax.html
http://www.streamcomputing.eu/blog/2011-02-01/gedit-opencl-syntax-highlighting.

	
