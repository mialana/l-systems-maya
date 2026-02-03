Get-ChildItem -Recurse -Path LSystemMaya, LSystem `
    -Include *.h, *.cpp, *.inl | 
    ForEach-Object { clang-format -i $_.FullName }
