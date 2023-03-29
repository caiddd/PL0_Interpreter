import os
code = "build/src/PL0"
path = "example" 
files = os.listdir(path) 
for file in files:
  if file == "CMakeLists.txt" or os.path.isdir(file): continue
  print('\n' + file + '\n')
  os.system(code + ' ' + path + "/" + file)