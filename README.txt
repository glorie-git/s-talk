Short for simple-talk, S-talk is a CLI chat interface for machines connected locally

Assignment 2 for Simon Fraser University's Summer 2021 CMPT 300: Operating Systems

S-talk

Issues:
When user enters a message longer than 1024 characters, only the last characters  that fit into the buffer are sent. The rest and gone.

Valgrind reports 5 errors including some initialized errors and some others things. I could clear them in time.

S-talk might report port in use for ports previously used. I didn't add the code that takes care of this
