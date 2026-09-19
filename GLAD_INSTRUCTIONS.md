

# Cmake for glad

Run this command to create compile_commands.json for clangd. This allows nvim 
to lint properly. Do this from the project root.

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON


# Ext command for glad

Run this command to link glad into the project.

glad --api gl:core=3.3 --out-path ext/glad



