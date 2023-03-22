# RDR2wheel

The Red Dead Redeption 2 selector wheel is a great way to replace typical mouse and keyboard graphical user interactions.  

The goal here is to create a user-friendly template to add the selector wheel to a Raylib project.

## Installation

There are great tutorials for getting setup with Raylib and emscripten

- [Raylib on Windows](youtu.be/-F6THkPkF2I)
- [enscripten on Windows](youtu.be/j6akryezlzc)

## Usage/Examples

The `.py` in the root of this repository builds the `main.c` file in the specified directory and outputs an `.html` with WASM assets.  

So far, I have been using it like this:

```ps
python .\buildWASM.py -i .\src\ -o .\src\ -c 123456 -n "RDR2 Wheel"
```

## Run Locally

I like to use VS Code with the Live Server extension from Ritwick Dey to develop the WASM `.html`.  

For quicker tests, using the Raylib notepad++ F6 command as shown in the tutorial is great.
