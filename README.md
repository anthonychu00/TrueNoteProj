The following is a tool using the C++ version of OpenCV to translate images of sheet music to note letters. As opposed to something like Convulutional Neural Networks, template matching was used instead, mostly for learning purposes.

Example:

Finding the notes in the sheet music.

![MatchingC](https://user-images.githubusercontent.com/38588759/97217001-00767680-179d-11eb-8b09-b5ff3b09ce89.png)

Contextualizing the notes and printing them out.

![Notes](https://user-images.githubusercontent.com/38588759/97217595-ea1cea80-179d-11eb-9de2-e94b364a7c62.png)


Weaknesses of the template matching model:

- If you do not have enough templates for the model to match against, it might miss some possible matches. This can be remedied by adding more templates, but it would cause the tool to run slower.

- It can find many false positives and find the same match "twice". These need to be filtered out of the final output.

- You have to scan the image multiple times, but I'm not sure how avoidable this is.

- Finding eighth notes is clunky since the note's head looks the same as a quarter note, would have to take into account the other eighth.
