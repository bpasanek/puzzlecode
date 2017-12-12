###Second Meeting - 12/12/2017

Jeremy Boggs and Tim Schott (in SLab)

Brad added a folder _posts to put Blogging stuff in

Here's a code snippet for pulling posts from that folder and putting up online:

{% for post in site.posts %}
<a href="{{ site.baseurl }}/{{ post.url }}">{{ post.title }}</a>
{% endfor %}

Was tested on the main page and worked.

An example of a Jekyll site that Jeremy made:  Jeremyboggs.github.io

Restyle our theme?  
Need a layouts folder and at least one: a default.
Jeremy trying it at gh-pages -- making a _layouts folder

(I'll add one to gh-pages.)
 
Trying to expose designer, maker, writeup etc. in projects. Want to get our metadata righ.

Should a poem have a repository...? maybe some of the bigger projects could. The games, for example, also the Increase project could have its own repo, I guess... But I don't know. Why not keep most code on master and most web stuff on gh-pages.

Back to the metadata question:  
Collect people as slugs  
e.g. neal-curtis  
Then you have a _people folder with all their information: dept, email, etc.

This is how Slab is doing it.

Jeremy really likes Jekyll data files

Have a single file then that is a people.yml file

remember jekyll serve --watch

There is good Jekyll documentation on data files.

----

Look up by author, want a blog, too.
Reflections on 

[Crossword Puzzle from urban dictionary...]

Tips and tricks:  
git branch  
git branch -r  

CSS style sheet can be referenced for Jekyll. Needs some empty yml frontmatter in the top of the file.

Bootstrap is twitter's framework: open-sourced.   
Steal buttons or whatever, something to look at to look at, at least.

Jeremy gives one more web development tip. CSS with superpowers: Sass, preprocessor, lets you create variables and functions that you can use throughout the site. 
