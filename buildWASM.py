import os
import argparse
from bs4 import BeautifulSoup

cwd = os.getcwd()

defaultColor = '#FFFFFF'


def colorParser(s):
    if s == defaultColor:
        return s
    if isinstance(s, str) and len(s) == 6:
        return '#' + s
    print('Color must be a 6 character string like FFFFFF, with no #')
    exit(0)


argParser = argparse.ArgumentParser(
    prog='Raylib WASM builder',
    description='Uses emcc to main.c with some additional cleanup',
    epilog='Requires setup of Raylib (youtu.be/-F6THkPkF2I) and emcc (youtu.be/j6akryezlzc)')
argParser.add_argument('-n', '--name', type=str,
                       default="Game", help='page title name')
argParser.add_argument('-c', '--color', type=colorParser, default=defaultColor,
                       help='background color of page (like FFFFFF, no #)')
argParser.add_argument('-i', '--input', type=str,
                       default=cwd, help='input directory')
argParser.add_argument('-o', '--output', type=str,
                       default=cwd, help='output directory')
argParser.add_argument(
    '-nb', '--nobuild', action='store_true', help='do not build WASM')
argParser.add_argument(
    '-nc', '--noclean', action='store_true', help='do not clean output .html')
args = vars(argParser.parse_args())

inputPath = os.path.join(args['input'], 'main.c')
if not os.path.exists(inputPath):
    print('Input path {} does not exist'.format(inputPath))
    exit(0)
outputPath = os.path.join(args['output'], 'game.html')

if not args['nobuild']:
    raylibSrcDir = 'C:/raylib/raylib/src'
    compilerArgs = "-Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I {0} -I {0}/external -L. -L {0} -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file {0}/shell.html {0}/web/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS=[\"_free\",\"_malloc\",\"_main\"] -s EXPORTED_RUNTIME_METHODS=ccall".format(
        raylibSrcDir)
    build_cmd = "emcc -o {} {} {}".format(outputPath, inputPath, compilerArgs)
    os.system(build_cmd)


if not args['noclean']:
    # Read the output .html
    with open(outputPath, 'r') as f:
        webpage = f.read()
    soup = BeautifulSoup(webpage, 'html.parser')
    # Replace head with cleanHead
    head = soup.select('head')[0]
    head.clear()
    cleanHead = "<title>" + args['name'] + "</title> <meta charset=\"utf-8\"/> <meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\"/> <meta content=\"width=device-width\" name=\"viewport\"/> <style>body{font-family: arial; margin: 0; padding: none; background-color: " + args[
        'color'] + "}.emscripten{padding-right: 0; margin-left: auto; margin-right: auto; display: block}div.emscripten{text-align: center}div.emscripten_border{border: none;}canvas.emscripten{border: 0 none; background: #000; width: min(100vw, 100vh);}</style>"
    head.append(BeautifulSoup(cleanHead, 'html.parser'))
    # Hide/delete some body elements
    soup.select('div')[0]['style'] = 'display: none;'
    soup.select('textarea')[0]['style'] = 'display: none;'
    soup.select('a')[0].decompose()
    # Overwrite .html
    with open(outputPath, 'w') as file:
        file.write(soup.prettify())
