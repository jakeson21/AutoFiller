# AutoFiller
AutoFiller is a cross platform program based on Qt that assists in tedious process of filling out electronic forms.

The user supplies the formatted data in the form of a csv file which the program reads in. The user then builds a command
string which specifies which data to select and where to place in the target form. Ideally the target form will
allow tabbed movement between each field. Once the command string is built, a single row of data can be entered with a single click.

## Building a Command String
Build a command string is a easy as specifying which column to send and which commands to send.
Data and commands are each enclosed in square brackets "[]" and follow one after the other as necessary.
Data fields will be represented by numeric values which correspond to which column they come from.

## Command List
The list of supported commands, or special keystrokes, is as follows:
```
[BACK]
[TAB]
[SPACE]
[LEFT]
[UP]
[RIGHT]
[DOWN]
[RETURN]
[ALT]
[ALTTAB]
[CTRL]
```


