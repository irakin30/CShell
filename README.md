A functional shell written in C

This shell can (to our knowledge):
  - Deal with special cases of exit and cd (that don't fork)
  - Run regular (other) commands with execvp
  - Read and separate multiple commands on one line with ; (up to 10 args were tested, with a char limit of 1000)
  - Deal with a single | (pipe)
  - Deal with a single < (redirect in)
  - Deal with a single > (redirect out)
  - Deal with a combo of 1 >, <, and | each (like the example on your webpage of form a < c.txt | b > d.txt)
  - Deal with many |s (pipes) (ex: fortune | wc | cowsay | lolcat | wc | cowsay > temp.txt; cat temp.txt)
  - Has >> functionality 