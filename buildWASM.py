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
args = vars(argParser.parse_args())

inputPath = os.path.join(args['input'], 'main.c')
if not os.path.exists(inputPath):
    print('Input path {} does not exist'.format(inputPath))
    exit(0)
outputPath = os.path.join(args['output'], 'game.html')

# Build WASM
raylibSrcDir = 'C:/raylib/raylib/src'
compilerArgs = "-Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I {0} -I {0}/external -L. -L {0} -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 -s FORCE_FILESYSTEM=1 --shell-file ./src/shell.html {0}/web/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS=[\"_free\",\"_malloc\",\"_main\"] -s EXPORTED_RUNTIME_METHODS=ccall".format(
    raylibSrcDir)
build_cmd = "emcc {} -o {} {}".format(inputPath, outputPath, compilerArgs)
# Pack resources
resDir = os.path.join('src', 'resources')
for subdir, dirs, files in os.walk(resDir):
    for file in files:
        resPath = os.path.join(subdir, file)
        resString = " --preload-file {}@{}".format(
            resPath, resPath .replace('src', ''))
        print(resString)
        build_cmd += resString
os.system(build_cmd)

# Change name and color in .html
with open(outputPath, 'r') as f:
    webpage = f.read()
soup = BeautifulSoup(webpage, 'html.parser')
soup.select('title')[0].string = args['name']
soup.select('body')[0]['style'] = 'background-color: ' + args['color'] + ';'
# Overwrite .html
with open(outputPath, 'w') as file:
    file.write(soup.prettify())
