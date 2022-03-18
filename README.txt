S-talk

Issues:
When user enters a message longer than 1024 characters, only the last characters  that fit into the buffer are sent. The rest and gone.

Valgrind reports 5 errors including some initialized errors and some others things. I could clear them in time.

S-talk might report port in use for ports previously used. I didn't add the code that takes care of this