# AutoFiller
AutoFiller is a cross platform program based on Qt that assists in the tedious process of filling out electronic forms.

The user supplies the formatted data in the form of a csv file which the program reads in. The user then builds a command
string which specifies which data to select and where to place in the target form. Ideally the target form will
allow tabbed movement between each field. Once the command string is built, a single row of data can be entered with a single click.

## Building a Command String
Building a command string is as easy as specifying which column to send and which commands to send.
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

## Example Command String
Imagine you've loaded in a data set and wish to use columns 1, 3 and 5 (in that order) and the form can
be navigated by a single TAB between fields. With the cursor positioned in the first field, the command string would look like:
```
[1][TAB][2][TAB][3]
```

To auto-fill the form the user would need to be sure the cursor is positioned in the first field, then click back to AutoFIller and
input the command string. Finally, click "Send Key Sequence". AutoFiller will switch to the last active window and simulate the data entry.


