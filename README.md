# RDR2wheel

The Red Dead Redemption 2 wheel HUD is a great way to replace typical mouse and keyboard graphical user interactions. Since Raylib makes using a gamepad extremely easy and I typically make button-heavy GUIs, the goal here is to create a user-friendly template to add the selector wheel to a Raylib project.  

![RDR2](https://miro.medium.com/v2/resize:fit:640/format:webp/1*KObw_YewVr50ABFDDv7srA.jpeg)

## Installation

There are great tutorials for getting setup with Raylib and emscripten

- [Raylib on Windows](https://www.youtube.com/watch?v=-F6THkPkF2I)
- [emscripten on Windows](https://www.youtube.com/watch?v=j6akryezlzc)

Then, download [`raygui.h`](https://github.com/raysan5/raygui/releases) and move it to `C:\raylib\raylib\src`.  
If using VS Code, add `"C:/raylib/raylib/src/**"` to the C include paths as seen in the `.vscode` directory.

## Usage/Examples

The `.py` in the root of this repository builds the `main.c` file in the specified directory and outputs an `.html` with WASM assets. So far, I have been using it like this:

```ps
python .\buildWASM.py -i .\src\ -o .\src\ -c 123456 -n "RDR2 Wheel"
```

## Run Locally

I like to use VS Code with the Live Server extension from Ritwick Dey to develop the WASM `.html`. For quicker tests, using the Raylib notepad++ F6 command as shown in the tutorial is great.
